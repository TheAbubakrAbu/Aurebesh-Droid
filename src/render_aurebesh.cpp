// render_aurebesh.cpp
//
// Lays text out in an embedded font, then paints and writes it as a palette
// PNG. The interface and what it promises are in render_aurebesh.hpp.

#define STB_TRUETYPE_IMPLEMENTATION
#include "font_image/stb_truetype.h"

#include "render_aurebesh.hpp"

#include <algorithm>
#include <climits>
#include <cmath>
#include <cstdint>
#include <iostream>
#include <unordered_map>
#include <vector>

#include <zlib.h>

#include "embedded_fonts.hpp"

namespace aurebesh {
namespace {

constexpr float font_pixel_height = 96;          // glyph size
constexpr int padding = 56;                      // border around the text
constexpr float max_line_width = 2000;           // wrap lines wider than this
constexpr int min_line_gap = 12;                 // fewest blank pixels between lines
constexpr std::size_t max_pixels = 40'000'000;   // refuse absurdly large images

constexpr float core_whiteness = 0.3f;  // on dark backgrounds, glyphs are tinted toward white
constexpr float light_ink = 0.3f;       // on light backgrounds, glyphs are shaded to this luminance
constexpr int glow_radius = 7;          // each of three box blurs, in pixels
constexpr float glow_boost = 2.5f;      // strokes are thin next to the blur, so lift it
constexpr float glow_strength = 0.85f;  // brightest the glow gets
constexpr float light_glow = 0.35f;     // a glow on a light background is a soft tint

// Every pixel is one of 16 glyph-coverage levels times 16 glow levels, so the
// image fits a 256-color palette. That PNG is about half the size of a
// full-color one and looks the same.
constexpr int cover_levels = 16;
constexpr int glow_levels = 16;

struct Font {
    stbtt_fontinfo info{};
    float scale = 0;
    int space_glyph = 0;
    int ascent = 0;       // pixels from the top of a line to its baseline
    int line_box = 0;     // pixels a line's glyphs can cover, top to bottom
    int line_height = 0;  // pixels from one line's top to the next line's top
};

bool parse(const EmbeddedFont& embedded, Font& font) {
    const int offset = stbtt_GetFontOffsetForIndex(embedded.data, 0);
    if (offset < 0 || !stbtt_InitFont(&font.info, embedded.data, offset)) return false;

    font.scale = stbtt_ScaleForPixelHeight(&font.info, font_pixel_height);
    font.space_glyph = stbtt_FindGlyphIndex(&font.info, ' ');

    // Several of these fonts draw glyphs taller or deeper than the ascent and
    // descent they declare, so measure the glyphs themselves. Lines are then
    // spaced so nothing is clipped and every glyph sits on the baseline.
    int top = 0, bottom = 0;  // font units, y pointing up
    auto measure = [&](int glyph) {
        int x0, y0, x1, y1;
        if (stbtt_GetGlyphBox(&font.info, glyph, &x0, &y0, &x1, &y1)) {
            top = std::max(top, y1);
            bottom = std::min(bottom, y0);
        }
    };
    measure(0);  // the box drawn for characters the font lacks
    for (int c = 0x21; c < 0x7F; ++c) {
        if (int glyph = stbtt_FindGlyphIndex(&font.info, c)) measure(glyph);
    }
    if (top <= bottom) return false;

    font.ascent = static_cast<int>(std::ceil(top * font.scale));
    font.line_box = font.ascent + static_cast<int>(std::ceil(-bottom * font.scale));

    // Space lines the way the font asks, unless its glyphs would then touch.
    int ascent, descent, line_gap;
    stbtt_GetFontVMetrics(&font.info, &ascent, &descent, &line_gap);
    font.line_height = std::max(static_cast<int>(std::lround((ascent - descent + line_gap) * font.scale)),
                                font.line_box + min_line_gap);
    return true;
}

struct Registry {
    std::unordered_map<std::string_view, Font> fonts;
    bool ok = true;
};

// Built on first use; thread-safe, and read-only afterwards.
const Registry& registry() {
    static const Registry instance = [] {
        Registry r;
        for (const EmbeddedFont& embedded : embedded_fonts()) {
            Font font;
            if (parse(embedded, font)) {
                r.fonts.emplace(embedded.name, font);
            } else {
                std::cerr << "Could not read embedded font " << embedded.name << '\n';
                r.ok = false;
            }
        }
        return r;
    }();
    return instance;
}

std::u32string decode_utf8(std::string_view text) {
    std::u32string out;
    out.reserve(text.size());
    for (std::size_t i = 0; i < text.size();) {
        const auto lead = static_cast<unsigned char>(text[i]);
        const int extra = lead < 0x80 ? 0 : (lead >> 5) == 0x6 ? 1 : (lead >> 4) == 0xE ? 2 : (lead >> 3) == 0x1E ? 3 : -1;
        char32_t c = extra == 0 ? lead : extra == 1 ? lead & 0x1F : extra == 2 ? lead & 0x0F : lead & 0x07;
        bool valid = extra >= 0 && i + extra < text.size();
        for (int k = 1; valid && k <= extra; ++k) {
            const auto next = static_cast<unsigned char>(text[i + k]);
            valid = (next & 0xC0) == 0x80;
            c = (c << 6) | (next & 0x3F);
        }
        out.push_back(valid ? c : U'�');
        i += valid ? extra + 1 : 1;
    }
    return out;
}

bool is_space(char32_t c) {
    return c == ' ' || c == '\t' || c == 0x00A0 || (c >= 0x2000 && c <= 0x200A) ||
           c == 0x202F || c == 0x205F || c == 0x3000;
}

// Control characters, zero-width characters, emoji variation selectors, and
// combining accents (some keyboards type é as e plus a separate accent).
bool is_invisible(char32_t c) {
    return c < 0x20 || c == 0x7F || (c >= 0x0300 && c <= 0x036F) || (c >= 0x200B && c <= 0x200F) ||
           c == 0x2060 || c == 0xFEFF || (c >= 0xFE00 && c <= 0xFE0F);
}

// Emoji, pictographs, and the symbols drawn as emoji. No font here has them,
// and a row of placeholder boxes reads worse than leaving them out, which
// matters for chat messages translated from the right-click menu.
bool is_emoji(char32_t c) {
    return (c >= 0x1F000 && c <= 0x1FAFF) || (c >= 0x2600 && c <= 0x27BF) || (c >= 0x2B00 && c <= 0x2BFF) ||
           (c >= 0x2190 && c <= 0x21FF) || (c >= 0x2300 && c <= 0x23FF) || c == 0x20E3 || (c >= 0xE0020 && c <= 0xE007F);
}

// Plain-ASCII stand-ins, used only when a font has no glyph of its own:
// phone keyboards type curly quotes, and names like Padmé carry accents.
std::string_view ascii_fallback(char32_t c) {
    switch (c) {
        case 0x2018: case 0x2019: case 0x201A: case 0x201B: case 0x2032: case 0x02BC: return "'";
        case 0x201C: case 0x201D: case 0x201E: case 0x201F: case 0x2033: return "\"";
        case 0x2010: case 0x2011: case 0x2012: case 0x2013: case 0x2014: case 0x2015: case 0x2212: return "-";
        case 0x2026: return "...";
        case 0x00D7: return "x";
        case 0x00DF: return "ss";
    }
    if (c >= 0x00C0 && c <= 0x00FF) {
        // Latin-1 letters. Uppercase U+00C0..U+00DE maps onto lowercase U+00E0..U+00FE.
        static constexpr std::string_view lower[32] = {
            "a", "a", "a", "a", "a", "a", "ae", "c", "e", "e", "e", "e", "i", "i", "i", "i",
            "d", "n", "o", "o", "o", "o", "o",  "",  "o", "u", "u", "u", "u", "y", "th", "y"};
        return lower[(c | 0x20) - 0xE0];
    }
    return {};
}

struct Item {
    int glyph;
    bool space;
};

// Turns one character into glyphs. Letters are drawn from their lowercase
// glyph, as the bot always has. A character the font lacks becomes its ASCII
// stand-in if there is one, is left out if it is an emoji, and otherwise
// becomes the font's missing-character box.
void append_glyphs(const Font& font, char32_t c, std::vector<Item>& out) {
    if (is_space(c)) {
        out.push_back({font.space_glyph, true});
        return;
    }
    if (is_invisible(c)) return;

    if (c >= 'A' && c <= 'Z') {
        if (int glyph = stbtt_FindGlyphIndex(&font.info, c - 'A' + 'a')) {
            out.push_back({glyph, false});
            return;
        }
    }
    if (int glyph = stbtt_FindGlyphIndex(&font.info, c)) {
        out.push_back({glyph, false});
        return;
    }
    if (std::string_view ascii = ascii_fallback(c); !ascii.empty()) {
        for (char a : ascii) append_glyphs(font, static_cast<unsigned char>(a), out);
        return;
    }
    if (is_emoji(c)) return;
    out.push_back({0, false});
}

float advance(const Font& font, int glyph) {
    int width, left_bearing;
    stbtt_GetGlyphHMetrics(&font.info, glyph, &width, &left_bearing);
    return width * font.scale;
}

// Greedy word wrap. A line breaks at a space so it stays within
// max_line_width; only a word wider than a whole line is split between letters.
void wrap(const Font& font, const std::vector<Item>& items, std::vector<std::vector<int>>& lines) {
    lines.emplace_back();
    float width = 0;
    std::size_t i = 0;
    while (i < items.size()) {
        const std::size_t spaces_begin = i;
        float spaces_width = 0;
        for (; i < items.size() && items[i].space; ++i) spaces_width += advance(font, items[i].glyph);

        const std::size_t word_begin = i;
        float word_width = 0;
        for (; i < items.size() && !items[i].space; ++i) word_width += advance(font, items[i].glyph);
        if (word_begin == i) break;  // trailing spaces

        if (!lines.back().empty()) {
            if (width + spaces_width + word_width > max_line_width) {
                lines.emplace_back();
                width = 0;
            } else {
                for (std::size_t s = spaces_begin; s < word_begin; ++s) lines.back().push_back(items[s].glyph);
                width += spaces_width;
            }
        }

        for (std::size_t g = word_begin; g < i; ++g) {
            const float step = advance(font, items[g].glyph);
            if (!lines.back().empty() && width + step > max_line_width) {
                lines.emplace_back();
                width = 0;
            }
            lines.back().push_back(items[g].glyph);
            width += step;
        }
    }
}

struct Placed {
    int glyph;
    float x;
};

// One box blur along rows, then along columns, with nothing beyond the edges.
// Three of these in a row look like a Gaussian blur.
void box_blur(std::vector<unsigned char>& pixels, int width, int height, int radius) {
    const int window = 2 * radius + 1;
    std::vector<unsigned char> rows(pixels.size());
    for (int y = 0; y < height; ++y) {
        const unsigned char* src = pixels.data() + static_cast<std::size_t>(y) * width;
        unsigned char* dst = rows.data() + static_cast<std::size_t>(y) * width;
        int sum = 0;
        for (int x = 0; x < radius && x < width; ++x) sum += src[x];
        for (int x = 0; x < width; ++x) {
            if (x + radius < width) sum += src[x + radius];
            dst[x] = static_cast<unsigned char>(sum / window);
            if (x - radius >= 0) sum -= src[x - radius];
        }
    }

    // Columns run a whole row at a time, which keeps memory access in order.
    std::vector<int> sums(width, 0);
    const auto add_row = [&](int y, int sign) {
        const unsigned char* src = rows.data() + static_cast<std::size_t>(y) * width;
        for (int x = 0; x < width; ++x) sums[x] += sign * src[x];
    };
    for (int y = 0; y < radius && y < height; ++y) add_row(y, 1);
    for (int y = 0; y < height; ++y) {
        if (y + radius < height) add_row(y + radius, 1);
        unsigned char* dst = pixels.data() + static_cast<std::size_t>(y) * width;
        for (int x = 0; x < width; ++x) dst[x] = static_cast<unsigned char>(sums[x] / window);
        if (y - radius >= 0) add_row(y - radius, -1);
    }
}

struct Rgb {
    float r, g, b;
};

Rgb unpack(std::uint32_t color) {
    return {static_cast<float>((color >> 16) & 0xFF), static_cast<float>((color >> 8) & 0xFF),
            static_cast<float>(color & 0xFF)};
}

Rgb mix(Rgb a, Rgb b, float t) {
    return {a.r + (b.r - a.r) * t, a.g + (b.g - a.g) * t, a.b + (b.b - a.b) * t};
}

float luminance(Rgb c) {
    return (0.2126f * c.r + 0.7152f * c.g + 0.0722f * c.b) / 255;
}

std::string bytes(Rgb c) {
    std::string out;
    for (float channel : {c.r, c.g, c.b}) out.push_back(static_cast<char>(std::lround(std::clamp(channel, 0.0f, 255.0f))));
    return out;
}

// The palette as PNG wants it: red, green, and blue for each entry, then (for
// a transparent background only) each entry's opacity. Entry c * glow_levels
// + g is glyph coverage level c over glow level g.
struct Palette {
    std::string colors;
    std::string opacity;
};

Palette make_palette(const Look& look) {
    const Rgb accent = unpack(look.color);
    const Rgb back = unpack(look.background);
    const bool light = !look.transparent && luminance(back) > 0.5f;

    // Light glyphs glow on dark backgrounds. On light backgrounds, the color
    // is shaded just dark enough to read, keeping its hue. A transparent image
    // uses the color itself, which reads on Discord's light and dark themes.
    Rgb ink = mix(accent, {255, 255, 255}, core_whiteness);
    if (light && luminance(accent) > light_ink) ink = mix(accent, {0, 0, 0}, 1 - light_ink / luminance(accent));
    if (light && luminance(accent) <= light_ink) ink = accent;
    if (look.transparent) ink = accent;

    Palette palette;
    for (int c = 0; c < cover_levels; ++c) {
        const float cover = static_cast<float>(c) / (cover_levels - 1);
        for (int g = 0; g < glow_levels; ++g) {
            const float halo = glow_strength * g / (glow_levels - 1);
            if (look.transparent) {
                palette.colors += bytes(accent);
                const float opacity = cover + (1 - cover) * halo;
                palette.opacity.push_back(static_cast<char>(std::lround(opacity * 255)));
            } else if (light) {
                palette.colors += bytes(mix(mix(back, accent, halo * light_glow), ink, cover));
            } else {
                const Rgb lit = {back.r + accent.r * halo, back.g + accent.g * halo, back.b + accent.b * halo};
                palette.colors += bytes(mix(lit, ink, cover));
            }
        }
    }
    return palette;
}

void append_u32(std::string& out, std::uint32_t value) {
    for (int shift = 24; shift >= 0; shift -= 8) out.push_back(static_cast<char>((value >> shift) & 0xFF));
}

void append_chunk(std::string& out, const char* type, std::string_view data) {
    append_u32(out, static_cast<std::uint32_t>(data.size()));
    const std::size_t start = out.size();
    out.append(type, 4);
    out.append(data);
    append_u32(out, static_cast<std::uint32_t>(
        crc32(0, reinterpret_cast<const Bytef*>(out.data() + start), static_cast<uInt>(out.size() - start))));
}

// Turns glyph coverage into a palette PNG: the glyphs, a glow around them,
// and the background.
std::string encode_png(const std::vector<unsigned char>& ink, int width, int height, const Look& look) {
    std::vector<unsigned char> glow;
    if (look.glow) {
        glow = ink;
        for (int pass = 0; pass < 3; ++pass) box_blur(glow, width, height, glow_radius);
    }

    // Each row starts with a filter byte; 0 (none) suits palette images best.
    std::string rows(static_cast<std::size_t>(width + 1) * height, '\0');
    for (int y = 0; y < height; ++y) {
        char* row = rows.data() + static_cast<std::size_t>(y) * (width + 1) + 1;
        for (int x = 0; x < width; ++x) {
            const std::size_t i = static_cast<std::size_t>(y) * width + x;
            const int cover = (ink[i] * (cover_levels - 1) + 127) / 255;
            const float halo = glow.empty() ? 0 : std::min(1.0f, glow[i] / 255.0f * glow_boost);
            const int lit = static_cast<int>(std::lround(halo * (glow_levels - 1)));
            row[x] = static_cast<char>(cover * glow_levels + lit);
        }
    }
    glow = {};

    uLongf packed_size = compressBound(static_cast<uLong>(rows.size()));
    std::string packed(packed_size, '\0');
    if (compress2(reinterpret_cast<Bytef*>(packed.data()), &packed_size, reinterpret_cast<const Bytef*>(rows.data()),
                  static_cast<uLong>(rows.size()), Z_DEFAULT_COMPRESSION) != Z_OK) {
        std::cerr << "Failed to compress PNG\n";
        return {};
    }
    packed.resize(packed_size);

    std::string header;
    append_u32(header, static_cast<std::uint32_t>(width));
    append_u32(header, static_cast<std::uint32_t>(height));
    header += std::string_view("\x08\x03\x00\x00\x00", 5);  // 8-bit palette, default compression, no interlace

    std::string png = "\x89PNG\r\n\x1a\n";
    const Palette palette = make_palette(look);
    append_chunk(png, "IHDR", header);
    append_chunk(png, "PLTE", palette.colors);
    if (!palette.opacity.empty()) append_chunk(png, "tRNS", palette.opacity);
    append_chunk(png, "IDAT", packed);
    append_chunk(png, "IEND", {});
    return png;
}

}  // namespace

bool load_fonts() {
    return registry().ok;
}

bool has_font(std::string_view font) {
    return registry().fonts.contains(font);
}

std::string render_png(std::string_view utf8, std::string_view font_name, const Look& look) {
    const auto found = registry().fonts.find(font_name);
    if (found == registry().fonts.end()) {
        std::cerr << "Unknown font " << font_name << '\n';
        return {};
    }
    const Font& font = found->second;
    const float scale = font.scale;

    // Each newline starts a new paragraph; each paragraph wraps on its own.
    std::vector<std::vector<int>> lines;
    std::vector<Item> paragraph;
    const std::u32string text = decode_utf8(utf8);
    for (std::size_t i = 0; i <= text.size(); ++i) {
        if (i == text.size() || text[i] == '\n') {
            wrap(font, paragraph, lines);
            paragraph.clear();
        } else {
            append_glyphs(font, text[i], paragraph);
        }
    }

    // Place glyphs along each line with the font's kerning, and find the
    // horizontal extent of the ink so the margins come out even.
    std::vector<std::vector<Placed>> placed(lines.size());
    int ink_left = INT_MAX, ink_right = INT_MIN;
    for (std::size_t l = 0; l < lines.size(); ++l) {
        float x = 0;
        int previous = -1;
        for (int glyph : lines[l]) {
            if (previous >= 0) x += stbtt_GetGlyphKernAdvance(&font.info, previous, glyph) * scale;
            placed[l].push_back({glyph, x});

            int x0, y0, x1, y1;
            const float whole = std::floor(x);
            stbtt_GetGlyphBitmapBoxSubpixel(&font.info, glyph, scale, scale, x - whole, 0, &x0, &y0, &x1, &y1);
            if (x1 > x0 && y1 > y0) {
                ink_left = std::min(ink_left, static_cast<int>(whole) + x0);
                ink_right = std::max(ink_right, static_cast<int>(whole) + x1);
            }

            x += advance(font, glyph);
            previous = glyph;
        }
    }
    if (ink_left > ink_right) return {};  // only spaces

    const int line_height = font.line_height;
    const int width = ink_right - ink_left + 2 * padding;
    const int height = static_cast<int>(lines.size() - 1) * line_height + font.line_box + 2 * padding;
    if (static_cast<std::size_t>(width) * height > max_pixels) {
        std::cerr << "Refusing to render a " << width << "x" << height << " image\n";
        return {};
    }

    // Draw coverage (0 = background, 255 = glyph), keeping the stronger value
    // where glyphs overlap so one glyph's box never erases its neighbor.
    std::vector<unsigned char> ink(static_cast<std::size_t>(width) * height, 0);
    std::vector<unsigned char> bitmap;
    for (std::size_t l = 0; l < placed.size(); ++l) {
        const int baseline = padding + static_cast<int>(l) * line_height + font.ascent;
        for (const Placed& p : placed[l]) {
            const float whole = std::floor(p.x);
            const float shift = p.x - whole;
            int x0, y0, x1, y1;
            stbtt_GetGlyphBitmapBoxSubpixel(&font.info, p.glyph, scale, scale, shift, 0, &x0, &y0, &x1, &y1);
            const int w = x1 - x0, h = y1 - y0;
            if (w <= 0 || h <= 0) continue;

            bitmap.assign(static_cast<std::size_t>(w) * h, 0);
            stbtt_MakeGlyphBitmapSubpixel(&font.info, bitmap.data(), w, h, w, scale, scale, shift, 0, p.glyph);

            const int left = padding - ink_left + static_cast<int>(whole) + x0;
            const int top = baseline + y0;
            for (int by = 0; by < h; ++by) {
                const int y = top + by;
                if (y < 0 || y >= height) continue;
                for (int bx = 0; bx < w; ++bx) {
                    const int x = left + bx;
                    if (x < 0 || x >= width) continue;
                    unsigned char& dst = ink[static_cast<std::size_t>(y) * width + x];
                    dst = std::max(dst, bitmap[static_cast<std::size_t>(by) * w + bx]);
                }
            }
        }
    }
    return encode_png(ink, width, height, look);
}

}  // namespace aurebesh

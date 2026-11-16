// render_aurebesh.hpp
//
// Draws text in one of the embedded fonts and returns it as a PNG. This part
// has no D++ dependency, so `AurebeshDroid --self-test` can exercise it
// without connecting to Discord.

#pragma once

#include <cstdint>
#include <string>
#include <string_view>

#include "theme.hpp"

namespace aurebesh {

// Parses every embedded font. Call once at startup. Returns false, after
// logging which font failed, if any font cannot be read.
bool load_fonts();

// True if the named font (such as "AurebeshBasic.otf") is embedded.
bool has_font(std::string_view font);

// How a translation looks. Colors are 0xRRGGBB.
struct Look {
    std::uint32_t color = theme::blue;       // glyphs and their glow
    std::uint32_t background = theme::navy;  // ignored when transparent
    bool transparent = false;
    bool glow = true;
};

// Draws UTF-8 text in the named font, wrapping long lines at spaces. Returns
// the bytes of a PNG file, or an empty string when there is nothing to draw
// (blank text) or the font is unknown.
std::string render_png(std::string_view utf8, std::string_view font, const Look& look);

}

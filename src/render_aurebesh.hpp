// render_aurebesh.hpp

#pragma once

#define STB_TRUETYPE_IMPLEMENTATION
#include "font_image/stb_truetype.h"

#define STBI_NO_HDR
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "font_image/stb_image_write.h"

#include <iostream>
#include <fstream>
#include <vector>
#include <cctype>
#include <cstring>
#include <filesystem>
#include <unistd.h>
#include <limits.h>

namespace fs = std::filesystem;
using namespace std;

inline fs::path exe_dir() {
    char buf[PATH_MAX]{};
    ssize_t n = ::readlink("/proc/self/exe", buf, sizeof(buf)-1);
    return(n > 0) ? fs::path(buf).parent_path() : fs::current_path();
}

inline bool is_regular(const fs::path& p) {
    std::error_code ec;
    return fs::exists(p, ec) && fs::is_regular_file(p, ec);
}

inline fs::path try_font(const fs::path& base, const std::string& name) {
    fs::path p = base / name;
    if(is_regular(p)) return p;
    if(!fs::path(name).has_extension()) {
        for(const char* ext : {".otf", ".ttf"}) {
            fs::path q = base /(name + ext);
            if(is_regular(q)) return q;
        }
    }
    return {};
}

inline fs::path find_font(const std::string& fontName) {
    for(const fs::path base : {
            exe_dir() / "assets" / "fonts",
            fs::current_path() / "src" / "assets" / "fonts",
            fs::current_path() / "assets" / "fonts"
        })
    {
        if(auto hit = try_font(base, fontName); !hit.empty()) {
            std::cout << "🔎 " << hit << '\n';
            return hit;
        } else {
            std::cout << "🔎 " <<(base / fontName) << "(not found)\n";
        }
    }
    throw std::runtime_error("Font not found: " + fontName);
}

inline bool renderTextToImage(
    const char* inputText, string& outPath,
    const string& imageName,
    const string& fontName,
    int fontSize = 96
) {
    fs::path fontPath;
    try {
        fontPath = find_font(fontName);
    } catch(const std::exception& e) {
        std::cerr << "❌ " << e.what() << '\n';
        return false;
    }

    fs::path outputPath = fs::temp_directory_path() / imageName;
    outPath = outputPath.string();

    int maxLineWidth = 2000;
    int padding = 40;

    ifstream fontFile(fontPath, ios::binary | ios::ate);
    if(!fontFile) {
        cerr << "❌ Failed to open font file: " << fontPath << '\n';
        return false;
    }

    streamsize fontSizeBytes = fontFile.tellg();
    fontFile.seekg(0, ios::beg);
    vector<unsigned char> fontBuffer(fontSizeBytes);
    fontFile.read(reinterpret_cast<char*>(fontBuffer.data()), fontSizeBytes);

    stbtt_fontinfo font;
    if(!stbtt_InitFont(&font, fontBuffer.data(), 0)) {
        cerr << "❌ Failed to initialize font.\n";
        return false;
    }

    float scale = stbtt_ScaleForPixelHeight(&font, fontSize);
    int ascent, descent, lineGap;
    stbtt_GetFontVMetrics(&font, &ascent, &descent, &lineGap);
    int lineHeight = static_cast<int>((ascent - descent + lineGap) * scale);

    vector<string> lines;
    string currentLine;
    int currentLineWidth = 0;
    int actualMaxLineWidth = 0;

    for(const char* p = inputText; *p; ++p) {
        char ch = tolower(*p);
        int ax, lsb;
        stbtt_GetCodepointHMetrics(&font, ch, &ax, &lsb);
        int advance = static_cast<int>(ax * scale);

        if(currentLineWidth + advance > maxLineWidth && !currentLine.empty()) {
            lines.push_back(currentLine);
            actualMaxLineWidth = max(actualMaxLineWidth, currentLineWidth);
            currentLine.clear();
            currentLineWidth = 0;
        }

        currentLine += ch;
        currentLineWidth += advance;
    }

    if(!currentLine.empty()) {
        lines.push_back(currentLine);
        actualMaxLineWidth = max(actualMaxLineWidth, currentLineWidth);
    }

    int imageWidth = actualMaxLineWidth + 2 * padding;
    int imageHeight = static_cast<int>(lines.size()) * lineHeight + 2 * padding;
    vector<unsigned char> image(imageWidth * imageHeight, 255);

    for(size_t i = 0; i < lines.size(); ++i) {
        int x = padding;
        const string& line = lines[i];

        for(char ch : line) {
            ch = tolower(ch);
            int ax, lsb;
            stbtt_GetCodepointHMetrics(&font, ch, &ax, &lsb);

            int cx1, cy1, cx2, cy2;
            stbtt_GetCodepointBitmapBox(&font, ch, scale, scale, &cx1, &cy1, &cx2, &cy2);

            int y = padding + static_cast<int>(i) * lineHeight +(lineHeight -(cy2 - cy1)) / 2;

            vector<unsigned char> charBitmap((cx2 - cx1) *(cy2 - cy1));
            stbtt_MakeCodepointBitmap(&font, charBitmap.data(), cx2 - cx1, cy2 - cy1, cx2 - cx1, scale, scale, ch);

            for(int by = 0; by < cy2 - cy1; ++by) {
                for(int bx = 0; bx < cx2 - cx1; ++bx) {
                    int dst_x = x + bx;
                    int dst_y = y + by;
                    if(dst_x < 0 || dst_x >= imageWidth || dst_y < 0 || dst_y >= imageHeight) continue;
                    image[dst_y * imageWidth + dst_x] = 255 - charBitmap[by *(cx2 - cx1) + bx];
                }
            }

            x += static_cast<int>(ax * scale);
        }
    }

    if(!stbi_write_png(outPath.c_str(), imageWidth, imageHeight, 1, image.data(), imageWidth)) {
        cerr << "❌ Failed to save image.\n";
        return false;
    }

    cout << "✅ Rendered image saved to " << outputPath << '\n';
    return true;
}
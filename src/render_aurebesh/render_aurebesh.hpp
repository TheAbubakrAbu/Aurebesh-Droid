// render_aurebesh.hpp

#pragma once

#define STB_TRUETYPE_IMPLEMENTATION
#include "stb_truetype.h"

#define STBI_NO_HDR
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"

#include <iostream>
#include <fstream>
#include <vector>
#include <cctype>
#include <cstring>
#include <filesystem>
#include <unistd.h>
#include <limits.h>

#ifdef __APPLE__
  #include <mach-o/dyld.h>
#endif

#ifdef _WIN32
  #include <windows.h>
#endif

namespace fs = std::filesystem;
using namespace std;

inline fs::path exe_dir() {
#if defined(_WIN32)           // Windows
    wchar_t buf[MAX_PATH]{};
    DWORD n = GetModuleFileNameW(nullptr, buf, MAX_PATH);
    return n ? fs::path(buf).parent_path() : fs::current_path();

#elif defined(__APPLE__)      // macOS
    char buf[PATH_MAX]{};
    uint32_t sz = sizeof(buf);
    if (_NSGetExecutablePath(buf, &sz) == 0)
        return fs::path(buf).parent_path();
    else
        return fs::current_path();

#else                          // Linux
    char buf[PATH_MAX]{};
    ssize_t n = ::readlink("/proc/self/exe", buf, sizeof(buf) - 1);
    return n > 0 ? fs::path(buf).parent_path() : fs::current_path();
#endif
}

inline fs::path find_font(const std::string& fontName)
{
    auto exe = exe_dir();

    static const fs::path SOURCE_SRC_DIR =
        fs::path(__FILE__).parent_path().parent_path();

    const fs::path candidates[] = {
        exe / "assets" / "fonts" / fontName,                      // /app/assets/…
        exe / "src" / "assets" / "fonts" / fontName,              // /app/src/…
        exe.parent_path() / "src" / "assets" / "fonts" / fontName,// in-tree build
        SOURCE_SRC_DIR / "assets" / "fonts" / fontName            // out-of-tree build
    };

    for (const auto& p : candidates) {
        std::cout << "🔎 " << p << '\n';
        if (fs::exists(p)) return p;
    }
    throw std::runtime_error("Font " + fontName + " not found");
}

inline bool renderTextToImage(const char* inputText, string& outPath, const string& imageName, const string& fontName, int fontSize = 96) {
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
            if(currentLineWidth > actualMaxLineWidth)
                actualMaxLineWidth = currentLineWidth;

            currentLine.clear();
            currentLineWidth = 0;
        }

        currentLine += ch;
        currentLineWidth += advance;
    }

    if(!currentLine.empty()) {
        lines.push_back(currentLine);
        if(currentLineWidth > actualMaxLineWidth)
            actualMaxLineWidth = currentLineWidth;
    }

    int imageWidth = actualMaxLineWidth + 2 * padding;
    int imageHeight = static_cast<int>(lines.size()) * lineHeight + 2 * padding;
    vector<unsigned char> image(imageWidth * imageHeight, 255);

    for(int i = 0; i < lines.size(); ++i) {
        int x = padding;
        const string& line = lines[i];

        for(char ch : line) {
            ch = tolower(ch);
            int ax, lsb;
            stbtt_GetCodepointHMetrics(&font, ch, &ax, &lsb);

            int cx1, cy1, cx2, cy2;
            stbtt_GetCodepointBitmapBox(&font, ch, scale, scale, &cx1, &cy1, &cx2, &cy2);

            int y = padding + i * lineHeight +(lineHeight -(cy2 - cy1)) / 2;

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
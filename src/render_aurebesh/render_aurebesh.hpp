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
#include <filesystem>

using namespace std;

inline bool renderTextToImage(const char* inputText, std::string& outPath, const std::string& imageName, const std::string& fontName, int fontSize = 96) {
    namespace fs = std::filesystem;

    fs::path fontPath = fs::path(__FILE__).parent_path().parent_path() / "assets" / "fonts" / fontName;
    if (!fs::exists(fontPath)) {
        fontPath = fs::current_path() / "assets" / "fonts" / fontName;
    }
    
    fs::path outputPath = fs::temp_directory_path() / imageName;
    outPath = outputPath.string();

    int maxLineWidth = 2000;
    int padding = 40;

    std::ifstream fontFile(fontPath, std::ios::binary | std::ios::ate);
    if(!fontFile) {
        std::cerr << "❌ Failed to open font file: " << fontPath << '\n';
        return false;
    }

    std::streamsize fontSizeBytes = fontFile.tellg();
    fontFile.seekg(0, std::ios::beg);
    std::vector<unsigned char> fontBuffer(fontSizeBytes);
    fontFile.read(reinterpret_cast<char*>(fontBuffer.data()), fontSizeBytes);

    stbtt_fontinfo font;
    if(!stbtt_InitFont(&font, fontBuffer.data(), 0)) {
        std::cerr << "❌ Failed to initialize font.\n";
        return false;
    }

    float scale = stbtt_ScaleForPixelHeight(&font, fontSize);
    int ascent, descent, lineGap;
    stbtt_GetFontVMetrics(&font, &ascent, &descent, &lineGap);
    int lineHeight = static_cast<int>((ascent - descent + lineGap) * scale);

    std::vector<std::string> lines;
    std::string currentLine;
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
    std::vector<unsigned char> image(imageWidth * imageHeight, 255);

    for(int i = 0; i < lines.size(); ++i) {
        int x = padding;
        const std::string& line = lines[i];

        for(char ch : line) {
            ch = tolower(ch);
            int ax, lsb;
            stbtt_GetCodepointHMetrics(&font, ch, &ax, &lsb);

            int cx1, cy1, cx2, cy2;
            stbtt_GetCodepointBitmapBox(&font, ch, scale, scale, &cx1, &cy1, &cx2, &cy2);

            int y = padding + i * lineHeight +(lineHeight -(cy2 - cy1)) / 2;

            std::vector<unsigned char> charBitmap((cx2 - cx1) *(cy2 - cy1));
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
        std::cerr << "❌ Failed to save image.\n";
        return false;
    }

    std::cout << "✅ Rendered image saved to " << outputPath << '\n';
    return true;
}
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
namespace fs = filesystem;

inline bool renderTextToImage(const char* inputText, string& outPath, const string& imageName, const string& fontFile, int fontSize = 96, int imageWidth = 800, int imageHeight = 200) {
    fs::path fontPath = fs::path(__FILE__).parent_path().parent_path() / "assets" / "fonts" / fontFile;
    fs::path outputPath = fs::path(__FILE__).parent_path().parent_path() / imageName;
    outPath = outputPath.string();

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
    ascent *= scale;

    vector<unsigned char> image(imageWidth * imageHeight, 255);
    int x = 50;

    for(const char* p = inputText; *p; ++p) {
        char ch = tolower(*p);

        int ax, lsb;
        stbtt_GetCodepointHMetrics(&font, ch, &ax, &lsb);

        int cx1, cy1, cx2, cy2;
        stbtt_GetCodepointBitmapBox(&font, ch, scale, scale, &cx1, &cy1, &cx2, &cy2);

        int y = static_cast<int>(ascent - cy2);

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

    if (!stbi_write_png(outPath.c_str(), imageWidth, imageHeight, 1, image.data(), imageWidth)) {
        cerr << "❌ Failed to save image.\n";
        return false;
    }

    cout << "✅ Rendered image saved to " << outputPath << '\n';
    return true;
}
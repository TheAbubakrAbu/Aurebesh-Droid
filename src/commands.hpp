#pragma once
#include <string>
#include "aurebesh.hpp"

inline std::string getAurebeshList() {
    std::string text;
    for (int i = 0; i < aurebeshSize; ++i) {
        const AurebeshLetter& letter = aurebeshLetters[i];
        text += "`" + letter.letter + "` → **" + letter.aurebeshName + "**\n";
    }
    return text;
}

inline std::string getAurebeshHelp() {
    return
        "### **Welcome to Aurebesh Droid**\n"
        "This bot translates Galactic Basic (English) into the **Aurebesh script** used throughout the Star Wars galaxy.\n\n"

        "### **Commands**\n"
        "• `/alphabet` – View the complete Aurebesh alphabet with English equivalents.\n"
        "• `/translate <text>` – Convert up to 50 English characters into Aurebesh and receive a rendered image.\n"
        "• `/help` – Show this help message.\n\n"

        "### **Features**\n"
        "• Uses a custom Aurebesh font rendered in real-time.\n"
        "• Case-insensitive input (A = a).\n\n"

        "### **Related Star Wars Apps**\n"
        "• **[Datapad | Aurebesh Translator](https://apps.apple.com/us/app/datapad-aurebesh-translator/id6450498054?platform=iphone)** – A feature-rich, immersive translator with keyboard support.\n"
        "• **[Aurebesh Translator](https://apps.apple.com/us/app/aurebesh-translator/id6670201513?platform=iphone)** – A lightweight, offline, distraction-free translator.\n\n"

        "### **Credits**\n"
        "• **Font:** Aurebesh.otf by [Pixel Sagas](https://www.fonts4free.net/aurebesh-font.html)\n"
        "• **Rendering Engine:** stb_truetype & stb_image_write by [Sean Barrett](https://github.com/nothings/stb)\n"
        "• **Bot Framework:** D++ (DPP) C++ Library by [brainbox](https://github.com/brainboxdotcc/DPP)\n\n"
        "• All other creative content is fan-made and not affiliated with or endorsed by Lucasfilm/Disney.\n\n"

        "Created by **[Abubakr Elmallah](https://abubakrelmallah.com)**\n\n"
        "[📂 GitHub Repository](https://github.com/TheAbubakrAbu/Aurebesh-Droid)\n\n"
        "May the Force be with you!";
}
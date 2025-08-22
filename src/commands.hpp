// commands.hpp

#pragma once

#include <iostream>
#include <random>

#include <dpp/dpp.h>

#include "aurebesh.hpp"
#include "holocron.hpp"
#include "render_aurebesh.hpp"

using namespace std;

const string thumbnail = "https://raw.githubusercontent.com/TheAbubakrAbu/Aurebesh-Droid/main/src/assets/images/aurebesh.png";
const auto color = 0x4295E2;

inline std::string to_alphabet_image_url(const std::string& name) {
    std::string out;
    for(char c : name) {
        if(std::isalnum(static_cast<unsigned char>(c)) || c == ' ')
            out.push_back(std::tolower(static_cast<unsigned char>(c)));
    }
    std::string url;
    for(char c : out) {
        if(c == ' ') url += "%20";
        else url += c;
    }
    return "https://raw.githubusercontent.com/TheAbubakrAbu/Aurebesh-Droid/main/src/assets/images/"  + url + "%20alphabet.jpg";
}

inline void alphabetCommand(const dpp::slashcommand_t& event, const std::string& name = "Aurebesh Basic") {
    std::string description;
    for(int i = 0; i < aurebeshSize; ++i) {
        const AurebeshLetter& letter = aurebeshLetters[i];
        description += "`" + letter.letter + "` → **" + letter.aurebeshName + "**\n";
    }

    dpp::embed embed = dpp::embed()
        .set_title(name + " Alphabet")
        .set_description(description)
        .set_color(color)
        .set_thumbnail(thumbnail)
        .set_image(to_alphabet_image_url(name));

    event.reply(dpp::message().add_embed(embed));
}

inline void helpCommand(const dpp::slashcommand_t& event) {
    string description =(
        "### **Welcome to Aurebesh Droid**\n"
        "This bot translates Galactic Basic(English) into the **Aurebesh script** used throughout the Star Wars galaxy.\n\n"

        "### **Commands**\n"
        "• `/alphabet` – View the complete Aurebesh alphabet with English equivalents.\n"
        "• `/translate <text>` – Convert English into Aurebesh and receive a rendered image.\n"
        "• `/help` – Show this help message.\n\n"

        "### **Features**\n"
        "• Uses a custom Aurebesh font rendered in real-time.\n"
        "• Case-insensitive input(A = a).\n\n"

        "### **Related Star Wars Apps**\n"
        "• **[Datapad | Aurebesh Translator](https://apps.apple.com/us/app/datapad-aurebesh-translator/id6450498054?platform=iphone)** – A feature-rich, immersive translator with keyboard support.\n"
        "• **[Aurebesh Translator](https://apps.apple.com/us/app/aurebesh-translator/id6670201513?platform=iphone)** – A lightweight, offline, distraction-free translator.\n\n"

        "### **Credits**\n"
        "• **Fonts:**\n"
        "    • [Standard Aurebesh](https://www.fonts4free.net/aurebesh-font.html) by Pixel Sagas\n"
        "    • [Cantina Aurebesh](https://www.fontspace.com/aurebesh-cantina-font-f22020) by Pixel Sagas\n"
        "    • [Droid Aurebesh](https://www.fontspace.com/droidobesh-depot-font-f55049) by AurekFonts\n"
        "    • [Mando'a Fonts](http://www.erikstormtrooper.com/mandalorian.htm) by Erikstormtrooper\n"
        "    • [Outer Rim](https://www.fontspace.com/outer-rim-af-font-f49641) by AurekFonts\n"
        "    • [Sith Font](https://www.fontspace.com/sith-af-font-f49639) by AurekFonts\n"
        "\n"
        "• **Rendering Engine:** stb_truetype & stb_image_write by [Sean Barrett](https://github.com/nothings/stb)\n"
        "• **Bot Framework:** D++(DPP) C++ Library by [brainbox](https://github.com/brainboxdotcc/DPP)\n\n"
        "• All other creative content is fan-made and not affiliated with or endorsed by Lucasfilm/Disney.\n\n"

        "Created by **[Abubakr Elmallah](https://abubakrelmallah.com)**\n\n"
        "[📂 GitHub Repository](https://github.com/TheAbubakrAbu/Aurebesh-Droid)\n\n"
        "May the Force be with you!"
    );

    dpp::embed embed = dpp::embed()
        .set_title("Aurebesh Droid Help")
        .set_description(description)
        .set_color(color)
        .set_thumbnail(thumbnail);

    event.reply(dpp::message().add_embed(embed));
}

inline void translateCommand(
    const dpp::slashcommand_t& event,
    const std::string& name = "Aurebesh Basic",
    const std::string& font = "AurebeshBasic.otf"
) {
    const string input = get<string>(event.get_parameter("text"));
    if(input.empty()) {
        event.reply("❌ Please enter at least one character.");
        return;
    }

    event.thinking();

    thread([event, input, name, font]() mutable {
        ostringstream oss;
        oss << "aurebesh_" << event.command.usr.id << '_' << event.command.channel_id << '_'
            << chrono::duration_cast<chrono::milliseconds>(chrono::system_clock::now().time_since_epoch()).count()
            << ".png";

        const string imgName = oss.str();

        string imgPath;
        if(!renderTextToImage(input.c_str(), imgPath, imgName, font)) {
            event.edit_response("❌ Failed to render Aurebesh image.");
            return;
        }

        const string description = "<@" + to_string(event.command.usr.id) + "> said:\n" + input;

        dpp::embed embed = dpp::embed()
            .set_title(name + " Translation")
            .set_description(description)
            .set_color(color)
            .set_thumbnail(thumbnail)
            .set_image("attachment://" + imgName);

        dpp::message msg;
        msg.add_embed(embed).add_file(imgName, dpp::utility::read_file(imgPath));

        event.edit_response(msg);
        remove(imgPath.c_str());
    }).detach();
}

inline void holocronCommand(const dpp::slashcommand_t& event, string command) {
    vector<pair<string, string>> quotes =(contains(command, "sith")) ? sith_quotes : jedi_quotes;

    if(command == "holocron") quotes.insert(quotes.end(), sith_quotes.begin(), sith_quotes.end());

    quotes.insert(quotes.end(), {
        { "Bendu", "I am the Bendu, the one in the middle. Between the light and the dark." },
        { "Bendu", "An object cannot make you good or evil. The temptation of power is always there, but those who are strong enough to control it are the ones who truly have the power." },
        { "Bendu", "You think the only way to gain power is by embracing the dark side. Wrong." },
        { "Bendu", "You do not comprehend what it is I am. And what I can do!" },
        { "Bendu", "I am beyond your understanding." }
    });

    random_device rd;
    mt19937 gen(rd());
    uniform_int_distribution<> dis(0, quotes.size() - 1);
    auto& [author, quote] = quotes[dis(gen)];

    HolocronInfo meta = get_holocron_metadata(author, command);

    dpp::embed embed = dpp::embed()
        .set_title(meta.title)
        .set_description("\"" + quote + "\"\n\n— **" + author + " " + meta.book + "**")
        .set_color(meta.color)
        .set_thumbnail(meta.image);

    event.reply(dpp::message().add_embed(embed));
}
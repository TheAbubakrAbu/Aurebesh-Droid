// help.hpp
//
// /help: what the bot does, every command, and the credits.

#pragma once

#include <string>
#include <string_view>

#include <dpp/dpp.h>

#include "scripts.hpp"
#include "theme.hpp"

// The script list comes from scripts.hpp, so new scripts show up on their own.
inline dpp::message help_message() {
    using theme::icon::alphabet, theme::icon::customize, theme::icon::help, theme::icon::jedi, theme::icon::quiz;

    std::string script_list;
    std::string_view group;
    std::size_t script_count = 0;
    for (const Script& script : scripts) {
        if (script.group.empty()) continue;
        ++script_count;
        if (script.group != group) {
            group = script.group;
            script_list += "\n**" + std::string(group) + ":** ";
        } else {
            script_list += " ";
        }
        script_list += "`/" + std::string(script.command) + "`";
    }

    const std::string description =
        "Translate Galactic Basic (English) into " + std::to_string(script_count) +
        " scripts from across the Star Wars galaxy, from **Aurebesh** to **Mando'a**. Each one is drawn live in its own font.";

    const std::string more_commands =
        std::string(alphabet) + " `/alphabet` Show the Aurebesh alphabet\n" +
        std::string(jedi) + " `/holocron` Unlock a Jedi or Sith holocron (or choose with `/holocron_jedi` or `/holocron_sith`)\n" +
        std::string(quiz) + " `/quiz` Read a Star Wars word written in Aurebesh\n" +
        std::string(help) + " `/help` Show how to use Aurebesh Droid";

    dpp::message message;
    message.add_embed(dpp::embed()
        .set_title(theme::titled(help, "Aurebesh Droid"))
        .set_description(description)
        .set_color(theme::blue)
        .set_thumbnail(theme::logo)
        .add_field(theme::titled(theme::icon::translate, "Translate"),
            "`/translate <text>` writes English in Basic Aurebesh. Choose another script with:" + script_list + "\n"
            "Capital letters are drawn the same as lowercase (A = a).\n"
            "Tap **" + theme::titled(customize, "Customize") + "** under a translation to change its script, color, background, and glow.\n"
            "Right-click any message (or long-press on mobile) and choose **Apps → Translate to Aurebesh**.")
        .add_field("🧭 More Commands", more_commands)
        // The same ending as Sabacc Droid's /help (see theme.hpp).
        .add_field("✒️ Fonts",
            "[Basic](https://www.fonts4free.net/aurebesh-font.html) and "
            "[Cantina](https://www.fontspace.com/aurebesh-cantina-font-f22020) Aurebesh by Pixel Sagas\n"
            "[Core](https://aurekfonts.github.io/?font=AurebeshAF), "
            "[Nexus](https://aurekfonts.github.io/?font=ABEquinox), "
            "[Droid](https://www.fontspace.com/droidobesh-depot-font-f55049), and "
            "[Pixel](https://aurekfonts.github.io/?font=AurebeshLU) Aurebesh by AurekFonts\n"
            "[Mando'a](http://www.erikstormtrooper.com/mandalorian.htm) by Erikstormtrooper\n"
            "[Outer Rim](https://www.fontspace.com/outer-rim-af-font-f49641), "
            "[Old Tongue](https://aurekfonts.github.io/?font=PrimeJedi), "
            "[Sith Script](https://aurekfonts.github.io/?font=SithAF), "
            "[Geonosian](https://aurekfonts.github.io/?font=GeonosianPM), "
            "[Trade Federation](https://aurekfonts.github.io/?font=TFGunray), and "
            "[Protobesh](https://aurekfonts.github.io/?font=ProtobeshAF) by AurekFonts")
        .add_field("🌌 More Star Wars",
            "- **[Sabacc Droid](" + std::string(theme::sabacc_droid_link) + ")**: Play Sabacc, the Star Wars card game, with up to 8 players.\n" +
            theme::star_wars_apps)
        .add_field("🛠️ Made By",
            "Created by **" + std::string(theme::creator) + "** with [D++](https://github.com/brainboxdotcc/DPP) and "
            "[stb_truetype](https://github.com/nothings/stb).")
        .set_footer(dpp::embed_footer().set_text(std::string(theme::disclaimer))));

    message.add_component(dpp::component()
        .add_component(dpp::component().set_label("Add Aurebesh Droid").set_emoji(std::string(theme::icon::add_bot)).set_url(std::string(theme::add_bot_link)))
        .add_component(dpp::component().set_label("Datapad App").set_emoji(std::string(theme::icon::app)).set_url(std::string(theme::datapad_app)))
        .add_component(dpp::component().set_label("GitHub").set_emoji(std::string(theme::icon::github)).set_url(std::string(theme::github_link))));
    return message;
}

inline void help_command(const dpp::slashcommand_t& event) {
    static const dpp::message reply = help_message();  // never changes
    event.reply(reply);
}

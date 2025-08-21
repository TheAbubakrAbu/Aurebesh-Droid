// main.cpp

#include <cstdlib>
#include <iostream>
#include <unordered_map>

#include <dpp/dpp.h>

#include "commands.hpp"

using namespace std;

int main() {
    const char* token = getenv("DISCORD_TOKEN");
    if(!token) {
        cerr << "DISCORD_TOKEN environment variable not set\n";
        return 1;
    }

    dpp::cluster bot(token, dpp::i_default_intents);

    bot.on_log(dpp::utility::cout_logger());

    bot.on_ready([&bot](const dpp::ready_t& event) {
        dpp::activity playing(dpp::activity_type::at_game, "💠 Jedi Holocron", "", "");
        bot.set_presence(dpp::presence(dpp::ps_online, playing));

        cout << "Bot is up: " << bot.me.username << '\n';

        if(dpp::run_once<struct register_commands>()) {
            std::vector<dpp::slashcommand> commands;

            // --- Translation commands ---
            commands.push_back(
                dpp::slashcommand("translate", "Convert English text to Basic Aurebesh", bot.me.id)
                    .add_option(dpp::command_option(dpp::co_string, "text", "Text to translate", true))
            );

            commands.push_back(
                dpp::slashcommand("translate_basic", "Convert English text to Basic Aurebesh", bot.me.id)
                    .add_option(dpp::command_option(dpp::co_string, "text", "Text to translate", true))
            );

            commands.push_back(
                dpp::slashcommand("translate_core", "Convert English text to Core Aurebesh", bot.me.id)
                    .add_option(dpp::command_option(dpp::co_string, "text", "Text to translate", true))
            );

            commands.push_back(
                dpp::slashcommand("translate_cantina", "Convert English text to Cantina Aurebesh", bot.me.id)
                    .add_option(dpp::command_option(dpp::co_string, "text", "Text to translate", true))
            );

            commands.push_back(
                dpp::slashcommand("translate_nexus", "Convert English text to Nexus Aurebesh", bot.me.id)
                    .add_option(dpp::command_option(dpp::co_string, "text", "Text to translate", true))
            );

            commands.push_back(
                dpp::slashcommand("translate_droid", "Convert English text to Droid Aurebesh", bot.me.id)
                    .add_option(dpp::command_option(dpp::co_string, "text", "Text to translate", true))
            );

            commands.push_back(
                dpp::slashcommand("translate_pixel", "Convert English text to Pixel Aurebesh", bot.me.id)
                    .add_option(dpp::command_option(dpp::co_string, "text", "Text to translate", true))
            );

            commands.push_back(
                dpp::slashcommand("translate_mandoa_new", "Convert English text to New Mando'a", bot.me.id)
                    .add_option(dpp::command_option(dpp::co_string, "text", "Text to translate", true))
            );

            commands.push_back(
                dpp::slashcommand("translate_mandoa_old", "Convert English text to Old Mando'a", bot.me.id)
                    .add_option(dpp::command_option(dpp::co_string, "text", "Text to translate", true))
            );

            commands.push_back(
                dpp::slashcommand("translate_outerrim", "Convert English text to Outer Rim", bot.me.id)
                    .add_option(dpp::command_option(dpp::co_string, "text", "Text to translate", true))
            );

            commands.push_back(
                dpp::slashcommand("translate_tongue", "Convert English text to Outer Rim Sith Tongue", bot.me.id)
                    .add_option(dpp::command_option(dpp::co_string, "text", "Text to translate", true))
            );

            commands.push_back(
                dpp::slashcommand("translate_geonosian", "Convert English text to Outer Rim Geonosian", bot.me.id)
                    .add_option(dpp::command_option(dpp::co_string, "text", "Text to translate", true))
            );

            commands.push_back(
                dpp::slashcommand("translate_trade", "Convert English text to Outer Rim Trade Federation", bot.me.id)
                    .add_option(dpp::command_option(dpp::co_string, "text", "Text to translate", true))
            );

            commands.push_back(
                dpp::slashcommand("translate_protobesh", "Convert English text to Outer Rim Protobesh", bot.me.id)
                    .add_option(dpp::command_option(dpp::co_string, "text", "Text to translate", true))
            );

            // --- Other commands ---
            commands.push_back(
                dpp::slashcommand("alphabet", "Display the Aurebesh alphabet", bot.me.id)
            );

            commands.push_back(
                dpp::slashcommand("aurebesh", "Display the Aurebesh alphabet", bot.me.id)
            );

            commands.push_back(
                dpp::slashcommand("holocron", "Use the Force and unlock the secrets of the Jedi or the Sith", bot.me.id)
            );

            commands.push_back(
                dpp::slashcommand("holocron_jedi", "Use the Force and unlock the secrets of the Jedi", bot.me.id)
            );

            commands.push_back(
                dpp::slashcommand("holocron_sith", "Use the Force and unlock the secrets of the Sith", bot.me.id)
            );

            commands.push_back(
                dpp::slashcommand("help", "Display Aurebesh Droid Help", bot.me.id)
            );

            bot.global_bulk_command_create(commands);
        }
    });

    bot.on_slashcommand([&bot](const dpp::slashcommand_t& event) {
        const std::string command = event.command.get_command_name();

        static const std::unordered_map<std::string, std::string> fontMap = {
            // Aurebesh
            { "translate_basic", "AurebeshBasic.otf" },
            { "translate_core", "AurebeshCore.otf" },
            { "translate_cantina", "AurebeshCantina.otf" },
            { "translate_nexus", "AurebeshEquinox.otf" },
            { "translate_droid", "AurebeshDroid.otf" },
            { "translate_pixel", "AurebeshPixel.otf" },

            // Mando'a
            { "translate_mandoa_new", "MandoNew.otf" },
            { "translate_mandoa_old", "MandoOld.otf" },

            // Outer Rim
            { "translate_outerrim",  "OuterRimBasic.otf" },
            { "translate_tongue",    "OuterRimTongue.otf" },
            { "translate_geonosian", "OuterRimHive.otf" },
            { "translate_trade",     "OuterRimTrade.otf" },
            { "translate_protobesh", "OuterRimProtobesh.otf" },
        };

        auto it = fontMap.find(command);

        if(command == "alphabet" || command == "aurebesh") {
            alphabetCommand(event);
        } else if(command == "help") {
            helpCommand(event);
        } else if(command == "translate") {
            translateCommand(event);
        } else if (it != fontMap.end()) {
            translateCommand(event, it->second);
        } else if(command.find("holocron") != std::string::npos) {
            holocronCommand(event, command);
        } else {
            //
        }
    });
    
    bot.start(dpp::st_wait);
}
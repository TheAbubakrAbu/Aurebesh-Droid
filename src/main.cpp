// main.cpp

#include <cstdlib>
#include <iostream>

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

        if (dpp::run_once<struct register_commands>()) {
            std::vector<dpp::slashcommand> commands;

            // --- Translation commands ---
            commands.push_back(
                dpp::slashcommand("translate", "Convert English text to Standard Aurebesh", bot.me.id)
                    .add_option(dpp::command_option(dpp::co_string, "text", "Text to translate", true))
            );

            commands.push_back(
                dpp::slashcommand("translate_standard", "Convert English text to Standard Aurebesh", bot.me.id)
                    .add_option(dpp::command_option(dpp::co_string, "text", "Text to translate", true))
            );

            commands.push_back(
                dpp::slashcommand("translate_cantina", "Convert English text to Cantina Aurebesh", bot.me.id)
                    .add_option(dpp::command_option(dpp::co_string, "text", "Text to translate", true))
            );

            commands.push_back(
                dpp::slashcommand("translate_droid", "Convert English text to Droid Aurebesh", bot.me.id)
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
                dpp::slashcommand("translate_sith", "Convert English text to Sith Outer Rim", bot.me.id)
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
        const string command = event.command.get_command_name();
        
        const unordered_map<string, string> fontMap = {
            // Aurebesh
            { "translate standard aurebesh", "AurebeshStandard.ttf" },
            { "translate cantina aurebesh", "AurebeshCantina.ttf" },
            { "translate droid aurebesh", "AurebeshDroid.otf" },

            // Mando'a
            { "translate new mandoa", "MandoNew.ttf" },
            { "translate old mandoa", "MandoOld.ttf" },

            // Outer Rim
            { "translate outer rim", "OuterRimBasic.otf" },
            { "translate sith", "OuterRimSith.ttf" }
        };

        auto it = fontMap.find(command);

        if(command == "alphabet" || command == "aurebesh") {
            alphabetCommand(event);
        } else if(command == "help") {
            helpCommand(event);
        } else if(command == "translate") {
            translateCommand(event);
        } if(it != fontMap.end()) {
            translateCommand(event, it->second);
        } else if(contains(command, "holocron")) {
            holocronCommand(event, command);
        }
    });
    
    bot.start(dpp::st_wait);
}
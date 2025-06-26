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

        if(dpp::run_once<struct register_commands>()) {
            bot.global_command_create(
                dpp::slashcommand("translate", "Convert English text to Aurebesh", bot.me.id)
                    .add_option(dpp::command_option(dpp::co_string, "text", "Text to translate", true))
            );

            bot.global_command_create(
                dpp::slashcommand("alphabet", "Display the Aurebesh alphabet", bot.me.id)
            );

            bot.global_command_create(
                dpp::slashcommand("aurebesh", "Display the Aurebesh alphabet", bot.me.id)
            );
            
            bot.global_command_create(
                dpp::slashcommand("holocron", "Use the Force and unlock the secrets of the Jedi or the Sith", bot.me.id)
            );

            bot.global_command_create(
                dpp::slashcommand("holocron_jedi", "Use the Force and unlock the secrets of the Jedi", bot.me.id)
            );

            bot.global_command_create(
                dpp::slashcommand("holocron_sith", "Use the Force and unlock the secrets of the Sith", bot.me.id)
            );

            bot.global_command_create(
                dpp::slashcommand("help", "Display Aurebesh Droid Help", bot.me.id)
            );
        }
    });

    bot.on_slashcommand([&bot](const dpp::slashcommand_t& event) {
        const string command = event.command.get_command_name();

        if(command == "alphabet" || command == "aurebesh") {
            alphabetCommand(event);
        } else if(command == "help") {
            helpCommand(event);
        } else if (command == "translate") {
            translateCommand(event);
        } else if(contains(command, "holocron")) {
            holocronCommand(event, command);
        }
    });

    bot.start(dpp::st_wait);
}
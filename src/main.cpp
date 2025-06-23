// main.cpp

#include <cstdlib>
#include <iostream>

#include <dpp/dpp.h>

#include "aurebesh.hpp"

using namespace std;

string getAurebeshList() {
    string text;
    for (int i = 0; i < aurebeshSize; ++i) {
        const AurebeshLetter& letter = aurebeshLetters[i];
        text += "`" + letter.letter + "` → **" + letter.aurebeshName + "**\n";
    }
    return text;
}

int main() {
    const char* token = std::getenv("DISCORD_TOKEN");
    if (!token) {
        std::cerr << "DISCORD_TOKEN environment variable not set\n";
        return 1;
    }

    dpp::cluster bot(token, dpp::i_default_intents);

    bot.on_log(dpp::utility::cout_logger());

    bot.on_ready([&bot](const dpp::ready_t& event) {
        dpp::activity playing(dpp::activity_type::at_game, "Aurebesh", "", "");
        bot.set_presence(dpp::presence(dpp::ps_online, playing));

        cout << "Bot is up: " << bot.me.username << '\n';

        if (dpp::run_once<struct register_commands>()) {
            bot.global_command_create(
                dpp::slashcommand("alphabet", "Display the Aurebesh alphabet", bot.me.id)
            );
        }
    });

    bot.on_slashcommand([&bot](const dpp::slashcommand_t& event) {
        if (event.command.get_command_name() == "alphabet") {
            dpp::embed embed = dpp::embed()
                .set_title("Aurebesh Alphabet")
                .set_description(getAurebeshList())
                .set_color(0x2A2E4B);

            event.reply(dpp::message().add_embed(embed));
        }
    });

    bot.start(dpp::st_wait);

    //
}
// main.cpp

#include <iostream>
#include <fstream>

#include <dpp/dpp.h>

#include "aurebesh.hpp"

void displayLetters() {
    for(int i = 0; i < 26; ++i) {
        std::cout << aurebeshLetters[i].aurebeshName;
    }
}

int main() {
    std::ifstream token_file("../token.txt");
    if (!token_file.is_open()) {
        std::cerr << "Could not open token.txt\n";
        return 1;
    }

    std::string token;
    std::getline(token_file, token);
    if (token.empty()) {
        std::cerr << "token.txt is empty\n";
        return 1;
    }

    dpp::cluster bot(token, dpp::i_default_intents);

    dpp::activity activity(dpp::activity_type::at_game, "Aurebesh", "", "");
    dpp::presence presence(dpp::ps_online, activity);
    bot.set_presence(presence);

    bot.on_ready([&bot](const dpp::ready_t&) {
        std::cout << "Bot is up: " << bot.me.username << '\n';
    });

    bot.start(dpp::st_wait);

    displayLetters();
}
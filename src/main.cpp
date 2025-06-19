#include <iostream>

#include <dpp/dpp.h>
#include <cstdlib>
#include "dotenv.h"

int main() {
    dotenv::load();

    const char* token = std::getenv("DISCORD_TOKEN");
    if (!token) {
        std::cerr << "Missing DISCORD_TOKEN in .env file." << std::endl;
        return 1;
    }

    std::cout << "Loaded token: " << token << std::endl;
    std::string BOT_TOKEN{token};

    dpp::cluster bot(BOT_TOKEN);

    bot.on_ready([&bot](const dpp::ready_t& event){
      std::cout << "Bot is up: " << bot.me.username << "\n";
    });
    
    bot.start(dpp::st_wait);
}
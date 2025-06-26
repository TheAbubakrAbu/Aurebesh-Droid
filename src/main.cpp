// main.cpp

#include <cstdlib>
#include <iostream>
#include <fstream>
#include <random>

#include <dpp/dpp.h>

#include "commands.hpp"
#include "holocron.hpp"
#include "render_aurebesh/render_aurebesh.hpp"

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
            dpp::embed embed = dpp::embed()
                .set_title("Aurebesh Alphabet")
                .set_description(getAurebeshList())
                .set_color(0x4295E2)
                .set_thumbnail("https://raw.githubusercontent.com/TheAbubakrAbu/Aurebesh-Droid/main/src/images/aurebesh.png");

            event.reply(dpp::message().add_embed(embed));
        } else if (command == "translate") {
            string input = std::get<string>(event.get_parameter("text"));

            if (input.empty() || input.size() > 50) {
                event.reply("❌ Please enter between 1 and 50 characters.");
                return;
            }

            event.thinking(false);

            string imagePath;
            uint64_t userId = event.command.usr.id;
            uint64_t channelId = event.command.channel_id;
            auto timestamp = std::chrono::system_clock::now().time_since_epoch().count();

            std::ostringstream oss;
            oss << "aurebesh_" << userId << "_" << channelId << "_" << timestamp << ".png";
            std::string imageName = oss.str();

            if (!renderTextToImage(input.c_str(), imagePath, imageName)) {
                event.edit_response("❌ Failed to render Aurebesh image.");
                return;
            }

            string description = "<@" + std::to_string(event.command.usr.id) + "> said: " + input;
            dpp::embed embed = dpp::embed()
                .set_title("Aurebesh Translation")
                .set_description(description)
                .set_color(0x4295E2)
                .set_thumbnail("https://raw.githubusercontent.com/TheAbubakrAbu/Aurebesh-Droid/main/src/images/aurebesh.png")
                .set_image("attachment://" + imageName);

            dpp::message msg;
            msg.add_embed(embed);
            msg.add_file(imageName, dpp::utility::read_file(imagePath));
            event.edit_response(msg);

            std::remove(imagePath.c_str());
        } else if(command.find("holocron") != string::npos) {
            string title = "💠 Jedi Holocron 💠";
            vector<pair<string, string>> quotes = jedi_quotes;
            auto color = 0x4295E2;
            string book = "📘";

            if(command.find("sith") != string::npos) {
                title = "🔺 Sith Holocron 🔺";
                quotes = sith_quotes;
                color = 0x8B0000;
                book = "📕";
            }

            quotes.insert(quotes.end(), {
                { "Bendu", "I am the Bendu, the one in the middle. Between the light and the dark." },
                { "Bendu", "An object cannot make you good or evil. The temptation of power is always there, but those who are strong enough to control it are the ones who truly have the power." },
                { "Bendu", "You think the only way to gain power is by embracing the dark side. Wrong." },
                { "Bendu", "You do not comprehend what it is I am. And what I can do!" },
                { "Bendu", "I am beyond your understanding." },
            });

            std::random_device rd;
            std::mt19937 gen(rd());
            std::uniform_int_distribution<> dis(0, quotes.size() - 1);
            auto& [author, quote] = quotes[dis(gen)];

            if(author == "Bendu") {
                color = 0xDAA520;
                book = "📙";
                title += " | 🔸 Bendu's Wisdom 🔸";
            }

            dpp::embed embed = dpp::embed()
                .set_title(title)
                .set_description("\"" + quote + "\"\n\n— **" + author + " " + book + "**")
                .set_color(color)
                .set_thumbnail("https://raw.githubusercontent.com/TheAbubakrAbu/Aurebesh-Droid/main/src/images/aurebesh.png");

            event.reply(dpp::message().add_embed(embed));
        } else if(command == "holocron") {
            vector<pair<string, string>> quotes = jedi_quotes;
            quotes.insert(quotes.end(), sith_quotes.begin(), sith_quotes.end());
            quotes.insert(quotes.end(), {
                { "Bendu", "I am the Bendu, the one in the middle. Between the light and the dark." },
                { "Bendu", "An object cannot make you good or evil. The temptation of power is always there, but those who are strong enough to control it are the ones who truly have the power." },
                { "Bendu", "You think the only way to gain power is by embracing the dark side. Wrong." },
                { "Bendu", "You do not comprehend what it is I am. And what I can do!" },
                { "Bendu", "I am beyond your understanding." },
            });

            string title = "💠 Jedi Holocron 💠";
            auto color = 0x4295E2;
            string book = "📘";

            std::random_device rd;
            std::mt19937 gen(rd());
            std::uniform_int_distribution<> dis(0, quotes.size() - 1);
            auto& [author, quote] = quotes[dis(gen)];

            if(author == "Bendu") {
                title = "🔸 Bendu's Wisdom 🔸";
                color = 0xDAA520;
                book = "📙";
            } else if(author.find("Darth") != string::npos || author.find("Count") != string::npos) {
                title = "🔺 Sith Holocron 🔺";
                color = 0x8B0000;
                book = "📕";
            }

            dpp::embed embed = dpp::embed()
                .set_title(title)
                .set_description("\"" + quote + "\"\n\n— **" + author + " " + book + "**")
                .set_color(color)
                .set_thumbnail("https://raw.githubusercontent.com/TheAbubakrAbu/Aurebesh-Droid/main/src/images/aurebesh.png");

            event.reply(dpp::message().add_embed(embed));
        } else if(command == "help") {
            dpp::embed embed = dpp::embed()
                .set_title("Aurebesh Droid Help")
                .set_description(getAurebeshHelp())
                .set_color(0x4295E2)
                .set_thumbnail("https://raw.githubusercontent.com/TheAbubakrAbu/Aurebesh-Droid/main/src/images/aurebesh.png");

            event.reply(dpp::message().add_embed(embed));
        }
    });

    bot.start(dpp::st_wait);
}
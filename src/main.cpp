// main.cpp

#include <cstdlib>
#include <iostream>
#include <fstream>

#include <dpp/dpp.h>

#include "commands.hpp"
#include "aurebesh.hpp"
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
        dpp::activity playing(dpp::activity_type::at_game, "Jedi Holocron", "", "");
        bot.set_presence(dpp::presence(dpp::ps_online, playing));

        cout << "Bot is up: " << bot.me.username << '\n';

        if(dpp::run_once<struct register_commands>()) {
            bot.global_command_create(
                dpp::slashcommand("alphabet", "Display the Aurebesh alphabet", bot.me.id)
            );

            bot.global_command_create(
                dpp::slashcommand("translate", "Convert English text to Aurebesh", bot.me.id)
                    .add_option(dpp::command_option(dpp::co_string, "text", "Text to translate", true))
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

        if(command == "alphabet") {
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
        } else if(command == "holcron_jedi") {

        } else if(command == "holocron_sith") {

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
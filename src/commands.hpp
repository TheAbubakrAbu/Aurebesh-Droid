// commands.hpp
//
// Every command the bot registers with Discord. main.cpp routes each one to
// the feature file that handles it. Descriptions start with a verb.

#pragma once

#include <string>
#include <string_view>
#include <vector>

#include <dpp/dpp.h>

#include "scripts.hpp"
#include "style.hpp"
#include "translate.hpp"

// The message menu entry (right-click a message, then Apps).
inline constexpr std::string_view translate_message_menu = "Translate to Aurebesh";

// With user_install, people can also add the bot to their own account and use
// it in any server, DM, or group DM. That needs User Install turned on in the
// Discord Developer Portal, so main.cpp falls back to servers only if Discord
// refuses.
inline std::vector<dpp::slashcommand> build_commands(dpp::snowflake app_id, bool user_install) {
    std::vector<dpp::slashcommand> commands;

    // Optional, so a plain /translate keeps working as before. Each choice's
    // value is its index in style.hpp.
    dpp::command_option color_choices(dpp::co_string, "color", "Color of the glyphs", false);
    for (std::size_t i = 0; i < colors.size(); ++i) {
        color_choices.add_choice(dpp::command_option_choice(std::string(colors[i].name), std::to_string(i)));
    }
    dpp::command_option background_choices(dpp::co_string, "background", "Background behind the glyphs", false);
    for (std::size_t i = 0; i < backgrounds.size(); ++i) {
        background_choices.add_choice(dpp::command_option_choice(std::string(backgrounds[i].name), std::to_string(i)));
    }

    for (const Script& script : scripts) {
        commands.push_back(
            dpp::slashcommand(std::string(script.command), script_description(script), app_id)
                .add_option(dpp::command_option(dpp::co_string, "text", "Text to translate", true)
                    .set_max_length(max_text_length))
                .add_option(color_choices)
                .add_option(background_choices));
    }

    commands.emplace_back("alphabet", "Show the Aurebesh alphabet", app_id);
    commands.emplace_back("aurebesh", "Show the Aurebesh alphabet", app_id);
    commands.emplace_back("holocron", "Unlock a Jedi or Sith holocron", app_id);
    commands.emplace_back("holocron_jedi", "Unlock a Jedi holocron", app_id);
    commands.emplace_back("holocron_sith", "Unlock a Sith holocron", app_id);
    commands.emplace_back("quiz", "Read a Star Wars word written in Aurebesh", app_id);
    commands.emplace_back("help", "Show how to use Aurebesh Droid", app_id);

    // A message command's name keeps its capitals only if the type is set first.
    dpp::slashcommand translate_message;
    translate_message.set_type(dpp::ctxm_message).set_name(std::string(translate_message_menu)).set_application_id(app_id);
    commands.push_back(translate_message);

    for (dpp::slashcommand& command : commands) {
        if (user_install) {
            command.integration_types = {dpp::ait_guild_install, dpp::ait_user_install};
            command.set_interaction_contexts({dpp::itc_guild, dpp::itc_bot_dm, dpp::itc_private_channel});
        } else {
            command.integration_types = {dpp::ait_guild_install};
            command.set_interaction_contexts({dpp::itc_guild, dpp::itc_bot_dm});
        }
    }
    return commands;
}

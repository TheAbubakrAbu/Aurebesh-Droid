// main.cpp
//
// Starts the bot and routes every slash command, message menu, button, and
// menu choice to the feature that handles it. `AurebeshDroid --self-test`
// runs the checks in self_test.cpp instead.

#include <cstdlib>
#include <exception>
#include <iostream>
#include <string>
#include <string_view>

#include <dpp/dpp.h>

#include "alphabet.hpp"
#include "commands.hpp"
#include "help.hpp"
#include "holocron.hpp"
#include "quiz.hpp"
#include "render_aurebesh.hpp"
#include "replies.hpp"
#include "self_test.hpp"
#include "translate.hpp"

namespace {

// Runs a handler. If it throws, logs what failed and tells the person.
template <typename Handler>
void guarded(const dpp::interaction_create_t& event, const std::string& what, Handler&& handle) {
    try {
        handle();
    } catch (const std::exception& e) {
        event.owner->log(dpp::ll_error, what + " failed: " + e.what());
        reply_notice(event, something_went_wrong);
    }
}

void on_slash_command(const dpp::slashcommand_t& event) {
    const std::string command = event.command.get_command_name();
    guarded(event, "/" + command, [&] {
        if (const Script* script = find_script(command)) translate_command(event, *script);
        else if (command == "alphabet" || command == "aurebesh") alphabet_command(event);
        else if (is_holocron_command(command)) holocron_command(event, command);
        else if (command == "quiz") quiz_command(event);
        else if (command == "help") help_command(event);
    });
}

void on_message_menu(const dpp::message_context_menu_t& event) {
    guarded(event, std::string(translate_message_menu), [&] {
        if (event.command.get_command_name() == translate_message_menu) translate_message_command(event);
    });
}

// "Open Another" under a holocron, the quiz's answers, and the Customize
// buttons. A button nothing recognizes (from an older version) still gets an
// answer, so Discord never shows "This interaction failed".
void on_button(const dpp::button_click_t& event) {
    const std::string& id = event.custom_id;
    guarded(event, "Button " + id, [&] {
        bool handled = false;
        if (id.starts_with(holocron_prefix)) {
            const std::string command = id.substr(holocron_prefix.size());
            handled = is_holocron_command(command);
            if (handled) holocron_command(event, command);
        } else if (id.starts_with(quiz_prefix)) {
            handled = quiz_control(event, id);
        } else {
            handled = style_control(event, id, "");
        }
        if (!handled) reply_notice(event, old_button);
    });
}

// The Customize menus: script, color, and background.
void on_menu_choice(const dpp::select_click_t& event) {
    guarded(event, "Menu " + event.custom_id, [&] {
        if (!style_control(event, event.custom_id, event.values.empty() ? "" : event.values.front())) reply_notice(event, old_button);
    });
}

}  // namespace

int main(int argc, char* argv[]) {
    if (argc > 1 && std::string_view(argv[1]) == "--self-test") {
        return self_test(argc > 2 ? argv[2] : nullptr);
    }

    const char* token = std::getenv("DISCORD_TOKEN");
    if (!token || !*token) {
        std::cerr << "DISCORD_TOKEN environment variable not set\n";
        return 1;
    }
    if (!aurebesh::load_fonts()) return 1;

    // Interactions arrive without any gateway intents, and nothing here reads
    // D++'s cache, so subscribe to no events and cache nothing.
    dpp::cluster bot(token, 0, 0, 0, 1, true, dpp::cache_policy::cpol_none);

    bot.on_log([](const dpp::log_t& event) {
        if (event.severity >= dpp::ll_info) {
            std::cout << "[" << dpp::utility::current_date_time() << "] "
                      << dpp::utility::loglevel(event.severity) << ": " << event.message << '\n';
        }
    });

    bot.on_ready([&bot](const dpp::ready_t&) {
        // "Playing 🔤 Aurebesh", like Sabacc Droid's "Playing 🃏 Sabacc".
        dpp::activity playing(dpp::activity_type::at_game, theme::titled(theme::icon::translate, "Aurebesh"), "", "");
        bot.set_presence(dpp::presence(dpp::ps_online, playing));
        bot.log(dpp::ll_info, "Aurebesh Droid is online as " + bot.me.username);

        if (dpp::run_once<struct register_commands>()) {
            bot.global_bulk_command_create(build_commands(bot.me.id, true), [&bot](const dpp::confirmation_callback_t& done) {
                if (!done.is_error()) {
                    bot.log(dpp::ll_info, "Commands registered for servers and personal installs");
                    return;
                }
                bot.log(dpp::ll_warning, "Discord refused personal-install commands (" + done.get_error().human_readable +
                                         "). Turn on User Install in the Developer Portal; registering for servers only.");
                bot.global_bulk_command_create(build_commands(bot.me.id, false));
            });
        }
    });

    bot.on_slashcommand(on_slash_command);
    bot.on_message_context_menu(on_message_menu);
    bot.on_button_click(on_button);
    bot.on_select_click(on_menu_choice);

    bot.start(dpp::st_wait);
}

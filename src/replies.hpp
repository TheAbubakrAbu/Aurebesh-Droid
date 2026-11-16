// replies.hpp
//
// Helpers every command uses: who is asking, and the notices only they see.

#pragma once

#include <string>
#include <string_view>

#include <dpp/dpp.h>

#include "theme.hpp"

// The name and picture the person goes by here: their server nickname and
// server avatar if they set them, otherwise their account's.
inline std::string display_name(const dpp::interaction& command) {
    if (!command.member.get_nickname().empty()) return command.member.get_nickname();
    if (!command.usr.global_name.empty()) return command.usr.global_name;
    return command.usr.username;
}

inline std::string display_name(const dpp::user& user) {
    return user.global_name.empty() ? user.username : user.global_name;
}

inline std::string avatar_url(const dpp::interaction& command) {
    const std::string server_avatar = command.member.get_avatar_url(128);
    return server_avatar.empty() ? command.usr.get_avatar_url(128) : server_avatar;
}

// A notice only the person who asked can see: "❌ " and a full sentence.
inline void reply_notice(const dpp::interaction_create_t& event, std::string_view sentence) {
    event.reply(dpp::message(theme::titled(theme::icon::error, sentence)).set_flags(dpp::m_ephemeral));
}

// Worded the same as Sabacc Droid's.
inline constexpr std::string_view something_went_wrong = "Something went wrong. Please try again.";
inline constexpr std::string_view old_button = "This button is from an older version of Aurebesh Droid. Run the command again for a new one.";

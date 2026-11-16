// theme.hpp
//
// What every reply shares, so the bot looks the same everywhere: its color,
// its images, and one icon per feature. Each icon is used in that feature's
// titles, its buttons, and /help.
//
// Conventions for replies, the same in Aurebesh Droid and Sabacc Droid:
//   - Titles are "<icon> <Title Case>".
//   - Buttons have an icon and a Title Case label, except buttons that are
//     the choice itself (a card, a quiz answer), which show just that.
//   - Notices only the user sees are "❌ " and a full sentence (see replies.hpp).
//   - Text uses straight quotes and apostrophes.
//   - /help ends with the credits, More Star Wars, and Made By fields, link
//     buttons to add the bot, the Datapad app, and GitHub, and the disclaimer
//     below as its footer.

#pragma once

#include <cstdint>
#include <string>
#include <string_view>

namespace theme {

inline constexpr std::uint32_t blue = 0x4295E2;  // the bot's color
inline constexpr std::uint32_t navy = 0x0B1626;  // behind translations, like the alphabet chart

inline const std::string images = "https://raw.githubusercontent.com/TheAbubakrAbu/Aurebesh-Droid/main/src/assets/images/";
inline const std::string logo = images + "aurebesh.jpg";

inline constexpr std::string_view github_link = "https://github.com/TheAbubakrAbu/Aurebesh-Droid";
inline constexpr std::string_view add_bot_link = "https://discord.ly/aurebesh-droid";

// Shared word for word with Sabacc Droid's /help.
inline constexpr std::string_view creator = "[Abubakr Elmallah](https://abubakrelmallah.com)";
inline constexpr std::string_view disclaimer = "Fan-made and not affiliated with or endorsed by Lucasfilm or Disney. May the Force be with you!";
inline constexpr std::string_view sabacc_droid_link = "https://discord.ly/sabaac-droid";
inline constexpr std::string_view datapad_app = "https://apps.apple.com/us/app/datapad-aurebesh-translator/id6450498054?platform=iphone";
inline constexpr std::string_view translator_app = "https://apps.apple.com/us/app/aurebesh-translator/id6670201513?platform=iphone";
inline const std::string star_wars_apps =
    "- **[Datapad | Aurebesh Translator](" + std::string(datapad_app) + ")**: An immersive Aurebesh translator with a themed keyboard.\n"
    "- **[Aurebesh Translator](" + std::string(translator_app) + ")**: A free, offline, ad-free translator.";

namespace icon {
inline constexpr std::string_view translate = "🔤";
inline constexpr std::string_view customize = "🎨";
inline constexpr std::string_view alphabet = "📜";
inline constexpr std::string_view jedi = "💠";
inline constexpr std::string_view sith = "🔺";
inline constexpr std::string_view bendu = "🔸";
inline constexpr std::string_view quiz = "🧩";
inline constexpr std::string_view correct = "✅";
inline constexpr std::string_view streak = "🔥";
inline constexpr std::string_view streak_over = "💔";
inline constexpr std::string_view help = "📖";
inline constexpr std::string_view add_bot = "➕";
inline constexpr std::string_view app = "📱";
inline constexpr std::string_view github = "📂";
inline constexpr std::string_view error = "❌";
}  // namespace icon

// "<icon> <text>", the form of every title.
inline std::string titled(std::string_view icon, std::string_view text) {
    return std::string(icon) + " " + std::string(text);
}

}  // namespace theme

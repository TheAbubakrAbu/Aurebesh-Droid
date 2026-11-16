// self_test.cpp
//
// Checks everything that can be checked without Discord. CI runs it before
// publishing a build; see self_test.hpp.

#include "self_test.hpp"

#include <algorithm>
#include <chrono>
#include <climits>
#include <cstdint>
#include <filesystem>
#include <fstream>
#include <initializer_list>
#include <iostream>
#include <optional>
#include <span>
#include <string>
#include <string_view>
#include <vector>

#include <dpp/dpp.h>

#include "alphabet.hpp"
#include "commands.hpp"
#include "help.hpp"
#include "holocron.hpp"
#include "quiz.hpp"
#include "render_aurebesh.hpp"
#include "scripts.hpp"
#include "style.hpp"
#include "translate.hpp"

namespace {

// Discord refuses a reply whose embeds break its size limits. They count
// characters; bytes are never fewer, so checking bytes is safe.
std::string embed_problem(const dpp::message& message) {
    for (const dpp::embed& embed : message.embeds) {
        std::size_t total = embed.title.size() + embed.description.size();
        if (embed.title.size() > 256) return "title is over 256";
        if (embed.description.size() > 4096) return "description is over 4096";
        if (embed.fields.size() > 25) return "more than 25 fields";
        for (const dpp::embed_field& field : embed.fields) {
            if (field.name.size() > 256 || field.value.size() > 1024) {
                return "field " + field.name + " is " + std::to_string(field.value.size()) + " of 1024";
            }
            total += field.name.size() + field.value.size();
        }
        if (embed.footer) total += embed.footer->text.size();
        if (embed.author) total += embed.author->name.size();
        if (total > 6000) return std::to_string(total) + " of 6000 in all";
    }
    return {};
}

// Discord refuses a message whose buttons and menus break its limits.
std::string component_problem(const dpp::message& message) {
    if (message.components.size() > 5) return "more than 5 rows";
    for (const dpp::component& row : message.components) {
        if (row.components.size() > 5) return "a row has more than 5 controls";
        for (const dpp::component& control : row.components) {
            if (control.custom_id.size() > 100) return "an id is over 100";
            if (control.type == dpp::cot_selectmenu) {
                if (row.components.size() != 1) return "a menu shares its row";
                if (control.options.empty() || control.options.size() > 25) return "a menu has 0 or over 25 choices";
                for (const dpp::select_option& option : control.options) {
                    if (option.label.size() > 100 || option.value.size() > 100 || option.description.size() > 100) {
                        return "menu choice " + option.label + " is too long";
                    }
                }
            } else if (control.label.size() > 80) {
                return "button " + control.label + " is too long";
            }
        }
    }
    return {};
}

// The reply conventions in theme.hpp: every title starts with a feature icon,
// and every button has an icon and a Title Case label (quiz answers are words,
// so they have no icon).
std::string convention_problem(const dpp::message& message) {
    namespace icon = theme::icon;
    const std::string_view icons[] = {icon::translate, icon::customize, icon::alphabet, icon::jedi, icon::sith,
                                      icon::bendu,     icon::quiz,      icon::help,     icon::error};
    for (const dpp::embed& embed : message.embeds) {
        const bool titled = std::any_of(std::begin(icons), std::end(icons), [&](std::string_view icon) {
            return embed.title.starts_with(std::string(icon) + " ");
        });
        if (!titled) return "title \"" + embed.title + "\" does not start with a feature icon";
    }
    for (const dpp::component& row : message.components) {
        for (const dpp::component& control : row.components) {
            if (control.type != dpp::cot_button) continue;
            const bool answer = control.custom_id.starts_with(quiz_prefix) && !control.custom_id.starts_with(quiz_next);
            if (!answer && control.emoji.name.empty()) return "button " + control.label + " has no icon";
            bool word_start = true;
            for (char c : control.label) {
                if (word_start && c >= 'a' && c <= 'z') return "button " + control.label + " is not Title Case";
                word_start = c == ' ';
            }
        }
    }
    return {};
}

}  // namespace

// Checks everything that can be checked without Discord: every script renders
// to a PNG, blank text renders nothing, the longest allowed text renders fast,
// and every reply fits Discord's limits. CI runs this before publishing a
// build. Given a folder, it also saves each script's image there to look at:
//   ./AurebeshDroid --self-test previews
int self_test(const char* folder) {
    using clock = std::chrono::steady_clock;
    const auto ms_since = [](clock::time_point start) {
        return std::chrono::duration<double, std::milli>(clock::now() - start).count();
    };
    const auto is_png = [](const std::string& data) { return data.starts_with("\x89PNG\r\n\x1a\n"); };

    if (!aurebesh::load_fonts()) return 1;
    if (folder) std::filesystem::create_directories(folder);

    int problems = 0;
    const auto check = [&problems](bool passed, const std::string& what) {
        std::cout << (passed ? "ok    " : "FAIL  ") << what << '\n';
        problems += passed ? 0 : 1;
    };

    const std::string sample = "Hello there! General Kenobi, it’s Order 66.";
    for (const Script& script : scripts) {
        const auto start = clock::now();
        const std::string png = aurebesh::render_png(sample, script.font, default_style(script).look());
        check(is_png(png), "/" + std::string(script.command) + ": " + std::to_string(png.size()) +
                           " bytes in " + std::to_string(ms_since(start)) + " ms");
        if (folder && is_png(png)) {
            std::ofstream(std::filesystem::path(folder) / (std::string(script.command) + ".png"), std::ios::binary) << png;
        }
    }

    const Script& basic = scripts.front();
    check(aurebesh::render_png("   ", basic.font, aurebesh::Look{}).empty(), "blank text draws nothing");
    check(aurebesh::render_png("Padmé", basic.font, aurebesh::Look{}) == aurebesh::render_png("Padme", basic.font, aurebesh::Look{}),
          "a separate accent mark draws nothing extra");

    // The reply has to reach Discord within 3 seconds, image included.
    const auto longest_length = static_cast<std::size_t>(max_text_length);
    std::string longest;
    while (longest.size() < longest_length) longest += "the quick brown fox jumps over the lazy dog ";
    longest.resize(longest_length);
    const auto start = clock::now();
    const std::string png = aurebesh::render_png(longest, basic.font, aurebesh::Look{});
    const double ms = ms_since(start);
    check(is_png(png) && ms < 1000, "longest text: " + std::to_string(png.size()) + " bytes in " + std::to_string(ms) + " ms");

    const auto check_fits = [&check](const dpp::message& message, const std::string& what) {
        const std::string problem = embed_problem(message);
        check(problem.empty(), what + (problem.empty() ? " fits in Discord" : ": " + problem));
    };
    check_fits(help_message(), "/help");
    check_fits(alphabet_message(), "/alphabet");

    // /help ends the same way as Sabacc Droid's (see theme.hpp).
    {
        const dpp::message help = help_message();
        const dpp::embed& embed = help.embeds.front();
        std::vector<std::string> ending;
        for (std::size_t i = embed.fields.size() >= 3 ? embed.fields.size() - 3 : 0; i < embed.fields.size(); ++i) {
            ending.push_back(embed.fields[i].name);
        }
        std::vector<std::string> links;
        for (const dpp::component& row : help.components) {
            for (const dpp::component& control : row.components) {
                if (!control.url.empty()) links.push_back(control.label);
            }
        }
        const bool shared = embed.fields.size() >= 3 && embed.footer && embed.footer->text == theme::disclaimer &&
                            embed.fields[embed.fields.size() - 2].value.find(theme::star_wars_apps) != std::string::npos &&
                            embed.fields.back().value.find(theme::creator) != std::string::npos;
        check(shared && ending == std::vector<std::string>{"✒️ Fonts", "🌌 More Star Wars", "🛠️ Made By"} &&
                  links == std::vector<std::string>{"Add Aurebesh Droid", "Datapad App", "GitHub"},
              "/help ends like Sabacc Droid's");
    }

    // The largest translation message: the longest text, the longest name, the
    // largest user id, and every menu open.
    dpp::interaction by;
    by.usr.id = dpp::snowflake(UINT64_MAX);
    by.usr.username = std::string(32, 'W');
    const Style biggest{scripts.size() - 1, colors.size() - 1, backgrounds.size() - 1, true};
    const Attribution credit{by.usr.username, "https://cdn.discordapp.com/embed/avatars/0.png",
                             "https://discord.com/channels/18446744073709551615/18446744073709551615/18446744073709551615"};
    const dpp::message translation = translation_message(biggest, longest, "png", credit, by.usr.id, true);
    check_fits(translation, "longest translation");
    const std::string controls = component_problem(translation);
    check(controls.empty(), "the Customize menus " + (controls.empty() ? std::string("fit in Discord") : controls));
    check(translation_text(translation) == longest, "a translation's text reads back out of its message");

    // Every style the menus can make survives the trip through a control's id.
    bool ids_ok = true;
    std::size_t longest_id = 0;
    for (std::size_t script = 0; script < scripts.size(); ++script) {
        if (scripts[script].group.empty()) continue;
        for (std::size_t color = 0; color < colors.size(); ++color) {
            for (std::size_t background = 0; background < backgrounds.size(); ++background) {
                for (bool glow : {true, false}) {
                    const Style style{script, color, background, glow};
                    for (std::string_view action : {"open", "close", "script", "color", "background", "glow", "reset"}) {
                        const std::string id = style_id(action, by.usr.id, style);
                        longest_id = std::max(longest_id, id.size());
                        const std::optional<StyleRequest> back = parse_style_id(id);
                        ids_ok = ids_ok && back && back->action == action && back->owner == by.usr.id && back->style == style;
                    }
                }
            }
        }
    }
    check(ids_ok && longest_id <= 100, "every style survives its control ids (longest is " + std::to_string(longest_id) + " of 100)");
    bool refused = true;
    for (std::string_view bad : {"", "1.0.0", "1.0.0.1.", "1.0.0.2", "0.0.0.1", "99.0.0.1", "1.x.0.1", "1..0.0.1"}) {
        refused = refused && !decode_style(bad);
    }
    check(refused && !parse_style_id("style:open:12x:1.0.0.1") && !parse_style_id("holocron:holocron"), "broken ids are refused");
    check(default_style(scripts.front()).script == 1, "/translate starts on Basic Aurebesh");

    // Every background with and without glow, and every color.
    bool looks_ok = true;
    for (std::size_t background = 0; background < backgrounds.size(); ++background) {
        for (bool glow : {true, false}) {
            const Style style{1, 0, background, glow};
            const std::string image = aurebesh::render_png(sample, style.script_info().font, style.look());
            looks_ok = looks_ok && is_png(image);
            if (folder) {
                std::ofstream(std::filesystem::path(folder) / ("background_" + std::to_string(background) + (glow ? "" : "_no_glow") + ".png"),
                              std::ios::binary) << image;
            }
        }
    }
    for (std::size_t color = 0; color < colors.size(); ++color) {
        const Style style{1, color, 0, true};
        looks_ok = looks_ok && is_png(aurebesh::render_png(sample, style.script_info().font, style.look()));
    }
    check(looks_ok, "every color and background renders");

    bool holocrons_fit = true;
    for (std::span<const Quote> pool : {std::span<const Quote>(jedi_quotes), std::span<const Quote>(sith_quotes),
                                        std::span<const Quote>(bendu_quotes)}) {
        for (const Quote& quote : pool) holocrons_fit = holocrons_fit && embed_problem(holocron_message(quote, "holocron_sith")).empty();
    }
    check(holocrons_fit, "every holocron quote fits in Discord");

    // Right-click translations: chat markup becomes plain words, and emoji
    // the font lacks are left out instead of drawn as boxes.
    dpp::message chat;
    dpp::user mentioned;
    mentioned.id = 42;
    mentioned.username = "obiwan";
    mentioned.global_name = "Obi-Wan";
    chat.mentions.emplace_back(mentioned, dpp::guild_member());
    chat.content = "**Hello** <@42> and <@!7>, see <#5> <:wave:99> </help:3> `code` <t:1:R>\n> quoted ||spoiler||";
    const std::string plain = plain_text(chat);
    check(plain == "Hello @Obi-Wan and @someone, see #channel  /help code \nquoted spoiler", "chat markup becomes plain text: " + plain);
    check(aurebesh::render_png("Hi 👋🏽🎉", basic.font, aurebesh::Look{}) == aurebesh::render_png("Hi", basic.font, aurebesh::Look{}),
          "emoji are left out of the drawing");

    // The quiz: every word draws, games survive the trip through their button
    // ids, broken ids are refused, and a finished game fits in Discord.
    bool words_ok = true;
    for (std::string_view word : quiz_words) {
        words_ok = words_ok && word.size() <= 80 && is_png(aurebesh::render_png(word, "AurebeshBasic.otf", aurebesh::Look{}));
    }
    check(words_ok, std::to_string(quiz_words.size()) + " quiz words draw");
    bool games_ok = true;
    for (int round = 0; round < 500; ++round) {
        Quiz quiz = new_quiz(round == 0 ? max_streak : static_cast<std::size_t>(round));
        quiz.wrong = static_cast<unsigned>(round % 16);
        for (std::size_t pick = 0; pick < quiz.options.size(); ++pick) {
            const std::optional<QuizPick> back = parse_quiz_id(quiz_id(quiz, pick));
            games_ok = games_ok && back && back->pick == pick && back->quiz.answer == quiz.answer && back->quiz.options == quiz.options &&
                       back->quiz.wrong == quiz.wrong && back->quiz.streak == quiz.streak && quiz_id(quiz, pick).size() <= 100;
        }
        games_ok = games_ok && parse_quiz_next_id(quiz_next_id(quiz.streak)) == quiz.streak;
    }
    bool quiz_refused = true;
    for (std::string_view bad : {"quiz:0:0.1.2.3:0:0:4", "quiz:0:1.2.3.4:0:0:0", "quiz:0:0.0.1.2:0:0:0", "quiz:999:0.1.2.999:0:0:0",
                                 "quiz:0:0.1.2.3:16:0:0", "quiz:0:0.1.2.3:0:100000:0", "quiz:0:0.1.2.3:0:0:0:", "quiz:0:0.1.2:0:0:0",
                                 "quiz:0:0.1.2.3:0:0", "quiz:x"}) {
        quiz_refused = quiz_refused && !parse_quiz_id(bad);
    }
    for (std::string_view bad : {"quiz:next:", "quiz:next:x", "quiz:next:1x", "quiz:next:100000", "quiz:nextt", "quiz:next5"}) {
        quiz_refused = quiz_refused && !parse_quiz_next_id(bad);
    }
    check(games_ok && quiz_refused && parse_quiz_next_id("quiz:next") == 0,
          "quiz games survive their button ids, and broken ids are refused");

    // Streaks: a word read on the first guess grows the streak, Next Word
    // carries it on, and a word that took a wrong guess ends it.
    {
        Quiz quiz = new_quiz(2);
        const std::size_t right = static_cast<std::size_t>(std::find(quiz.options.begin(), quiz.options.end(), quiz.answer) - quiz.options.begin());
        const std::size_t wrong = (right + 1) % quiz.options.size();
        const QuizTurn first_try = play_quiz_pick(quiz, right, "", "Rey");
        const dpp::message solved = quiz_message(first_try.quiz, first_try.notes, true, "png");
        bool next_ok = false;
        for (const dpp::component& row : solved.components) {
            for (const dpp::component& control : row.components) next_ok = next_ok || control.custom_id == "quiz:next:3";
        }
        const QuizTurn missed = play_quiz_pick(quiz, wrong, "", "Finn");
        const QuizTurn late = play_quiz_pick(missed.quiz, right, missed.notes, "Rey");
        const Quiz first_word = new_quiz();
        const std::size_t first_right = static_cast<std::size_t>(
            std::find(first_word.options.begin(), first_word.options.end(), first_word.answer) - first_word.options.begin());
        const QuizTurn fresh = play_quiz_pick(first_word, first_right, "", "Poe");  // a streak of 1 isn't worth a line
        const bool shown = quiz_message(quiz, "", false, "png").embeds.front().description.find("**2 in a row** so far") != std::string::npos;
        check(first_try.solved && first_try.quiz.streak == 3 && first_try.notes.find("**3 in a row** read on the first guess!") != std::string::npos &&
                  next_ok && shown && !missed.solved && late.solved && late.quiz.streak == 0 &&
                  late.notes.find("The streak of **2** ends here.") != std::string::npos && quiz_notes(solved) == first_try.notes &&
                  fresh.notes.find("streak") == std::string::npos,
              "quiz streaks grow on first guesses, carry on to the next word, and end on a miss");
    }

    const std::string long_name(32, 'W');
    const std::string notes = "❌ " + long_name + " guessed DROID\n❌ " + long_name + " guessed FORCE\n❌ " + long_name +
                              " guessed JEDI\n✅ **" + long_name + "** read it: **MILLENNIUM FALCON**\n💔 The streak of **99999** ends here.";
    const dpp::message finished = quiz_message(new_quiz(max_streak), notes, true, "png");
    const std::string quiz_controls = component_problem(finished);
    check(embed_problem(finished).empty() && quiz_controls.empty(), "a finished quiz fits in Discord" + quiz_controls);

    // Registration: within Discord's limits, and the message menu keeps its capitals.
    for (bool user_install : {true, false}) {
        const std::vector<dpp::slashcommand> commands = build_commands(1, user_install);
        bool commands_ok = commands.size() <= 100;
        for (const dpp::slashcommand& command : commands) commands_ok = commands_ok && command.options.size() <= 25;
        commands_ok = commands_ok && commands.back().name == translate_message_menu && commands.back().type == dpp::ctxm_message;
        check(commands_ok, std::to_string(commands.size()) + " commands register" + (user_install ? " with personal installs" : " for servers only"));
    }


    // Every kind of reply follows the conventions in theme.hpp.
    std::vector<std::pair<std::string, dpp::message>> replies = {
        {"/help", help_message()},
        {"/alphabet", alphabet_message()},
        {"a translation", translation_message(Style{}, "Hello", "png", credit, 1, false)},
        {"a translation being customized", translation_message(Style{}, "Hello", "png", credit, 1, true)},
        {"a Jedi holocron", holocron_message(jedi_quotes[0], "holocron_jedi")},
        {"a Sith holocron", holocron_message(sith_quotes[0], "holocron_sith")},
        {"the Bendu", holocron_message(bendu_quotes[0], "holocron")},
        {"a quiz", quiz_message(new_quiz(), "", false, "png")},
        {"a finished quiz", finished},
    };
    for (const auto& [what, message] : replies) {
        const std::string problem = convention_problem(message);
        check(problem.empty(), what + (problem.empty() ? " follows the reply conventions" : ": " + problem));
    }

    // The same summary line as Sabacc Droid's self-test.
    std::cout << '\n' << (problems == 0 ? std::string("All checks passed")
                                        : std::to_string(problems) + (problems == 1 ? " problem" : " problems")) << '\n';
    return problems == 0 ? 0 : 1;
}

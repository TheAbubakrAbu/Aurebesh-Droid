// quiz.hpp
//
// /quiz shows a Star Wars word drawn in Aurebesh with four answers to pick
// from. A wrong pick is crossed off for everyone, and the first right pick
// wins. Words read on the first guess in a row make a streak, which Next Word
// carries on. Like the Customize controls, the buttons carry the whole game in
// their ids, so the bot keeps no state.

#pragma once

#include <algorithm>
#include <array>
#include <charconv>
#include <optional>
#include <random>
#include <string>
#include <string_view>
#include <vector>

#include <dpp/dpp.h>

#include "render_aurebesh.hpp"
#include "replies.hpp"
#include "theme.hpp"

inline constexpr auto quiz_words = std::to_array<std::string_view>({
    "JEDI", "SITH", "YODA", "VADER", "LEIA", "DROID", "FORCE", "EWOK", "JAWA", "HOTH", "ENDOR", "NABOO", "KYBER",
    "REBEL", "EMPIRE", "CLONE", "PORG", "GROGU", "SENATE", "SABACC", "BESKAR", "BANTHA", "AHSOKA", "KENOBI",
    "ANAKIN", "PADAWAN", "CANTINA", "WOOKIEE", "BLASTER", "DAGOBAH", "HAN SOLO", "TATOOINE", "HOLOCRON",
    "MUSTAFAR", "ALDERAAN", "REPUBLIC", "BOBA FETT", "CORUSCANT", "KASHYYYK", "MANDALORE", "SKYWALKER",
    "CHEWBACCA", "LIGHTSABER", "HYPERSPACE", "DEATH STAR", "MOS EISLEY", "STORMTROOPER", "MILLENNIUM FALCON",
    "JABBA", "LANDO", "KAMINO", "TIE FIGHTER",
});

inline constexpr std::string_view quiz_prefix = "quiz:";
inline constexpr std::string_view quiz_next = "quiz:next";
inline constexpr std::string_view quiz_question = "What does this say?";

inline constexpr std::size_t max_streak = 99999;

struct Quiz {
    std::size_t answer = 0;                // index into quiz_words
    std::array<std::size_t, 4> options{};  // indices into quiz_words, one of them the answer
    unsigned wrong = 0;                    // bit i set: option i was picked and is wrong
    std::size_t streak = 0;                // words read on the first guess in a row before this one
};

// Wrong answers are drawn from the words closest in length to the right one,
// so the length alone does not give it away.
inline Quiz new_quiz(std::size_t streak = 0) {
    thread_local std::mt19937 rng{std::random_device{}()};
    Quiz quiz;
    quiz.streak = std::min(streak, max_streak);
    quiz.answer = std::uniform_int_distribution<std::size_t>(0, quiz_words.size() - 1)(rng);

    std::vector<std::size_t> others;
    for (std::size_t i = 0; i < quiz_words.size(); ++i) {
        if (i != quiz.answer) others.push_back(i);
    }
    std::shuffle(others.begin(), others.end(), rng);
    const std::size_t length = quiz_words[quiz.answer].size();
    const auto distance = [&](std::size_t word) {
        const std::size_t other = quiz_words[word].size();
        return other > length ? other - length : length - other;
    };
    std::stable_sort(others.begin(), others.end(), [&](std::size_t a, std::size_t b) { return distance(a) < distance(b); });
    std::shuffle(others.begin(), others.begin() + 8, rng);

    quiz.options = {quiz.answer, others[0], others[1], others[2]};
    std::shuffle(quiz.options.begin(), quiz.options.end(), rng);
    return quiz;
}

// "quiz:<answer>:<option>.<option>.<option>.<option>:<wrong>:<streak>:<pick>"
inline std::string quiz_id(const Quiz& quiz, std::size_t pick) {
    std::string id = std::string(quiz_prefix) + std::to_string(quiz.answer) + ":";
    for (std::size_t i = 0; i < quiz.options.size(); ++i) id += (i ? "." : "") + std::to_string(quiz.options[i]);
    return id + ":" + std::to_string(quiz.wrong) + ":" + std::to_string(quiz.streak) + ":" + std::to_string(pick);
}

// "quiz:next:<streak>"
inline std::string quiz_next_id(std::size_t streak) {
    return std::string(quiz_next) + ":" + std::to_string(streak);
}

// The streak a Next Word button carries, or nothing if the id is broken.
inline std::optional<std::size_t> parse_quiz_next_id(std::string_view id) {
    if (id == quiz_next) return 0;
    if (!id.starts_with(quiz_next) || id.size() <= quiz_next.size() + 1 || id[quiz_next.size()] != ':') return std::nullopt;
    id.remove_prefix(quiz_next.size() + 1);
    std::size_t streak = 0;
    const auto [end, error] = std::from_chars(id.data(), id.data() + id.size(), streak);
    if (error != std::errc() || end != id.data() + id.size() || streak > max_streak) return std::nullopt;
    return streak;
}

struct QuizPick {
    Quiz quiz;
    std::size_t pick;
};

inline std::optional<QuizPick> parse_quiz_id(std::string_view id) {
    if (!id.starts_with(quiz_prefix)) return std::nullopt;
    id.remove_prefix(quiz_prefix.size());

    // Eight numbers: answer, four options, wrong, streak, pick. Separators are
    // checked loosely; the values themselves are checked strictly below.
    std::size_t numbers[8];
    for (std::size_t i = 0; i < std::size(numbers); ++i) {
        if (i > 0) {
            if (id.empty() || (id.front() != ':' && id.front() != '.')) return std::nullopt;
            id.remove_prefix(1);
        }
        const auto [end, error] = std::from_chars(id.data(), id.data() + id.size(), numbers[i]);
        if (error != std::errc()) return std::nullopt;
        id.remove_prefix(static_cast<std::size_t>(end - id.data()));
    }
    if (!id.empty()) return std::nullopt;

    QuizPick result;
    result.quiz.answer = numbers[0];
    result.quiz.options = {numbers[1], numbers[2], numbers[3], numbers[4]};
    result.quiz.wrong = static_cast<unsigned>(numbers[5]);
    result.quiz.streak = numbers[6];
    result.pick = numbers[7];

    const Quiz& quiz = result.quiz;
    if (quiz.answer >= quiz_words.size() || numbers[5] > 0xF || quiz.streak > max_streak || result.pick >= quiz.options.size()) {
        return std::nullopt;
    }
    std::array<std::size_t, 4> sorted = quiz.options;
    std::sort(sorted.begin(), sorted.end());
    if (sorted.back() >= quiz_words.size() || std::adjacent_find(sorted.begin(), sorted.end()) != sorted.end() ||
        std::find(sorted.begin(), sorted.end(), quiz.answer) == sorted.end()) {
        return std::nullopt;
    }
    return result;
}

inline std::string quiz_image(const Quiz& quiz) {
    return aurebesh::render_png(quiz_words[quiz.answer], "AurebeshBasic.otf", aurebesh::Look{});
}

// The game as a message. The notes are the guesses so far, one per line, and
// always come after the first blank line (quiz_notes reads them back).
inline dpp::message quiz_message(const Quiz& quiz, const std::string& notes, bool solved, const std::string& png) {
    std::string description(quiz_question);
    if (!solved && quiz.wrong == 0 && quiz.streak >= 2) {
        description += "\n" + std::string(theme::icon::streak) + " **" + std::to_string(quiz.streak) + " in a row** so far";
    }
    if (!notes.empty()) description += "\n\n" + notes;

    dpp::message message;
    message.add_embed(dpp::embed()
        .set_title(theme::titled(theme::icon::quiz, "Aurebesh Quiz"))
        .set_description(description)
        .set_color(theme::blue)
        .set_image("attachment://quiz.png")
        .set_footer(dpp::embed_footer().set_text("Stuck? /alphabet shows every letter.")));
    message.add_file("quiz.png", png, "image/png");

    dpp::component answers;
    for (std::size_t i = 0; i < quiz.options.size(); ++i) {
        const bool right = quiz.options[i] == quiz.answer;
        const bool crossed_off = quiz.wrong & (1u << i);
        dpp::component button = dpp::component()
            .set_label(std::string(quiz_words[quiz.options[i]]))
            .set_style(crossed_off ? dpp::cos_danger : solved && right ? dpp::cos_success : dpp::cos_secondary)
            .set_id(quiz_id(quiz, i));
        button.set_disabled(solved || crossed_off);
        answers.add_component(button);
    }
    message.add_component(answers);

    if (solved) {
        message.add_component(dpp::component().add_component(dpp::component()
            .set_label("Next Word").set_emoji(std::string(theme::icon::quiz)).set_style(dpp::cos_primary)
            .set_id(quiz_next_id(quiz.streak))));
    }
    return message;
}

inline void quiz_command(const dpp::interaction_create_t& event, std::size_t streak = 0) {
    const Quiz quiz = new_quiz(streak);
    event.reply(quiz_message(quiz, "", false, quiz_image(quiz)));
}

struct QuizTurn {
    Quiz quiz;  // when solved, its streak is the one Next Word carries on
    std::string notes;
    bool solved = false;
};

// What one pick does to the game. Every wrong pick is crossed off; a right
// pick on the first guess grows the streak, and any other right pick ends it.
inline QuizTurn play_quiz_pick(Quiz quiz, std::size_t pick, std::string notes, const std::string& guesser) {
    if (!notes.empty()) notes += "\n";
    const std::string word(quiz_words[quiz.options[pick]]);
    if (quiz.options[pick] != quiz.answer) {
        quiz.wrong |= 1u << pick;
        notes += theme::titled(theme::icon::error, "**" + guesser + "** guessed **" + word + "**");
        return {quiz, notes, false};
    }

    notes += std::string(theme::icon::correct) + " **" + guesser + "** read it: **" + word + "**";
    const std::size_t streak = quiz.wrong == 0 ? std::min(quiz.streak + 1, max_streak) : 0;
    if (streak >= 2) {
        notes += "\n" + std::string(theme::icon::streak) + " **" + std::to_string(streak) + " in a row** read on the first guess!";
    } else if (quiz.streak >= 2) {
        notes += "\n" + std::string(theme::icon::streak_over) + " The streak of **" + std::to_string(quiz.streak) + "** ends here.";
    }
    quiz.streak = streak;
    return {quiz, notes, true};
}

// The guesses so far, read back out of the game's message.
inline std::string quiz_notes(const dpp::message& message) {
    if (message.embeds.empty()) return {};
    const std::string& description = message.embeds.front().description;
    const std::size_t start = description.find("\n\n");
    return start == std::string::npos ? std::string() : description.substr(start + 2);
}

// An answer button or Next Word. Returns false if the id is not the quiz's.
inline bool quiz_control(const dpp::interaction_create_t& event, std::string_view id) {
    if (id.starts_with(quiz_next)) {
        const std::optional<std::size_t> streak = parse_quiz_next_id(id);
        if (!streak) return false;
        quiz_command(event, *streak);
        return true;
    }
    const std::optional<QuizPick> pick = parse_quiz_id(id);
    if (!pick) return false;

    if (pick->quiz.wrong & (1u << pick->pick)) {
        reply_notice(event, "Someone already guessed that one.");
        return true;
    }
    const QuizTurn turn = play_quiz_pick(pick->quiz, pick->pick, quiz_notes(event.command.msg), display_name(event.command));
    event.reply(dpp::ir_update_message, quiz_message(turn.quiz, turn.notes, turn.solved, quiz_image(turn.quiz)));
    return true;
}

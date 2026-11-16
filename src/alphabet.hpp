// alphabet.hpp
//
// /alphabet and /aurebesh: every Aurebesh letter's name, and the chart.

#pragma once

#include <iterator>
#include <string>
#include <string_view>

#include <dpp/dpp.h>

#include "theme.hpp"

struct Letter {
    std::string_view latin;  // what it stands for, such as "A" or "CH"
    std::string_view name;   // its Aurebesh name, such as "Aurek"
};

inline constexpr Letter letters[] {
    {"A", "Aurek"}, {"B", "Besh"},  {"C", "Cresh"}, {"D", "Dorn"},  {"E", "Esk"},   {"F", "Forn"},  {"G", "Grek"},
    {"H", "Herf"},  {"I", "Isk"},   {"J", "Jenth"}, {"K", "Krill"}, {"L", "Leth"},  {"M", "Mern"},  {"N", "Nern"},
    {"O", "Osk"},   {"P", "Peth"},  {"Q", "Qek"},   {"R", "Resh"},  {"S", "Senth"}, {"T", "Trill"}, {"U", "Usk"},
    {"V", "Vev"},   {"W", "Wesk"},  {"X", "Xesh"},  {"Y", "Yirt"},  {"Z", "Zerek"},
};

// Letter pairs that have an Aurebesh letter of their own.
inline constexpr Letter letter_pairs[] {
    {"CH", "Cherek"}, {"AE", "Enth"}, {"EO", "Onith"}, {"KH", "Krenth"},
    {"NG", "Nen"},    {"OO", "Orenth"}, {"SH", "Sen"}, {"TH", "Thesh"},
};

inline dpp::message alphabet_message() {
    // Three columns of nine letters or fewer.
    const auto column = [](std::size_t begin, std::size_t end) {
        std::string lines;
        for (std::size_t i = begin; i < end; ++i) {
            lines += "`" + std::string(letters[i].latin) + "` " + std::string(letters[i].name) + "\n";
        }
        return lines;
    };
    std::string pairs;
    for (const Letter& pair : letter_pairs) {
        if (!pairs.empty()) pairs += "  ·  ";
        pairs += "`" + std::string(pair.latin) + "` " + std::string(pair.name);
    }

    return dpp::message().add_embed(dpp::embed()
        .set_title(theme::titled(theme::icon::alphabet, "Aurebesh Alphabet"))
        .set_description("Every Aurebesh letter has a name, from **Aurek** to **Zerek**.")
        .add_field("A–I", column(0, 9), true)
        .add_field("J–R", column(9, 18), true)
        .add_field("S–Z", column(18, std::size(letters)), true)
        .add_field("Letter Pairs", pairs)
        .set_color(theme::blue)
        .set_image(theme::images + "aurebesh%20basic%20alphabet.jpg"));
}

inline void alphabet_command(const dpp::slashcommand_t& event) {
    static const dpp::message reply = alphabet_message();  // never changes
    event.reply(reply);
}

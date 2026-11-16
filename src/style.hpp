// style.hpp
//
// The colors and backgrounds a translation can be restyled with, and how a
// translation's current style is written into its buttons' ids. The bot keeps
// no state: everything it needs to redraw a translation is in the message.

#pragma once

#include <array>
#include <charconv>
#include <cstdint>
#include <optional>
#include <string>
#include <string_view>

#include "render_aurebesh.hpp"
#include "scripts.hpp"
#include "theme.hpp"

struct Swatch {
    std::string_view name;
    std::string_view emoji;
    std::uint32_t value;  // 0xRRGGBB
};

// The first color means "whatever the script uses".
inline constexpr std::array colors {
    Swatch{"Script Color",       "✨", 0},
    Swatch{"Kyber Blue",         "🔵", theme::blue},
    Swatch{"Jedi Green",         "🟢", 0x3DDC6A},
    Swatch{"Sith Red",           "🔴", 0xE0393E},
    Swatch{"Mace Windu Purple",  "🟣", 0xA45EE5},
    Swatch{"Temple Guard Gold",  "🟡", 0xF2C53D},
    Swatch{"Mandalorian Amber",  "🟠", 0xE8A33D},
    Swatch{"Kamino Teal",        "🌊", 0x3CC8B4},
    Swatch{"Ahsoka White",       "⚪", 0xE6EDF5},
};

struct Backdrop {
    std::string_view name;
    std::string_view emoji;
    std::uint32_t value;  // 0xRRGGBB
    bool transparent;
};

inline constexpr std::array backgrounds {
    Backdrop{"Holonet Navy",   "🌌", theme::navy, false},
    Backdrop{"Deep Space",     "🌑", 0x020306, false},
    Backdrop{"Imperial Gray",  "🔩", 0x22262C, false},
    Backdrop{"Sith Shadow",    "🔻", 0x1C0709, false},
    Backdrop{"Dagobah Swamp",  "🐸", 0x0B1A10, false},
    Backdrop{"Tatooine Sand",  "🌵", 0xE8D5B0, false},
    Backdrop{"Datapad White",  "📄", 0xF2F5F9, false},
    Backdrop{"Transparent",    "🔲", 0,        true},
};

struct Style {
    std::size_t script = 1;  // index into scripts
    std::size_t color = 0;
    std::size_t background = 0;
    bool glow = true;

    const Script& script_info() const { return scripts[script]; }

    std::uint32_t color_value() const {
        return color == 0 ? script_info().color : colors[color].value;
    }

    aurebesh::Look look() const {
        return {color_value(), backgrounds[background].value, backgrounds[background].transparent, glow};
    }

    bool operator==(const Style&) const = default;
};

// /translate draws Basic Aurebesh, the same as /translate_basic, so it starts
// from that entry and the script menu shows it as chosen.
inline Style default_style(const Script& script) {
    Style style;
    for (std::size_t i = 0; i < scripts.size(); ++i) {
        const bool match = script.group.empty() ? !scripts[i].group.empty() && scripts[i].font == script.font
                                                : &scripts[i] == &script;
        if (match) {
            style.script = i;
            break;
        }
    }
    return style;
}

// A style as text, such as "1.0.0.1": script, color, background, glow.
inline std::string encode_style(const Style& style) {
    return std::to_string(style.script) + "." + std::to_string(style.color) + "." +
           std::to_string(style.background) + "." + (style.glow ? "1" : "0");
}

inline std::optional<Style> decode_style(std::string_view text) {
    std::size_t parts[4];
    for (std::size_t i = 0; i < std::size(parts); ++i) {
        if (i > 0) {
            if (text.empty() || text.front() != '.') return std::nullopt;
            text.remove_prefix(1);
        }
        const auto [end, error] = std::from_chars(text.data(), text.data() + text.size(), parts[i]);
        if (error != std::errc()) return std::nullopt;
        text.remove_prefix(static_cast<std::size_t>(end - text.data()));
    }
    if (!text.empty() || parts[0] >= scripts.size() || scripts[parts[0]].group.empty() ||
        parts[1] >= colors.size() || parts[2] >= backgrounds.size() || parts[3] > 1) {
        return std::nullopt;
    }
    return Style{parts[0], parts[1], parts[2], parts[3] == 1};
}

// Finds a menu choice by its value, which is the choice's index as text.
inline std::optional<std::size_t> parse_index(std::string_view text, std::size_t count) {
    std::size_t index = 0;
    const auto [end, error] = std::from_chars(text.data(), text.data() + text.size(), index);
    if (error != std::errc() || end != text.data() + text.size() || index >= count) return std::nullopt;
    return index;
}

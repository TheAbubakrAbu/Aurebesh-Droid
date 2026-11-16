// scripts.hpp
//
// Every translation command lives in this one table. main.cpp registers and
// dispatches commands from it, /help and the Customize menu list it, and
// CMakeLists.txt reads the font names out of it to compile those fonts into the
// binary. Adding a script only takes a new line here.

#pragma once

#include <array>
#include <cstdint>
#include <string>
#include <string_view>

#include "theme.hpp"

struct Script {
    std::string_view command;  // slash command name
    std::string_view name;     // shown in titles, menus, and the command's description
    std::string_view group;    // heading it is listed under in /help; empty for /translate
    std::string_view font;     // font file in src/assets/fonts
    std::uint32_t color;       // glyph glow and embed color, 0xRRGGBB
};

// Each family of scripts glows in its own color.
namespace script_color {
inline constexpr std::uint32_t aurebesh = theme::blue;
inline constexpr std::uint32_t mandoa = 0xE8A33D;     // amber
inline constexpr std::uint32_t outer_rim = 0x3CC8B4;  // teal
inline constexpr std::uint32_t sith = 0xE0393E;       // red
}  // namespace script_color

inline constexpr std::array scripts {
    Script{"translate",            "Basic Aurebesh",   "",          "AurebeshBasic.otf",     script_color::aurebesh},

    Script{"translate_basic",      "Basic Aurebesh",   "Aurebesh",  "AurebeshBasic.otf",     script_color::aurebesh},
    Script{"translate_core",       "Core Aurebesh",    "Aurebesh",  "AurebeshCore.otf",      script_color::aurebesh},
    Script{"translate_cantina",    "Cantina Aurebesh", "Aurebesh",  "AurebeshCantina.otf",   script_color::aurebesh},
    Script{"translate_nexus",      "Nexus Aurebesh",   "Aurebesh",  "AurebeshEquinox.otf",   script_color::aurebesh},
    Script{"translate_droid",      "Droid Aurebesh",   "Aurebesh",  "AurebeshDroid.otf",     script_color::aurebesh},
    Script{"translate_pixel",      "Pixel Aurebesh",   "Aurebesh",  "AurebeshPixel.otf",     script_color::aurebesh},

    Script{"translate_mandoa_new", "New Mando'a",      "Mando'a",   "MandoNew.otf",          script_color::mandoa},
    Script{"translate_mandoa_old", "Old Mando'a",      "Mando'a",   "MandoOld.otf",          script_color::mandoa},

    Script{"translate_outerrim",   "Outer Rim",        "Outer Rim", "OuterRimBasic.otf",     script_color::outer_rim},
    Script{"translate_tongue",     "Old Tongue",       "Outer Rim", "OuterRimTongue.otf",    script_color::outer_rim},
    Script{"translate_sith",       "Sith Script",      "Outer Rim", "OuterRimSith.otf",      script_color::sith},
    Script{"translate_geonosian",  "Geonosian",        "Outer Rim", "OuterRimHive.otf",      script_color::outer_rim},
    Script{"translate_trade",      "Trade Federation", "Outer Rim", "OuterRimTrade.otf",     script_color::outer_rim},
    Script{"translate_protobesh",  "Protobesh",        "Outer Rim", "OuterRimProtobesh.otf", script_color::outer_rim},
};

inline const Script* find_script(std::string_view command) {
    for (const Script& script : scripts) {
        if (script.command == command) return &script;
    }
    return nullptr;
}

// The slash command's description, such as "Translate English into Core Aurebesh".
inline std::string script_description(const Script& script) {
    return "Translate English into " + std::string(script.name);
}

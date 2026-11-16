// embedded_fonts.hpp
//
// The fonts are compiled into the binary, so it runs from any folder with no
// font files next to it. cmake/embed_fonts.cmake generates the definition.

#pragma once

#include <cstddef>
#include <span>
#include <string_view>

struct EmbeddedFont {
    std::string_view name;  // file name, such as AurebeshBasic.otf
    const unsigned char* data;
    std::size_t size;
};

std::span<const EmbeddedFont> embedded_fonts();

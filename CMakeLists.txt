# CMakeLists.txt

# One-time setup:
#   chmod +x run.sh
#
# To build and run Aurebesh Droid (every time after that):
#   ./run.sh

cmake_minimum_required(VERSION 3.16)
project(AurebeshDroid LANGUAGES CXX)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# --- Language standard ---
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# --- Smaller release binary ---
# Give every function its own section so the linker can drop the parts of D++
# this bot never calls (see the link options below). Set before D++ is added
# so it applies there too.
if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
  add_compile_options("$<$<CONFIG:Release>:-ffunction-sections>" "$<$<CONFIG:Release>:-fdata-sections>")
endif()

# --- Dependencies ---
# Only the parts of D++ this bot uses: no voice, coroutines, tests, or install rules.
find_package(OpenSSL REQUIRED)
find_package(ZLIB REQUIRED)  # D++ needs it too; the renderer uses it to write PNGs
set(BUILD_SHARED_LIBS OFF CACHE BOOL "" FORCE)
set(BUILD_TESTING OFF CACHE BOOL "" FORCE)
set(BUILD_VOICE_SUPPORT OFF CACHE BOOL "" FORCE)
set(DPP_NO_CORO ON CACHE BOOL "" FORCE)
set(DPP_BUILD_TEST OFF CACHE BOOL "" FORCE)
set(DPP_INSTALL OFF CACHE BOOL "" FORCE)
set(RUN_LDCONFIG OFF CACHE BOOL "" FORCE)
add_subdirectory(extern/DPP EXCLUDE_FROM_ALL)

# --- Fonts ---
# Every font named in src/scripts.hpp is compiled into the binary, so the bot
# needs no font files at runtime. A script naming a missing font fails the build.
set_property(DIRECTORY APPEND PROPERTY CMAKE_CONFIGURE_DEPENDS "${CMAKE_SOURCE_DIR}/src/scripts.hpp")
file(STRINGS "${CMAKE_SOURCE_DIR}/src/scripts.hpp" _script_lines REGEX "\\.(otf|ttf)\"")
string(REGEX MATCHALL "[A-Za-z0-9_-]+\\.(otf|ttf)" FONT_NAMES "${_script_lines}")
list(REMOVE_DUPLICATES FONT_NAMES)
set(FONT_FILES "")
foreach(name IN LISTS FONT_NAMES)
  list(APPEND FONT_FILES "${CMAKE_SOURCE_DIR}/src/assets/fonts/${name}")
endforeach()
string(REPLACE ";" "|" _font_arg "${FONT_FILES}")

set(EMBEDDED_FONTS_CPP "${CMAKE_BINARY_DIR}/embedded_fonts.cpp")
add_custom_command(
  OUTPUT "${EMBEDDED_FONTS_CPP}"
  COMMAND "${CMAKE_COMMAND}" "-DFONTS=${_font_arg}" "-DOUTPUT=${EMBEDDED_FONTS_CPP}"
          -P "${CMAKE_SOURCE_DIR}/cmake/embed_fonts.cmake"
  DEPENDS ${FONT_FILES} "${CMAKE_SOURCE_DIR}/cmake/embed_fonts.cmake"
  COMMENT "Embedding fonts"
  VERBATIM
)

# --- Executable ---
add_executable(AurebeshDroid src/main.cpp src/self_test.cpp src/render_aurebesh.cpp "${EMBEDDED_FONTS_CPP}")
target_include_directories(AurebeshDroid PRIVATE src)
target_link_libraries(AurebeshDroid PRIVATE dpp OpenSSL::SSL OpenSSL::Crypto ZLIB::ZLIB)

# Drop unused sections and strip symbols from Linux release builds.
if(CMAKE_SYSTEM_NAME STREQUAL "Linux")
  target_link_options(AurebeshDroid PRIVATE "$<$<CONFIG:Release>:LINKER:--gc-sections,-s>")
endif()

# --- Environment handling ---
set(_token "")
if(EXISTS "${CMAKE_SOURCE_DIR}/.env")
  file(READ "${CMAKE_SOURCE_DIR}/.env" ENV_FILE_TEXT)
  string(REGEX MATCH "DISCORD_TOKEN[ \t]*=[ \t]*([^\n\r]*)" _match "${ENV_FILE_TEXT}")
  set(_token "${CMAKE_MATCH_1}")
  string(STRIP "${_token}" _token)
endif()

if(_token)
  add_custom_target(run
    COMMAND "${CMAKE_COMMAND}" -E env
            "DISCORD_TOKEN=${_token}"
            "$<TARGET_FILE:AurebeshDroid>"
    DEPENDS AurebeshDroid
    WORKING_DIRECTORY "${CMAKE_BINARY_DIR}"
    VERBATIM
  )
else()
  add_custom_target(run
    COMMAND "$<TARGET_FILE:AurebeshDroid>"
    DEPENDS AurebeshDroid
    WORKING_DIRECTORY "${CMAKE_BINARY_DIR}"
    VERBATIM
  )
endif()

# --- macOS rpath handling ---
if(APPLE)
    set_target_properties(AurebeshDroid PROPERTIES
        BUILD_RPATH "${CMAKE_PREFIX_PATH}/lib;/opt/homebrew/lib;/usr/local/lib")
endif()

# CmakeLists.txt

# To build and run Aurebesh Droid:
#   chmod +x run-dev.sh run-release.sh
#   ./run-dev.sh      # builds and runs in Debug mode
#   ./run-release.sh  # builds and runs in Release mode

cmake_minimum_required(VERSION 3.10)
project(AurebeshDroid LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

file(GLOB_RECURSE SOURCES CONFIGURE_DEPENDS src/*.cpp)

# Exclude all dotfiles and swap/backup files
list(FILTER SOURCES EXCLUDE REGEX "/\\..*")          # Excludes .DS_Store, .gitkeep, etc.
list(FILTER SOURCES EXCLUDE REGEX ".*~$")            # Excludes editor backup files (e.g., file.cpp~)
list(FILTER SOURCES EXCLUDE REGEX ".*\\.swp$")       # Excludes Vim swap files

add_executable(AurebeshDroid ${SOURCES})

if(EXISTS "${CMAKE_SOURCE_DIR}/.env")
    file(READ "${CMAKE_SOURCE_DIR}/.env" ENV_FILE_TEXT)
    string(REGEX MATCH "^[ \t]*DISCORD_TOKEN[ \t]*=[ \t]*([^\n\r]*)"
           _match "${ENV_FILE_TEXT}")
    set(DISCORD_TOKEN_VALUE "${CMAKE_MATCH_1}")
    string(STRIP "${DISCORD_TOKEN_VALUE}" DISCORD_TOKEN_VALUE)

    add_custom_target(run
        COMMAND ${CMAKE_COMMAND} -E env
                DISCORD_TOKEN=${DISCORD_TOKEN_VALUE}
                $<TARGET_FILE:AurebeshDroid>
        DEPENDS AurebeshDroid
        WORKING_DIRECTORY ${CMAKE_BINARY_DIR})
endif()

find_package(OpenSSL REQUIRED)
set(BUILD_TESTING OFF)
add_subdirectory(extern/DPP) 

target_link_libraries(AurebeshDroid PRIVATE dpp OpenSSL::SSL OpenSSL::Crypto)

if(APPLE)
    set_target_properties(AurebeshDroid PROPERTIES
        BUILD_RPATH "/opt/homebrew/lib;/usr/local/lib")
endif()
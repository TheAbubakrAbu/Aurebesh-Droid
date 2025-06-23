# CmakeLists.txt

cmake_minimum_required(VERSION 3.10)
project(AurebeshDroid LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

file(GLOB_RECURSE SOURCES CONFIGURE_DEPENDS src/*.cpp)
add_executable(AurebeshDroid ${SOURCES})

file(READ "${CMAKE_SOURCE_DIR}/.env" ENV_FILE_TEXT)

string(REGEX MATCH "^[ \t]*DISCORD_TOKEN[ \t]*=[ \t]*([^\n\r]*)"
       _match "${ENV_FILE_TEXT}")
set(DISCORD_TOKEN_VALUE "${CMAKE_MATCH_1}")
string(STRIP "${DISCORD_TOKEN_VALUE}" DISCORD_TOKEN_VALUE)

if(DISCORD_TOKEN_VALUE STREQUAL "")
    message(FATAL_ERROR "Could not find DISCORD_TOKEN in .env")
endif()

add_custom_target(run
    COMMAND ${CMAKE_COMMAND} -E env
            DISCORD_TOKEN=${DISCORD_TOKEN_VALUE}
            ./AurebeshDroid
    DEPENDS AurebeshDroid
    WORKING_DIRECTORY ${CMAKE_BINARY_DIR})

find_package(OpenSSL REQUIRED)
find_package(dpp CONFIG REQUIRED)

target_link_libraries(AurebeshDroid
    PRIVATE
        dpp::dpp
        OpenSSL::SSL
        OpenSSL::Crypto)

if(APPLE)
    set_target_properties(AurebeshDroid PROPERTIES
        BUILD_RPATH "/opt/homebrew/lib;/usr/local/lib")
endif()
# CmakeLists.txt

# One-time setup:
#   chmod +x run-dev.sh
#
# To build and run Aurebesh Droid (every time after that):
#   ./run-dev.sh

cmake_minimum_required(VERSION 3.10)
project(AurebeshDroid LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

file(GLOB_RECURSE SOURCES CONFIGURE_DEPENDS src/*.cpp)
include_directories(src)

list(FILTER SOURCES EXCLUDE REGEX "/\\..*")
list(FILTER SOURCES EXCLUDE REGEX ".*~$")
list(FILTER SOURCES EXCLUDE REGEX ".*\\.swp$")

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
set(BUILD_SHARED_LIBS OFF)
add_subdirectory(extern/DPP) 

target_link_libraries(AurebeshDroid PRIVATE dpp OpenSSL::SSL OpenSSL::Crypto)

if(APPLE)
    set_target_properties(AurebeshDroid PROPERTIES
        BUILD_RPATH "/opt/homebrew/lib;/usr/local/lib")
endif()
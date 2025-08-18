# CmakeLists.txt

# One-time setup:
#   chmod +x run-dev.sh
#
# To build and run Aurebesh Droid (every time after that):
#   ./run-dev.sh

cmake_minimum_required(VERSION 3.16)  # bump for modern features
project(AurebeshDroid LANGUAGES CXX)

# --- Language standard ---
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# --- Source discovery ---
file(GLOB_RECURSE SOURCES CONFIGURE_DEPENDS src/*.cpp)
list(FILTER SOURCES EXCLUDE REGEX "/\\..*")
list(FILTER SOURCES EXCLUDE REGEX ".*~$")
list(FILTER SOURCES EXCLUDE REGEX ".*\\.swp$")

# --- Executable ---
add_executable(AurebeshDroid ${SOURCES})
target_include_directories(AurebeshDroid PRIVATE src)

# --- Environment handling ---
if(EXISTS "${CMAKE_SOURCE_DIR}/.env")
    file(READ "${CMAKE_SOURCE_DIR}/.env" ENV_FILE_TEXT)

    # Match DISCORD_TOKEN line anywhere in the file, not just first line
    string(REGEX MATCH "DISCORD_TOKEN[ \t]*=[ \t]*([^\n\r]*)" _match "${ENV_FILE_TEXT}")
    set(DISCORD_TOKEN_VALUE "${CMAKE_MATCH_1}")
    string(STRIP "${DISCORD_TOKEN_VALUE}" DISCORD_TOKEN_VALUE)

    if(DISCORD_TOKEN_VALUE)
        add_custom_target(run
            COMMAND ${CMAKE_COMMAND} -E env
                    DISCORD_TOKEN=${DISCORD_TOKEN_VALUE}
                    $<TARGET_FILE:AurebeshDroid>
            DEPENDS AurebeshDroid
            WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
        )
    endif()
endif()

# --- Dependencies ---
find_package(OpenSSL REQUIRED)
set(BUILD_TESTING OFF CACHE BOOL "" FORCE)
set(BUILD_SHARED_LIBS OFF CACHE BOOL "" FORCE)
add_subdirectory(extern/DPP)

target_link_libraries(AurebeshDroid PRIVATE dpp OpenSSL::SSL OpenSSL::Crypto)

# --- macOS rpath handling ---
if(APPLE)
    set_target_properties(AurebeshDroid PROPERTIES
        BUILD_RPATH "${CMAKE_PREFIX_PATH}/lib;/opt/homebrew/lib;/usr/local/lib")
endif()
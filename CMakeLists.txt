cmake_minimum_required(VERSION 3.5)
project(AurebeshDroid LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

file(GLOB_RECURSE SOURCES CONFIGURE_DEPENDS src/*.cpp)
add_executable(AurebeshDroid ${SOURCES})

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
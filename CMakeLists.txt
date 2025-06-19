cmake_minimum_required(VERSION 3.22)
project(AurebeshDroid LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Add source files
file(GLOB_RECURSE SOURCES CONFIGURE_DEPENDS src/*.cpp)

# Find packages
find_package(OpenSSL REQUIRED)
find_package(dpp CONFIG REQUIRED)

# Build the executable
add_executable(AurebeshDroid ${SOURCES})

# dotenv-cpp is header-only, so just add its include path
target_include_directories(AurebeshDroid
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/extern/cpp-dotenv/include
)

# Link libraries
target_link_libraries(AurebeshDroid
    PRIVATE
        dpp::dpp
        OpenSSL::SSL
        OpenSSL::Crypto
)

# Add rpath for macOS so it finds libdpp.dylib
if(APPLE)
    set_target_properties(AurebeshDroid PROPERTIES
        BUILD_RPATH "/usr/local/lib"
    )
endif()
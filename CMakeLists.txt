cmake_minimum_required(VERSION 3.22)
project(innuendo LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# -------------------------------
# Build options
# -------------------------------
option(BUILD_GUI   "Build GUI client" OFF)
option(BUILD_CLI   "Build CLI client" ON)
option(BUILD_RELAY "Build relay node" ON)
option(BUILD_TESTS "Build tests" OFF)
option(EMBED_TOR   "Embed Tor instead of using system daemon" OFF)

# -------------------------------
# Output directories
# -------------------------------
# Libraries
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
# Executables
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

# Per-configuration support (Debug/Release)
foreach(CONFIG IN ITEMS Debug Release RelWithDebInfo MinSizeRel)
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_${CONFIG} ${CMAKE_BINARY_DIR}/bin)
    set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY_${CONFIG} ${CMAKE_BINARY_DIR}/lib)
    set(CMAKE_LIBRARY_OUTPUT_DIRECTORY_${CONFIG} ${CMAKE_BINARY_DIR}/lib)
endforeach()

# -------------------------------
# OS-specific prefix
# -------------------------------
if(APPLE)
    if(CMAKE_OSX_ARCHITECTURES MATCHES "arm64")
        set(HOMEBREW_PREFIX "/opt/homebrew")
    else()
        set(HOMEBREW_PREFIX "/usr/local")
    endif()
    list(APPEND CMAKE_PREFIX_PATH "${HOMEBREW_PREFIX}")
elseif(UNIX)
    set(HOMEBREW_PREFIX "/usr")
endif()

# -------------------------------
# Boost
# -------------------------------
find_package(Boost CONFIG REQUIRED)
if(NOT Boost_FOUND)
    message(FATAL_ERROR "Boost not found. Please install Boost or adjust BOOST_ROOT")
endif()

# -------------------------------
# Core library always built
# -------------------------------
add_subdirectory(src/core)

# Optional components
if(BUILD_CLI)
    add_subdirectory(src/cli)
endif()

if(BUILD_RELAY)
    add_subdirectory(src/relay)
endif()

if(BUILD_GUI)
    add_subdirectory(src/gui)
endif()

if(BUILD_TESTS)
    add_subdirectory(src/test)
endif()


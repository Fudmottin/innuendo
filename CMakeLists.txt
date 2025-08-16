cmake_minimum_required(VERSION 3.22)
project(innuendo LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Options
option(BUILD_GUI "Build GUI client" OFF)
option(BUILD_CLI "Build CLI client" ON)
option(BUILD_RELAY "Build relay node" ON)
option(BUILD_TESTS "Build tests" OFF)
option(EMBED_TOR "Embed Tor instead of using system daemon" ON)

# Include FetchContent once
include(FetchContent)

# JUCE setup (optional)
if(BUILD_GUI)
    add_subdirectory(JUCE)
endif()

# Catch2 Test Harness (optional)
if(BUILD_TESTS)
    FetchContent_Declare(
        catch2
        GIT_REPOSITORY https://github.com/catchorg/Catch2.git
        GIT_TAG v3.4.0
    )
    FetchContent_MakeAvailable(catch2)
endif()

# Tor embedding setup (optional)
if(EMBED_TOR)
    FetchContent_Declare(
        tor
        GIT_REPOSITORY https://git.torproject.org/tor.git
        GIT_TAG release-0.4.8
    )
    FetchContent_MakeAvailable(tor)
endif()

# Homebrew prefixes on macOS
if(APPLE)
    list(APPEND CMAKE_PREFIX_PATH
        /opt/homebrew
        /usr/local
    )
endif()

# Find Boost
find_package(Boost 1.81 REQUIRED COMPONENTS system)

if(NOT Boost_FOUND)
    message(FATAL_ERROR "Boost not found; please install via Homebrew or your package manager.")
endif()

message(STATUS "Boost include dirs: ${Boost_INCLUDE_DIRS}")
message(STATUS "Boost libraries: ${Boost_LIBRARIES}")

# Include directory
set(INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/src/include)

# Core sources
file(GLOB_RECURSE CORE_SRC CONFIGURE_DEPENDS src/core/*.cpp src/core/*.h)

add_library(innuendo_core ${CORE_SRC})
target_include_directories(innuendo_core PUBLIC
    ${INCLUDE_DIR}
    ${Boost_INCLUDE_DIRS}
)
target_link_libraries(innuendo_core PUBLIC ${Boost_LIBRARIES})
target_compile_features(innuendo_core PUBLIC cxx_std_20)

# CLI target
if(BUILD_CLI)
    file(GLOB_RECURSE CLI_SRC CONFIGURE_DEPENDS src/cli/*.cpp src/cli/*.h)
    if(CLI_SRC)
        add_executable(innuendo_cli ${CLI_SRC})
        target_include_directories(innuendo_cli PRIVATE ${INCLUDE_DIR})
        target_compile_features(innuendo_cli PRIVATE cxx_std_20)
        target_link_libraries(innuendo_cli PRIVATE innuendo_core)
    endif()
endif()

# Relay target
if(BUILD_RELAY)
    file(GLOB_RECURSE RELAY_SRC CONFIGURE_DEPENDS src/relay/*.cpp src/relay/*.h)
    if(RELAY_SRC)
        add_executable(innuendod ${RELAY_SRC})
        target_include_directories(innuendod PRIVATE ${INCLUDE_DIR})
        target_compile_features(innuendod PRIVATE cxx_std_20)
        target_link_libraries(innuendod PRIVATE innuendo_core)
    endif()
endif()

# GUI target (optional)
if(BUILD_GUI)
    file(GLOB_RECURSE GUI_SRC CONFIGURE_DEPENDS src/gui/*.cpp src/gui/*.h)
    if(GUI_SRC)
        add_executable(innuendo_gui ${GUI_SRC})
        target_include_directories(innuendo_gui PRIVATE ${INCLUDE_DIR})
        target_compile_features(innuendo_gui PRIVATE cxx_std_20)
        target_link_libraries(innuendo_gui PRIVATE innuendo_core
                              juce::juce_gui_basics juce::juce_gui_extra)
    endif()
endif()

# Tests (optional)
if(BUILD_TESTS)
    enable_testing()
    file(GLOB_RECURSE TEST_SRC CONFIGURE_DEPENDS src/test/*.cpp src/test/*.h)
    if(TEST_SRC)
        add_executable(innuendo_tests ${TEST_SRC})
        target_include_directories(innuendo_tests PRIVATE ${INCLUDE_DIR})
        target_compile_features(innuendo_tests PRIVATE cxx_std_20)
        target_link_libraries(innuendo_tests PRIVATE innuendo_core Catch2::Catch2WithMain)
        add_test(NAME innuendo_tests COMMAND innuendo_tests)
    endif()
endif()


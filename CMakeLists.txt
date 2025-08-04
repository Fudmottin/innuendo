cmake_minimum_required(VERSION 3.16)
project(innuendo LANGUAGES C CXX)

# Use target_compile_features instead of global CMAKE_CXX_STANDARD
# This ensures proper interface propagation and better CMake semantics
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Options
option(BUILD_GUI "Build GUI client" ON)
option(BUILD_CLI "Build CLI client" ON)
option(BUILD_RELAY "Build relay node" ON)
option(BUILD_TESTS "Build tests" ON)
option(EMBED_TOR "Embed Tor instead of using system daemon" ON)

# Include FetchContent once
include(FetchContent)

# JUCE setup (git submodule)
add_subdirectory(JUCE)

# Catch2 Test Harness for TDD
FetchContent_Declare(
  catch2
  GIT_REPOSITORY https://github.com/catchorg/Catch2.git
  GIT_TAG v3.4.0
)
FetchContent_MakeAvailable(catch2)

# Tor embedding setup
if(EMBED_TOR)
    FetchContent_Declare(
        tor
        GIT_REPOSITORY https://git.torproject.org/tor.git
        GIT_TAG release-0.4.8
    )
    FetchContent_MakeAvailable(tor)
endif()

# Common sources
file(GLOB_RECURSE COMMON_SRC CONFIGURE_DEPENDS src/common/*.cpp src/common/*.h)
add_library(innuendo_common ${COMMON_SRC})
target_include_directories(innuendo_common PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src>
)
target_compile_features(innuendo_common PUBLIC cxx_std_20)

# Tor library, add only if files exist
file(GLOB_RECURSE TOR_SRC CONFIGURE_DEPENDS src/tor/*.cpp src/tor/*.h)
if(TOR_SRC)
    add_library(innuendo_tor ${TOR_SRC})
    target_include_directories(innuendo_tor PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src>
    )
    target_compile_features(innuendo_tor PUBLIC cxx_std_20)
    target_link_libraries(innuendo_tor PUBLIC innuendo_common)
endif()

if(EMBED_TOR AND TARGET innuendo_tor)
    target_link_libraries(innuendo_common PUBLIC innuendo_tor)
endif()

# CLI target
if(BUILD_CLI)
    file(GLOB_RECURSE CLI_SRC CONFIGURE_DEPENDS src/cli/*.cpp src/cli/*.h)
    if(CLI_SRC)
        add_executable(innuendo_cli ${CLI_SRC})
        target_compile_features(innuendo_cli PRIVATE cxx_std_20)
        target_link_libraries(innuendo_cli PRIVATE innuendo_common innuendo_tor)
    endif()
endif()

# GUI target using JUCE
if(BUILD_GUI)
    file(GLOB_RECURSE GUI_SRC CONFIGURE_DEPENDS src/gui/*.cpp src/gui/*.h)
    if(GUI_SRC)
        add_executable(innuendo_gui ${GUI_SRC})
        target_compile_features(innuendo_gui PRIVATE cxx_std_20)
        target_link_libraries(innuendo_gui PRIVATE innuendo_common innuendo_tor juce::juce_gui_basics juce::juce_gui_extra)
    endif()
endif()

# Relay target
if(BUILD_RELAY)
    file(GLOB_RECURSE RELAY_SRC CONFIGURE_DEPENDS src/relay/*.cpp src/relay/*.h)
    if(RELAY_SRC)
        add_executable(innuendod ${RELAY_SRC})
        target_compile_features(innuendod PRIVATE cxx_std_20)
        target_link_libraries(innuendod PRIVATE innuendo_common innuendo_tor)
    endif()
endif()

# Tests
if(BUILD_TESTS)
    enable_testing()
    file(GLOB_RECURSE TEST_SRC CONFIGURE_DEPENDS src/test/*.cpp src/test/*.h)
    if(TEST_SRC)
        add_executable(innuendo_tests ${TEST_SRC})
        target_compile_features(innuendo_tests PRIVATE cxx_std_20)
        target_link_libraries(innuendo_tests PRIVATE innuendo_common innuendo_tor Catch2::Catch2WithMain)
        add_test(NAME innuendo_tests COMMAND innuendo_tests)
    endif()
endif()


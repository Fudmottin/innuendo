cmake_minimum_required(VERSION 3.22)
project(innuendo LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Suppress CMP0167 warning (use FindBoost even though deprecated)
if(POLICY CMP0167)
    cmake_policy(SET CMP0167 OLD)
endif()

# Build options
option(BUILD_GUI   "Build GUI client" OFF)
option(BUILD_CLI   "Build CLI client" ON)
option(BUILD_RELAY "Build relay node" ON)
option(BUILD_TESTS "Build tests" OFF)
option(EMBED_TOR   "Embed Tor instead of using system daemon" OFF)

# Homebrew prefixes on macOS
if(APPLE)
    list(APPEND CMAKE_PREFIX_PATH
        /opt/homebrew
        /usr/local
    )
endif()

# Boost
find_package(Boost 1.81 REQUIRED COMPONENTS system)
message(STATUS "Boost include dirs: ${Boost_INCLUDE_DIRS}")
message(STATUS "Boost libraries: ${Boost_LIBRARIES}")

# Readline (manual detection, avoids missing Config warning)
if(APPLE)
    list(APPEND CMAKE_PREFIX_PATH
        /opt/homebrew/opt/readline
        /usr/local/opt/readline
    )
endif()

find_path(READLINE_INCLUDE_DIR readline/readline.h
    HINTS /opt/homebrew/include /usr/local/include /usr/include
)
find_library(READLINE_LIB readline
    HINTS /opt/homebrew/lib /usr/local/lib /usr/lib
)
find_library(HISTORY_LIB history
    HINTS /opt/homebrew/lib /usr/local/lib /usr/lib
)

if(READLINE_INCLUDE_DIR AND READLINE_LIB AND HISTORY_LIB)
    message(STATUS "Found Readline: ${READLINE_LIB}")
    add_compile_definitions(HAVE_READLINE)
    set(Readline_FOUND TRUE)
    set(Readline_LIBRARIES ${READLINE_LIB} ${HISTORY_LIB})
    set(Readline_INCLUDE_DIRS ${READLINE_INCLUDE_DIR})
else()
    message(STATUS "Readline not found — falling back to std::getline")
    set(Readline_FOUND FALSE)
endif()

include(FetchContent)

# Optional deps
if(BUILD_GUI)
    add_subdirectory(JUCE)
endif()

if(BUILD_TESTS)
    FetchContent_Declare(
        catch2
        GIT_REPOSITORY https://github.com/catchorg/Catch2.git
        GIT_TAG v3.4.0
    )
    FetchContent_MakeAvailable(catch2)
endif()

if(EMBED_TOR)
    FetchContent_Declare(
        tor
        GIT_REPOSITORY https://git.torproject.org/tor.git
        GIT_TAG release-0.4.8
    )
    FetchContent_MakeAvailable(tor)
endif()

# Core library
set(INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/src/include)
file(GLOB_RECURSE CORE_SRC CONFIGURE_DEPENDS src/core/*.cpp src/core/*.h)

add_library(innuendo_core ${CORE_SRC})
target_include_directories(innuendo_core PUBLIC
    ${INCLUDE_DIR}
    ${Boost_INCLUDE_DIRS}
)
target_link_libraries(innuendo_core PUBLIC ${Boost_LIBRARIES})
target_compile_features(innuendo_core PUBLIC cxx_std_20)

# CLI
if(BUILD_CLI)
    file(GLOB_RECURSE CLI_SRC CONFIGURE_DEPENDS src/cli/*.cpp src/cli/*.h)
    if(CLI_SRC)
        add_executable(innuendo ${CLI_SRC})
        target_include_directories(innuendo PRIVATE ${INCLUDE_DIR})
        target_compile_features(innuendo PRIVATE cxx_std_20)
        target_link_libraries(innuendo PRIVATE innuendo_core)
        if(Readline_FOUND)
            target_include_directories(innuendo PRIVATE ${Readline_INCLUDE_DIRS})
            target_link_libraries(innuendo PRIVATE ${Readline_LIBRARIES})
        endif()
    endif()
endif()

# Relay
if(BUILD_RELAY)
    file(GLOB_RECURSE RELAY_SRC CONFIGURE_DEPENDS src/relay/*.cpp src/relay/*.h)
    if(RELAY_SRC)
        add_executable(innuendod ${RELAY_SRC})
        target_include_directories(innuendod PRIVATE ${INCLUDE_DIR})
        target_compile_features(innuendod PRIVATE cxx_std_20)
        target_link_libraries(innuendod PRIVATE innuendo_core)
    endif()
endif()

# GUI
if(BUILD_GUI)
    file(GLOB_RECURSE GUI_SRC CONFIGURE_DEPENDS src/gui/*.cpp src/gui/*.h)
    if(GUI_SRC)
        add_executable(innuendo_gui ${GUI_SRC})
        target_include_directories(innuendo_gui PRIVATE ${INCLUDE_DIR})
        target_compile_features(innuendo_gui PRIVATE cxx_std_20)
        target_link_libraries(innuendo_gui PRIVATE
            innuendo_core
            juce::juce_gui_basics
            juce::juce_gui_extra
        )
    endif()
endif()

# Tests
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


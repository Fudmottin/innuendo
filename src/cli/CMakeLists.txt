# src/cli/CMakeLists.txt

file(GLOB_RECURSE CLI_SRC CONFIGURE_DEPENDS *.cpp *.h)

# Homebrew ncurses prefix on macOS
if(APPLE)
    list(APPEND CMAKE_PREFIX_PATH "/opt/homebrew/opt/ncurses")
endif()

# Find wide-character ncurses (ncursesw)
find_path(NCURSESW_INCLUDE_DIR ncursesw/ncurses.h
    HINTS /opt/homebrew/include /usr/include /usr/local/include
)
find_library(NCURSESW_LIBRARY ncursesw
    HINTS /opt/homebrew/lib /usr/lib /usr/local/lib
)

if(NCURSESW_INCLUDE_DIR AND NCURSESW_LIBRARY)
    message(STATUS "Found ncursesw: ${NCURSESW_INCLUDE_DIR} ${NCURSESW_LIBRARY}")
else()
    message(FATAL_ERROR "ncursesw not found. Please install via 'brew install ncurses' on macOS or your package manager on Linux")
endif()

if(CLI_SRC)
    add_executable(innuendo ${CLI_SRC})
    target_include_directories(innuendo PRIVATE ${INCLUDE_DIR} ${NCURSESW_INCLUDE_DIR})
    target_compile_features(innuendo PRIVATE cxx_std_20)
    target_link_libraries(innuendo PRIVATE innuendo_core ${NCURSESW_LIBRARY})

    # Macro for code to wrap ncursesw calls
    target_compile_definitions(innuendo PRIVATE USE_NCURSESW)

    # Unicode / wide-character support
    if(MSVC)
        target_compile_options(innuendo PRIVATE /utf-8)
    else()
        target_compile_options(innuendo PRIVATE -finput-charset=UTF-8 -fexec-charset=UTF-8)
    endif()
endif()


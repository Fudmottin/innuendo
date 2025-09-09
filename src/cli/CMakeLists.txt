# src/cli/CMakeLists.txt

file(GLOB_RECURSE CLI_SRC CONFIGURE_DEPENDS *.cpp *.h)

if(CLI_SRC)
    add_executable(innuendo ${CLI_SRC})
    target_compile_features(innuendo PRIVATE cxx_std_20)
    target_link_libraries(innuendo PRIVATE innuendo_core)

    # --- Locate ncursesw ---
    if(APPLE)
        # Prefer Homebrew ncurses (Apple Silicon or Intel)
        set(NCURSES_PREFIX "")
        foreach(prefix /opt/homebrew /usr/local)
            if(EXISTS "${prefix}/opt/ncurses/include/ncursesw/ncurses.h")
                set(NCURSES_PREFIX "${prefix}/opt/ncurses")
                break()
            endif()
        endforeach()

        if(NCURSES_PREFIX STREQUAL "")
            message(FATAL_ERROR "Homebrew ncurses not found. Install with: brew install ncurses")
        endif()

        target_include_directories(innuendo PRIVATE ${NCURSES_PREFIX}/include)
        target_link_directories(innuendo PRIVATE ${NCURSES_PREFIX}/lib)
        target_link_libraries(innuendo PRIVATE ncursesw)
        target_compile_definitions(innuendo PRIVATE USE_NCURSESW)

        message(STATUS "Using Homebrew ncurses from ${NCURSES_PREFIX}")

    else() # Linux and others
        find_package(Curses REQUIRED)
        target_include_directories(innuendo PRIVATE ${CURSES_INCLUDE_DIR})
        target_link_libraries(innuendo PRIVATE ${CURSES_LIBRARIES})
        target_compile_definitions(innuendo PRIVATE USE_NCURSESW)
    endif()

    # --- Encoding options ---
    if(MSVC)
        target_compile_options(innuendo PRIVATE /utf-8)
    else()
        target_compile_options(innuendo PRIVATE -finput-charset=UTF-8 -fexec-charset=UTF-8)
    endif()
endif()


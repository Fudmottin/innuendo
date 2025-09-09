file(GLOB_RECURSE TEST_SRC CONFIGURE_DEPENDS *.cpp *.h)

if(TEST_SRC)
    add_executable(innuendo_tests ${TEST_SRC})
    target_include_directories(innuendo_tests PRIVATE ${INCLUDE_DIR})
    target_compile_features(innuendo_tests PRIVATE cxx_std_20)
    target_link_libraries(innuendo_tests PRIVATE
        innuendo_core
        Catch2::Catch2WithMain
    )
    add_test(NAME innuendo_tests COMMAND innuendo_tests)
endif()


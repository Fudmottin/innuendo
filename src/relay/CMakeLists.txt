file(GLOB_RECURSE RELAY_SRC CONFIGURE_DEPENDS *.cpp *.h)

if(RELAY_SRC)
    add_executable(innuendod ${RELAY_SRC})
    target_include_directories(innuendod PRIVATE ${INCLUDE_DIR})
    target_compile_features(innuendod PRIVATE cxx_std_20)
    target_link_libraries(innuendod PRIVATE innuendo_core)
endif()


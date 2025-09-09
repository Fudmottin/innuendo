set(INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/../include)
file(GLOB_RECURSE CORE_SRC CONFIGURE_DEPENDS *.cpp *.h)

add_library(innuendo_core ${CORE_SRC})
target_include_directories(innuendo_core PUBLIC
    ${INCLUDE_DIR}
    ${Boost_INCLUDE_DIRS}
)
target_link_libraries(innuendo_core PUBLIC ${Boost_LIBRARIES})
target_compile_features(innuendo_core PUBLIC cxx_std_20)



find_package(Boost REQUIRED container)

ez_add_library(ez_core ${CMAKE_CURRENT_SOURCE_DIR})


target_link_libraries(ez_core PUBLIC Boost::container)

add_subdirectory(tests)

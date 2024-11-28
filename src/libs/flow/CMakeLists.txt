find_package(Boost REQUIRED)

ez_add_library(ez_flow ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(ez_flow
    PUBLIC
        ez_core
        ez_io
        Boost::boost
)

add_subdirectory(tests)

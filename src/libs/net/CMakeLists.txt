ez_add_library(ez_net ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(ez_net
    PUBLIC
        ez_core
        Boost::boost
)


add_subdirectory(tests)

ez_add_header_only_library(ez_io ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(ez_io
    INTERFACE
        ez_core
        Boost::boost
)

add_subdirectory(tests)

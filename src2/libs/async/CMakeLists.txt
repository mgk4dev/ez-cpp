
find_package(Boost REQUIRED)

ez_add_header_only_library(ez_async ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(ez_async
    INTERFACE
        ez_core
)

target_link_libraries(ez_async
    INTERFACE
        Boost::boost
)

add_subdirectory(tests)

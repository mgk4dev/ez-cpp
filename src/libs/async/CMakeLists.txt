
find_package(Boost REQUIRED)

ez_add_library(ez_async ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(ez_async
    PUBLIC
        ez_core
        Boost::boost
)

add_subdirectory(tests)

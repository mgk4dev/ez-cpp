ez_add_library(ez_rpc ${CMAKE_CURRENT_SOURCE_DIR})

target_sources(ez_rpc
  PRIVATE
    src/serializer.cpp
)

find_package(Boost REQUIRED)

target_link_libraries(ez_rpc
    PUBLIC
        ez_core
        ez_async
        Boost::boost
)

add_subdirectory(tests)

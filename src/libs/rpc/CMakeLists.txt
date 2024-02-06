find_package(Protobuf MODULE REQUIRED)
find_package(Boost REQUIRED)

########

add_library(ez_rpc_protobuf STATIC
    protobuf/messages.proto
)

protobuf_generate(TARGET ez_rpc_protobuf )
target_include_directories(ez_rpc_protobuf PUBLIC ${CMAKE_CURRENT_BINARY_DIR} )
target_link_libraries(ez_rpc_protobuf PUBLIC protobuf::libprotobuf)


########

ez_add_library(ez_rpc ${CMAKE_CURRENT_SOURCE_DIR})

target_sources(ez_rpc
  PRIVATE
    src/serializer.cpp
)

target_link_libraries(ez_rpc
    PUBLIC
        ez_core
        ez_async
        Boost::boost
    PRIVATE
        ez_rpc_protobuf
)

########

add_subdirectory(tests)

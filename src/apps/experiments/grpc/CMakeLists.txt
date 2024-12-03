find_package(protobuf CONFIG REQUIRED)
find_package(gRPC CONFIG REQUIRED)

###############################################################


add_library(ez_app_grpc_proto service.proto)
target_link_libraries(ez_app_grpc_proto
    PUBLIC
        protobuf::libprotobuf
        gRPC::grpc
        gRPC::grpc++
)
target_include_directories(ez_app_grpc_proto PUBLIC ${CMAKE_CURRENT_BINARY_DIR})
get_target_property(grpc_cpp_plugin_location gRPC::grpc_cpp_plugin LOCATION)
protobuf_generate(TARGET ez_app_grpc_proto LANGUAGE cpp)
protobuf_generate(
    TARGET ez_app_grpc_proto
    LANGUAGE grpc
    GENERATE_EXTENSIONS .grpc.pb.h .grpc.pb.cc
    PLUGIN "protoc-gen-grpc=${grpc_cpp_plugin_location}"
)


###############################################################

add_executable(ez_app_grpc_client client.cpp)
target_link_libraries(ez_app_grpc_client PRIVATE ez_app_grpc_proto)

###############################################################

add_executable(ez_app_grpc_server server.cpp)
target_link_libraries(ez_app_grpc_server PRIVATE ez_app_grpc_proto)

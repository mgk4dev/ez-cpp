
find_package(Boost REQUIRED)
find_package(GTest REQUIRED)
find_package(protobuf REQUIRED)

message("********* Boost found: ${Boost_INCLUDE_DIRS}")
message("********* GTest found: ${GTest_FOUND}")

enable_testing()
include(GoogleTest)

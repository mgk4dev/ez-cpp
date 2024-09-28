
find_package(Boost REQUIRED)
set(BOOST_LIBRARYDIR $<IF:$<CONFIG:Debug>,${boost_BIN_DIRS_DEBUG},${boost_BIN_DIRS_RELEASE}>)
set(BOOST_INCLUDEDIR $<IF:$<CONFIG:Debug>,${Boost_INCLUDE_DIRS_DEBUG},${Boost_INCLUDE_DIRS_RELEASE}>)

find_package(GTest REQUIRED)
find_package(protobuf REQUIRED)

message("********* Boost found: ${Boost_INCLUDE_DIRS}")
message("********* GTest found: ${GTest_FOUND}")

enable_testing()
include(GoogleTest)

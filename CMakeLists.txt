cmake_minimum_required(VERSION 3.20)

project(ez-cpp LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)

include(cmake/conan.cmake)
include(cmake/compiler.cmake)
include(cmake/lib.cmake)

add_subdirectory(src)

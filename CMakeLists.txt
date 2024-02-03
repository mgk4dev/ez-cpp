cmake_minimum_required(VERSION 3.27)

enable_language(CXX)

project(ez-cpp LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)

include(cmake/all.cmake)

add_subdirectory(src)

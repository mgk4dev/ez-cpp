message("++++ CMAKE_C_COMPILER : ${CMAKE_C_COMPILER}")
message("++++ CMAKE_CXX_COMPILER : ${CMAKE_CXX_COMPILER}")

####### Flags
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -Wpedantic -Werror")


####### Modules

# set(CMAKE_EXPERIMENTAL_CXX_MODULE_CMAKE_API "2182bf5c-ef0d-489a-91da-49dbc3090d2a")

# set(CMAKE_CXX_EXTENSIONS OFF)
# add_definitions(-DCMAKE_EXPORT_COMPILE_COMMANDS=ON)

# set(CMake_TEST_CXXModules_UUID "a246741c-d067-4019-a8fb-3d16b0c9d1d3")

# set(CMAKE_EXPERIMENTAL_CXX_MODULE_DYNDEP 1)
# string(CONCAT CMAKE_EXPERIMENTAL_CXX_SCANDEP_SOURCE
#   "<CMAKE_CXX_COMPILER> <DEFINES> <INCLUDES> <FLAGS> -E -x c++ <SOURCE>"
#   " -MT <DYNDEP_FILE> -MD -MF <DEP_FILE>"
#   " -fmodules-ts -fdep-file=<DYNDEP_FILE> -fdep-output=<OBJECT> -fdep-format=trtbd"
#   " -o <PREPROCESSED_SOURCE>")
# set(CMAKE_EXPERIMENTAL_CXX_MODULE_MAP_FORMAT "gcc")
# set(CMAKE_EXPERIMENTAL_CXX_MODULE_MAP_FLAG "-fmodules-ts -fmodule-mapper=<MODULE_MAP_FILE> -fdep-format=trtbd -x c++")



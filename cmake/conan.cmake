message("++++ Root project directory : ${CMAKE_SOURCE_DIR}")
message("++++ Build directory        : ${CMAKE_CURRENT_BINARY_DIR}")

execute_process(COMMAND
        conan install
                -of ${CMAKE_CURRENT_BINARY_DIR}
                --build missing
                -s build_type=${CMAKE_BUILD_TYPE}
                ${CMAKE_SOURCE_DIR}/conanfile.txt
        WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
        RESULT_VARIABLE conan_cmd_result)

if(NOT conan_cmd_result EQUAL 0)
        message(FATAL_ERROR "Conan failed: ${conan_cmd_result}")
endif()

find_package(GTest REQUIRED)

enable_testing()
include(GoogleTest)


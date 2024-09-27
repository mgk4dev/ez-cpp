

# execute_process(COMMAND
#         conan install
#                 -of ${CMAKE_CURRENT_BINARY_DIR}
#                 --build missing
#                 -s build_type=Release
#                 -r conan-center
#                 ${CMAKE_SOURCE_DIR}/conanfile.txt
#         WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
#         RESULT_VARIABLE conan_cmd_result)

# if(NOT conan_cmd_result EQUAL 0)
#         message(FATAL_ERROR "Conan failed: ${conan_cmd_result}")
# endif()

#set(QT_CREATOR_SKIP_CONAN_SETUP ON)
#set(CMAKE_TOOLCHAIN_FILE "${CMAKE_CURRENT_BINARY_DIR}/build/generators/conan_toolchain.cmake")


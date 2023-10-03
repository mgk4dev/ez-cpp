message("++++ Root project directory : ${CMAKE_SOURCE_DIR}")
message("++++ Build directory        : ${CMAKE_CURRENT_BINARY_DIR}")

execute_process(COMMAND 
	conan install 
		-of ${CMAKE_CURRENT_BINARY_DIR} 
		--build missing
		${CMAKE_SOURCE_DIR}/conanfile.txt  
	WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
	RESULT_VARIABLE conan_cmd_result)

if(NOT conan_cmd_result EQUAL 0)
	message(FATAL_ERROR "Conan failed")
endif()

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
include(${CMAKE_CURRENT_BINARY_DIR}/conan_paths.cmake)

find_package(GTest MODULE REQUIRED)

enable_testing()
include(GoogleTest)


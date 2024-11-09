
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_LIST_DIR}/sanitizers/")



option(SANITIZE_ALL "Enable all sanitizers" Off)

option(SANITIZE_ADDRESS "Enable AddressSanitizer for sanitized targets." Off)
option(SANITIZE_MEMORY "Enable MemorySanitizer for sanitized targets." Off)
option(SANITIZE_THREAD "Enable ThreadSanitizer for sanitized targets." Off)
option(SANITIZE_UNDEFINED "Enable UndefinedBehaviorSanitizer for sanitized targets." Off)


function (enable_sanitizers)
    set(SANITIZE_ADDRESS ON CACHE BOOL "" FORCE )
    set(SANITIZE_UNDEFINED ON CACHE BOOL "" FORCE)
endfunction()


if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    enable_sanitizers()
endif()


if(SANITIZE_ALL)
    enable_sanitizers()
endif()

find_package(Sanitizers)

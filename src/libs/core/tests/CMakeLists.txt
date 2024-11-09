
find_package(Boost REQUIRED)
find_package(Qt6 COMPONENTS REQUIRED Core)

file(GLOB_RECURSE SRC_FILES *.cpp *.hpp)

add_executable(ez_core_tests ${SRC_FILES})
add_sanitizers(ez_core_tests)

target_link_libraries(ez_core_tests 
    PRIVATE
        ez_core 
        Qt6::Core
        GTest::gtest 
        GTest::gtest_main 
        GTest::gmock 
        GTest::gmock_main
)

#gtest_discover_tests(ez_core_tests)

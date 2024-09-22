
file(GLOB_RECURSE SRC_FILES *.cpp)

add_executable(ez_core_tests ${SRC_FILES})

target_link_libraries(ez_core_tests 
    PRIVATE
        ez_core 
        GTest::gtest 
        GTest::gtest_main 
        GTest::gmock 
        GTest::gmock_main
)

#gtest_discover_tests(ez_core_tests)

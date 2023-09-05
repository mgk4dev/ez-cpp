
add_executable(ez_core_tests
    main.cpp 
    lib.cpp
)

target_link_libraries(ez_core_tests 
    PRIVATE
        ez_core 
        GTest::gtest 
        GTest::gtest_main 
        GTest::gmock 
        GTest::gmock_main
)

gtest_discover_tests(ez_core_tests)

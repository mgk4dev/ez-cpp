
add_executable(ez_foo_tests
    main.cpp 
    tst_foo.cpp

)

target_link_libraries(ez_foo_tests
    PRIVATE
        ez_0template
        ez_core 
        GTest::gtest 
        GTest::gtest_main 
        GTest::gmock 
        GTest::gmock_main
)


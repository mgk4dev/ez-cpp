add_executable(ez_flow_tests
    main.cpp 
    tst_parser.cpp
)

target_include_directories(ez_flow_tests PRIVATE "../src")

target_link_libraries(ez_flow_tests
    PRIVATE
        ez_flow
        GTest::gtest 
        GTest::gtest_main 
        GTest::gmock 
        GTest::gmock_main
)



add_executable(ez_rpc_tests
    main.cpp 
    tst_rpc.cpp
    tst_serializer.cpp
)

target_link_libraries(ez_rpc_tests
    PRIVATE
        ez_rpc
        GTest::gtest 
        GTest::gtest_main 
        GTest::gmock 
        GTest::gmock_main
)


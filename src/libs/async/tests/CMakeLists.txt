
find_package(Boost COMPONENTS boost REQUIRED)

add_executable(ez_async_tests
    main.cpp 
    tst_async.cpp
    tst_receiver.cpp
)

target_link_libraries(ez_async_tests
    PRIVATE
        ez_core 
        ez_async
        GTest::gtest 
        GTest::gtest_main 
        GTest::gmock 
        GTest::gmock_main
        Boost::boost
)

#gtest_discover_tests(ez_core_tests)

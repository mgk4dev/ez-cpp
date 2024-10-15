find_package(Boost REQUIRED)
find_package(Qt6 COMPONENTS REQUIRED Core)

add_executable(ez_async_tests
    qadapters/QExecutor.hpp
    qadapters/QDelay.hpp

    main.cpp 
    tst_async.cpp
    tst_Receiver.cpp
)

target_link_libraries(ez_async_tests
    PRIVATE
        ez_core 
        ez_async
        Qt6::Core
        GTest::gtest 
        GTest::gtest_main 
        GTest::gmock 
        GTest::gmock_main
        Boost::boost
)

#gtest_discover_tests(ez_core_tests)

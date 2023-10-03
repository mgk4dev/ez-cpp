
find_package(Boost COMPONENTS boost REQUIRED)

add_executable(ez_core_tests
    main.cpp 

    tst_Async.cpp
    tst_AsyncSchedule.cpp
    tst_Atomic.cpp
    tst_Box.cpp
    tst_Cow.cpp
    tst_Enum.cpp
    tst_ForConstexpr.cpp
    tst_Result.cpp
    tst_ScopeGuard.cpp
    tst_StrongType.cpp  
    tst_Trait.cpp
)

target_link_libraries(ez_core_tests 
    PRIVATE
        ez_core 
        GTest::gtest 
        GTest::gtest_main 
        GTest::gmock 
        GTest::gmock_main
        Boost::boost
)

#gtest_discover_tests(ez_core_tests)

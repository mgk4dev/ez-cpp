
add_executable(ez_core_tests

    main.cpp
    tst_Box.cpp
    tst_Enum.cpp
    tst_Result.cpp
    tst_Shared.cpp
    tst_Traits.cpp
    tst_TypeUtils.cpp
    tst_Atomic.cpp
    tst_Cow.cpp
    tst_ScopeGuard.cpp
    tst_StrongType.cpp
    tst_Tuple.cpp
    tst_Utils.cpp
)

target_link_libraries(ez_core_tests 
    PRIVATE
        ez_core 
        GTest::gtest 
        GTest::gtest_main 
        GTest::gmock 
        GTest::gmock_main
)

#gtest_discover_tests(ez_core_tests)


add_executable(ez_core_tests
    main.cpp 

    tst_atomic.cpp
    tst_box.cpp
    tst_cow.cpp
    tst_enum.cpp
    tst_for_constexpr.cpp
    tst_meta.cpp
    tst_result.cpp
    tst_scope_guard.cpp
    tst_shared.cpp
    tst_strong_type.cpp
    tst_trait.cpp
    tst_tuple.cpp
    tst_utils.cpp
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


add_executable(ez_foo_tests
    main.cpp 
    tst_foo.cpp
)

find_package(Boost REQUIRED)
find_package(GTest REQUIRED)


target_link_libraries(ez_foo_tests
    PRIVATE
        ez_0template
        gtest::gtest
        boost::boost
)

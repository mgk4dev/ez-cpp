
add_executable(ez_net_tests
    main.cpp 
    tst_net.cpp
)

find_package(Boost REQUIRED)
find_package(GTest REQUIRED)


target_link_libraries(ez_net_tests
    PRIVATE
        ez_net
        gtest::gtest
        boost::boost
)


add_executable(ez_io_tests
    main.cpp 
    tst_io.cpp
)

find_package(Boost REQUIRED)
find_package(GTest REQUIRED)


target_link_libraries(ez_io_tests
    PRIVATE
        ez_io
        gtest::gtest
        boost::boost
)

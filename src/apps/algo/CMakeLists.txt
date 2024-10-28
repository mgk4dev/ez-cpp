
function (ez_add_algo_bin name)
    add_executable("ez_experiments_algo_${name}"
        ${name}.cpp
    )
    set_target_properties(
        "ez_experiments_algo_${name}"
        PROPERTIES LINKER_LANGUAGE CXX)

    target_link_libraries(
        "ez_experiments_algo_${name}"
        ez_core
        benchmark::benchmark_main
        GTest::gtest
        GTest::gtest_main
    )

endfunction()


find_package(benchmark REQUIRED)


#############################################

ez_add_algo_bin(sort)
ez_add_algo_bin(overlapping_intervals)


function (ez_add_leetcode_bin name)
    add_executable("ez_leetcode_${name}"
        ${name}.cpp
    )
    enable_sanitizers("ez_leetcode_${name}")
    set_target_properties(
        "ez_leetcode_${name}"
        PROPERTIES LINKER_LANGUAGE CXX)

    target_link_libraries(
        "ez_leetcode_${name}"
        ez_core
        benchmark::benchmark_main
        GTest::gtest
        GTest::gtest_main
    )

endfunction()


find_package(benchmark REQUIRED)


#############################################

ez_add_leetcode_bin(merge_sorted_array)
ez_add_leetcode_bin(remove_element)
ez_add_leetcode_bin(remove_duplicates_from_sorted_array)
ez_add_leetcode_bin(majority_element)
ez_add_leetcode_bin(rotate_array)
ez_add_leetcode_bin(max_profit)
ez_add_leetcode_bin(jump_game)
ez_add_leetcode_bin(stl)

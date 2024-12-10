
function (ez_add_algo_bin name)
    add_executable("ez_algo_${name}"
        ${name}.cpp
    )
    enable_sanitizers("ez_algo_${name}")
    set_target_properties(
        "ez_algo_${name}"
        PROPERTIES LINKER_LANGUAGE CXX)

    target_link_libraries(
        "ez_algo_${name}"
        PRIVATE
            ez_core
            benchmark::benchmark_main
            GTest::gtest
            GTest::gtest_main
    )

endfunction()


find_package(benchmark REQUIRED)
find_package(eve CONFIG REQUIRED)

#############################################

ez_add_algo_bin(merge_sorted_array)
ez_add_algo_bin(remove_element)
ez_add_algo_bin(remove_duplicates_from_sorted_array)
ez_add_algo_bin(majority_element)
ez_add_algo_bin(rotate_array)
ez_add_algo_bin(max_profit)
ez_add_algo_bin(jump_game)
ez_add_algo_bin(stl)
ez_add_algo_bin(odd_even_tree)
ez_add_algo_bin(prefix_pairs)
ez_add_algo_bin(alien_dict)
ez_add_algo_bin(k_nearest_neighbors)
ez_add_algo_bin(k_majority_elements)

ez_add_algo_bin(simd)
target_link_libraries(ez_algo_simd PRIVATE eve::eve)

ez_add_algo_bin(shortest_path)

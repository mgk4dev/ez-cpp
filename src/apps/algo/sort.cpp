

#include "formatters.hpp"
#include "random.hpp"

#include <algorithm>
#include <vector>
#include <print>

#include <benchmark/benchmark.h>

inline void insertion_sort(auto begin, auto end)
{
    if (begin == end) return;

    auto iter = begin + 1;

    while (iter != end) {
        auto current = iter;
        auto previous = iter - 1;
        while (*current < *previous && current != begin) {
            std::swap(*current, *previous);
            --current;
            --previous;
        }
        ++iter;
    }
}

static void bm_insertion_sort(benchmark::State& state, size_t count)
{
    auto data = ez::generate_random_vector(count, 0, 100);
    for (auto _ : state) {
        auto test_data = data;
        insertion_sort(test_data.begin(), test_data.end());
    }
}

static void bm_std_sort(benchmark::State& state, size_t count)
{
    auto data = ez::generate_random_vector(count, 0, 100);
    for (auto _ : state) {
        auto test_data = data;
        std::sort(test_data.begin(), test_data.end());
    }
}

BENCHMARK_CAPTURE(bm_insertion_sort, 10, 10);
BENCHMARK_CAPTURE(bm_std_sort, 10, 10);

BENCHMARK_CAPTURE(bm_insertion_sort, 100, 100);
BENCHMARK_CAPTURE(bm_std_sort, 100, 100);

BENCHMARK_CAPTURE(bm_insertion_sort, 1000, 1000);
BENCHMARK_CAPTURE(bm_std_sort, 1000, 1000);

BENCHMARK_CAPTURE(bm_insertion_sort, 10000, 10000);
BENCHMARK_CAPTURE(bm_std_sort, 10000, 10000);

BENCHMARK_MAIN();

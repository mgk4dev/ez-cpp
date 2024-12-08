#include <eve/eve.hpp>
#include <eve/module/algo.hpp>

#include <algorithm>

#include <benchmark/benchmark.h>

#include <ez/Utils.hpp>

using namespace ez;

template <typename T>
std::vector<T> make_vector(u64 size)
{
    std::vector<T> result;
    result.resize(size);
    return result;
}

inline constexpr u64 element_count = 1000000;

auto find_max_std(const auto& elements)
{
    return *std::max_element(elements.begin(), elements.end());
}

auto find_max_simd(const auto& elements) { return eve::algo::max_element(elements); }

template <typename T>
static void benchmark_std(benchmark::State& state)
{
    auto elements = make_vector<T>(element_count);

    for (auto _ : state) { benchmark::DoNotOptimize(find_max_std(elements)); }
}

template <typename T>
static void benchmark_simd(benchmark::State& state)
{
    auto elements = make_vector<T>(element_count);

    for (auto _ : state) { benchmark::DoNotOptimize(find_max_simd(elements)); }
}

BENCHMARK(benchmark_std<u8>);
BENCHMARK(benchmark_simd<u8>);

BENCHMARK(benchmark_std<u16>);
BENCHMARK(benchmark_simd<u16>);

BENCHMARK(benchmark_std<u32>);
BENCHMARK(benchmark_simd<u32>);

BENCHMARK(benchmark_std<u64>);
BENCHMARK(benchmark_simd<u64>);

BENCHMARK_MAIN();

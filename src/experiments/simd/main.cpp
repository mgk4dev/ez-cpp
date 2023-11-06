#include <eve/module/algo.hpp>
#include <eve/module/core.hpp>

#include <eve/wide.hpp>

#include <benchmark/benchmark.h>

#include <iostream>
#include <new>
#include <ranges>

using namespace eve;

template <typename T, uint alignment>
struct Element {
    alignas(alignment) T val;
};

constexpr size_t element_count = 1024 * 8;
constexpr size_t data_alignment = sizeof(wide<uint32_t>);

using E = Element<uint32_t, data_alignment>;

std::vector<E> data_ = [] {
    std::vector<E> vec;
    vec.resize(element_count);
    return vec;
}();

std::span<uint32_t> data{reinterpret_cast<uint32_t*>(data_.data()), element_count};

auto max = std::ranges::max_element(data);

auto max_element(std::span<uint32_t> data)
{
    using Pack = wide<uint32_t>;
    auto begin = reinterpret_cast<Pack*>(data.data());
    auto end = begin + data.size() / Pack::size();
    auto max_result = begin;

    Pack max{data.front()};

    std::optional<std::ptrdiff_t> match;

    while (begin != end) {
        auto found = *begin > max;

        if (eve::any(found)) {
            match = eve::last_true(found);
            max_result = begin + *match;
            max = Pack{*max_result};
        }

        begin += 1;
    }

    return data.begin() + std::distance(reinterpret_cast<Pack*>(data.data()), max_result);
}

static void bm_find_std(benchmark::State& state)
{
    for (auto _ : state) {
        auto __ = std::ranges::max_element(data);
        benchmark::DoNotOptimize(__);
    }
}

static void bm_find_eve(benchmark::State& state)
{
    for (auto _ : state) {
        auto __ = eve::algo::max_element(data);
        benchmark::DoNotOptimize(__);
    }
}

static void bm_find_simd(benchmark::State& state)
{
    if (max_element(data) != std::ranges::max_element(data)) {
        std::cout << "max_element not working";
        std::terminate();
    }

    for (auto _ : state) {
        auto __ = max_element(data);
        benchmark::DoNotOptimize(__);
    }
}

BENCHMARK(bm_find_std);
BENCHMARK(bm_find_eve);
BENCHMARK(bm_find_simd);

BENCHMARK_MAIN();

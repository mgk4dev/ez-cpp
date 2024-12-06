#include <eve/eve.hpp>

#include <iostream>
#include <vector>

#include <benchmark/benchmark.h>

static void BENCHMARK_game(benchmark::State& state)
{
    for (auto _ : state) {}
}

BENCHMARK(BENCHMARK_game);

BENCHMARK_MAIN();

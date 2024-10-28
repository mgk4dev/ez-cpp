
#include <algorithm>
#include <iostream>
#include <optional>
#include <vector>

#include <benchmark/benchmark.h>

struct Range {
    int begin, end;

    bool contains(int val) const { return begin <= val && val <= end; }
};

using Ranges = std::vector<Range>;

void print(std::string_view name, const Range& rng)
{
    std::cout << name << " : ";
    std::cout << "(" << rng.begin << ", " << rng.end << ") ";
    std::cout << std::endl;
}

void print(std::string_view name, const Ranges& rngs)
{
    std::cout << name << " : ";
    for (auto&& rng : rngs) { std::cout << "(" << rng.begin << ", " << rng.end << ") "; }
    std::cout << std::endl;
}

/////////////////////////////////////////////////////////////////////////////////////////////

bool has_overlap(const Range& lhs, const Range& rhs)
{
    return lhs.contains(rhs.begin) || lhs.contains(rhs.end) || rhs.contains(lhs.begin) ||
           rhs.contains(lhs.end);
}

std::optional<Range> get_overlap(const Range& lhs, const Range& rhs)
{
    if (!has_overlap(lhs, rhs)) return std::nullopt;
    auto begin = std::max(lhs.begin, rhs.begin);
    auto end = std::min(lhs.end, rhs.end);
    return Range{begin, end};
}

Ranges merge(const Ranges& rngs1, const Ranges& rngs2)
{
    Ranges result;

    for (auto&& range : rngs1) {
        print("range", range);

        for (auto&& overlapping : std::ranges::equal_range(
                 rngs2, range, [](Range lhs, Range rhs) { return lhs.end < rhs.begin; })) {
            print("  overlapping: ", overlapping);
            result.push_back(get_overlap(range, overlapping).value());
        }
    }

    return result;
}

void run(const Ranges& rngs1, const Ranges& rngs2)
{
    std::cout << "-------------------------" << std::endl;

    print("rngs1", rngs1);
    print("rngs2", rngs2);

    std::cout << "----" << std::endl;

    const auto merged = merge(rngs1, rngs2);
    std::cout << "----" << std::endl;

    print("merge", merged);
}

void run()
{
    // clang-format off
        run({ {1, 3}, {5, 8}, {9, 15} },
            { {1, 2}, {7, 8} });
    // clang-format on

    // clang-format off
    run({ {1, 3}, {5, 10}, {11, 15} },
        { {6, 8}, {9, 10} });
    // clang-format on

    // clang-format off
    run({ {1, 3}, {9, 10}, {11, 15} },
        { {4, 5}, {19, 20} });
    // clang-format on

    // clang-format off
    run({ {1, 3}, {9, 10}, {11, 15} },
        { {1,  100} });
    // clang-format on
}

static void BM_run(benchmark::State& state)
{
    // Perform setup here
    for (auto _ : state) {
        // This code gets timed
        run();
    }
}
// Register the function as a benchmark
BENCHMARK(BM_run);
// Run the benchmark
BENCHMARK_MAIN();

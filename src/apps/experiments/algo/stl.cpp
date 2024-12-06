#include <gtest/gtest.h>

#include "formatters.hpp"

#include <algorithm>
#include <numeric>
#include <unordered_set>
#include <vector>
#include <print>

int main(int argc, char** argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}

TEST(Stl, bounds)
{
    std::vector v{1, 2, 3, 4};

    {
        auto iter = std::lower_bound(v.begin(), v.end(), 2);
        ASSERT_EQ(*iter, 2);
    }

    {
        auto iter = std::upper_bound(v.begin(), v.end(), 2);
        ASSERT_EQ(*iter, 3);
    }
}

TEST(Stl, reverse)
{
    std::vector v{1, 2, 3, 4};
    std::reverse(v.begin(), v.end());
    std::vector expected{4, 3, 2, 1};

    ASSERT_EQ(v, expected);
}

TEST(Stl, rotate)
{
    std::vector v{1, 2, 3, 4};
    auto begin = v.begin();

    std::rotate(begin, begin + 1, begin + 3);
    std::vector expected{2, 3, 1, 4};
    ASSERT_EQ(v, expected);
}

TEST(Stl, permutation)
{
    std::unordered_multiset<std::string> words{"stop", "pots", "tops", "opts", "post", "spot"};

    std::string word = "opst";

    do {
        std::println("'{}' {}", word, bool(words.count(word)));
    } while (std::next_permutation(word.begin(), word.end()));

    std::vector v{1, 2, 3, 4};
}

TEST(Stl, partial_sum)
{
    std::vector a{1, 2, 3, 4};
    std::vector<int> b(4), c(4);

    std::partial_sum(a.begin(), a.end(), b.begin());
    std::partial_sum(a.begin(), a.end(), c.begin(), std::multiplies<>{});

    std::println("a : [{}]", a);
    std::println("b : [{}]", b);
    std::println("c : [{}]", c);
}

TEST(Stl, adjacent)
{
    std::vector a{1, 2, 2, 2, 3, 4, 4, 4};

    auto first = a.begin();
    const auto end = a.end();

    do {
        first = std::adjacent_find(first, end);

        if (first == end) break;

        std::println("Adjacent {} at position  {}", *first, std::distance(a.begin(), first));

        ++first;

    } while (first != end);
}

TEST(Stl, accumulate_reduce)
{
    std::vector a{1, 2, 3, 4};

    auto acc = std::accumulate(a.begin(), a.end(), 0);
    auto red = std::reduce(a.begin(), a.end(), 0);

    std::println("Accumulate result {}", acc);
    std::println("Reduce     result {}", red);
}

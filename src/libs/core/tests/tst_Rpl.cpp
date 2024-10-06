#include <gtest/gtest.h>

#include <ez/rpl/All.hpp>

#include <ez/Tuple.hpp>

using namespace ez;

TEST(Rpl, chain_types)
{
    std::vector<int> input;
    auto f = [](int val) { return val > 2; };
    auto t = [](int& val) { return val * val; };

    // clang-format off
    auto types = rpl::get_chain_input_types<
        std::vector<int>&, // input
        decltype(rpl::filter(f)),
        decltype(rpl::transform(t)),
        decltype(rpl::to_vector()),
        decltype(rpl::sort())
    >();


    using ExpectedType = TypeList<
        std::vector<int>&,
        int&,
        int&&,
        std::vector<int>&&,
        std::vector<int>&&>;

    constexpr bool are_same = std::is_same_v<EZ_REMOVE_CVR_T(types),ExpectedType>;
    // clang-format on

    static_assert(are_same);
}

TEST(Rpl, example)
{
    std::vector input{1, 2, 3, 4};

    auto f = [](int val) { return val > 2; };
    auto t = [](int& val) { return val * val; };

    auto result = rpl::run(input,             // All      vector&
                           rpl::filter(f),    // I -> I   int& -> int&
                           rpl::transform(t)  //,  // I -> I   int& -> int&&
    );

    // unused(result);
}

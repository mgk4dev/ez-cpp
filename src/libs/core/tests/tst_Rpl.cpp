#include <gtest/gtest.h>

#include <ez/rpl/All.hpp>

#include <ez/Tuple.hpp>

using namespace ez;

TEST(Rpl, example)
{
    std::vector input{1, 2, 3, 4};

    // auto f = [](int val) { return val > 2; };
    // auto t = [](int val) { return  val * val; };

    // // clang-format off
    // auto result = rpl::run(
    //     input,
    //     rpl::filter(f),
    //     rpl::to_vector()
    // );
    // // clang-format on


    auto filter = rpl::filter([](int val) { return val > 2; });
    auto stage = filter.make(meta::type<int&>);

    using InputType = decltype(input);


    rpl::Chain<InputType, EZ_DECAY_T(filter)> chain{filter};


}

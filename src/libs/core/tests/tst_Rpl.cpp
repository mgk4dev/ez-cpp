#include <gtest/gtest.h>

#include <ez/rpl/All.hpp>

#include <ez/Tuple.hpp>

using namespace ez;

TEST(Rpl, example)
{
    std::vector input{1, 2, 3, 4};


    auto f  = [](int val) { return val > 2; };
    auto t = [](int val) { return Tuple{val, val * val * 1.};};
    auto a = [](int left, double right) { std::cout << "Otuput : " << left << " " << right << std::endl;  };

    // clang-format off
    rpl::run(
        input,
        rpl::filter(f),
        rpl::transform(t),
        rpl::apply(a)
    );
    // clang-format on



    for (auto&& val : input) {
        if (f(val)) {
            auto v2 = t(val);
            std::apply(a, v2);
        }
    }

}

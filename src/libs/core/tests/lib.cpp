#include <gtest/gtest.h>

import ez;

using namespace ez;

TEST(lib, types)
{
    Box<int> val1;
    Cow<std::string> val2;
    Enum<int, double> val3{};
    Result<int, char> val4{};

    (void)val1;
    (void)val2;
    (void)val3;
    (void)val4;
}

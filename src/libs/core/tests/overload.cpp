#include <gtest/gtest.h>

#include <ez/overload.hpp>

#include <variant>

using namespace ez;

TEST(overload, visit_variant)
{
    std::variant<bool, int, std::string> v = 35;
    dispatch(
        v, [](bool val) { std::cout << "It is a bool " << val; },
        [](int val) { std::cout << "It is a int " << val; },
        [](const std::string& val) { std::cout << "It is a string " << val; });
}

#include <gtest/gtest.h>

#include <ez/Contract.hpp>

using namespace ez;

TEST(Contract, throw_exception)
{
    ASSERT_THROW(EZ_CONTRACT(4 < 3, contract::throw_logic_error), std::logic_error);
}

TEST(Contract, print_error)
{
    EZ_CONTRACT(4 < 3, contract::print_error);
}

TEST(Contract, custom_handler)
{
    EZ_CONTRACT(4 < 3, [](const contract::Expression& e) { std::cout << e.file << std::endl; });
}

TEST(Contract, multi_handlers)
{
    int res = 0;
    auto h1 = [&](const auto&) { res = 10; };
    auto h2 = [&](const auto&) { res *= 2; };

    EZ_CONTRACT(4 < 3, h1, h2);

    ASSERT_EQ(res, 20);
}

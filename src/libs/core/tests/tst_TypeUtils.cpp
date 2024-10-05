#include <gtest/gtest.h>

#include <ez/TypeUtils.hpp>

using namespace ez;

TEST(TypeList, contains) { static_assert(meta::type_list<int, double>.contains(meta::type<int>)); }

TEST(TypeList, remove_duplicates)
{
    using Tl = TypeList<int, double, float, int, double, float>;

    auto u = meta::remove_duplicates(Tl{});

    using Unique = decltype(u);

    static_assert(meta::type<Unique> == meta::type<TypeList<int, double, float>>);
}

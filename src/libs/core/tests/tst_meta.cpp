#include <gtest/gtest.h>

#include <ez/meta.hpp>

using namespace ez;

TEST(TypeList, contains) { static_assert(type_list<int, double>.contains(type<int>)); }

TEST(TypeList, remove_duplicates)
{
    using Tl = TypeList<int, double, float, int, double, float>;

    auto u = remove_duplicates(Tl{});

    using Unique = decltype(u);

    static_assert(type<Unique> == type<TypeList<int, double, float>>);
}

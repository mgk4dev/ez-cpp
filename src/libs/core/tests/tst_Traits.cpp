#include <gtest/gtest.h>

#include <ez/Result.hpp>
#include <ez/Traits.hpp>
#include <ez/TypeUtils.hpp>

using namespace ez;

TEST(Trait, AnyRef)
{
    using V = std::vector<int>;

    auto f = [](trait::AnyRef<V> auto&& arg) constexpr { return meta::type<decltype(arg)>; };

    V val;

    static_assert(f(val) == meta::type<V&>);
    static_assert(f(std::as_const(val)) == meta::type<const V&>);
    static_assert(f(std::move(val)) == meta::type<V&&>);
}

TEST(Traits, Fn)
{
    auto f = [](int, double) -> std::string { return "hello"; };
    using F = decltype(f);
    static_assert(trait::Fn<F, std::string(int, double)>);

    auto call = [](trait::Fn<std::string(int, double)> auto&& arg) { return arg(1, 5.5); };

    ASSERT_EQ(call(f), "hello");
}

TEST(Traits, Is)
{
    using T = int;
    static_assert(trait::Is<T, int>);
    static_assert(!trait::Is<T, double>);

    auto f = [](trait::Is<int> auto&&) {};
    f(25);
}

TEST(Traits, IsTemplate)
{
    using T = std::vector<int>;
    static_assert(trait::IsTemplate<T, std::vector>);
    static_assert(!trait::IsTemplate<T, std::map>);
    static_assert(!trait::IsTemplate<int, std::map>);

    using TT = Result<int, std::string>;
    static_assert(trait::IsTemplate<TT, Result>);

    auto f = [](trait::IsTemplate<std::vector> auto&&) {};

    f(std::vector<int>{});
    f(std::vector<double>{});
}

TEST(Traits, DerivedFrom)
{
    struct A {
        virtual ~A() = default;
        virtual int foo() const = 0;
    };
    struct B : public A {
        int foo() const { return 25; }
    };

    static_assert(trait::DerivedFrom<B, A>);

    auto f = [](trait::DerivedFrom<A> auto&&) {};

    f(B{});
}

#include <gtest/gtest.h>

#include <ez/Result.hpp>
#include <ez/Trait.hpp>

using namespace ez;

template <typename T>
T& get_ref()
{
    static std::decay_t<T> val;
    return val;
}

TEST(Trait, AnyRef)
{
    using V = std::vector<int>;
    auto f = [](trait::AnyRef<V> auto&&) {};

    f(get_ref<V>());
    f(get_ref<const V>());
    f(std::move(get_ref<V>()));
}

TEST(Traits, Fn)
{
    auto f = [](int, double&) -> std::string { return {}; };
    using F = decltype(f);
    static_assert(trait::Fn<F, std::string(int, double&)>);
}

TEST(Traits, Is)
{
    using T = int;
    static_assert(trait::Is<T, int>);
    static_assert(!trait::Is<T, double>);
}

TEST(Traits, IsTemplate)
{
    using T = std::vector<int>;
    static_assert(trait::IsTemplate<T, std::vector>);
    static_assert(!trait::IsTemplate<T, std::map>);

    using TT = Result<int, std::string>;
    static_assert(trait::IsTemplate<TT, Result>);
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
}

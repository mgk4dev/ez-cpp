#include <gtest/gtest.h>

#include <ez/Shared.hpp>

using namespace ez;

TEST(Shared, inplace_shared)
{
    Shared<std::string> shared{in_place, 4, '='};
    ASSERT_EQ(shared.value(), "====");
}

TEST(Shared, lvalue_constructor)
{
    const auto vec = std::vector<int>{1, 2, 3};

    {
        Shared<std::vector<int>> shared{vec};
        ASSERT_EQ(shared.value(), vec);
    }
    {
        Shared<std::vector<int>> shared = vec;
        ASSERT_EQ(shared.value(), vec);
    }
}

TEST(Shared, rvalue_constructor)
{
    {
        Shared<std::vector<int>> shared{std::vector<int>{1, 2, 3}};
        ASSERT_EQ(shared.value(), (std::vector<int>{1, 2, 3}));
    }

    {
        Shared<std::vector<int>> shared = std::vector<int>{1, 2, 3};
        ASSERT_EQ(shared.value(), (std::vector<int>{1, 2, 3}));
    }

    {
        auto vec = std::vector<int>{1, 2, 3};
        auto dataPtr = vec.data();
        Shared<std::vector<int>> shared = std::move(vec);
        ASSERT_EQ(shared.value(), (std::vector<int>{1, 2, 3}));
        ASSERT_EQ(dataPtr, shared->data());
    }
}

namespace shared {

struct A {
    virtual ~A() = default;
    virtual int foo() const { return 0; }
};
struct B : public A {
    int foo() const { return 25; }
};
}  // namespace shared

TEST(Shared, derived)
{
    using shared::A;
    using shared::B;
    {
        Shared<A> a{B{}};
        ASSERT_EQ(a->foo(), 25);
    }

    {
        Shared<B> b;
        Shared<A> a{b};
        ASSERT_EQ(a->foo(), 25);
    }

    {
        Shared<B> b;
        Shared<A> a;
        a = b;
        ASSERT_EQ(a->foo(), 25);
    }
}

TEST(Shared, copy_incremets_use_count)
{
    Shared<std::string> shared;
    ASSERT_EQ(shared.use_count(), 1);

    auto shared2 = shared;

    ASSERT_EQ(shared.use_count(), 2);
}

TEST(Shared, detach_decrements_use_count)
{
    Shared<std::string> shared;
    shared = "hello";
    ASSERT_TRUE(shared.is_unique());
    ASSERT_EQ(shared.use_count(), 1);

    auto shared2 = shared;
    ASSERT_FALSE(shared.is_unique());
    ASSERT_EQ(shared.use_count(), 2);

    shared.detach();
    ASSERT_TRUE(shared.is_unique());
    ASSERT_EQ(shared.use_count(), 1);

    ASSERT_EQ(shared.value(), "hello");
}

TEST(Shared, equality)
{
    Shared<int> val;
    val = 10;
    ASSERT_EQ(val, 10);

    Shared<int> val2 = 10;
    ASSERT_EQ(val, val2);
}

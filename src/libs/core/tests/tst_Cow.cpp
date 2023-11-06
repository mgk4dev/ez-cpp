#include <gtest/gtest.h>

#include <ez/cow.hpp>

using namespace ez;

TEST(Cow, inplace_cow)
{
    Cow<std::string> cow{in_place, 4, '='};
    ASSERT_EQ(cow.value(), "====");
}

TEST(Cow, lvalue_constructor)
{
    const auto vec = std::vector<int>{1, 2, 3};

    {
        Cow<std::vector<int>> cow{vec};
        ASSERT_EQ(cow.value(), vec);
    }
    {
        Cow<std::vector<int>> cow = vec;
        ASSERT_EQ(cow.value(), vec);
    }
}

TEST(Cow, rvalue_constructor)
{
    {
        Cow<std::vector<int>> cow{std::vector<int>{1, 2, 3}};
        ASSERT_EQ(cow.value(), (std::vector<int>{1, 2, 3}));
    }

    {
        Cow<std::vector<int>> cow = std::vector<int>{1, 2, 3};
        ASSERT_EQ(cow.value(), (std::vector<int>{1, 2, 3}));
    }

    {
        auto vec = std::vector<int>{1, 2, 3};
        auto dataPtr = vec.data();
        Cow<std::vector<int>> cow = std::move(vec);
        ASSERT_EQ(cow.value(), (std::vector<int>{1, 2, 3}));
        ASSERT_EQ(dataPtr, cow->data());
    }
}

namespace cow {

struct A {
    virtual ~A() = default;
    virtual int foo() const { return 0; }
};
struct B : public A {
    int foo() const { return 25; }
};
}

TEST(Cow, derived)
{
    using cow::A;
    using cow::B;
    {
        Cow<A> a{B{}};
        ASSERT_EQ(a->foo(), 25);
    }

    {
        Cow<B> b;
        Cow<A> a{b};
        ASSERT_EQ(a->foo(), 25);
    }

    {
        Cow<B> b;
        Cow<A> a;
        a = b;
        ASSERT_EQ(a->foo(), 25);
    }
}

TEST(Cow, copy_incremets_use_count)
{
    Cow<std::string> cow;
    ASSERT_EQ(cow.use_count(), 1);

    auto cow2 = cow;

    ASSERT_EQ(cow.use_count(), 2);
}

TEST(Cow, detach_increments_use_count)
{
    Cow<std::string> cow;
    cow = "hello";
    ASSERT_TRUE(cow.is_unique());
    ASSERT_EQ(cow.use_count(), 1);

    auto cow2 = cow;
    ASSERT_FALSE(cow.is_unique());
    ASSERT_EQ(cow.use_count(), 2);

    cow.detach();
    ASSERT_TRUE(cow.is_unique());
    ASSERT_EQ(cow.use_count(), 1);

    ASSERT_EQ(cow.value(), "hello");
}

TEST(Cow, mutate_unique_doesnt_detach)
{
    Cow<std::vector<int>> cow;
    const auto& vector = cow.value();

    cow.edit([](auto& vector) { vector.push_back(1); });

    ASSERT_EQ(&vector, &(cow.value()));
}

TEST(Cow, mutate_cow_detaches)
{
    Cow<std::vector<int>> cow;
    auto lightCopy = cow;

    const auto* vector = &cow.value();

    cow.detach();
    cow.edit([](auto& vector) { vector.push_back(1); });

    ASSERT_NE(vector, &(cow.value()));
}

TEST(Cow, assign_unique_doesnt_detach)
{
    Cow<std::vector<int>> cow;
    const auto& vector = cow.value();

    cow = std::vector<int>{1, 2, 3};

    ASSERT_EQ(&vector, &(cow.value()));
    ASSERT_EQ(cow->size(), 3);
}

TEST(Cow, assign_cow_detaches)
{
    Cow<std::vector<int>> cow;
    auto lightCopy = cow;

    const auto* vector = &cow.value();

    cow = std::vector<int>{1, 2, 3};

    ASSERT_NE(vector, &(cow.value()));
    ASSERT_EQ(cow->size(), 3);
}

#include <gtest/gtest.h>

#include <ez/box.hpp>

using namespace ez;

TEST(Box, inplace_box)
{
    Box<std::string> box{in_place, 4, '='};
    ASSERT_EQ(box.value(), "====");
}

TEST(Box, lvalue_constructor)
{
    const auto vec = std::vector<int>{1, 2, 3};

    {
        Box<std::vector<int>> box{vec};
        ASSERT_EQ(box.value(), vec);
    }
    {
        Box<std::vector<int>> box = vec;
        ASSERT_EQ(box.value(), vec);
    }
}

TEST(Box, rvalue_constructor)
{
    {
        Box<std::vector<int>> box{std::vector<int>{1, 2, 3}};
        ASSERT_EQ(box.value(), (std::vector<int>{1, 2, 3}));
    }

    {
        Box<std::vector<int>> box = std::vector<int>{1, 2, 3};
        ASSERT_EQ(box.value(), (std::vector<int>{1, 2, 3}));
    }

    {
        auto vec = std::vector<int>{1, 2, 3};
        auto dataPtr = vec.data();
        Box<std::vector<int>> box = std::move(vec);
        ASSERT_EQ(box.value(), (std::vector<int>{1, 2, 3}));
        ASSERT_EQ(dataPtr, box->data());
    }
}

namespace box {

struct A {
    virtual ~A() = default;
    virtual int foo() const { return 0; }
};
struct B : public A {
    int foo() const { return 25; }
};
}  // namespace box

TEST(Box, derived)
{
    using box::A;
    using box::B;
    {
        Box<A> a{B{}};
        ASSERT_EQ(a->foo(), 25);
    }

    {
        Box<B> b;
        Box<A> a{std::move(b)};
        ASSERT_EQ(a->foo(), 25);
    }

    {
        Box<B> b;
        Box<A> a;
        a = std::move(b);
        ASSERT_EQ(a->foo(), 25);
    }
}

namespace box {
struct Message {
    virtual ~Message() = default;
    virtual int foo() = 0;
};

template <typename Impl>
struct Model : public Message {
    Impl impl;

    Model(Impl impl) : impl{impl} {}

    int foo() override { return impl.foo(); }
};

template <typename Impl>
Box<Message> to_message(Impl impl)
{
    return Model<Impl>(std::move(impl));
}

struct Impl1 {
    int foo() { return 1; }
};

struct Impl2 {
    int foo() { return 2; }
};

}  // namespace box

TEST(Box, polymorphism)
{
    using namespace box;
    std::vector<Box<Message>> elems;

    elems.push_back(to_message(Impl1{}));
    elems.push_back(to_message(Impl2{}));

    for (auto&& elem : elems) std::cout << " foo : " << elem->foo() << std::endl;
}

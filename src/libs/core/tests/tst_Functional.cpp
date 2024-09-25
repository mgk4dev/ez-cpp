#include <gtest/gtest.h>

#include <ez/Functional.hpp>

using namespace ez;
struct ByteArray {};
struct String {};
struct Overloaded {
    void foo() {}
    void foo(ByteArray) {}
    void foo(ByteArray, const String&) {}
    void const_foo() const {}
    void const_foo(ByteArray) const {}
    void const_foo(ByteArray, const String&) const {}
    void mixed_foo() {}
    void mixed_foo(ByteArray) const {}
};
void free_fn_overloaded() {}
void free_fn_overloaded(ByteArray) {}
void free_fn_overloaded(ByteArray, const String&) {}
void free_fn_overloaded_get(ByteArray) {}
ByteArray free_fn_overloaded_get() { return ByteArray(); }

TEST(FnOverload, overload)
{
    ASSERT_EQ(fn_overload<>(&free_fn_overloaded), static_cast<void (*)()>(&free_fn_overloaded));
    ASSERT_EQ(fn_overload<ByteArray>(&free_fn_overloaded),
              static_cast<void (*)(ByteArray)>(&free_fn_overloaded));
    ASSERT_EQ((fn_overload<ByteArray, const String&>(&free_fn_overloaded)),
              static_cast<void (*)(ByteArray, const String&)>(&free_fn_overloaded));

    ASSERT_EQ(fn_overload<>(&free_fn_overloaded_get),
              static_cast<ByteArray (*)()>(&free_fn_overloaded_get));
    ASSERT_EQ(fn_overload<ByteArray>(&free_fn_overloaded_get),
              static_cast<void (*)(ByteArray)>(&free_fn_overloaded_get));

    ASSERT_EQ(fn_overload<>(&Overloaded::foo),
              static_cast<void (Overloaded::*)()>(&Overloaded::foo));
    ASSERT_EQ(fn_overload<ByteArray>(&Overloaded::foo),
              static_cast<void (Overloaded::*)(ByteArray)>(&Overloaded::foo));
    ASSERT_EQ((fn_overload<ByteArray, const String&>(&Overloaded::foo)),
              static_cast<void (Overloaded::*)(ByteArray, const String&)>(&Overloaded::foo));

    ASSERT_EQ(fn_overload<>(&Overloaded::const_foo),
              static_cast<void (Overloaded::*)() const>(&Overloaded::const_foo));
    ASSERT_EQ(fn_overload<ByteArray>(&Overloaded::const_foo),
              static_cast<void (Overloaded::*)(ByteArray) const>(&Overloaded::const_foo));
    ASSERT_EQ(
        (fn_overload<ByteArray, const String&>(&Overloaded::const_foo)),
        static_cast<void (Overloaded::*)(ByteArray, const String&) const>(&Overloaded::const_foo));

    ASSERT_EQ(fn_non_const_overload<>(&Overloaded::mixed_foo),
              static_cast<void (Overloaded::*)()>(&Overloaded::mixed_foo));
    ASSERT_EQ(fn_const_overload<ByteArray>(&Overloaded::mixed_foo),
              static_cast<void (Overloaded::*)(ByteArray) const>(&Overloaded::mixed_foo));
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////7
TEST(FunctionView, empty)
{
    FunctionView<void()> fv;
    ASSERT_FALSE(fv);
}

namespace {
int get42() { return 42; }
}  // namespace

TEST(FunctionView, freeFunction)
{
    FunctionView<int()> fv(get42);
    ASSERT_TRUE(fv);
    ASSERT_EQ(fv(), 42);
}

TEST(FunctionView, lambda)
{
    auto get42 = []() { return 42; };
    FunctionView<int()> fv(get42);
    ASSERT_TRUE(fv);
    ASSERT_EQ(fv(), 42);
}

TEST(FunctionView, callable)
{
    struct Callable {
        int operator()() const { return 42; }
    };

    Callable c;
    FunctionView<int()> fv(c);
    ASSERT_TRUE(fv);
    ASSERT_EQ(fv(), 42);
}

TEST(FunctionView, constCallable)
{
    struct Callable {
        int operator()() const { return 42; }
    };

    const Callable c;
    FunctionView<int()> fv(c);
    ASSERT_TRUE(fv);
    ASSERT_EQ(fv(), 42);
}

TEST(FunctionView, overload)
{
    struct Callable {
        int operator()() const { return 42; }
        std::string operator()(int) const { return "42"; }
        std::string operator()(const std::string& s1, const std::string& s2) const
        {
            return s1 + s2;
        }
    };

    Callable c;

    {
        FunctionView<int()> fv(c);
        ASSERT_TRUE(fv);
        ASSERT_EQ(fv(), 42);
    }

    {
        FunctionView<std::string(int)> fv(c);
        ASSERT_TRUE(fv);
        ASSERT_EQ(fv(0), "42");
    }

    {
        FunctionView<std::string(const std::string&, const std::string&)> fv(c);
        ASSERT_TRUE(fv);
        ASSERT_EQ(fv("4", "2"), "42");
    }
}

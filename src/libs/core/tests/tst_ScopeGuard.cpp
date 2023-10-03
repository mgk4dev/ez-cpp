#include <gtest/gtest.h>

#include <ez/ScopeGuard.hpp>

TEST(ScopeGuard, simple_action)
{
    int counter = 0;

    {
        EZ_ON_SCOPE_EXIT { ++counter; };
    }

    ASSERT_EQ(counter, 1);
}

struct Foo {
    int counter = 0;

    void doStuff() { ++counter; }

    void run()
    {
        EZ_ON_SCOPE_EXIT { doStuff(); };
    }
};

TEST(ScopeGuard, this_capture)
{
    Foo foo;
    foo.run();
    ASSERT_EQ(foo.counter, 1);
}

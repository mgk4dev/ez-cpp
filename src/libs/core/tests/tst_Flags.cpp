#include <gtest/gtest.h>

#include <ez/Flags.hpp>

using namespace ez;

enum MyFlags { NoOption, Option1 = 1, Option2 = 2, Option3 = 4 };

TEST(Flags, make_flags_regular_enum)
{
    make_flags(Option1);
    SUCCEED();
}

TEST(Flags, test_regular_enum)
{
    auto f = make_flags(Option1);
    ASSERT_TRUE(f.test(Option1));
    ASSERT_FALSE(f.test(Option2));
}

TEST(Flags, set_regular_enum)
{
    Flags<MyFlags> f;
    f.set(Option1, true);
    ASSERT_TRUE(f.test(Option1));
    ASSERT_FALSE(f.test(Option2));

    f.set(Option1, false);
    ASSERT_FALSE(f.test(Option1));
}

TEST(Flags, or_regular_enum)
{
    Flags<MyFlags> f = Option1 | Option2;
    ASSERT_TRUE(f.test(Option1));
    ASSERT_TRUE(f.test(Option2));
    ASSERT_FALSE(f.test(Option3));

    Flags<MyFlags> f1 = Option1;
    Flags<MyFlags> f2 = Option2;
    f = f1 | f2;
    ASSERT_TRUE(f.test(Option1));
    ASSERT_TRUE(f.test(Option2));
    ASSERT_FALSE(f.test(Option3));
}

enum class Options { NoOption, Option1 = 1, Option2 = 2, Option3 = 4 };

TEST(Flags, make_flags_class_enum)
{
    make_flags(Options::Option1);
    SUCCEED();
}

TEST(Flags, test_class_enum)
{
    auto f = make_flags(Options::Option1);
    ASSERT_TRUE(f.test(Options::Option1));
    ASSERT_FALSE(f.test(Options::Option2));
}

TEST(Flags, test_multiple_class_enum)
{
    auto f = make_flags(Options::Option1, Options::Option2);
    ASSERT_TRUE(f.all(Options::Option1, Options::Option2));
    ASSERT_FALSE(f.all(Options::Option1, Options::Option3));
}

TEST(Flags, boolean_operator_class_enum)
{
    auto f = make_flags(Options::Option1);
    ASSERT_TRUE(bool(f));
    ASSERT_FALSE(bool(Flags<Options>{}));
}

TEST(Flags, any_class_enum)
{
    auto f = make_flags(Options::Option1, Options::Option2);
    ASSERT_TRUE(f.any(Options::Option1, Options::Option3));
    ASSERT_TRUE(f.any(Options::Option2, Options::Option3));
    ASSERT_FALSE(f.any(Options::Option3));
}

TEST(Flags, set_class_enum)
{
    Flags<Options> f;
    f.set(Options::Option1, true);
    ASSERT_TRUE(f.test(Options::Option1));
    ASSERT_FALSE(f.test(Options::Option2));

    f.set(Options::Option1, false);
    ASSERT_FALSE(f.test(Options::Option1));
}

TEST(Flags, or_class_enum)
{
    Flags<Options> f = as_int(Options::Option1) | as_int(Options::Option2);
    ASSERT_TRUE(f.test(Options::Option1));
    ASSERT_TRUE(f.test(Options::Option2));
    ASSERT_FALSE(f.test(Options::Option3));

    Flags<Options> f1 = Options::Option1;
    Flags<Options> f2 = Options::Option2;
    f = f1 | f2;
    ASSERT_TRUE(f.test(Options::Option1));
    ASSERT_TRUE(f.test(Options::Option2));
    ASSERT_FALSE(f.test(Options::Option3));
}

TEST(Flags, equality_operator_class_enum)
{
    Flags<Options> f1 = Options::Option1;
    Flags<Options> f2 = Options::Option1;
    ASSERT_TRUE(f1 == f2);

    f1 = f1 | as_int(Options::Option2);
    ASSERT_FALSE(f1 == f2);
    ASSERT_TRUE(f1 != f2);

    f2 = f2 | as_int(Options::Option2);
    ASSERT_TRUE(f1 == f2);

    f1 = Flags<Options>();
    ASSERT_FALSE(f1 == f2);
}

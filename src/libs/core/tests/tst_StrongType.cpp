#include <gtest/gtest.h>

#include <ez/StrongType.hpp>

#include <unordered_set>

using namespace ez;

TEST(StrongType, ctor)
{
    using Type = StrongType<std::string, struct TypeTag>;

    Type v{5, '*'};

    ASSERT_EQ(v.value(), "*****");
}

///////////////////////////////////////////////////////////////////////////////
/// Unary oprerators

TEST(StrongType, unary_plus_op)
{
    using Type = StrongType<int, struct TypeTag, mixin::UnaryPlusOp>;
    auto var = +Type{10};

    ASSERT_EQ(var.value(), 10);
}

TEST(StrongType, unary_minus_op)
{
    using Type = StrongType<int, struct TypeTag, mixin::UnaryMinusOp>;
    auto var = -Type{10};

    ASSERT_EQ(var.value(), -10);
}

TEST(StrongType, unary_not_op)
{
    using Type = StrongType<bool, struct TypeTag, mixin::UnaryNotOp>;
    auto var = !Type{false};

    ASSERT_TRUE(var.value());
}

///////////////////////////////////////////////////////////////////////////////
/// Prefix operators
TEST(StrongType, incr_op)
{
    using Type = StrongType<int, struct TypeTag, mixin::IncrOp>;
    auto var = Type{10};

    ASSERT_EQ((++var).value(), 11);
    ASSERT_EQ((var++).value(), 11);
    ASSERT_EQ((var).value(), 12);
}

TEST(StrongType, decr_op)
{
    using Type = StrongType<int, struct TypeTag, mixin::DecrOp>;
    auto var = Type{10};

    ASSERT_EQ((--var).value(), 9);
    ASSERT_EQ((var--).value(), 9);
    ASSERT_EQ((var).value(), 8);
}

///////////////////////////////////////////////////////////////////////////////
/// Compound oprerators

TEST(StrongType, plus_eq_op)
{
    using Type = StrongType<int, struct TypeTag, mixin::PlusEqOp>;
    auto var1 = Type{10};
    auto var2 = Type{10};
    var1 += var2;

    ASSERT_EQ(var1.value(), 20);
}

TEST(StrongType, minus_eq_op)
{
    using Type = StrongType<int, struct TypeTag, mixin::MinusEqOp>;
    auto var1 = Type{10};
    auto var2 = Type{10};
    var1 -= var2;

    ASSERT_EQ(var1.value(), 0);
}

TEST(StrongType, star_eq_op)
{
    using Type = StrongType<int, struct TypeTag, mixin::StarEqOp>;
    auto var1 = Type{10};
    auto var2 = Type{10};
    var1 *= var2;

    ASSERT_EQ(var1.value(), 100);
}

TEST(StrongType, div_eq_op)
{
    using Type = StrongType<int, struct TypeTag, mixin::DivEqOp>;
    auto var1 = Type{10};
    auto var2 = Type{10};
    var1 /= var2;

    ASSERT_EQ(var1.value(), 1);
}

TEST(StrongType, module_eq_op)
{
    using Type = StrongType<int, struct TypeTag, mixin::ModuloEqOp>;
    auto var1 = Type{10};
    auto var2 = Type{5};
    var1 %= var2;

    ASSERT_EQ(var1.value(), 10 % 5);
}

///////////////////////////////////////////////////////////////////////////////
/// Binary oprerators

TEST(StrongType, binary_plus_op)
{
    using Type = StrongType<int, struct TypeTag, mixin::BinaryPlusOp>;
    auto var1 = Type{10};
    auto var2 = Type{5};

    ASSERT_EQ((var1 + var2).value(), 15);
}

TEST(StrongType, binary_minus_op)
{
    using Type = StrongType<int, struct TypeTag, mixin::BinaryMinusOp>;
    auto var1 = Type{10};
    auto var2 = Type{5};

    ASSERT_EQ((var1 - var2).value(), 5);
}

TEST(StrongType, binary_star_op)
{
    using Type = StrongType<int, struct TypeTag, mixin::BinaryStarOp>;
    auto var1 = Type{10};
    auto var2 = Type{5};

    ASSERT_EQ((var1 * var2).value(), 50);
}

TEST(StrongType, binary_div_op)
{
    using Type = StrongType<int, struct TypeTag, mixin::BinaryDivOp>;
    auto var1 = Type{10};
    auto var2 = Type{5};

    ASSERT_EQ((var1 / var2).value(), 2);
}

TEST(StrongType, binary_mod_op)
{
    using Type = StrongType<int, struct TypeTag, mixin::BinaryModuloOp>;
    auto var1 = Type{10};
    auto var2 = Type{5};

    ASSERT_EQ((var1 % var2).value(), 10 % 5);
}

TEST(StrongType, binary_or_op)
{
    using Type = StrongType<bool, struct TypeTag, mixin::BinaryOrOp>;
    auto var1 = Type{true};
    auto var2 = Type{false};

    ASSERT_TRUE((var1 || var2).value());
}

TEST(StrongType, binary_and_op)
{
    using Type = StrongType<bool, struct TypeTag, mixin::BinaryAndOp>;
    auto var1 = Type{true};
    auto var2 = Type{false};

    ASSERT_FALSE((var1 && var2).value());
}

///////////////////////////////////////////////////////////////////////////////
/// Value oprerators
TEST(StrongType, value_plus_op)
{
    using Type = StrongType<int, struct TypeTag, mixin::ValuePlusOp>;
    auto var = Type{10};

    ASSERT_EQ((var + 5).value(), 15);
}
TEST(StrongType, value_minus_op)
{
    using Type = StrongType<int, struct TypeTag, mixin::ValueMinusOp>;
    auto var = Type{10};

    ASSERT_EQ((var - 5).value(), 5);
}
TEST(StrongType, value_star_op)
{
    using Type = StrongType<int, struct TypeTag, mixin::ValueStarOp>;
    auto var = Type{10};

    ASSERT_EQ((var * 5).value(), 50);
}
TEST(StrongType, value_div_op)
{
    using Type = StrongType<int, struct TypeTag, mixin::ValueDivOp>;
    auto var = Type{10};

    ASSERT_EQ((var / 5).value(), 2);
}
TEST(StrongType, value_modulo_op)
{
    using Type = StrongType<int, struct TypeTag, mixin::ValueModuloOp>;
    auto var = Type{10};

    ASSERT_EQ((var % 3).value(), 10 % 3);
}

///////////////////////////////////////////////////////////////////////////////
/// Compound value oprerators
TEST(StrongType, value_plus_eq_op)
{
    using Type = StrongType<int, struct TypeTag, mixin::ValuePlusEqOp>;
    auto var = Type{10};
    var += 5;

    ASSERT_EQ(var.value(), 15);
}
TEST(StrongType, value_minus_eq_op)
{
    using Type = StrongType<int, struct TypeTag, mixin::ValueMinusEqOp>;
    auto var = Type{10};
    var -= 5;

    ASSERT_EQ(var.value(), 5);
}
TEST(StrongType, value_star_eq_op)
{
    using Type = StrongType<int, struct TypeTag, mixin::ValueStarEqOp>;
    auto var = Type{10};
    var *= 5;

    ASSERT_EQ(var.value(), 50);
}
TEST(StrongType, value_div_eq_op)
{
    using Type = StrongType<int, struct TypeTag, mixin::ValueDivEqOp>;
    auto var = Type{10};
    var /= 5;

    ASSERT_EQ(var.value(), 2);
}
TEST(StrongType, value_modulo_eq_op)
{
    using Type = StrongType<int, struct TypeTag, mixin::ValueModuloEqOp>;
    auto var = Type{10};
    var %= 3;

    ASSERT_EQ(var.value(), 10 % 3);
}

///////////////////////////////////////////////////////////////////////////////

TEST(StrongType, comparable)
{
    using Type = StrongType<int, struct TypeTag, mixin::Comparable>;

    Type v1{10}, v2{20};

    ASSERT_EQ(v1, v1);

    ASSERT_NE(v1, v2);

    ASSERT_LT(v1, v2);

    ASSERT_LE(v1, v1);
    ASSERT_LE(v1, v2);

    ASSERT_GT(v2, v1);

    ASSERT_GE(v1, v1);
    ASSERT_GE(v2, v1);
}

TEST(StrongType, swap)
{
    using Type = StrongType<std::string, struct TypeTag, mixin::ArrowOp>;

    Type v1{"toto"}, v2{"titi"};

    std::swap(v1, v2);

    ASSERT_EQ(v1.value(), "titi");
    ASSERT_EQ(v2.value(), "toto");
}

TEST(StrongType, hashable)
{
    using Type = StrongType<std::string, struct TypeTag, mixin::Hashable, mixin::EqComparable>;

    std::unordered_set<Type> set;

    set.insert(Type("toto"));
    set.insert(Type("titi"));

    ASSERT_EQ(set.count(Type("toto")), 1);
    ASSERT_EQ(set.count(Type("titi")), 1);
    ASSERT_EQ(set.count(Type("pioupiou")), 0);
}

//===========================================================================

template <typename StrongT, typename ValueType>
struct AddAndDevideByTen : public mixin::MixinBase<StrongT, struct CustomMixinTag> {
    StrongT operator+(const StrongT& rhs) const
    {
        return StrongT((this->value() + rhs.value()) * 10);
    }
};

TEST(StrongType, custom_mixin)
{
    using Type = StrongType<int, struct TypeTag, AddAndDevideByTen>;

    Type v1{10}, v2{20};

    ASSERT_EQ((v1 + v2).value(), 300);
}

TEST(StrongType, disambiguation)
{
    struct NotAmbiguous
        : public StrongType<int, struct NotAmbiguousTag, mixin::PlusEqOp, mixin::ValuePlusEqOp> {
        using Self::Self;
        using Extension<mixin::PlusEqOp>::operator+=;
        using Extension<mixin::ValuePlusEqOp>::operator+=;
    };

    NotAmbiguous var{0};

    var += 3;
    var += NotAmbiguous(5);

    ASSERT_EQ(var.value(), 8);
}

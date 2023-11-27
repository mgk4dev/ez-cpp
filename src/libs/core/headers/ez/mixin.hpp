#pragma once

#include <ez/preprocessor.hpp>

#include <utility>

#define EZ_MIXIN_COMPOUND_OPERATOR(MixinName, op)                                 \
    template <typename Self, typename ValueType>                                  \
    struct MixinName : public MixinBase<Self, struct EZ_CONCAT(MixinName, Tag)> { \
        Self& operator op(const Self& rhs)                                        \
        {                                                                         \
            this->value() op rhs.value();                                         \
            return this->self();                                                  \
        }                                                                         \
        Self& operator op(Self&& rhs)                                             \
        {                                                                         \
            this->value() op std::move(rhs.value());                              \
            return this->self();                                                  \
        }                                                                         \
    }

#define EZ_MIXIN_COMPOUND_VALUE_OPERATOR(MixinName, op)                           \
    template <typename Self, typename ValueType>                                  \
    struct MixinName : public MixinBase<Self, struct EZ_CONCAT(MixinName, Tag)> { \
        Self& operator op(const ValueType& val)                                   \
        {                                                                         \
            this->value() op val;                                                 \
            return this->self();                                                  \
        }                                                                         \
    }

#define EZ_MIXIN_COMPOUND_GENRIC_VALUE_OPERATOR(MixinName, op)                    \
    template <typename Self, typename ValueType>                                  \
    struct MixinName : public MixinBase<Self, struct EZ_CONCAT(MixinName, Tag)> { \
        template <typename T>                                                     \
        Self& operator op(T&& val)                                                \
        {                                                                         \
            this->value() op std::forward<T>(val);                                \
            return this->self();                                                  \
        }                                                                         \
    }

#define EZ_MIXIN_UNARY_OPERATOR(MixinName, op)                                    \
    template <typename Self, typename ValueType>                                  \
    struct MixinName : public MixinBase<Self, struct EZ_CONCAT(MixinName, Tag)> { \
        Self operator op() const                                                  \
        {                                                                         \
            return Self(op this->value());                                        \
        }                                                                         \
    }

#define EZ_MIXIN_BINARY_OPERATOR(MixinName, op)                                   \
    template <typename Self, typename ValueType>                                  \
    struct MixinName : public MixinBase<Self, struct EZ_CONCAT(MixinName, Tag)> { \
        Self operator op(const Self& rhs) const                                   \
        {                                                                         \
            return Self(this->value() op rhs.value());                            \
        }                                                                         \
        Self operator op(Self&& rhs) const                                        \
        {                                                                         \
            return Self(this->value() op std::move(rhs.value()));                 \
        }                                                                         \
    }

#define EZ_MIXIN_VALUE_BINARY_OPERATOR(MixinName, op)                             \
    template <typename Self, typename ValueType>                                  \
    struct MixinName : public MixinBase<Self, struct EZ_CONCAT(MixinName, Tag)> { \
        Self operator op(ValueType&& val) const                                   \
        {                                                                         \
            return Self(this->value() op std::move(val));                         \
        }                                                                         \
        Self operator op(const ValueType& val) const                              \
        {                                                                         \
            return Self(this->value() op val);                                    \
        }                                                                         \
    }

namespace ez::mixin {

template <typename Self, typename Tag>
struct MixinBase {
protected:
    const Self& self() const noexcept { return static_cast<const Self&>(*this); }
    Self& self() noexcept { return static_cast<Self&>(*this); }
    const auto& value() const noexcept { return self().value(); }
    auto& value() noexcept { return self().value(); }
};

EZ_MIXIN_UNARY_OPERATOR(UnaryPlusOp, +);
EZ_MIXIN_UNARY_OPERATOR(UnaryMinusOp, -);
EZ_MIXIN_UNARY_OPERATOR(UnaryNotOp, !);

EZ_MIXIN_COMPOUND_OPERATOR(PlusEqOp, +=);
EZ_MIXIN_COMPOUND_OPERATOR(MinusEqOp, -=);
EZ_MIXIN_COMPOUND_OPERATOR(StarEqOp, *=);
EZ_MIXIN_COMPOUND_OPERATOR(DivEqOp, /=);
EZ_MIXIN_COMPOUND_OPERATOR(ModuloEqOp, %=);

EZ_MIXIN_BINARY_OPERATOR(BinaryPlusOp, +);
EZ_MIXIN_BINARY_OPERATOR(BinaryMinusOp, -);
EZ_MIXIN_BINARY_OPERATOR(BinaryStarOp, *);
EZ_MIXIN_BINARY_OPERATOR(BinaryDivOp, /);
EZ_MIXIN_BINARY_OPERATOR(BinaryModuloOp, %);
EZ_MIXIN_BINARY_OPERATOR(BinaryOrOp, ||);
EZ_MIXIN_BINARY_OPERATOR(BinaryAndOp, &&);

EZ_MIXIN_VALUE_BINARY_OPERATOR(ValuePlusOp, +);
EZ_MIXIN_VALUE_BINARY_OPERATOR(ValueMinusOp, -);
EZ_MIXIN_VALUE_BINARY_OPERATOR(ValueStarOp, *);
EZ_MIXIN_VALUE_BINARY_OPERATOR(ValueDivOp, /);
EZ_MIXIN_VALUE_BINARY_OPERATOR(ValueModuloOp, %);

EZ_MIXIN_COMPOUND_VALUE_OPERATOR(ValuePlusEqOp, +=);
EZ_MIXIN_COMPOUND_VALUE_OPERATOR(ValueMinusEqOp, -=);
EZ_MIXIN_COMPOUND_VALUE_OPERATOR(ValueStarEqOp, *=);
EZ_MIXIN_COMPOUND_VALUE_OPERATOR(ValueDivEqOp, /=);
EZ_MIXIN_COMPOUND_VALUE_OPERATOR(ValueModuloEqOp, %=);

EZ_MIXIN_COMPOUND_GENRIC_VALUE_OPERATOR(GenericValuePlusEqOp, +=);
EZ_MIXIN_COMPOUND_GENRIC_VALUE_OPERATOR(GenericValueMinusEqOp, -=);
EZ_MIXIN_COMPOUND_GENRIC_VALUE_OPERATOR(GenericValueStarEqOp, *=);
EZ_MIXIN_COMPOUND_GENRIC_VALUE_OPERATOR(GenericValueDivEqOp, /=);
EZ_MIXIN_COMPOUND_GENRIC_VALUE_OPERATOR(GenericValueModuloEqOp, %=);

template <typename Self, typename ValueType>
struct IncrOp : public MixinBase<Self, struct ComparableTag> {
    Self& operator++()
    {
        ++this->value();
        return this->self();
    }

    Self operator++(int)
    {
        auto tmp = this->self();
        operator++();
        return tmp;
    }
};

template <typename Self, typename ValueType>
struct DecrOp : public MixinBase<Self, struct ComparableTag> {
    Self& operator--()
    {
        --this->value();
        return this->self();
    }

    Self operator--(int)
    {
        auto tmp = this->self();
        operator--();
        return tmp;
    }
};

template <typename Self, typename ValueType>
struct EqComparable : public MixinBase<Self, ValueType> {
    bool operator==(const Self& rhs) const { return this->value() == rhs.value(); }

    bool operator!=(const Self& rhs) const { return !(*this == rhs); }
};

template <typename Self, typename ValueType>
struct Comparable : public EqComparable<Self, ValueType> {
    bool operator<(const Self& rhs) const { return this->value() < rhs.value(); }

    bool operator>(const Self& rhs) const { return this->value() > rhs.value(); }

    bool operator<=(const Self& rhs) const { return this->value() <= rhs.value(); }

    bool operator>=(const Self& rhs) const { return this->value() >= rhs.value(); }
};

template <typename Self, typename ValueType>
struct ArrowOp : public MixinBase<Self, struct ArrowOpTag> {
    const ValueType* operator->() const noexcept { return &this->value(); }

    ValueType* operator->() noexcept { return &this->value(); }
};

template <typename Self, typename ValueType>
struct Hashable : public MixinBase<Self, struct HashableTag> {};

//=========================================================

template <typename Self, typename ValueType>
struct NumberSemantic : BinaryPlusOp<Self, ValueType>,
                        PlusEqOp<Self, ValueType>,
                        BinaryMinusOp<Self, ValueType>,
                        MinusEqOp<Self, ValueType>,
                        ValueStarOp<Self, ValueType>,
                        GenericValueStarEqOp<Self, ValueType>,
                        ValueDivOp<Self, ValueType>,
                        GenericValueDivEqOp<Self, ValueType>,
                        Comparable<Self, ValueType> {};

}  // namespace ez::mixin

#pragma once

#include <ez/Mixin.hpp>
#include <ez/ValueWrapper.hpp>

#include <concepts>
#include <functional>

namespace ez {
///
/// StrongType is a type wrapper to add semantic to a base type and avoid programming mistakes:
/// Example of ambiguous code:
/// @code
///
/// int width  = 10;
/// int height = 20;
/// void resize(Widget& w, int width, int height);
/// resize(w, height, width); //oops
/// @endcode
///
/// A StrongType is built from a
/// - Value type : the wrapper type
/// - A type tag : to avoid unwanted assignments
/// - An optional set of skills/extensions using compile time mixins.
///
/// Example 1 : Width/Height
/// @code
/// using Width = StrongType<int, struct WidthTag, mixin::NumberSemantic>;
/// using Height = StrongType<int, struct HeightTag, mixin::NumberSemantic>;
/// void resize(Widget& w, Width width, Height height);
/// Width width  = 10;
/// Height height = 20;
///
/// resize(w, height, width); !! This will not compile !!
///
/// @endcode
///
/// Example 2 : Custom extensions
/// @code
/// template <typename StrongT, typename ValueType>
/// struct MyExtension : public mixin::Mixin<StrongT, struct CustomMixinTag>
/// {
///     auto convert() const
///     {
///         return ...;
///     }
/// };
/// @endcode
/// using Width = StrongType<int, struct WidthTag, mixin::NumberSemantic, MyExtension>;
/// Width w = 10;
/// auto val = w.convert();
///
/// Limitations:
/// Mixins same operators will induce some ambiguous operator overload resolution.
/// @code
/// using Ambiguous = StrongType<int, struct _, mixin::CompoundPlusOp, mixin::CompoundValuePlusOp>;
/// Ambiguous var{0};
/// var += 3; // this doesn't compile as it is ambiguous
/// var += Ambiguous(5); // this doesn't compile as it is ambiguous
/// @endcode
/// The wrokaround is expose explicitly the ambiguous operators:
/// @code
/// struct NotAmbiguous : public StrongType<int, struct NotAmbiguousTag, mixin::PlusEqOp,
/// mixin::ValuePlusEqOp>
/// {
///     using self_type::self_type;
///     using Extension<mixin::PlusEqOp>::operator+=;
///     using Extension<mixin::ValuePlusEqOp>::operator+=;
/// };
/// NotAmbiguous var{0};
/// var += 3;
/// var += NotAmbiguous(5);
/// @endcode
///
/// For a complete list of extensions, please check <ez/mixin.hpp>
/// For more examples, please check the unit tests.
///
template <typename T, typename Tag, template <typename, typename> class... Mixins>
class StrongType : public ValueWrapper<T>, public Mixins<StrongType<T, Tag, Mixins...>, T>... {
public:
    using Self = StrongType<T, Tag, Mixins...>;
    using typename ValueWrapper<T>::ValueType;

    template <template <typename, typename> class Mixin>
    using Extension = Mixin<StrongType, ValueType>;

    using ValueWrapper<T>::ValueWrapper;
    using ValueWrapper<T>::value;
    using ValueWrapper<T>::swap;
};

template <typename T>
inline constexpr bool is_hashable()
{
    return std::is_base_of_v<ez::mixin::Hashable<T, typename T::ValueType>, T>;
}

}  // namespace ez

namespace std {
template <typename T, typename Tag, template <typename, typename> class... Mixins>
struct hash<::ez::StrongType<T, Tag, Mixins...>> {
    using Type = ::ez::StrongType<T, Tag, Mixins...>;
    using Enable = typename std::enable_if<::ez::is_hashable<Type>()>::type;

    size_t operator()(const ::ez::StrongType<T, Tag, Mixins...>& var) const
    {
        return std::hash<T>{}(var.value());
    }
};
}  // namespace std

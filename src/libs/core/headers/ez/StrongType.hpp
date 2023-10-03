#pragma once

#include <ez/Mixin.hpp>
#include <ez/ValueWrapper.hpp>

#include <concepts>
#include <functional>

namespace ez {

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

private:
    T m_value;
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

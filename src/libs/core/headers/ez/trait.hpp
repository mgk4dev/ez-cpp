#pragma once

#include <concepts>
#include <type_traits>

namespace ez::trait::detail {

template <typename... T>
struct IsInvocable;

template <typename Callable, typename R, typename... Args>
struct IsInvocable<Callable, R(Args...)> {
    static constexpr bool value = std::is_invocable_v<Callable, Args...> &&
                                  std::is_same_v<std::invoke_result_t<Callable, Args...>, R>;
};

template <typename T, template <typename...> typename Template>
struct IsTemplateImpl {
    static constexpr bool value = false;
};

template <template <typename...> typename Template1,
          template <typename...>
          typename Template2,
          typename... Args>
struct IsTemplateImpl<Template1<Args...>, Template2> {
    static constexpr bool value = std::same_as<Template1<Args...>, Template2<Args...>>;
};

}  // namespace ez::trait::detail

namespace ez::trait {

template <typename T, typename U>
concept Is = std::same_as<T, U>;

template <typename T, typename... Ts>
concept OneOf = (std::same_as<T, Ts> || ...);

template <typename T, typename U>
concept AnyRef = std::same_as<std::decay_t<T>, U>;

template <typename To, typename From>
concept ConvertibleTo = std::convertible_to<From, To>;

template <typename Derived, typename Base>
concept DerivedFrom = std::is_base_of_v<Base, Derived>;

template <typename T, template <typename...> typename Template>
concept IsTemplate = detail::IsTemplateImpl<T, Template>::value;

template <typename Callable, typename Signature>
concept Fn = detail::IsInvocable<Callable, Signature>::value;

}  // namespace ez::trait

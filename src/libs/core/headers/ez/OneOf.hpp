#pragma once

#include <ez/Overload.hpp>
#include <ez/Traits.hpp>
#include <ez/Utils.hpp>

#include <variant>

namespace ez {

template <typename T>
struct CaseT {
    auto operator=(auto&& f) const
    {
        return [&f](trait::AnyRef<T> auto&& ref) { return f(EZ_FWD(ref)); };
    }
};

template <>
struct CaseT<void> {
    auto operator=(auto&& f) const
    {
        return [&f](auto&& ref) { return f(EZ_FWD(ref)); };
    }
};

template <typename T = void>
constexpr CaseT<T> Case;

/// OneOf is an extension of std::variant with visitation features.
/// Usage:
/// @code
/// OneOf<int, double, std::string> val = 25;
/// ASSERT_TRUE(val.is<int>());
/// std::cout << val.as<int>();
/// val.match(
///     [](int val)   { std::cout << "It is an int : " << val ; }
///     [](double val){ std::cout << "It is an double : " << val ; }
///     [](auto&& val){ std::cout << "It is something else " ; }
/// );
/// @endcode
template <typename... Ts>
class OneOf : public std::variant<Ts...> {
public:
    using Super = std::variant<Ts...>;

    using Super::Super;
    using Super::operator=;

    static constexpr unsigned int count() { return sizeof...(Ts); }

    template <typename T>
    bool is() const
    {
        return std::holds_alternative<T>(*this);
    }

    template <typename T>
    const T& as() const
    {
        return std::get<T>(*this);
    }

    template <typename T>
    T& as()
    {
        return std::get<T>(*this);
    }

    decltype(auto) match(auto&&... visitors) const&
    {
        return apply_visitor(Overload{EZ_FWD(visitors)...});
    }

    decltype(auto) match(auto&&... visitors) &
    {
        return apply_visitor(Overload{EZ_FWD(visitors)...});
    }

    decltype(auto) match(auto&&... visitors) &&
    {
        return std::move(*this).apply_visitor(Overload{EZ_FWD(visitors)...});
    }

    decltype(auto) apply_visitor(auto&& visitor) const&
    {
        return std::visit(EZ_FWD(visitor), *this);
    }
    decltype(auto) apply_visitor(auto&& visitor) & { return std::visit(EZ_FWD(visitor), *this); }
    decltype(auto) apply_visitor(auto&& visitor) &&
    {
        return std::visit(EZ_FWD(visitor), std::move(*this));
    }

    decltype(auto) operator>>(auto&& visitor) const& { return std::visit(EZ_FWD(visitor), *this); }
    decltype(auto) operator>>(auto&& visitor) & { return std::visit(EZ_FWD(visitor), *this); }
    decltype(auto) operator>>(auto&& visitor) &&
    {
        return std::visit(EZ_FWD(visitor), std::move(*this));
    }
};

}  // namespace ez

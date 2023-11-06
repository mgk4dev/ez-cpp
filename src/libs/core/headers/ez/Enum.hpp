#pragma once

#include <ez/trait.hpp>
#include <ez/utils.hpp>

#include <variant>

namespace ez {

template <typename... Visitors>
struct Overload : Visitors... {
    using Visitors::operator()...;
};

template <class... Visitors>
Overload(Visitors...) -> Overload<Visitors...>;

///////////////////////////////////////////////////////////////////////////////

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

template <typename... Ts>
class Enum : public std::variant<Ts...> {
public:
    using std::variant<Ts...>::variant;

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
        Overload visitor{EZ_FWD(visitors)...};
        return std::visit(visitor, *this);
    }

    decltype(auto) match(auto&&... visitors) &
    {
        Overload visitor{EZ_FWD(visitors)...};
        return std::visit(visitor, *this);
    }

    decltype(auto) match(auto&&... visitors) &&
    {
        Overload visitor{EZ_FWD(visitors)...};
        return std::visit(visitor, std::move(*this));
    }
};

}  // namespace ez

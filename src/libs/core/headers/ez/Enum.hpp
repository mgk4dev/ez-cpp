#pragma once

#include <ez/Trait.hpp>

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
    template <typename F>
    auto operator=(F&& f) const
    {
        return [&f](trait::AnyRef<T> auto&& ref) { return f(std::forward<decltype(ref)>(ref)); };
    }
};

template <>
struct CaseT<void> {
    template <typename F>
    auto operator=(F&& f) const
    {
        return [&f](auto&& ref) { return f(std::forward<decltype(ref)>(ref)); };
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

    template <typename... Visitors>
    decltype(auto) match(Visitors&&... visitors) const&
    {
        Overload visitor{std::forward<Visitors>(visitors)...};
        return std::visit(visitor, *this);
    }

    template <typename... Visitors>
    decltype(auto) match(Visitors&&... visitors) &
    {
        Overload visitor{std::forward<Visitors>(visitors)...};
        return std::visit(visitor, *this);
    }

    template <typename... Visitors>
    decltype(auto) match(Visitors&&... visitors) &&
    {
        Overload visitor{std::forward<Visitors>(visitors)...};
        return std::visit(visitor, std::move(*this));
    }
};

}  // namespace ez

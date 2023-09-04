#pragma once

#include <variant>

namespace ez {

template <typename... Visitors>
struct Overload : Visitors... {
    using Visitors::operator()...;
};

template <class... Visitors>
Overload(Visitors...) -> Overload<Visitors...>;

/// dispatch allows to visit a variant using callables.
/// Example:
/// @code {.c++}
/// std::variant<bool, int, std::string> v = some_value();
/// variant::dispatch(v,
///     [](bool val) {std::cout << "It is a bool " << val;},
///     [](int val) {std::cout << "It is a int " << val;}
///     [](const std::string& val) {std::cout << "It is a string " << val;}
/// );
/// @endcode
template <typename Variant, typename... Visitors>
decltype(auto) dispatch(Variant&& variant, Visitors&&... visitors)
{
    Overload visitor{std::forward<Visitors>(visitors)...};
    return std::visit(visitor, variant);
}

}  // namespace ez

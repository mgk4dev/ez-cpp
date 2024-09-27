#pragma once

namespace ez {
template <typename... Visitors>
struct Overload : Visitors... {
    using Visitors::operator()...;
};

template <class... Visitors>
Overload(Visitors...) -> Overload<Visitors...>;

}  // namespace ez

#pragma once

#include <ez/async/Async.hpp>

namespace ez {

template <typename T = void>
using Future = async::Async<T, std::suspend_never>;

}  // namespace ez

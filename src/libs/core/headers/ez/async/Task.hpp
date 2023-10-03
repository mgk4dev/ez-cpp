#pragma once

#include <ez/async/Async.hpp>

namespace ez {

template <typename T = void>
using Task = async::Async<T, std::suspend_always>;

}  // namespace ez

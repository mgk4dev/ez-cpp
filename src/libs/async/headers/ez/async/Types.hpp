#pragma once

#include <coroutine>
#include <stdexcept>

namespace ez::async {
template <typename P = void>
using Coroutine = std::coroutine_handle<P>;

struct CoroutineDeleter {
    void operator()(auto coroutine) const
    {
        if (coroutine) { coroutine.destroy(); }
    }
};

template <typename Promise>
auto make_coroutine(Promise& p)
{
    return Coroutine<Promise>::from_promise(p);
}

inline void safe_resume(Coroutine<> coroutine)
{
    if (coroutine) coroutine.resume();
}

class BrokenPromise : public std::runtime_error {
public:
    BrokenPromise() : std::runtime_error{"Broken promise"} {}
};

class ValueNotSet : public std::runtime_error {
public:
    ValueNotSet() : std::runtime_error{"Value not set"} {}
};

}  // namespace ez::async

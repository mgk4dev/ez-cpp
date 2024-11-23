#pragma once

#include <ez/Resource.hpp>

#include <coroutine>
#include <stdexcept>

namespace ez::async {
template <typename P = void>
using CoHandle = std::coroutine_handle<P>;

struct CoroutineDeleter {
    void operator()(auto coroutine) const
    {
        if (coroutine) { coroutine.destroy(); }
    }
};

template <typename Promise>
using UniqueCoroutine = Resource<CoHandle<Promise>, CoroutineDeleter>;

template <typename Promise>
auto make_coroutine(Promise& p)
{
    return CoHandle<Promise>::from_promise(p);
}

inline void safe_resume(CoHandle<> coroutine)
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

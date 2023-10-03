#pragma once

#include <ez/Trait.hpp>
#include <ez/async/Async.hpp>

namespace ez::trait {

// clang-format off
template <typename T>
concept Awaiter = requires(T awaiter) {
    { awaiter.await_ready() } -> Is<bool>;
    { awaiter.await_suspend(std::declval<Coroutine<>>()) } -> Is<Coroutine<>>;
    { awaiter.await_resume()};
};

// clang-format on

}  // namespace ez::trait

#pragma once

#include <ez/async/Types.hpp>

#include <ez/Traits.hpp>
#include <ez/Utils.hpp>

namespace ez::trait {

// clang-format off
template <typename T>
concept Awaiter = requires(T awaitable) {
    { awaitable.await_ready() } -> Is<bool>;
    { awaitable.await_suspend(std::declval<async::Coroutine<>>()) } -> OneOf<async::Coroutine<>, void, bool>;
    { awaitable.await_resume()};
};

template <typename T>
concept ImplementsCowaitOp = requires(T val)
{
    { std::move(val).operator co_await() } -> Awaiter;
};

template <typename T>
concept HasGlobalCowaitOp = requires(T val)
{
    { operator co_await(val) } -> Awaiter;
};

template<typename T>
concept Awaitable = Awaiter<T> || ImplementsCowaitOp<T> || HasGlobalCowaitOp<T>;


template <trait::Awaitable Awaitable>
auto get_awaiter(Awaitable&& value) -> decltype(auto)
{
    if constexpr (trait::ImplementsCowaitOp<Awaitable>)
        return EZ_FWD(value).operator co_await();
    else if constexpr (trait::HasGlobalCowaitOp<Awaitable>)
        return operator co_await(EZ_FWD(value));
    else if constexpr (trait::Awaiter<Awaitable>) {
        return EZ_FWD(value);
    }
}

template <trait::Awaitable Awaitable, typename = void>
struct AwaitableTraits {};

template <trait::Awaitable Awaitable>
struct AwaitableTraits<Awaitable> {
    using Awaiter = decltype(get_awaiter(std::declval<Awaitable>()));
    using R = std::decay_t<decltype(std::declval<Awaiter>().await_resume())>;
};

// clang-format on

}  // namespace ez::trait

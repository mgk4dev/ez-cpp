#pragma once

#include <utility>

namespace ez::async {

template <typename Impl>
struct Executor {
    static void post(Impl&, auto&& task);
};

template <typename E>
void post(E& executor, auto&& task)
{
    Executor<E>::post(executor, std::forward<decltype(task)>(task));
}

}  // namespace ez::async

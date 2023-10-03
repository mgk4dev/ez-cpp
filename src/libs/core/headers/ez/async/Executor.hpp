#pragma once

#include <type_traits>

namespace ez::async {

template <typename E>
struct Executor {
    template <typename F>
    static void post(E& executor, F&& f)
    {
        executor.post(std::forward<F>(f));
    }
};

}  // namespace ez::async

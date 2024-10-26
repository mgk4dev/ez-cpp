#pragma once

namespace ez::async {

template <typename Impl>
struct Executor {
    static void post(Impl&, auto&& task);
};

}  // namespace ez::async

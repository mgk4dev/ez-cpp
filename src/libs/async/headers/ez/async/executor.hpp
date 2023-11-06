#pragma once

#include <ez/utils.hpp>

namespace ez::async {

template <typename E>
struct Executor {
    static void post(E& executor, auto&& f) { executor.post(EZ_FWD(f)); }
};

}  // namespace ez::async

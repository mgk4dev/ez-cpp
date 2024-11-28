#pragma once

#include <stdexcept>

namespace ez::flow {
struct Panic : std::runtime_error {
    using std::runtime_error::runtime_error;
};
}  // namespace ez::flow

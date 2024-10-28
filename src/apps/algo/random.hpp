#pragma once

#include <algorithm>
#include <random>
#include <vector>

namespace ez {
template <typename T>
inline std::vector<T> generate_random_vector(size_t count,
                                             T min = T{0},
                                             T max = std::numeric_limits<T>::max())
{
    std::random_device device;
    std::mt19937 engine{device()};
    std::uniform_int_distribution<T> distribution{min, max};

    auto gen = [&]() { return distribution(engine); };

    std::vector<T> vec(count);
    std::generate(vec.begin(), vec.end(), gen);

    return vec;
}

}  // namespace ez

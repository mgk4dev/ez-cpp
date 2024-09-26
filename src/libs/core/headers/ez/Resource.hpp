#pragma once

#include <utility>

namespace ez {
template <typename T, typename Deleter>
class Resource {
public:
    Resource(T val) : m_val{std::move(val)} {}
    Resource(const Resource&) = delete;

    Resource& operator=(const Resource&) = delete;

    Resource(Resource&& other) { *this = std::move(other); }

    Resource& operator=(Resource&& other)
    {
        std::swap(m_val, other.m_val);
        return *this;
    }

    ~Resource() { Deleter{}(m_val); }

    T& get() { return m_val; }
    const T& get() const { return m_val; }

    T* operator->() { return &m_val; }
    const T* operator->() const { return &m_val; }

private:
    T m_val;
};

}  // namespace ez

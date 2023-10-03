#pragma once

#include <utility>

namespace ez {

template <typename T>
class ValueWrapper {
public:
    using ValueType = T;

    template <typename... Args>
    explicit ValueWrapper(Args&&... args) noexcept(std::is_nothrow_constructible_v<T, Args...>)
        : m_value(std::forward<Args>(args)...)
    {
    }

    ValueWrapper(const ValueWrapper<T>&) = default;
    ValueWrapper(ValueWrapper<T>&&) = default;
    ValueWrapper(ValueWrapper<T>&) = default;

    ValueWrapper& operator=(const ValueWrapper<T>&) = default;
    ValueWrapper& operator=(ValueWrapper<T>&&) = default;
    ValueWrapper& operator=(ValueWrapper<T>&) = default;

    T& value() & { return m_value; }
    const T& value() const& { return m_value; }
    T&& value() && { return std::move(m_value); }

    void swap(ValueWrapper& rhs) { std::swap(value(), rhs.value()); }

private:
    T m_value;
};

}  // namespace ez

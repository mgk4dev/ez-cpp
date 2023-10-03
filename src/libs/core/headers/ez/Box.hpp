#pragma once

#include <ez/Trait.hpp>
#include <ez/Utils.hpp>

#include <memory>

namespace ez {

template <typename T>
class Box {
public:
    Box() : m_data{std::make_unique<T>()} {}

    template <typename... Args>
    Box(Inplace, Args&&... args) : m_data{std::make_unique<T>(std::forward<Args>(args)...)}
    {
    }

    Box(trait::DerivedFrom<T> auto&& val)
        : m_data{std::make_unique<std::decay_t<decltype(val)>>(std::forward<decltype(val)>(val))}

    {
    }

    Box(const T& val) : m_data{std::make_unique<T>(val)} {}
    Box(T& val) : m_data{std::make_unique<T>(val)} {}
    Box(T&& val) : m_data{std::make_unique<T>(std::move(val))} {}

    Box(Box&) = delete;
    Box(const Box&) = delete;
    Box(Box&&) = default;

    template <trait::DerivedFrom<T> U>
    Box(Box<U>&& val) : m_data{std::move(val.m_data)}
    {
    }

    ///////////////////////////////////////////////////////////////////////////

    Box& operator=(Box&) = delete;
    Box& operator=(const Box&) = delete;
    Box& operator=(Box&&) = default;

    template <trait::DerivedFrom<T> U>
    Box& operator=(Box<U>&& rhs)
    {
        m_data = std::move(rhs.m_data);
        return *this;
    }

    ///////////////////////////////////////////////////////////////////////////

    T* operator->() { return m_data.get(); }
    const T* operator->() const { return m_data.get(); }

    T& value() & { return *m_data; }
    const T& value() const& { return *m_data; }
    T&& value() && { return std::move(*m_data); }

private:
    template <typename U>
    friend class Box;

private:
    std::unique_ptr<T> m_data;
};

template <typename T>
Box(T&&) -> Box<std::decay_t<T>>;
}  // namespace ez

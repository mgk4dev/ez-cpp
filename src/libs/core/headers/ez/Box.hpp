#pragma once

#include <ez/Traits.hpp>
#include <ez/Utils.hpp>

#include <memory>

namespace ez {

///
/// Box is a heap allocated unique value wrapper. It can hold a type T or any type deriving from T.
/// Box can be used for passing abstract types and manipulate them as move only values. Usage:
/// @code
/// struct A { virtual ~A() = default; virtual int foo() const { return 0; } };
/// struct B : public A { int foo() const { return 25; } };
/// Box<A> box{B{}};
/// ASSERT_EQ(box->foo(), 25);
/// @endcode
///
template <typename T>
class Box {
public:
    static constexpr bool has_clone_method = requires(const T val) {
        { val.clone() } -> trait::Is<Box<T>>;
    };

    static constexpr bool is_clonable =
        has_clone_method || std::is_trivially_constructible_v<T> ||
        ((!std::is_polymorphic_v<T> || std::is_final_v<T>) && std::is_copy_constructible_v<T>);

    Box() : m_data{std::make_unique<T>()} {}

    Box(Inplace, auto&&... args) : m_data{std::make_unique<T>(EZ_FWD(args)...)} {}

    Box(trait::DerivedFrom<T> auto&& val) : m_data{std::make_unique<EZ_DECAY_T(val)>(EZ_FWD(val))}

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

    Box clone() const
        requires is_clonable
    {
        if constexpr (has_clone_method) { return m_data->clone(); }
        else {
            return Box<T>(value());
        }
    }

    ///////////////////////////////////////////////////////////////////////////

    T* operator->() { return m_data.get(); }
    const T* operator->() const { return m_data.get(); }

    T& operator*() & { return *m_data; }
    const T& operator*() const& { return *m_data; }
    T&& operator*() && { return std::move(*m_data); }

    T& value() & { return *m_data; }
    const T& value() const& { return *m_data; }
    T&& value() && { return std::move(*m_data); }

    operator T&() & { return *m_data; }
    operator const T&() const& { return *m_data; }
    operator T&&() && { return std::move(*m_data); }

private:
    template <typename U>
    friend class Box;

private:
    std::unique_ptr<T> m_data;
};

template <typename T>
Box(T&&) -> Box<std::decay_t<T>>;
}  // namespace ez

#pragma once

#include <ez/Utils.hpp>

namespace ez {
template <typename Enum>
class Flags {
public:
    using UnderlyingInt = std::underlying_type_t<Enum>;

    constexpr Flags(Enum val) noexcept : m_value(ez::as_int(val)) {}

    constexpr Flags(UnderlyingInt val) noexcept : m_value(val) {}

    Flags() = default;
    Flags(const Flags&) = default;
    Flags(Flags&&) = default;
    Flags& operator=(const Flags&) = default;
    Flags& operator=(Flags&&) = default;

public:
    constexpr Flags operator|(Flags another) const noexcept
    {
        return Flags(m_value | another.m_value);
    }
    constexpr Flags operator^(Flags another) const noexcept
    {
        return Flags(m_value ^ another.m_value);
    }
    constexpr Flags operator|=(Flags another) noexcept
    {
        m_value |= another.m_value;
        return m_value;
    }
    constexpr Flags operator^=(Flags another) noexcept
    {
        m_value ^= another.m_value;
        return m_value;
    }

    template <typename T>
    friend constexpr bool operator==(const Flags<T>& a, const Flags<T>& b) noexcept;

    template <typename T>
    friend constexpr bool operator!=(const Flags<T>& a, const Flags<T>& b) noexcept;

    constexpr bool test(Enum flag) const noexcept
    {
        return (m_value & ez::as_int(flag)) == ez::as_int(flag);
    }
    template <typename... Values>
    constexpr bool all(Values... values) const noexcept
    {
        return (test(values) && ...);
    }
    template <typename... Values>
    constexpr bool any(Values... values) const noexcept
    {
        return (test(values) || ...);
    }
    constexpr explicit operator bool() const noexcept { return bool(m_value); }
    constexpr UnderlyingInt value() const noexcept { return m_value; }
    constexpr Flags& set(Enum flag, bool on = true) noexcept
    {
        if (on) { m_value |= ez::as_int(flag); }
        else {
            m_value &= ~ez::as_int(flag);
        }
        return *this;
    }
    constexpr void clear() noexcept { m_value = 0; }
    constexpr void clear(Enum flag) noexcept { m_value &= ~ez::as_int(flag); }

private:
    UnderlyingInt m_value{0};
};

template <typename T>
constexpr bool operator==(const Flags<T>& a, const Flags<T>& b) noexcept
{
    return a.m_value == b.m_value;
}

template <typename T>
constexpr bool operator!=(const Flags<T>& a, const Flags<T>& b) noexcept
{
    return !(a == b);
}

template <typename Enum>
constexpr Flags<Enum> make_flags(Enum val) noexcept
{
    return Flags<Enum>(val);
}

template <typename Enum, typename... Args>
constexpr Flags<Enum> make_flags(Enum first, Args... args) noexcept
{
    return Flags<Enum>(first) | make_flags(args...);
}

}  // namespace ez

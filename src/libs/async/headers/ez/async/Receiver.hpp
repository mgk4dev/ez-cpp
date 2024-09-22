#pragma once

#include <ez/async/Types.hpp>

#include <ez/Enum.hpp>
#include <ez/Utils.hpp>

#include <coroutine>
#include <exception>

namespace ez::async {

template <typename T>
class Receiver;

namespace detail {

template <typename Self>
struct ReturnValue {
    void return_value(auto&& val) { static_cast<Self&>(*this).set_value(EZ_FWD(val)); }
};

template <typename Self>
struct ReturnVoid {
    void return_void() { static_cast<Self&>(*this).set_value(); }
};

}  // namespace detail

// clang-format off
template <typename T>
using ReveiverBase = std::conditional_t<
    std::is_same_v<T, void>,
    detail::ReturnVoid<Receiver<T>>,
    detail::ReturnValue<Receiver<T>>
>;
// clang-format on

template <typename T>
class Receiver : public ReveiverBase<T> {
public:
    using Value = std::conditional_t<std::is_same_v<T, void>, Unit, T>;

    bool has_value() const noexcept { return m_value.template is<Value>(); }
    void unhandled_exception() { m_value.template emplace<2>(std::current_exception()); }

    void set_value(auto&&... val) { m_value.template emplace<Value>(EZ_FWD(val)...); }

    decltype(auto) get()
    {
        if (m_value.template is<std::exception_ptr>())
            std::rethrow_exception(m_value.template as<std::exception_ptr>());

        if (m_value.template is<std::monostate>()) throw ValueNotSet{};

        if constexpr (std::is_same_v<T, void>) { return; }
        else {
            return m_value.template as<T>();
        }
    }

private:
    Enum<std::monostate, Value, std::exception_ptr> m_value = std::monostate{};
};

}  // namespace ez::async

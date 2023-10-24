#pragma once

#include <ez/Enum.hpp>
#include <ez/Utils.hpp>

namespace ez::async {

template <typename T>
struct Receiver {
    using Value = std::conditional_t<std::is_same_v<T, void>, Unit, T>;
    Enum<std::monostate, Value, std::exception_ptr> value = std::monostate{};

    template <typename... U>
    void set_value(U&&... val)
    {
        value = Value{std::forward<U>(val)...};
    }

    bool has_value() const noexcept { return !value.template is<std::monostate>(); }

    decltype(auto) get() const&
    {
        if (value.template is<std::exception_ptr>())
            std::rethrow_exception(value.template as<std::exception_ptr>());

        if constexpr (std::is_same_v<T, void>) { return; }
        else {
            return value.template as<T>();
        }
    }

    decltype(auto) get() &
    {
        if (value.template is<std::exception_ptr>())
            std::rethrow_exception(value.template as<std::exception_ptr>());

        if constexpr (std::is_same_v<T, void>) { return; }
        else {
            return value.template as<T>();
        }
    }

    void unhandled_exception() { value = std::current_exception(); }
};

}  // namespace ez::async

#pragma once

#include <format>
#include <functional>

namespace ez {

enum class LogLevel : uint { Info = 1, Warning = 2, Error = 4, Trace = 8 };

struct Logger {
    std::function<void(LogLevel, const std::string&)> log_impl = [](auto&&...) {};

    template <typename... Args>
    void info(std::format_string<Args...> str, Args&&... args)
    {
        log_impl(LogLevel::Info, std::format(str, std::forward<Args>(args)...));
    }

    template <typename... Args>
    void warning(std::format_string<Args...> str, Args&&... args)
    {
        log_impl(LogLevel::Info, std::format(str, std::forward<Args>(args)...));
    }

    template <typename... Args>
    void error(std::format_string<Args...> str, Args&&... args)
    {
        log_impl(LogLevel::Info, std::format(str, std::forward<Args>(args)...));
    }

    template <typename... Args>
    void trace(std::format_string<Args...> str, Args&&... args)
    {
        log_impl(LogLevel::Info, std::format(str, std::forward<Args>(args)...));
    }
};

}  // namespace ez

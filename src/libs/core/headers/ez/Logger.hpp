#pragma once

#include <functional>

#include <format>


namespace ez {
enum class LogLevel { Info = 1, Warning = 2, Error = 4, Trace = 8 };

struct Logger {
    std::function<void(LogLevel, const std::string&)> log_impl = [](auto&&...) {};

    template <typename... Args>
    void info(std::format_string<char, Args...> str, Args&&... args)
    {
        log_impl(LogLevel::Info, std::format(str, std::forward<Args>(args)...));
    }

    template <typename... Args>
    void warning(std::format_string<Args...> str, Args&&... args)
    {
        log_impl(LogLevel::Warning, std::format(str, std::forward<Args>(args)...));
    }

    template <typename... Args>
    void error(std::format_string<Args...> str, Args&&... args)
    {
        log_impl(LogLevel::Error, std::format(str, std::forward<Args>(args)...));
    }

    template <typename... Args>
    void trace(std::format_string<Args...> str, Args&&... args)
    {
        log_impl(LogLevel::Trace, std::format(str, std::forward<Args>(args)...));
    }
};

}  // namespace ez

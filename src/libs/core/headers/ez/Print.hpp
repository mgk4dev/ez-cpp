#pragma once

#include <format>
#include <iostream>
#include <iterator>

namespace ez {

template <typename... Args>
void print(std::ostream& stream, std::format_string<Args...> base, Args&&... args)
{
    std::format_to(std::ostreambuf_iterator<char>(stream), base, std::forward<Args>(args)...);
}

template <typename... Args>
void println(std::ostream& stream, std::format_string<Args...> base, Args&&... args)
{
    std::format_to(std::ostreambuf_iterator<char>(stream), base, std::forward<Args>(args)...);
    stream << std::endl;
}

template <typename... Args>
void print(std::format_string<Args...> base, Args&&... args)
{
    print(std::cout, base, std::forward<Args>(args)...);
}

template <typename... Args>
void println(std::format_string<Args...> base, Args&&... args)
{
    println(std::cout, base, std::forward<Args>(args)...);
}

}  // namespace ez

#pragma once

#include <ez/Contract.hpp>

#include <chrono>

namespace ez {
class ElapsedTime {
public:
    using Duration = std::chrono::system_clock::duration;

    void start();
    auto startTime();
    void end();
    auto elapsed() const;

private:
    std::chrono::system_clock::time_point m_start;
    std::chrono::system_clock::time_point m_end;
};

inline void ElapsedTime::start() { m_start = std::chrono::system_clock::now(); }

inline auto ElapsedTime::startTime() { return m_start; }

inline void ElapsedTime::end() { m_end = std::chrono::system_clock::now(); }

inline auto ElapsedTime::elapsed() const
{
    EZ_ASSERT(m_end >= m_start);
    return m_end - m_start;
}
}  // namespace ez

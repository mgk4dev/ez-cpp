#pragma once

#include <ez/async/Operation.hpp>

#include <ez/Shared.hpp>

#include <QTimer>

namespace ez::async {

template <typename R = Unit>
class QDelay {
public:
    std::chrono::milliseconds duration;
    R return_value;
    QTimer timer;

    QDelay(std::chrono::milliseconds d, R ret_value = Unit{})
        : duration{d}, return_value{EZ_FWD(ret_value)}
    {
        timer.setInterval(duration);
        timer.setSingleShot(true);
    }

    constexpr bool done() { return false; }

    void start(auto continuation)
    {
        timer.callOnTimeout(std::move(continuation));
        timer.start();
    }

    auto result() noexcept
    {
        if constexpr (!std::is_same_v<R, Unit>) { return std::move(return_value); }
    }

    void cancel() { timer.stop(); }
};

template <typename R = Unit>
inline auto qdelay(std::chrono::milliseconds duration, R&& return_value = {})
{
    return Operation<QDelay<std::decay_t<R>>>{duration, EZ_FWD(return_value)};
}

}  // namespace ez::async

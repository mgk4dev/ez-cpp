#pragma once

#include <ez/async/operation.hpp>

#include <ez/option.hpp>

#include <boost/asio/steady_timer.hpp>

namespace ez::async {

using SteadyTimer = boost::asio::steady_timer;
using Duration = std::chrono::steady_clock::duration;

template <typename R>
struct Delay {
    Option<SteadyTimer> timer;
    Duration duration;
    R return_value;

    Delay(Duration d, auto&& ret_value) : duration{d}, return_value{EZ_FWD(ret_value)} {}

    void start(IoContext& context) { timer.emplace(context); }

    bool is_ready() const { return false; }

    void on_done(auto continuation)
    {
        timer->expires_after(duration);
        timer->async_wait(
            [continuation = std::move(continuation)](boost::system::error_code error) {
                if (!error) { continuation(); }
            });
    }

    void cancel() { timer->cancel(); }

    auto result()
    {
        if constexpr (!std::is_same_v<R, Unit>) { return std::move(return_value); }
    }
};

template <typename R = Unit>
inline auto delay(IoContext& context, Duration duration, R&& return_value = {})
{
    return Operation{context, Delay<std::decay_t<R>>{duration, EZ_FWD(return_value)}};
}

}  // namespace ez::async

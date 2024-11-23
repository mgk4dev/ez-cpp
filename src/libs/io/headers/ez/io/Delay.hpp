#pragma once

#include <ez/io/Context.hpp>

#include <ez/async/Operation.hpp>

#include <boost/asio/steady_timer.hpp>

namespace ez::io {

using SteadyTimer = boost::asio::steady_timer;
using Duration = std::chrono::steady_clock::duration;

template <typename R = Unit>
struct Delay {
    SteadyTimer timer;
    Duration duration;
    R return_value;
    Delay(Context& context, Duration d, R ret_value = Unit{})
        : timer{context}, duration{d}, return_value{EZ_FWD(ret_value)}
    {
    }

    bool done() const { return false; }

    void start(auto continuation)
    {
        timer.expires_after(duration);
        timer.async_wait(
            [continuation = std::move(continuation)](boost::system::error_code error) mutable {
                if (!error) { continuation(); }
            });
    }

    void cancel() { timer.cancel(); }
    auto result()
    {
        if constexpr (!std::is_same_v<R, Unit>) { return std::move(return_value); }
    }
};
template <typename R = Unit>
inline async::Operation<Delay<std::remove_cvref_t<R>>> delay(Context& context,
                                                             Duration duration,
                                                             R&& return_value = {})
{
    return {context, duration, EZ_FWD(return_value)};
}

}  // namespace ez::io

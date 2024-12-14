#include "Types.hpp"

#include "../EntityUtils.hpp"

namespace ez::flow::engine {
std::chrono::system_clock::duration DurationStorage::to_std_duration() const
{
    using namespace std::chrono;

    switch (unit) {
        case ast::DurationUnit::Ms: return milliseconds{count};
        case ast::DurationUnit::Sec: return seconds{count};
        case ast::DurationUnit::Min: return minutes{count};
        case ast::DurationUnit::Day: return days{count};
        case ast::DurationUnit::Week: return weeks{count};
    }
    std::terminate();
}

std::chrono::milliseconds DurationStorage::to_milliseconds() const
{
    using namespace std::chrono;
    milliseconds result = milliseconds{[this] {
        switch (unit) {
            case ast::DurationUnit::Ms: return count;
            case ast::DurationUnit::Sec: return count * 1000;
            case ast::DurationUnit::Min: return count * 1000 * 60;
            case ast::DurationUnit::Day: return count * 1000 * 60 * 24;
            case ast::DurationUnit::Week: return count * 1000 * 60 * 24 * 7;
        }
        std::terminate();
    }()};

    return result;
}

EZ_FLOW_TYPE_IMPL(Duration)
{
    Type result;
    result.id = result.name = "duration";
    result.construct.call = entity::make_constructor<Duration>(std::in_place, 0, ast::DurationUnit::Sec);
    result.representation.call = [](const Entity& var) -> std::string {
        const DurationStorage& duration = var.as<Duration>().value();

        return std::format("{}{}", duration.count, [&] {
            switch (duration.unit) {
                case ast::DurationUnit::Ms: return "ms";
                case ast::DurationUnit::Sec: return "sec";
                case ast::DurationUnit::Min: return "min";
                case ast::DurationUnit::Day: return "day";
                case ast::DurationUnit::Week: return "week";
            }
            std::terminate();
        }());

        return "false";
    };

    return result;
}

}  // namespace ez::flow::engine

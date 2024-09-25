#pragma once

#include <ez/Flags.hpp>
#include <ez/Option.hpp>
#include <ez/Utils.hpp>

#include <algorithm>
#include <atomic>
#include <cmath>
#include <string>

namespace ez {
enum class ExecutionStatus { Pending, Running, CancelRequest, Cancelled, Finished, Warning, Error };

enum class ExecutionLogCategory : unsigned char {
    Info,
    Warning,
    Error,
    Success,
    Debug,
};

enum class ExecutionEventType { StatusChanged, ProgressChanged, MessageAdded, SubReportAdded };

enum class ExecutionReportFlag {
    Empty = 0,
    IsTerminal = 1
};

struct ExecutionEvent {
    ExecutionEventType type;
    Option<size_t> index;  ///< Index of the message or the report in the sub item array
};

struct ExecutionLogMessage {
    ExecutionLogMessage() = default;
    ExecutionLogMessage(ExecutionLogCategory ctg, std::string header_ = {}, std::string body_ = {})
        : header{std::move(header_)}, body(std::move(body_)), category{ctg}
    {
    }

    ExecutionLogMessage& operator<<(std::string_view text)
    {
        body.append(text.begin(), text.end());
        return *this;
    }

    std::string header;
    std::string body;
    ExecutionLogCategory category = ExecutionLogCategory::Info;
    bool is_notification = false;
};

inline bool operator==(const ExecutionLogMessage& lhs, const ExecutionLogMessage& rhs)
{
    return std::tie(lhs.header, lhs.body, lhs.category, lhs.is_notification) ==
           std::tie(rhs.header, rhs.body, rhs.category, rhs.is_notification);
}

class ExecutionProgress {
public:
    using value_type = unsigned char;

    ExecutionProgress() {}

    ExecutionProgress(int value) { *this = value_type(value); }

    ExecutionProgress(double value) { *this = static_cast<value_type>(round(value)); }

    ExecutionProgress(size_t current, size_t total)
    {
        *this = static_cast<value_type>(current * 100 / total);
    }

    ExecutionProgress(const ExecutionProgress& p) : m_value{p.value()} {}

    ExecutionProgress& operator=(const ExecutionProgress& p) { return *this = p.value(); }

    ExecutionProgress& operator=(value_type value)
    {
        m_value = std::clamp<value_type>(value, 0u, 100u);
        return *this;
    }

    bool operator==(ExecutionProgress another) const { return m_value == another.m_value; }

    bool operator!=(ExecutionProgress another) const { return m_value != another.m_value; }

    bool operator<(ExecutionProgress another) const { return m_value < another.m_value; }

    operator value_type() const { return m_value; }

    value_type value() const { return m_value; }

    float normalized() const { return float(m_value) / 100; }

    bool try_update(ExecutionProgress expected, ExecutionProgress value)
    {
        value_type expectedValue = expected.m_value;
        return m_value.compare_exchange_weak(expectedValue, value.m_value);
    }

private:
    std::atomic<value_type> m_value{0};
};

using ExecutionReportFlags = Flags<ExecutionReportFlag>;
using ExecutionProgressWeight = uint32_t;

inline ExecutionProgressWeight bounded_progress_weight(std::size_t val)
{
    static const std::size_t max = std::numeric_limits<ExecutionProgressWeight>::max();
    return std::clamp<std::size_t>(val, 1, max);
}

inline bool is_final(ez::ExecutionStatus s)
{
    switch (s) {
        case ez::ExecutionStatus::Pending:
        case ez::ExecutionStatus::Running:
        case ez::ExecutionStatus::CancelRequest: return false;
        case ez::ExecutionStatus::Cancelled:
        case ez::ExecutionStatus::Finished:
        case ez::ExecutionStatus::Warning:
        case ez::ExecutionStatus::Error: return true;
    }
    return false;
}

inline std::string to_string(ez::ExecutionStatus s)
{
    switch (s) {
        case ez::ExecutionStatus::Pending: return "Pending";
        case ez::ExecutionStatus::Running: return "Running";
        case ez::ExecutionStatus::CancelRequest: return "Cancel request";
        case ez::ExecutionStatus::Cancelled: return "Cancelled";
        case ez::ExecutionStatus::Finished: return "Finished";
        case ez::ExecutionStatus::Warning: return "Warning";
        case ez::ExecutionStatus::Error: return "Error";
    }
    return "Unknown";
}

inline std::string to_string(ez::ExecutionEvent e)
{
    switch (e.type) {
        case ez::ExecutionEventType::StatusChanged: return "Status changed";
        case ez::ExecutionEventType::ProgressChanged: return "Progress changed";
        case ez::ExecutionEventType::MessageAdded: return "Message added";
        case ez::ExecutionEventType::SubReportAdded: return "Sub report added";
    }
    return "";
}

inline std::string to_string(ez::ExecutionLogCategory c)
{
    switch (c) {
        case ez::ExecutionLogCategory::Info: return "Info";
        case ez::ExecutionLogCategory::Warning: return "Warning";
        case ez::ExecutionLogCategory::Error: return "Error";
        case ez::ExecutionLogCategory::Success: return "Success";
        case ez::ExecutionLogCategory::Debug: return "Debug";
    }
    return "";
}

}  // namespace ez

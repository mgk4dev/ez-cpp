#pragma once

#include <ez/reporting/ExecutionReport.hpp>

namespace ez
{
///
/// ExecutionStatusGuard is helper acting like RAII to help setting the status
/// of an ExecutionReport.
/// In the constructor, it sets the status of the ExecutionContext to
/// ExecutionStatus::Running.
/// In the destructor, unless it is released, it sets the status of the
/// ExecutionReport to
/// ExecutionStatus::Finished except when the status is already
/// ExecutionStatus::Cancelled
/// or ExecutionStatus::Error.
///
class ExecutionStatusGuard
{
public:
    ExecutionStatusGuard() = default;
    ExecutionStatusGuard(ExecutionReport& reprort);

    ~ExecutionStatusGuard();

    static void init(ExecutionReport& report);
    static void finish(ExecutionReport& report);

    ExecutionStatusGuard(ExecutionStatusGuard&& rhs);

    ExecutionStatusGuard& operator=(ExecutionStatusGuard&& rhs);

    ExecutionStatusGuard(const ExecutionStatusGuard&) = delete;
    ExecutionStatusGuard& operator=(const ExecutionStatusGuard&) = delete;

    void release();

private:
    ExecutionReport* m_report{nullptr};
};
}  // namespace p4d

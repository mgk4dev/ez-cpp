#include <ez/reporting/ExecutionStatusGuard.hpp>

namespace ez {
ExecutionStatusGuard::ExecutionStatusGuard(ExecutionReport& report) : m_report(&report)
{
    init(report);
}

ExecutionStatusGuard::~ExecutionStatusGuard()
{
    if (!m_report) return;

    finish(*m_report);
}

void ExecutionStatusGuard::init(ExecutionReport& report)
{
    if (report.status() == ExecutionStatus::Pending) {
        report.set_start_time();
        report.set_status(ExecutionStatus::Running);
    }
}

void ExecutionStatusGuard::finish(ExecutionReport& report)
{
    report.set_end_time();
    // Cancel sub reports that were not started yet
    bool has_warning = false;
    bool has_error = false;

    report.visit_sub_reports([&](auto& sub_report) {
        switch (sub_report.status()) {
            case ExecutionStatus::Pending: {
                sub_report.set_status(ExecutionStatus::Cancelled);
                break;
            }
            case ExecutionStatus::Running: {
                if (sub_report.data_ptr()->error_logged) {
                    sub_report.set_status(ExecutionStatus::Error);
                    has_error = true;
                }
                else if (sub_report.data_ptr()->warning_logged) {
                    sub_report.set_status(ExecutionStatus::Warning);
                    has_warning = true;
                }
                break;
            }
            case ExecutionStatus::Warning: {
                has_warning = true;
                break;
            }
            case ExecutionStatus::Error: {
                has_error = true;
                break;
            }
            default: break;
        }
    });

    // Change the status only if not error or cancelled
    // If the status is CancelRequest, this means that the cancel request
    // was ignored and that the task finished the work.
    if (report.status() == ExecutionStatus::Running ||
        report.status() == ExecutionStatus::CancelRequest) {
        if (has_error || report.data_ptr()->error_logged || std::uncaught_exceptions()) {
            report.set_status(ExecutionStatus::Error);
        }
        else {
            if (has_warning || report.data_ptr()->warning_logged) {
                report.set_status(ExecutionStatus::Warning);
            }
            else {
                report.set_status(ExecutionStatus::Finished);
            }
        }
    }
}

ExecutionStatusGuard::ExecutionStatusGuard(ExecutionStatusGuard&& rhs)
{
    std::swap(m_report, rhs.m_report);
}

ExecutionStatusGuard& ExecutionStatusGuard::operator=(ExecutionStatusGuard&& rhs)
{
    std::swap(m_report, rhs.m_report);
    return *this;
}

void ExecutionStatusGuard::release() { m_report = nullptr; }

}  // namespace ez

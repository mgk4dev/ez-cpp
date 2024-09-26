#pragma once

#include <ez/Atomic.hpp>
#include <ez/NamedArgument.hpp>
#include <ez/StrongType.hpp>
#include <ez/Time.hpp>
#include <ez/Traits.hpp>
#include <ez/Tuple.hpp>
#include <ez/Utils.hpp>

#include <ez/reporting/AppendOnlyStableVector.hpp>
#include <ez/reporting/Types.hpp>

#include <algorithm>
#include <cassert>
#include <functional>
#include <map>
#include <memory>
#include <numeric>
#include <optional>
#include <type_traits>
#include <vector>

namespace ez {
///
/// ExecutionReprot is a link class between a task (asynchronous or not)
/// and an observer (that can be in a different thread).
/// It is used to report the progress of the task, the log messages and
/// potentially ask to stop the task.
/// It is possible to create a hierarchy of ExecutionReport(s) by calling
/// ExecutionReport::create_sub_report.
/// ExecutionReport is implicitly shared and can be copied safely across
/// threads.
/// The ExecutionReport api is thread safe for 1 writer and multiple readers.
/// Usage:
///   @code
///   ExecutionReport report;
///   // Task side
///   auto task = [](ExecutionReport report) {
///      report.set_status(ExecutionStatus::Running);
///
///      report.set_progress(30);
///      report.info() << "I'm at "<< 30 << "%";
///
///      report.set_progress(80);
///      report.info() << "I'm at "<< 80 << "%";
///
///      report.set_status(ExecutionStatus::Finished);
///
///   };
///   auto f = std::async(std::launch::async, task, report);
///   f.wait();
///   @endcode
///
/// ExecutionReport is a hierarchy of ExecutionReportItem.
/// ExecutionReportItem is a variant type that holds either a
/// ExecutionLogMessage or a ExecutionReport.
///   @code
///   ExecutionReport report;
///   report.info() << "hello";
///   report.create_sub_report(reporting::name = "sub report);
///
///   ExecutionReportItem message = report.subitem_at(0);
///   ASSERT_TRUE(message.is_message());
///   ASSERT_EQ(message.to_message().message, "hello");
///   ASSERT_EQ(item.parent_report(), report);
///
///   ExecutionReportItem sub_report = report.subitem_at(1);
///   ASSERT_TRUE(sub_report.is_execution_report());
///   ASSERT_EQ(item.to_report().name(), "sub report");
///   ASSERT_EQ(item.parent_report(), report);
///   @endcode
///
///

class ExecutionReport;

/// The observer should be thread save as it might be called from an execution
/// thread. The observer shall not mutate the ExecutionReport.
using ExecutionObserver = std::function<void(ExecutionReport, ExecutionEvent)>;
using ExecutionObserverId = int;
const ExecutionObserverId InvalidExecutionObserverId = -1;

namespace reporting {
using Name = StrongType<std::string, struct ReportTitleTag>;
using Description = StrongType<std::string, struct ReportDescriptionTag>;
using ProgressWeight = StrongType<ExecutionProgressWeight, struct ReportProgressWeightTag>;
using ReportFlags = StrongType<ExecutionReportFlags, struct ExecutionReportFlagsTag>;

constexpr NamedArgument<Name> name{};
constexpr NamedArgument<Description> description{};
constexpr NamedArgument<ReportFlags> flags{};
constexpr NamedArgument<ExecutionObserver> observer{};
constexpr NamedArgument<ProgressWeight> weight{};

}  // namespace reporting

struct ExecutionReportData;

class ExecutionReportItem {
public:
    ExecutionReportItem(const ExecutionReportItem&) = default;
    ExecutionReportItem(ExecutionReportItem&&) = default;
    ExecutionReportItem& operator=(const ExecutionReportItem&) = default;
    ExecutionReportItem& operator=(ExecutionReportItem&&) = default;

    /// @return true if the object is properly initialized
    explicit operator bool() const;

    /// @return true if the object holds a log message
    bool is_message() const;

    /// @return true if the object holds an execution report
    bool is_execution_report() const;

    /// Converts to object to an ExecutionLogMessage
    const ExecutionLogMessage& to_message() const;

    /// Converts to object to an ExecutionReportData
    const ExecutionReportData& to_report_data() const;

    /// @returns the parent report data.
    const ExecutionReportData* parent_report_data() const;

    /// Converts to object to an ExecutionReport
    ExecutionReport to_report() const;

    /// @returns the parent report
    ExecutionReport parent_report() const;

private:
    friend class ExecutionReport;

    const ExecutionReportData* m_parent = nullptr;
    enum class Type { Invalid, Report, Message } m_type = Type::Invalid;

    union {
        const ExecutionLogMessage* message;
        const ExecutionReportData* report_data;
    } m_payload;

    /// Null constructor. An ExecutionReportItem created with this constructor is invalid.
    ExecutionReportItem() { m_payload.message = nullptr; }
    ExecutionReportItem(const ExecutionLogMessage* message,
                        const ExecutionReportData* parent) noexcept;
    explicit ExecutionReportItem(const ExecutionReportData* report) noexcept;
};

struct ExecutionReportData : std::enable_shared_from_this<ExecutionReportData> {
    std::weak_ptr<ExecutionReportData> parent;

    internal::AppendOnlyStableVector<ExecutionReportItem> subitems;
    internal::AppendOnlyStableVector<ExecutionLogMessage> messages;
    internal::AppendOnlyStableVector<ExecutionReport> sub_reports;

    std::atomic<ExecutionObserverId> next_observer_id{0};
    Atomic<std::map<ExecutionObserverId, ExecutionObserver>> observers;

    std::string name;
    std::string description;
    size_t id = 0;

    /// These members don't need to be atomic or protected even if they are shared
    /// since they are updated in an atomic way by a single thread
    ExecutionProgress progress = 0;
    ExecutionProgressWeight progress_weight_in_parent = 1;
    ///
    std::atomic<ExecutionStatus> status{ExecutionStatus::Pending};
    ///
    ExecutionReportFlags flags{ExecutionReportFlag::Empty};

    ElapsedTime duration;

    ExecutionReport to_report() const;

    /// Flags for ExecutionStatusGuard to know what was logged.
    std::atomic_bool warning_logged{false};
    std::atomic_bool error_logged{false};
};

class ExecutionLogMessageBuilder {
public:
    ExecutionLogMessageBuilder(ExecutionReport&,
                               ExecutionLogCategory category,
                               const std::string& header = {});
    ExecutionLogMessageBuilder(ExecutionReport&,
                               ExecutionLogCategory category,
                               std::string&& header);

    ExecutionLogMessageBuilder(const ExecutionLogMessageBuilder&) = delete;
    ExecutionLogMessageBuilder(ExecutionLogMessageBuilder&&);

    ExecutionLogMessageBuilder& operator=(const ExecutionLogMessageBuilder&) = delete;
    ExecutionLogMessageBuilder& operator=(ExecutionLogMessageBuilder&&);

    ~ExecutionLogMessageBuilder();

    ExecutionLogMessageBuilder& operator<<(std::string_view) &;

private:
    ExecutionReport* m_report = nullptr;
    ExecutionLogMessage m_message;
};

class ExecutionReport {
public:
    ExecutionReport();
    ExecutionReport(const ExecutionReport&) = default;
    ExecutionReport(ExecutionReport&&) = default;
    ExecutionReport& operator=(const ExecutionReport&) = default;
    ExecutionReport& operator=(ExecutionReport&&) = default;

    template <typename... Args>
    ExecutionReport(Inplace, Args&&...);

    /// Null constructor. The created object doesn't hold any data.
    static ExecutionReport null();

    /// Creates an ExecutionReport from a std::shared_ptr<ExecutionReportData>
    static ExecutionReport from_data(std::shared_ptr<ExecutionReportData> data);

    /// @returns true if the object is not null.
    explicit operator bool() const;

    bool operator==(const ExecutionReport& another) const;
    bool operator!=(const ExecutionReport& another) const;

    const ExecutionReportData* data_ptr() const;

    /// @return true is the report is root (no parent)
    bool is_root() const;

    const ExecutionReportItem& as_item() const;

    /// @return the parent report. If the object is root, this function
    /// return a null ExecutionReport.
    ExecutionReport parent() const;

    const std::string& name() const;
    void set_name(const std::string&);
    void set_name(std::string&&);

    const std::string& description() const;
    void set_description(const std::string&);
    void set_description(std::string&&);

    size_t id() const;
    void set_id(size_t id);

    auto start_time() const;
    void set_start_time();

    void set_end_time();
    auto duration() const;
    std::string duration_str() const;

    ExecutionReportFlags flags() const;
    void set_flags(ExecutionReportFlags flags);
    bool test_flag(ExecutionReportFlag flag) const;

    //// API

    /// Add an observer to this execution report
    /// The observer should be thread-safe as it might be called from an execution
    /// thread.
    /// @return a unique observer id
    ExecutionObserverId add_observer(ExecutionObserver);

    /// @return true if and only if the report has at least one observer.
    bool has_observers() const;

    /// Remove the specified observer from the execution report
    /// @note Removing an InvalidExecutionObserverId (-1) has no effect
    void remove_observer(ExecutionObserverId);

    //// Write API: task side Thread safety condition: Max 1 writer, multiple
    /// readers
    /// Create a sub report.
    template <typename... Args>
    ExecutionReport create_sub_report(Args&&...);

    /// Add a sub report.
    void add_sub_report(ExecutionReport);

    //// Logging
    /// Add a log message
    void log(const ExecutionLogMessage& msg);
    void log(ExecutionLogMessage&& msg);

    ExecutionLogMessageBuilder info(const std::string& header = {});
    ExecutionLogMessageBuilder info(std::string&& header);

    ExecutionLogMessageBuilder warning(const std::string& header = {});
    ExecutionLogMessageBuilder warning(std::string&& header);

    ExecutionLogMessageBuilder error(const std::string& header = {});
    ExecutionLogMessageBuilder error(std::string&& header);

    ExecutionLogMessageBuilder success(const std::string& header = {});
    ExecutionLogMessageBuilder success(std::string&& header);

    ExecutionLogMessageBuilder debug(const std::string& header = {});
    ExecutionLogMessageBuilder debug(std::string&& header);

    //// Status, progress, cancel
    /// Set the execution status of the ExecutionReport.
    void set_status(ExecutionStatus s);

    /// Set the progress of ExecutionReport
    void set_progress(ExecutionProgress p);

    /// Set the progress of ExecutionReport from a current and a total progress
    void set_progress(size_t current, size_t total);

    /// True if this report or any of its sub reports is Pending, Running or CancelRequest
    bool is_busy() const;

    /// True if any sub report is busy
    bool has_busy_sub_report() const;

    //// Read API: client side. Thread safety condition: Max 1 writer, multiple
    /// readers

    size_t subitem_count() const;
    const ExecutionReportItem& subitem_at(size_t i) const;
    const internal::AppendOnlyStableVector<ExecutionReportItem>& subitems() const;

    size_t sub_report_count() const;
    ExecutionReport sub_report_at(size_t i);
    const ExecutionReport& sub_report_at(size_t i) const;

    template <typename F>
    void visit_sub_reports(F&& f);

    template <typename F>
    void visit_sub_reports(F&& f) const;

    size_t log_message_count() const;
    const ExecutionLogMessage& log_message_at(size_t i) const;

    /// @return the ExecutionStatus of the ExecutionReport
    ExecutionStatus status() const;

    /// @return the ExecutionProgress of the ExecutionReport
    ExecutionProgress progress() const;

    /// Progress weight in the parent progress.
    /// A null weight means that the report doesn't participate to the parent progress.
    ExecutionProgressWeight progress_weight() const;
    void set_progress_weight(ExecutionProgressWeight);

    /// Equivalent to set_progress_weight(0)
    void exclude_from_parent_progress();

private:
    ExecutionReport(std::nullptr_t);
    /// Notify the observer (if valid) of a new ExecutionEvent.
    void notify(ExecutionEventType type, Option<size_t> index = std::nullopt);
    void try_propagate_progress_to_parent();
    void update_progress_from_sub_reports();
    bool try_set_progress(ExecutionProgress expected, ExecutionProgress value);

private:
    std::shared_ptr<ExecutionReportData> m_data;
    ExecutionReportItem m_self;
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace internal {
template <typename... Args>
void apply_args(ExecutionReport& report, Args&&... args)
{
    ez::dispatch_args(
        Tuple{std::forward<Args>(args)...},
        [&](trait::AnyRef<reporting::Name> auto&& name) { report.set_name(EZ_FWD(name).value()); },
        [&](trait::AnyRef<reporting::Description> auto&& description) {
            report.set_description(EZ_FWD(description).value());
        },
        [&](reporting::ReportFlags flags) { report.set_flags(flags.value()); },
        [&](const ExecutionObserver& observer) { report.add_observer(observer); },
        [&](reporting::ProgressWeight w) { report.set_progress_weight(w.value()); });
}
}  // namespace internal

template <typename... Args>
ExecutionReport::ExecutionReport(Inplace, Args&&... args)
    : m_data(std::make_shared<ExecutionReportData>()), m_self(m_data.get())
{
    m_data->name = "Root";
    internal::apply_args(*this, std::forward<Args>(args)...);
}

template <typename... Args>
ExecutionReport ExecutionReport::create_sub_report(Args&&... args)
{
    ExecutionReport ctx{in_place, std::forward<Args>(args)...};
    add_sub_report(ctx);
    return ctx;
}

template <typename F>
void ExecutionReport::visit_sub_reports(F&& f) const
{
    m_data->sub_reports.visit(std::forward<F>(f));
}

template <typename F>
void ExecutionReport::visit_sub_reports(F&& f)
{
    m_data->sub_reports.visit(std::forward<F>(f));
}

inline auto ExecutionReport::duration() const { return m_data->duration.elapsed(); }

inline auto ExecutionReport::start_time() const { return m_data->duration.startTime(); }

}  // namespace ez

namespace std {
template <>
struct hash<ez::ExecutionReport> {
    std::size_t operator()(const ez::ExecutionReport& report) const noexcept
    {
        return hash<const void*>{}(report.data_ptr());
    }
};
}  // namespace std

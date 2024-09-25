#include <ez/reporting/ExecutionReport.hpp>

#include <ez/Contract.hpp>

namespace ez {
ExecutionReportItem::ExecutionReportItem(const ExecutionLogMessage* message,
                                         const ExecutionReportData* parent) noexcept
    : m_parent(parent), m_type(Type::Message)
{
    m_payload.message = message;
}

ExecutionReportItem::ExecutionReportItem(const ExecutionReportData* report) noexcept
    : m_parent(report->parent.lock().get()), m_type(Type::Report)
{
    m_payload.report_data = report;
}

ExecutionReportItem::operator bool() const
{
    return m_type == Type::Message || m_type == Type::Report;
}

bool ExecutionReportItem::is_message() const { return m_type == Type::Message; }

bool ExecutionReportItem::is_execution_report() const { return m_type == Type::Report; }

const ExecutionLogMessage& ExecutionReportItem::to_message() const
{
    EZ_ASSERT(is_message() && "Item is not a message");
    return *m_payload.message;
}

const ExecutionReportData& ExecutionReportItem::to_report_data() const
{
    EZ_ASSERT(is_execution_report() && "Item is not an execution report");
    return *m_payload.report_data;
}

const ExecutionReportData* ExecutionReportItem::parent_report_data() const { return m_parent; }

ExecutionReport ExecutionReportItem::to_report() const
{
    EZ_ASSERT(is_execution_report() && "Item is not an execution report");
    return m_payload.report_data->to_report();
}

ExecutionReport ExecutionReportItem::parent_report() const
{
    if (m_parent) return m_parent->to_report();
    return ExecutionReport::null();
}

ExecutionReport ExecutionReportData::to_report() const
{
    auto ptr = shared_from_this();
    return ExecutionReport::from_data(std::const_pointer_cast<ExecutionReportData>(ptr));
}

///////////////////////////////////////////////////////////////////////////////

ExecutionLogMessageBuilder::ExecutionLogMessageBuilder(ExecutionReport& report,
                                                       ExecutionLogCategory category,
                                                       const std::string& header)
    : m_report(&report), m_message(category, header)
{
}

ExecutionLogMessageBuilder::ExecutionLogMessageBuilder(ExecutionReport& report,
                                                       ExecutionLogCategory category,
                                                       std::string&& header)
    : m_report(&report), m_message(category, std::move(header))
{
}

ExecutionLogMessageBuilder::ExecutionLogMessageBuilder(ExecutionLogMessageBuilder&& rhs)
{
    *this = std::move(rhs);
}

ExecutionLogMessageBuilder& ExecutionLogMessageBuilder::operator=(ExecutionLogMessageBuilder&& rhs)
{
    m_report = rhs.m_report;
    m_message = std::move(rhs.m_message);
    rhs.m_report = nullptr;
    return *this;
}

ExecutionLogMessageBuilder::~ExecutionLogMessageBuilder()
{
    if (!m_report) return;

    try {
        m_report->log(m_message);
    }
    catch (...) {
    }
}

ExecutionLogMessageBuilder& ExecutionLogMessageBuilder::operator<<(std::string_view fragment) &
{
    m_message << fragment;
    return *this;
}

///////////////////////////////////////////////////////////////////////////////

ExecutionReport ExecutionReport::null() { return ExecutionReport{nullptr}; }

ExecutionReport::ExecutionReport(std::nullptr_t) {}

ExecutionReport::ExecutionReport() : ez::ExecutionReport(in_place) {}

ExecutionReport ExecutionReport::from_data(std::shared_ptr<ExecutionReportData> data)
{
    ExecutionReport report{nullptr};
    report.m_data = std::move(data);
    report.m_self = ExecutionReportItem{report.m_data.get()};
    return report;
}

const ExecutionReportData* ExecutionReport::data_ptr() const { return m_data.get(); }

bool ExecutionReport::is_root() const { return m_data->parent.expired(); }

const ExecutionReportItem& ExecutionReport::as_item() const { return m_self; }

//// API
ExecutionReport::operator bool() const { return m_data.get() != nullptr; }

bool ExecutionReport::operator==(const ExecutionReport& another) const
{
    return data_ptr() == another.data_ptr();
}

bool ExecutionReport::operator!=(const ExecutionReport& another) const
{
    return data_ptr() != another.data_ptr();
}

const std::string& ExecutionReport::name() const { return m_data->name; }

void ExecutionReport::set_name(const std::string& name) { m_data->name = name; }

void ExecutionReport::set_name(std::string&& name) { m_data->name = std::move(name); }

const std::string& ExecutionReport::description() const { return m_data->description; }

void ExecutionReport::set_description(const std::string& description)
{
    m_data->description = description;
}

void ExecutionReport::set_description(std::string&& description)
{
    m_data->description = std::move(description);
}

size_t ExecutionReport::id() const { return m_data->id; }

void ExecutionReport::set_id(size_t id) { m_data->id = id; }

void ExecutionReport::set_start_time() { m_data->duration.start(); }

void ExecutionReport::set_end_time() { m_data->duration.end(); }

std::string ExecutionReport::duration_str() const { return std::format("{}", duration()); }

ExecutionReportFlags ExecutionReport::flags() const { return m_data->flags; }

void ExecutionReport::set_flags(ExecutionReportFlags flags) { m_data->flags = flags; }

bool ExecutionReport::test_flag(ExecutionReportFlag flag) const { return m_data->flags.test(flag); }

ExecutionObserverId ExecutionReport::add_observer(ExecutionObserver obs)
{
    auto id = m_data->next_observer_id++;
    m_data->observers->insert({id, std::move(obs)});
    return id;
}

bool ExecutionReport::has_observers() const { return !m_data->observers->empty(); }

void ExecutionReport::remove_observer(ExecutionObserverId id) { m_data->observers->erase(id); }

void ExecutionReport::add_sub_report(ExecutionReport ctx)
{
    ctx.m_data->parent = m_data;
    m_data->sub_reports.append(ctx);

    ExecutionReportItem item(ctx.m_data.get());  // Do not move before ctx.m_data->parent = m_data;
    m_data->subitems.append(item);
    notify(ExecutionEventType::SubReportAdded, m_data->subitems.size() - 1);
}

void ExecutionReport::log(const ExecutionLogMessage& msg) { log(ExecutionLogMessage{msg}); }

void ExecutionReport::log(ExecutionLogMessage&& msg)
{
    auto category = msg.category;

    switch (category) {
        case ExecutionLogCategory::Warning: m_data->warning_logged = true; break;
        case ExecutionLogCategory::Error: m_data->error_logged = true; break;
        default: break;
    }

    m_data->messages.append(std::move(msg));
    ExecutionReportItem item(&m_data->messages.last(), m_data.get());
    m_data->subitems.append(item);
    notify(ExecutionEventType::MessageAdded, m_data->subitems.size() - 1);
}

ExecutionLogMessageBuilder ExecutionReport::info(const std::string& header)
{
    return ExecutionLogMessageBuilder{*this, ExecutionLogCategory::Info, header};
}

ExecutionLogMessageBuilder ExecutionReport::info(std::string&& header)
{
    return ExecutionLogMessageBuilder{*this, ExecutionLogCategory::Info, std::move(header)};
}

ExecutionLogMessageBuilder ExecutionReport::warning(const std::string& header)
{
    return ExecutionLogMessageBuilder{*this, ExecutionLogCategory::Warning, header};
}

ExecutionLogMessageBuilder ExecutionReport::warning(std::string&& header)
{
    return ExecutionLogMessageBuilder{*this, ExecutionLogCategory::Warning, std::move(header)};
}

ExecutionLogMessageBuilder ExecutionReport::error(const std::string& header)
{
    return ExecutionLogMessageBuilder{*this, ExecutionLogCategory::Error, header};
}

ExecutionLogMessageBuilder ExecutionReport::error(std::string&& header)
{
    return ExecutionLogMessageBuilder{*this, ExecutionLogCategory::Error, std::move(header)};
}

ExecutionLogMessageBuilder ExecutionReport::success(const std::string& header)
{
    return ExecutionLogMessageBuilder{*this, ExecutionLogCategory::Success, header};
}

ExecutionLogMessageBuilder ExecutionReport::success(std::string&& header)
{
    return ExecutionLogMessageBuilder{*this, ExecutionLogCategory::Success, std::move(header)};
}

ExecutionLogMessageBuilder ExecutionReport::debug(const std::string& header)
{
    return ExecutionLogMessageBuilder{*this, ExecutionLogCategory::Debug, header};
}

ExecutionLogMessageBuilder ExecutionReport::debug(std::string&& header)
{
    return ExecutionLogMessageBuilder{*this, ExecutionLogCategory::Debug, std::move(header)};
}

void ExecutionReport::set_status(ExecutionStatus status)
{
    if (m_data->status != status) {
        m_data->status = status;
        if (status == ExecutionStatus::Error)
            m_data->error_logged = true;
        else if (status == ExecutionStatus::Warning)
            m_data->warning_logged = true;

        notify(ExecutionEventType::StatusChanged);
        if (m_data->status == ExecutionStatus::Finished) set_progress(100);

        // Ensure sub reports are also canceled when canceling parent
        if (status == ExecutionStatus::Cancelled) {
            m_data->sub_reports.visit([](auto& report) {
                if (!is_final(report.status())) { report.set_status(ExecutionStatus::Cancelled); }
            });
        }
    }
}

void ExecutionReport::set_progress(ExecutionProgress p)
{
    if (m_data->progress != p) {
        m_data->progress = p;
        notify(ExecutionEventType::ProgressChanged);
        try_propagate_progress_to_parent();
    }
}

bool ExecutionReport::try_set_progress(ExecutionProgress oldValue, ExecutionProgress newValue)
{
    if (m_data->progress.try_update(oldValue, newValue)) {
        notify(ExecutionEventType::ProgressChanged);
        try_propagate_progress_to_parent();
        return true;
    }
    return false;
}

void ExecutionReport::try_propagate_progress_to_parent()
{
    if (m_data->progress_weight_in_parent) {
        if (auto sharedParent = m_data->parent.lock()) {
            sharedParent->to_report().update_progress_from_sub_reports();
        }
    }
}

void ExecutionReport::update_progress_from_sub_reports()
{
    auto progress_from_sub_reports = [this]() -> std::optional<ExecutionProgress> {
        uint64_t totalWeights = 0;
        double totalProgress = 0.;

        m_data->sub_reports.visit([&](const ExecutionReport& report) {
            if (auto w = report.progress_weight())  // if active
            {
                totalProgress += report.progress().value() * uint64_t(w);
                totalWeights += w;
            }
        });

        if (totalWeights) return ExecutionProgress{totalProgress / totalWeights};
        return std::nullopt;
    };

    while (true) {
        ExecutionProgress old = progress();
        std::optional<ExecutionProgress> new_ = progress_from_sub_reports();
        if (!new_ || try_set_progress(old, *new_)) break;
    }
}

void ExecutionReport::set_progress(size_t current, size_t total)
{
    if (total == 0) { return; }

    set_progress(int((current * 100) / total));
}

bool ExecutionReport::is_busy() const { return !is_final(status()) || has_busy_sub_report(); }

bool ExecutionReport::has_busy_sub_report() const
{
    for (size_t i = 0; i < sub_report_count(); ++i)
        if (sub_report_at(i).is_busy()) return true;

    return false;
}

void ExecutionReport::set_progress_weight(ExecutionProgressWeight w)
{
    m_data->progress_weight_in_parent = w;
}

ExecutionProgressWeight ExecutionReport::progress_weight() const
{
    return m_data->progress_weight_in_parent;
}

void ExecutionReport::exclude_from_parent_progress() { set_progress_weight(0); }

//// Read API: client side. Thread safety condition: Max 1 writer, multiple
/// readers
ExecutionReport ExecutionReport::parent() const
{
    if (auto sharedParent = m_data->parent.lock()) {
        return ExecutionReport::from_data(sharedParent);
    }
    return ExecutionReport::null();
}

size_t ExecutionReport::subitem_count() const { return m_data->subitems.size(); }

const ExecutionReportItem& ExecutionReport::subitem_at(size_t i) const
{
    return m_data->subitems.at(i);
}

const internal::AppendOnlyStableVector<ExecutionReportItem>& ExecutionReport::subitems() const
{
    return m_data->subitems;
}

size_t ExecutionReport::sub_report_count() const { return m_data->sub_reports.size(); }

ExecutionReport ExecutionReport::sub_report_at(size_t i) { return m_data->sub_reports.at(i); }

const ExecutionReport& ExecutionReport::sub_report_at(size_t i) const
{
    return m_data->sub_reports.at(i);
}

size_t ExecutionReport::log_message_count() const { return m_data->messages.size(); }

const ExecutionLogMessage& ExecutionReport::log_message_at(size_t i) const
{
    return m_data->messages.at(i);
}

ExecutionStatus ExecutionReport::status() const { return m_data->status; }

ExecutionProgress ExecutionReport::progress() const { return m_data->progress; }

void ExecutionReport::notify(ExecutionEventType type, Option<size_t> index)
{
    ExecutionEvent event{type, index};
    m_data->observers.edit([this, event](auto& v) {
        for (const auto& observer : v) observer.second(*this, event);
    });

    auto shared_parent = m_data->parent.lock();

    while (shared_parent) {
        shared_parent->observers.edit([this, event](auto& v) {
            for (const auto& observer : v) observer.second(*this, event);
        });

        shared_parent = shared_parent->parent.lock();
    }
}

}  // namespace ez

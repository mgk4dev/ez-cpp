#include <gtest/gtest.h>

#include <ez/ExecutionReport.hpp>

#include <future>
#include <queue>

using namespace ez;

TEST(ExecutionReport, status_properly_set)
{
    auto task = [](ExecutionReport report) { report.set_status(ExecutionStatus::Finished); };

    ExecutionReport report;

    auto f = std::async(std::launch::async, task, report);

    f.wait();

    ASSERT_EQ(report.status(), ExecutionStatus::Finished);
}

TEST(ExecutionReport, args)
{
    ExecutionReport report{std::in_place,
                           reporting::name = "name",
                           reporting::description = "description",
                           reporting::flags = make_flags(ExecutionReportFlag::IsTerminal),
                           reporting::observer = [](auto&&, auto&&) {},
                           reporting::weight = 22u};

    ASSERT_EQ(report.name(), "name");
    ASSERT_EQ(report.description(), "description");

    ASSERT_TRUE(report.test_flag(ExecutionReportFlag::IsTerminal));
    ASSERT_TRUE(report.has_observers());
    ASSERT_EQ(report.progress_weight(), 22u);

    auto sub_report = report.create_sub_report(
        reporting::name = "name", reporting::description = "description",
        reporting::flags = make_flags(ExecutionReportFlag::IsTerminal),
        reporting::observer = [](auto&&, auto&&) {}, reporting::weight = 22u);

    ASSERT_EQ(sub_report.name(), "name");
    ASSERT_EQ(sub_report.description(), "description");
    ASSERT_TRUE(sub_report.test_flag(ExecutionReportFlag::IsTerminal));
    ASSERT_TRUE(sub_report.has_observers());
    ASSERT_EQ(sub_report.progress_weight(), 22u);
}

TEST(ExecutionReport, execution_guard_normal_execution)
{
    ExecutionReport report;
    ASSERT_EQ(report.status(), ExecutionStatus::Pending);

    {
        ExecutionStatusGuard guard{report};
        ASSERT_EQ(report.status(), ExecutionStatus::Running);
    }
    ASSERT_EQ(report.status(), ExecutionStatus::Finished);
}

TEST(ExecutionReport, execution_guard_error)
{
    ExecutionReport report;
    ASSERT_EQ(report.status(), ExecutionStatus::Pending);

    {
        ExecutionStatusGuard guard{report};
        ASSERT_EQ(report.status(), ExecutionStatus::Running);
        report.set_status(ExecutionStatus::Error);
    }
    ASSERT_EQ(report.status(), ExecutionStatus::Error);
}

TEST(ExecutionReport, execution_guard_cancelled)
{
    ExecutionReport report;
    ASSERT_EQ(report.status(), ExecutionStatus::Pending);

    {
        ExecutionStatusGuard guard{report};
        ASSERT_EQ(report.status(), ExecutionStatus::Running);
        report.set_status(ExecutionStatus::Cancelled);
    }
    ASSERT_EQ(report.status(), ExecutionStatus::Cancelled);
}

TEST(ExecutionReport, execution_guard_sets_error_status_on_exception)
{
    ExecutionReport report;
    try {
        ExecutionStatusGuard guard{report};
        throw std::runtime_error("error");
    }
    catch (...) {
    }
    ASSERT_EQ(report.status(), ExecutionStatus::Error);
}

TEST(ExecutionReport, execution_guard_sets_error_status_on_error_log)
{
    ExecutionReport report;
    {
        ExecutionStatusGuard guard{report};
        report.error("Some error");
    }
    ASSERT_EQ(report.status(), ExecutionStatus::Error);
}

TEST(ExecutionReport, execution_guard_sets_warning_status_on_warning_log)
{
    ExecutionReport report;
    {
        ExecutionStatusGuard guard{report};
        report.warning("Some warning");
    }
    ASSERT_EQ(report.status(), ExecutionStatus::Warning);
}

TEST(ExecutionReport, execution_guard_priorizes_error_status_over_warning_status)
{
    ExecutionReport report;
    {
        ExecutionStatusGuard guard{report};
        report.warning("Some warning");
        report.error("Some error");
    }
    ASSERT_EQ(report.status(), ExecutionStatus::Error);

    // Order of logs shouldn't affect the outcome.
    report = {};
    ASSERT_EQ(report.status(), ExecutionStatus::Pending);
    {
        ExecutionStatusGuard guard{report};
        report.error("Some error");
        report.warning("Some warning");
    }
    ASSERT_EQ(report.status(), ExecutionStatus::Error);
}

TEST(ExecutionReport, execution_guard_set_error_warning_on_running_sub_report)
{
    // Test case for MATIC-3043
    ExecutionReport root;
    auto report1 = root.create_sub_report(reporting::name = "Report 1");
    auto report11 = report1.create_sub_report(reporting::name = "Report 1 1");
    auto report111 = report11.create_sub_report(reporting::name = "Report 1 1 1");
    auto report12 = report1.create_sub_report(reporting::name = "Report 1 2");

    ASSERT_EQ(report1.status(), ExecutionStatus::Pending);
    ASSERT_EQ(report11.status(), ExecutionStatus::Pending);
    ASSERT_EQ(report111.status(), ExecutionStatus::Pending);
    ASSERT_EQ(report12.status(), ExecutionStatus::Pending);

    {
        ExecutionStatusGuard calibrateSceneGuard{report1};
        ExecutionStatusGuard calibrateCamerasGuard{report11};
        ExecutionStatusGuard::init(report111);
        report111.error();
    }
    ASSERT_EQ(report1.status(), ExecutionStatus::Error);
    ASSERT_EQ(report11.status(), ExecutionStatus::Error);
    ASSERT_EQ(report111.status(), ExecutionStatus::Error);
    ASSERT_EQ(report12.status(), ExecutionStatus::Cancelled);
}

TEST(ExecutionReport, execution_guard_cancels_unstarted_sub_reports)
{
    ExecutionReport report;
    auto sub_report1 = report.create_sub_report(reporting::name = "sub_report1");
    auto sub_report2 = report.create_sub_report(reporting::name = "sub_report2");
    ASSERT_EQ(sub_report1.status(), ExecutionStatus::Pending);
    ASSERT_EQ(sub_report2.status(), ExecutionStatus::Pending);

    {
        ExecutionStatusGuard guard{report};
        ExecutionStatusGuard sub_reportGuard{sub_report1};
        sub_report1.set_status(ExecutionStatus::Error);
    }
    ASSERT_EQ(sub_report1.status(), ExecutionStatus::Error);
    ASSERT_EQ(sub_report2.status(), ExecutionStatus::Cancelled);
}

TEST(ExecutionReport, error_message_does_not_change_the_status)
{
    ExecutionReport report;
    ASSERT_EQ(report.status(), ExecutionStatus::Pending);
    report.error("An error");
    ASSERT_NE(report.status(), ExecutionStatus::Error);
}

TEST(ExecutionReport, log_messages_are_properly_saved)
{
    auto task = [](ExecutionReport report) {
        report.info("msg");
        report.warning("msg");
        report.error("msg");
        report.debug("msg");
    };

    ExecutionReport report;

    auto f = std::async(std::launch::async, task, report);
    f.wait();

    ASSERT_EQ(report.subitem_count(), 4);

    for (size_t i = 0; i < report.subitem_count(); ++i) {
        ExecutionReportItem item = report.subitem_at(i);
        ASSERT_TRUE(item);
        ASSERT_TRUE(item.is_message());
        ASSERT_EQ(item.to_message().header, "msg");
        ASSERT_EQ(item.parent_report(), report);
    }
}

TEST(ExecutionReport, sub_reports_are_properly_created)
{
    auto task = [](ExecutionReport report) {
        report.create_sub_report(reporting::name = "SR1");
        report.create_sub_report(reporting::name = "SR2");
    };

    ExecutionReport report;

    auto f = std::async(std::launch::async, task, report);
    f.wait();

    ASSERT_EQ(report.subitem_count(), 2);

    for (size_t i = 0; i < report.subitem_count(); ++i) {
        ExecutionReportItem item = report.subitem_at(i);
        ASSERT_TRUE(item);
        ASSERT_TRUE(item.is_execution_report());
        ASSERT_EQ(item.to_report().name(), std::format("SR{}", i + 1));
        ASSERT_EQ(item.parent_report(), report);
    }
}

TEST(ExecutionReport, can_create_hierarchy_of_reports_and_log_messages)
{
    auto log_task = [](ExecutionReport report) {
        report.info("msg");
        report.warning("msg");
        report.error("msg");
        report.debug("msg");
    };

    auto task = [log_task](ExecutionReport report) {
        auto f1 = std::async(std::launch::async, log_task,
                             report.create_sub_report(reporting::name = "SR1"));
        auto f2 = std::async(std::launch::async, log_task,
                             report.create_sub_report(reporting::name = "SR2"));

        f1.wait();
        f2.wait();
    };

    ExecutionReport report;

    auto f = std::async(std::launch::async, task, report);
    f.wait();

    ASSERT_EQ(report.subitem_count(), 2);

    for (size_t i = 0; i < report.subitem_count(); ++i) {
        ExecutionReportItem item = report.subitem_at(i);

        ASSERT_TRUE(item);
        ASSERT_TRUE(item.is_execution_report());
        ASSERT_EQ(item.parent_report(), report);

        ASSERT_EQ(item.to_report().subitem_count(), 4);
        ASSERT_EQ(item.to_report().log_message_count(), 4);
    }
}

TEST(ExecutionReport, parent_report_is_properly_set)
{
    ExecutionReport report;
    auto sub_report = report.create_sub_report(reporting::name = "SR");
    ASSERT_EQ(sub_report.parent(), report);
}

TEST(ExecutionReport, order_of_messages_and_sub_reports_preserved)
{
    ExecutionReport report;
    report.info("toto");
    report.create_sub_report(reporting::name = "SR1");
    report.info("titi");
    report.create_sub_report(reporting::name = "SR2");

    ASSERT_EQ(report.subitem_count(), 4);
    ASSERT_TRUE(report.subitem_at(0).is_message());
    ASSERT_TRUE(report.subitem_at(1).is_execution_report());
    ASSERT_TRUE(report.subitem_at(2).is_message());
    ASSERT_TRUE(report.subitem_at(3).is_execution_report());
}

TEST(ExecutionReport, events_are_properly_observed)
{
    uint32_t status_event_count = 0;
    uint32_t hierarchy_event_count = 0;
    uint32_t progress_event_count = 0;

    auto observer = [&](const ExecutionReport& /*report*/, ExecutionEvent event) {
        switch (event.type) {
            case ExecutionEventType::StatusChanged: ++status_event_count; break;
            case ExecutionEventType::ProgressChanged: ++progress_event_count; break;
            case ExecutionEventType::MessageAdded:
            case ExecutionEventType::SubReportAdded: ++hierarchy_event_count; break;
        }
    };

    ExecutionReport report{std::in_place, reporting::observer = observer};
    report.set_status(ExecutionStatus::Running);
    report.set_progress(0);
    report.info("toto");
    report.set_progress(50);
    report.create_sub_report(reporting::name = "SR1");
    report.set_progress(100);
    report.set_status(ExecutionStatus::Finished);

    ASSERT_EQ(status_event_count, 2);
    ASSERT_EQ(hierarchy_event_count, 2);
    ASSERT_EQ(progress_event_count, 2);
}

TEST(ExecutionReport, child_execution_report_can_notify_without_parent)
{
    ExecutionReport sub_report;

    {
        ExecutionReport root_report;
        sub_report = root_report.create_sub_report(reporting::name = "sub");
    }

    ASSERT_NO_THROW(sub_report.set_progress(50));
}

TEST(ExecutionReport, add_remove_observer)
{
    ExecutionReport report;

    int event_count1 = 0, event_count2 = 0, event_count3 = 0;

    auto observer1 = [&](ExecutionReport, ExecutionEvent event) {
        if (event.type == ExecutionEventType::StatusChanged) ++event_count1;
    };

    auto observer2 = [&](ExecutionReport, ExecutionEvent event) {
        if (event.type == ExecutionEventType::ProgressChanged) ++event_count2;
    };
    auto observer3 = [&](ExecutionReport, ExecutionEvent event) {
        if (event.type == ExecutionEventType::MessageAdded) ++event_count3;
    };

    ASSERT_FALSE(report.has_observers());

    auto id1 = report.add_observer(observer1);
    auto id2 = report.add_observer(observer2);
    auto id3 = report.add_observer(observer3);
    ASSERT_TRUE(report.has_observers());

    report.set_status(ExecutionStatus::Running);
    report.set_progress(25);
    report.info("blabla1");

    ASSERT_EQ(1, event_count1);
    ASSERT_EQ(1, event_count2);
    ASSERT_EQ(1, event_count3);

    report.remove_observer(id1);
    report.remove_observer(id2);
    report.remove_observer(id3);

    ASSERT_FALSE(report.has_observers());

    report.set_status(ExecutionStatus::Running);
    report.set_progress(75);
    report.info("blabla2");

    ASSERT_EQ(1, event_count1);
    ASSERT_EQ(1, event_count2);
    ASSERT_EQ(1, event_count3);
}

TEST(ExecutionReport, add_remove_observers_and_log_messages)
{
    ExecutionReport report;

    Atomic<std::queue<ExecutionObserverId>> ids;
    const int iteration_count = 1000;

    auto add_observers_task = [&]() {
        for (int i = 0; i < iteration_count; ++i) {
            auto id = report.add_observer([](ExecutionReport, ExecutionEvent) {});
            ids->push(id);
        }
    };

    auto log_task = [&]() {
        for (int i = 0; i < iteration_count; ++i) report.info(std::to_string(i));
    };

    auto remove_observers_task = [&]() {
        int i = 0;
        while (i < iteration_count) {
            ids.edit([&](auto& v) {
                if (!v.empty()) {
                    report.remove_observer(v.front());
                    v.pop();
                    ++i;
                }
            });
        }
    };

    auto f1 = std::async(std::launch::async, add_observers_task);
    auto f2 = std::async(std::launch::async, log_task);
    auto f3 = std::async(std::launch::async, remove_observers_task);

    f1.wait();
    f2.wait();
    f3.wait();

    ASSERT_TRUE(ids->empty());
}

TEST(ExecutionReport, reader_and_writer_example)
{
    const size_t iteration_count = 100;

    ExecutionReport root_report;

    auto read_task = [](ExecutionReport report) {
        std::string str;
        const size_t count = report.subitem_count();

        for (size_t i = 0; i < count; ++i) {
            auto& item = report.subitem_at(i);

            str.append(item.is_message() ? item.to_message().header : item.to_report().name())
                .append("\n");
        }
        return str;
    };

    std::vector<std::future<std::string>> futures;

    auto observer = [&futures, read_task](ExecutionReport report, ExecutionEvent /*event*/) {
        futures.push_back(std::async(read_task, report));
    };

    root_report.add_observer(observer);

    for (size_t i = 0; i < iteration_count; ++i) {
        root_report.debug(std::format("message {}", i));
    }

    ASSERT_EQ(futures.size(), iteration_count);

    for (auto& f : futures) { f.wait(); }
}

TEST(ExecutionReport, progress_is_properly_propagated)
{
    ExecutionReport root_report;
    ExecutionReport sub_report1 =
        root_report.create_sub_report(reporting::name = "SR1", reporting::weight = 1u);
    ExecutionReport sub_report2 =
        root_report.create_sub_report(reporting::name = "SR2", reporting::weight = 2u);

    ASSERT_EQ(root_report.progress().value(), 0);

    sub_report1.set_progress(50);
    ASSERT_EQ(root_report.progress().value(), round(0.34 * 50));

    sub_report2.set_progress(50);
    ASSERT_EQ(root_report.progress().value(), round(0.34 * 50 + 0.66 * 50));
    ASSERT_EQ(root_report.progress().value(), 50);

    sub_report1.set_progress(100);
    ASSERT_EQ(root_report.progress().value(), round(0.34 * 100 + 0.66 * 50));

    sub_report2.set_progress(100);
    ASSERT_EQ(root_report.progress().value(), 100);
}

TEST(ExecutionReport, progress_is_properly_propagated_from_sub_sub_reports)
{
    ExecutionReport root_report;
    ExecutionReport sub_report1 = root_report.create_sub_report(reporting::name = "SR1");
    ExecutionReport sub_report2 = root_report.create_sub_report(reporting::name = "SR2");

    ExecutionReport subsub_report1 = sub_report1.create_sub_report(reporting::name = "ssr1");
    ExecutionReport subsub_report2 = sub_report1.create_sub_report(reporting::name = "ssr2");
    ExecutionReport subsub_report3 = sub_report2.create_sub_report(reporting::name = "ssr3");
    ExecutionReport subsub_report4 = sub_report2.create_sub_report(reporting::name = "ssr4");

    ASSERT_EQ(root_report.progress().value(), 0);

    subsub_report1.set_progress(50);
    ASSERT_EQ(root_report.progress().value(), round(50. / 4));

    subsub_report2.set_progress(50);
    ASSERT_EQ(root_report.progress().value(), round(100. / 4));

    subsub_report3.set_progress(50);
    ASSERT_EQ(root_report.progress().value(), round(150. / 4));

    subsub_report4.set_progress(50);
    ASSERT_EQ(root_report.progress().value(), 50);

    subsub_report1.set_progress(100);
    ASSERT_EQ(root_report.progress().value(), round(250. / 4));

    subsub_report2.set_progress(100);
    ASSERT_EQ(root_report.progress().value(), round(300. / 4));

    subsub_report3.set_progress(100);
    ASSERT_EQ(root_report.progress().value(), round(350. / 4));

    subsub_report4.set_progress(100);
    ASSERT_EQ(root_report.progress().value(), 100);
}

TEST(ExecutionReport, progress_is_not_propagated_when_disabled)
{
    ExecutionReport root_report;
    ExecutionReport sub_report1 =
        root_report.create_sub_report(reporting::name = "SR1", reporting::weight = 1u);
    ExecutionReport sub_report2 =
        root_report.create_sub_report(reporting::name = "SR2", reporting::weight = 2u);
    ExecutionReport sub_report3 =
        root_report.create_sub_report(reporting::name = "SR3", reporting::weight = 0u);

    ASSERT_EQ(root_report.progress().value(), 0);

    sub_report1.set_progress(50);
    ASSERT_EQ(root_report.progress().value(), round(0.34 * 50));

    sub_report2.set_progress(50);
    ASSERT_EQ(root_report.progress().value(), round(0.34 * 50 + 0.66 * 50));
}

TEST(ExecutionReport, nested_report_status_is_propagated_properly)
{
    {
        ExecutionReport root_report;

        ExecutionReport sub_report1 = root_report.create_sub_report(reporting::name = "SR1");
        ExecutionReport sub_report2 = root_report.create_sub_report(reporting::name = "SR2");

        {
            ExecutionStatusGuard guard{root_report};
            sub_report1.set_status(ExecutionStatus::Warning);
        }
        ASSERT_EQ(root_report.status(), ExecutionStatus::Warning);
    }

    {
        ExecutionReport root_report;

        ExecutionReport sub_report1 = root_report.create_sub_report(reporting::name = "SR1");
        ExecutionReport sub_report2 = root_report.create_sub_report(reporting::name = "SR2");

        {
            ExecutionStatusGuard guard{root_report};
            sub_report1.set_status(ExecutionStatus::Error);
        }
        ASSERT_EQ(root_report.status(), ExecutionStatus::Error);
    }
}

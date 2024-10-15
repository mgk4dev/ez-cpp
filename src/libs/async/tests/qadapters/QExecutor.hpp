#pragma once

#include <ez/async/Executor.hpp>

#include <QCoreApplication>
#include <QThreadPool>

namespace ez::async {

inline QCoreApplication& qapp()
{
    Q_ASSERT(QCoreApplication::instance());
    return *QCoreApplication::instance();
}

template <>
struct Executor<QCoreApplication> {
    static void post(QCoreApplication& executor, auto&& task)
    {
        QMetaObject::invokeMethod(&executor, EZ_FWD(task), Qt::ConnectionType::QueuedConnection);
    }
};

template <>
struct Executor<QThreadPool> {
    static void post(QThreadPool& executor, auto&& task) { executor.start(EZ_FWD(task)); }
};

}  // namespace ez::async

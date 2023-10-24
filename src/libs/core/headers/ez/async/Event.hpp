#pragma once

#include <atomic>

class Event {
public:
    void notifiy()
    {
        m_flag.test_and_set();
        m_flag.notify_all();
    }

    void wait() { m_flag.wait(false); }

private:
    std::atomic_flag m_flag;
};

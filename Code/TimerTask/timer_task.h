#ifndef TIMER_TASK_H_
#define TIMER_TASK_H_

#include <thread>
#include <atomic>
#include <chrono>
#include <functional>

class TimerTask
{
public:
    TimerTask() : m_execute(false)
    {}

    ~TimerTask() 
    {
        if (m_execute.load(std::memory_order_acquire))
            stop();
    }

    void stop() 
    {
        m_execute.store(false, std::memory_order_release);
        if (m_thd.joinable())
            m_thd.join();
    }

    // template task
    template<typename FUNC>
    void start(FUNC task, int interval_ms, bool immediately = true) 
    {
        if (m_execute.load(std::memory_order_acquire)) {
            stop();
        }
        m_execute.store(true, std::memory_order_release);
        m_thd = std::thread([this, task, interval_ms, immediately]() {
            while (m_execute.load(std::memory_order_acquire)) {
                if (immediately) {
                    task();
                    std::this_thread::sleep_for(std::chrono::milliseconds(interval_ms));
                } else {
                    std::this_thread::sleep_for(std::chrono::milliseconds(interval_ms));
                    task();
                }
            }
        });
    }

    bool is_running() const noexcept 
    {
        return (m_execute.load(std::memory_order_acquire) &&
                m_thd.joinable());
    }

private:
    std::atomic<bool> m_execute;
    std::thread m_thd;
};

#endif


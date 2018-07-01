#ifndef TIMER_TASK_H_
#define TIMER_TASK_H_

#include <thread>
#include <atomic>
#include <chrono>
#include <functional>


class TimerTask {
public:
    TimerTask() : _execute(false)
    {}

    ~TimerTask() {
        if (_execute.load(std::memory_order_acquire))
            stop();
    }

    void stop() {
        _execute.store(false, std::memory_order_release);
        if (_thd.joinable())
            _thd.join();
    }

    void start(int interval_ms, std::function<void(void)> task) {
        if (_execute.load(std::memory_order_acquire)) {
            stop();
        }
        _execute.store(true, std::memory_order_release);
        _thd = std::thread([this, interval_ms, task]() {
                    while (_execute.load(std::memory_order_acquire)) {
                        task();
                        std::this_thread::sleep_for(std::chrono::milliseconds(interval_ms));
                    }
                });
    }

    bool is_running() const noexcept {
        return (_execute.load(std::memory_order_acquire) && _thd.joinable());
    }

private:
    std::atomic<bool> _execute;
    std::thread _thd;
};

#endif



#ifndef ASYNC_TIMER_H_
#define ASYNC_TIMER_H_

#include <thread>
#include <chrono>
#include <queue>
#include <mutex>
#include <unistd.h>
#include "list_util.h"


template <typename T>
class CAsyncTimerTask
{
public:
    struct InnerTask {
        T raw_task;
        uint64_t timestamp_ms;

        InnerTask(T task, uint64_t t)
        : raw_task(task), timestamp_ms(t)
        {}

        bool operator<(const InnerTask& other) const {
            return (*this).timestamp_ms > other.timestamp_ms;
        }
    };


public:
    CAsyncTimerTask(void (*func)(T), const int max_size = 0, const int max_work = 1, const int delay_time_ms = 1000)
    : m_func(func)
    {
        m_max_size = max_size > 0 ? max_size : 0;
        m_max_work = max_work > 0 ? max_work : 1;
        m_dalay_time_ms = delay_time_ms > 0 ? delay_time_ms : 1000;

        for (size_t i = 0; i < m_max_work; ++i) {
            threads.push_back(std::thread(worker, (void *)this));
        }
        threads.push_back(std::thread(master, (void *)this));
    }

    virtual ~CAsyncTimerTask() 
    {
        m_run = false;

        // master中数据处理
        {
            std::unique_lock<std::mutex> lck(m_mtx);

            while (!m_timer_tasks.empty()) {
                InnerTask inner_task = m_timer_tasks.top();
                m_list.push_back(inner_task.raw_task);
                m_timer_tasks.pop();
            }
        }

        // worker数据处理
        m_list.shutdown();

        for (auto it = threads.begin(); it != threads.end(); ++it) {
            it->join();
        }
    }
    
    int add_task(T & task)
    {
        if (m_max_size > 0 && m_timer_tasks.size() >= m_max_size) {
            return -1;
        }

        uint64_t process_time = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count() + m_dalay_time_ms;
        InnerTask inner_task(task, process_time);

        std::unique_lock<std::mutex> lck(m_mtx);
        m_timer_tasks.push(inner_task);
        return 0;
    }

protected:
    static void master(void * arg)
    {
        ((CAsyncTimerTask<T> *)arg)->handle_master();
    }

    static void worker(void * arg)
    {
        ((CAsyncTimerTask<T> *)arg)->handle_worker();
    }


    // 一个主线程 判断时间
    void handle_master()
    {
        while(m_run) {

            uint64_t sleep_time_ms = 500;   // 500ms

            {
                std::unique_lock<std::mutex> lck(m_mtx);
                uint64_t cur_timestamp_ms = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();

                while (!m_timer_tasks.empty()) {
                    InnerTask inner_task = m_timer_tasks.top();

                    if (cur_timestamp_ms >= inner_task.timestamp_ms) {
                        m_list.push_back(inner_task.raw_task);
                        m_timer_tasks.pop();

                    } else {
                        sleep_time_ms = inner_task.timestamp_ms - cur_timestamp_ms;
                        break;
                    }
                }
            }

            // todo 不同task有不同的dalay_time  select监听信号+超时时间/信号量设置超时时间?
            std::this_thread::sleep_for(std::chrono::milliseconds(sleep_time_ms));
        }
    }

    // 多个从线程 执行函数
    void handle_worker() {
        while(1) {
            T task;
            if (!m_list.pop_front(task, TYPE_BLOCK)) {//will block
                if (!m_run) {
                    return;
                }
                usleep(30);
                continue;
            }
            (*m_func)(task);
        }
    }
        
private:
    bool m_run = true;

    size_t m_max_size;
    size_t m_max_work;
    size_t m_dalay_time_ms;
    std::list<std::thread> threads;

    void (*m_func)(T);

    std::mutex m_mtx;
    std::priority_queue<InnerTask> m_timer_tasks;

    CThreadSafeList<T> m_list;
};


#endif /* ASYNC_TIMER_H_ */

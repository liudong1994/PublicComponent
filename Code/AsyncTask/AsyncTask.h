#ifndef ASYNC_TASK_H_
#define ASYNC_TASK_H_

#include <thread>
#include <list>
#include <unistd.h>

#include "ThreadSafeList.h"


template <typename T>
class CAsyncTask
{
public:
    CAsyncTask(void (*func)(T), const int max_size = 0, const int max_work = 1) : m_func(func) {
        m_max_size = max_size > 0 ? max_size : 0;
        m_max_work = max_work > 0 ? max_work : 1;

        for (size_t i = 0; i < m_max_work; ++i) {
            m_threads.push_back(std::thread(entry, (void *)this));
        }
    }

    virtual ~CAsyncTask() {
        m_list.shutdown();
        for (auto it = m_threads.begin(); it != m_threads.end(); ++it) {
            it->join();
        }
    }

    int add_task(T & task) {
        if (m_max_size > 0 && m_list.size() >= m_max_size) {
            return -1;
        }
        m_list.push_back(task);

        return 0;
    }

protected:
    static void entry(void * pContext) {
        ((CAsyncTask<T> *)pContext)->handle();
    }

    void handle() {
        while(1) {
            T task;
            if (!m_list.pop_front(task, TYPE_BLOCK)) {
                if (m_list.is_stop()) {
                    return;
                }
                continue;
            }
            (*m_func)(task);
        }
    }


private:
    void (*m_func)(T);

    std::list<std::thread> m_threads;
    CThreadSafeList<T> m_list;
    size_t m_max_size;
    size_t m_max_work;
};

#endif


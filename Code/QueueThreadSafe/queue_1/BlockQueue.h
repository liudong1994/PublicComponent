#ifndef BLOCK_QUEUE_H_
#define BLOCK_QUEUE_H_

#include <deque>

#include "Mutex.h"
#include "Condition.h"


template<typename T>
class CBlockQueue {
public:
    CBlockQueue()
        : m_queue(),
        m_mutex(),
        m_cond(m_mutex)
    {}

    void put(const T & x) {
        CMutexGuard lock(m_mutex);
        m_queue.push_back(x);
        m_cond.notify();
    }

    T take() {
        CMutexGuard lock(m_mutex);
        while (m_queue.empty()) {
            m_cond.wait();
        }

        T front(m_queue.front());
        m_queue.pop_front();
        return front;
    }

    size_t size() {
        CMutexGuard lock(m_mutex);
        return m_queue.size();
    }

private:
    CBlockQueue(const CBlockQueue &);
    void operator=(const CBlockQueue &);

    std::deque<T> m_queue;
    CMutex m_mutex;
    CCondition m_cond;
};

#endif // BLOCK_QUEUE_H_


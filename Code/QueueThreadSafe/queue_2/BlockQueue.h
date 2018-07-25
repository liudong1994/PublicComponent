#ifndef BLOCK_QUEUE_H_
#define BLOCK_QUEUE_H_

#include <deque>
#include <mutex>    // C++11
#include <condition_variable>   // C++11


template<typename T>
class CBlockQueue {
public:
    CBlockQueue() {

    }

    CBlockQueue(const CBlockQueue &) = delete;
    void operator=(const CBlockQueue &) = delete;


    void put(const T & x) {
        std::unique_lock<std::mutex> lock(m_mutex);
        m_queue.push_back(x);
        m_cond.notify_one();
    }

    T take() {
        std::unique_lock<std::mutex> lock(m_mutex);
        while (m_queue.empty()) {
            m_cond.wait(lock);
        }

        T front(m_queue.front());
        m_queue.pop_front();
        return front;
    }

    size_t size() {
        // std::unique_lock<std::mutex> lock(m_mutex);
        std::lock_guard<std::mutex> lock(m_mutex);
        return m_queue.size();
    }


private:
    std::deque<T> m_queue;
    std::mutex  m_mutex;
    std::condition_variable m_cond;
};

#endif // BLOCK_QUEUE_H_


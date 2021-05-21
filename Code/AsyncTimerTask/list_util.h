
#ifndef LIST_UTIL_H_
#define LIST_UTIL_H_
#include <list>
#include <mutex>
#include <condition_variable>

const int TYPE_BLOCK = 1;
const int TYPE_NOT_BLOCK = 2;

template <typename T>
class CThreadSafeList
{
public:
    CThreadSafeList():
        stop(false),
        m_size(0) {
    }
public:
    void shutdown() {
        std::unique_lock<std::mutex> lck(m_mtx);
        stop = true;
        m_cv.notify_all();
    }
    bool is_stop() {
        return stop;
    }
    void push_back(T & a) {
        std::unique_lock<std::mutex> lck(m_mtx);
        m_list.push_back(a);
        ++m_size;
        m_cv.notify_one();
    }

    bool pop_front(T & a, const int type = TYPE_BLOCK) {
        if (type == TYPE_NOT_BLOCK) {
            return getWithoutBlock(a);
        }

        return getWithBlock(a);
    }

    size_t size() {
        return m_size;
    }

protected:
    bool getWithoutBlock(T & a) {
        std::unique_lock<std::mutex> lck(m_mtx, std::defer_lock);
        if (!lck.try_lock()) {
            return false;
        }

        if (m_list.empty()) {
            return false;
        }
        a = m_list.front();
        m_list.pop_front();

        return true;
    }

    bool getWithBlock(T & a) {
        std::unique_lock<std::mutex> lck(m_mtx);
        while (m_list.empty()) {
            if (stop) {
                return false;
            }
            m_cv.wait(lck);
        }
        a = m_list.front();
        m_list.pop_front();
        --m_size;

        return true;
    }

private:
    bool stop;
    std::condition_variable m_cv;
    std::mutex m_mtx;
    std::list<T> m_list;
    uint64_t m_size;
};

#endif /* LIST_UTIL_H_ */

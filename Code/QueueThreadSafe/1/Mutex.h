#ifndef MUTEX_H_
#define MUTEX_H_

#include <pthread.h>


class CMutex {
public:
    CMutex() {
        pthread_mutex_init(&m_mutex, NULL);
    }

    ~CMutex() {
        pthread_mutex_destroy(&m_mutex);
    }

    void lock() {
        pthread_mutex_lock(&m_mutex);
    }

    void unlock() {
        pthread_mutex_unlock(&m_mutex);
    }

    pthread_mutex_t *get_mutex() {
        return &m_mutex;
    }

private:
    CMutex(const CMutex &);
    void operator=(const CMutex &);

    pthread_mutex_t m_mutex;
};

class CMutexGuard {
public:
    explicit CMutexGuard(CMutex & mutex)
        : m_mutex(mutex)
    {
        m_mutex.lock();
    }

    ~CMutexGuard() {
         m_mutex.unlock();
    }

private:
    CMutex & m_mutex;
};

#endif // MUTEX_H_


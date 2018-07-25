#ifndef CONDITION_H_
#define CONDITION_H_

#include "Mutex.h"


class CCondition {
public:
    explicit CCondition(CMutex & mutex)
        : m_mutex(mutex)
    {
        pthread_cond_init(&m_pcond, NULL);
    }

    ~CCondition() {
        pthread_cond_destroy(&m_pcond);
    }

    void wait() {
        pthread_cond_wait(&m_pcond, m_mutex.get_mutex());
    }

    void notify() {
        pthread_cond_signal(&m_pcond);
    }

    void notify_all() {
         pthread_cond_broadcast(&m_pcond);
    }

private:
    CCondition(const CCondition &);
    void operator=(const CCondition &);

    CMutex & m_mutex;
    pthread_cond_t m_pcond;
};


#endif // CONTION_H_


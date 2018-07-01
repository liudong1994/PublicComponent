#ifndef _CACHE_H_
#define _CACHE_H_

#include <string>
#include <unordered_map>
#include <memory>
#include <pthread.h>
#include <time.h>
#include <sstream>
#include <typeinfo>
#include "timer_task.h"


namespace CACHE {

enum CACHE_CODE {
    CACHE_OK = 0,
    CACHE_ERROR = -1,
    CACHE_KEY_NOT_EXIST = 1,
    CACHE_KEY_EMPTY = 2,
    CACHE_NOT_NUM = 3,
};


template <typename V>
class Cache {
public:
    struct Item {
        V _value;
        time_t _deadtime;

        Item() : _deadtime(-1) {}

        Item(const V & value, const time_t deadtime) :
            _value(value), _deadtime(deadtime)
        {}
    };

    Cache();
    Cache(const Cache&) = delete;
    Cache & operator=(const Cache&) = delete;
    ~Cache();

public:
    int get(const std::string& key, V& value);
    int set(const std::string& key, const V& value, const time_t expire = -1);
    int ttl(const std::string& key, time_t& expire);
    int del(const std::string& key);
    int incr(const std::string& key, V& value, const time_t expire = -1);
    int incrby(const std::string& key, const V& inc, V& value, const time_t expire = -1);
    std::string print();

private:
    bool is_value_num(const V& value);
    static void clean_expire(Cache<V>* obj);


private:
    std::unordered_map<std::string, std::shared_ptr<Item>> m_cache_map;
    pthread_rwlock_t m_lock;
    TimerTask m_timer;
};

template <typename V>
Cache<V>::Cache() {
    pthread_rwlock_init(&m_lock, nullptr);
    m_timer.start(1000, std::bind(clean_expire, this));
}

template <typename V>
Cache<V>::~Cache() {
    if (m_timer.is_running())
        m_timer.stop();

    pthread_rwlock_destroy(&m_lock);
}

template <typename V>
int Cache<V>::get(const std::string& key, V& value) {
    int ret = CACHE_OK;
    if (key.empty()) {
        return CACHE_KEY_EMPTY;
    }

    pthread_rwlock_rdlock(&m_lock);
    auto itr = m_cache_map.find(key);
    if (itr == m_cache_map.end()) {
        ret = CACHE_KEY_NOT_EXIST;
    } else {
        value = itr->second->_value;
    }
    pthread_rwlock_unlock(&m_lock);

    return ret;
}

template <typename V>
int Cache<V>::set(const std::string& key, const V& value, const time_t expire) {
    int ret = CACHE_OK;
    if (key.empty()) {
        return CACHE_KEY_EMPTY;
    }

    time_t deadtime = expire;
    if (-1 != expire) {
        time_t now = time(nullptr);
        deadtime = now + expire;
    }

    std::shared_ptr<Item> item = std::make_shared<Item>(value, deadtime);

    pthread_rwlock_wrlock(&m_lock);
    m_cache_map[key] = item;
    pthread_rwlock_unlock(&m_lock);

    return ret;
}

template <typename V>
int Cache<V>::ttl(const std::string& key, time_t& expire) {
    int ret = CACHE_OK;
    if (key.empty()) {
        return CACHE_KEY_EMPTY;
    }

    pthread_rwlock_rdlock(&m_lock);
    auto itr = m_cache_map.find(key);
    if (itr == m_cache_map.end()) {
        ret = CACHE_KEY_NOT_EXIST;
    } else {
        int deadtime = itr->second->_deadtime;
        if (-1 == deadtime) {
            expire = -1;
        } else {
            time_t now = time(nullptr);
            expire = deadtime - now;
        }
    }
    pthread_rwlock_unlock(&m_lock);

    return ret;
}

template <typename V>
int Cache<V>::del(const std::string& key) {
    int ret = CACHE_OK;
    if (key.empty()) {
        return CACHE_KEY_EMPTY;
    }

    pthread_rwlock_wrlock(&m_lock);
    int opt = m_cache_map.erase(key);
    pthread_rwlock_unlock(&m_lock);

    if (0 == opt) {
        ret = CACHE_KEY_NOT_EXIST;
    }

    return ret;
}

template <typename V>
bool Cache<V>::is_value_num(const V& value) {
    if (
        typeid(value) == typeid(char)
        || typeid(value) == typeid(unsigned char)
        || typeid(value) == typeid(signed char)
        || typeid(value) == typeid(short)
        || typeid(value) == typeid(unsigned short)
        || typeid(value) == typeid(int)
        || typeid(value) == typeid(unsigned)
        || typeid(value) == typeid(long)
        || typeid(value) == typeid(unsigned long)
        || typeid(value) == typeid(long long)
        || typeid(value) == typeid(unsigned long long)
        || typeid(value) == typeid(float)
        || typeid(value) == typeid(double)
        || typeid(value) == typeid(long double)
    ) {
        return true;
    }

    return false;
}

template <typename V>
int Cache<V>::incr(const std::string& key, V& value, const time_t expire) {
    int ret = CACHE_OK;
    if (key.empty()) {
        return CACHE_KEY_EMPTY;
    }

    if (!is_value_num(value)) {
        return CACHE_NOT_NUM;
    }

    time_t deadtime = expire;
    if (-1 != expire) {
        time_t now = time(nullptr);
        deadtime = now + expire;
    }

    pthread_rwlock_wrlock(&m_lock);
    auto itr = m_cache_map.find(key);
    if (itr == m_cache_map.end()) {
        value = 1;
        std::shared_ptr<Item> item = std::make_shared<Item>(value, deadtime);
        m_cache_map[key] = item;
    } else {
        value = ++itr->second->_value;
        itr->second->_deadtime = deadtime;
    }
    pthread_rwlock_unlock(&m_lock);

    return ret;
}

template <typename V>
int Cache<V>::incrby(const std::string& key, const V& inc, V& value, const time_t expire) {
    int ret = CACHE_OK;
    if (key.empty()) {
        return CACHE_KEY_EMPTY;
    }

    if (!is_value_num(value)) {
        return CACHE_NOT_NUM;
    }

    time_t deadtime = expire;
    if (-1 != expire) {
        time_t now = time(nullptr);
        deadtime = now + expire;
    }

    pthread_rwlock_wrlock(&m_lock);
    auto itr = m_cache_map.find(key);
    if (itr == m_cache_map.end()) {
        value = inc;
        std::shared_ptr<Item> item = std::make_shared<Item>(value, deadtime);
        m_cache_map[key] = item;
    } else {
        itr->second->_value += inc;
        value = itr->second->_value;
        itr->second->_deadtime = deadtime;
    }
    pthread_rwlock_unlock(&m_lock);

    return ret;
}

template <typename V>
std::string Cache<V>::print() {
    std::stringstream ss;
    ss << "\n----------------------------------------\n";
    pthread_rwlock_rdlock(&m_lock);
    ss << "[key]\t[value]\t[expire]\t(" << m_cache_map.size() << " keys)\n";
    for (auto itr = m_cache_map.begin(); itr != m_cache_map.end(); itr++) {
        ss << itr->first << "\t"; 
        ss << itr->second->_value << "\t";
        if (-1 == itr->second->_deadtime) {
            ss << -1 << "\n";
        } else {
            time_t now = time(nullptr);
            ss << itr->second->_deadtime - now << "\n";
        }
    }
    pthread_rwlock_unlock(&m_lock);
    ss << "----------------------------------------\n";
    return ss.str();
}

template <typename V>
void Cache<V>::clean_expire(Cache<V>* obj) {
    if (obj == nullptr) {
        return;
    }

    for (auto itr = obj->m_cache_map.begin(); itr != obj->m_cache_map.end();) {
        auto temp = itr++;
        time_t now = time(nullptr);
        if (temp->second->_deadtime > 0 && temp->second->_deadtime < now) {
            pthread_rwlock_wrlock(&obj->m_lock);
            obj->m_cache_map.erase(temp);
            pthread_rwlock_unlock(&obj->m_lock);
        }
    }
}

}

#endif


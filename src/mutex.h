/**
  ******************************************************************************
  * @file           : mutex.h
  * @author         : hyn
  * @brief          : None
  * @attention      : None
  * @date           : 2023/4/1
  ******************************************************************************
  */
#pragma once


#include <boost/noncopyable.hpp>
#include <functional>
#include <pthread.h>
#include <semaphore.h>
#include <semaphore>
#include <atomic>
#include <list>

#include "Logger.h"


namespace hyn::mutex {

/**
*@类名：Semaphore : noncopyable
*@参数：m_semaphore
*/
class Semaphore : boost::noncopyable {
public:
    /**
     *@作用：构造函数
     *@参数：信号值的大小
     *@返回值：null
     */
    explicit Semaphore(uint32_t count = 0);

    ~Semaphore();

    /**
     *@作用：获取信号量
     *@参数：null
     *@返回值：null
     */
    void wait();

    /**
     *@作用：释放信号量
     *@参数：null
     *@返回值：null
     */
    void notify();

private:
    sem_t m_semaphore{};
};

/**
 * @brief 局部锁的模板实现
 */
template<class T>
struct ScopedLockImpl {
public:

    explicit ScopedLockImpl(T &mutex)
            : m_mutex(mutex) {
        m_mutex.lock();
        m_locked = true;
    }

    /**
     * @brief 析构函数,自动释放锁
     */
    ~ScopedLockImpl() {
        unlock();
    }


    void lock() {
        if (!m_locked) {
            m_mutex.lock();
            m_locked = true;
        }
    }

    void unlock() {
        if (m_locked) {
            m_mutex.unlock();
            m_locked = false;
        }
    }

private:
    /// mutex
    T &m_mutex;
    /// 是否已上锁
    bool m_locked;
};

/**
 *@作用：局部读锁模板实现
 *@参数：null
 *@返回值：null
 */
template<class T>
struct ReadScopedLockImpl {
public:

    /**
     * @brief 构造函数
     * @param[in] mutex 读写锁
     */
    explicit ReadScopedLockImpl(T &mutex)
            : m_mutex(mutex) {
        m_mutex.rdlock();
        m_locked = true;
    }

    ~ReadScopedLockImpl() {
        unlock();
    }


    void lock() {
        if (!m_locked) {
            m_mutex.rdlock();
            m_locked = true;
        }
    }


    void unlock() {
        if (m_locked) {
            m_mutex.unlock();
            m_locked = false;
        }
    }

private:
    T &m_mutex;
    bool m_locked;
};

/**
 * @brief 局部写锁模板实现
 */
template<class T>
struct WriteScopedLockImpl {
public:

    explicit WriteScopedLockImpl(T &mutex)
            : m_mutex(mutex) {
        m_mutex.wrlock();
        m_locked = true;
    }


    ~WriteScopedLockImpl() {
        unlock();
    }


    void lock() {
        if (!m_locked) {
            m_mutex.wrlock();
            m_locked = true;
        }
    }


    void unlock() {
        if (m_locked) {
            m_mutex.unlock();
            m_locked = false;
        }
    }

private:
    T &m_mutex;
    bool m_locked;
};

/**
 * @brief 互斥量
 */
class Mutex : boost::noncopyable {
public:
    /// 局部锁
    typedef ScopedLockImpl<Mutex> Lock;


    Mutex() {
        pthread_mutex_init(&m_mutex, nullptr);
    }


    ~Mutex() {
        pthread_mutex_destroy(&m_mutex);
    }


    void lock() {
        pthread_mutex_lock(&m_mutex);
    }

    void unlock() {
        pthread_mutex_unlock(&m_mutex);
    }

private:
    pthread_mutex_t m_mutex{};
};

/**
 * @brief 读写互斥量
 */
class RWMutex : boost::noncopyable {
public:

    typedef ReadScopedLockImpl<RWMutex> ReadLock;


    typedef WriteScopedLockImpl<RWMutex> WriteLock;


    RWMutex() {
        pthread_rwlock_init(&m_lock, nullptr);
    }


    ~RWMutex() {
        pthread_rwlock_destroy(&m_lock);
    }

    void rdlock() {
        pthread_rwlock_rdlock(&m_lock);
    }

    void wrlock() {
        pthread_rwlock_wrlock(&m_lock);
    }

    void unlock() {
        pthread_rwlock_unlock(&m_lock);
    }

private:
    pthread_rwlock_t m_lock{};
};

/**
 * @brief 自旋锁
 */
class Spinlock : boost::noncopyable {
public:
    /// 局部锁
    typedef ScopedLockImpl<Spinlock> Lock;

    /**
     * @brief 构造函数
     */
    Spinlock() {
        pthread_spin_init(&m_mutex, 0);
    }

    /**
     * @brief 析构函数
     */
    ~Spinlock() {
        pthread_spin_destroy(&m_mutex);
    }

    /**
     * @brief 上锁
     */
    void lock() {
        pthread_spin_lock(&m_mutex);
    }

    /**
     * @brief 解锁
     */
    void unlock() {
        pthread_spin_unlock(&m_mutex);
    }

private:
    /// 自旋锁
    pthread_spinlock_t m_mutex;
};

/**
 * @brief 原子锁
 */
class CASLock : boost::noncopyable {
public:
    /// 局部锁
    typedef ScopedLockImpl<CASLock> Lock;

    /**
     * @brief 构造函数
     */
    CASLock() {
        m_mutex.clear();
    }

    /**
     * @brief 析构函数
     */
    ~CASLock() = default;

    /**
     * @brief 上锁
     */
    void lock() {
        while (std::atomic_flag_test_and_set_explicit(&m_mutex, std::memory_order_acquire));
    }

    /**
     * @brief 解锁
     */
    void unlock() {
        std::atomic_flag_clear_explicit(&m_mutex, std::memory_order_release);
    }

private:
    /// 原子状态
    volatile std::atomic_flag m_mutex;
};

/**
 *@brief 协程锁
 */
class FiberSemaphore;

}

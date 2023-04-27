/**
  ******************************************************************************
  * @file           : mutex.cpp
  * @author         : hyn
  * @brief          : None
  * @attention      : None
  * @date           : 2023/4/1
  ******************************************************************************
  */



#include "mutex.h"
#include "Logger.h"
#include "Scheduler.h"
#include "fiber.h"

hyn::mutex::Semaphore::Semaphore(const uint32_t count) {
    if (sem_init(&m_semaphore, 0, count)) {
        error("sem init error");
        THROW_RUNTIME_ERROR_IF(1, "sem init error");
    }
}

hyn::mutex::Semaphore::~Semaphore() {
    sem_destroy(&m_semaphore);
}

void hyn::mutex::Semaphore::wait() {
    if (sem_wait(&m_semaphore)) {
        fatal("sem wait error");
        THROW_RUNTIME_ERROR_IF(1, "sem wait error");
    }
}

void hyn::mutex::Semaphore::notify() {
    if (sem_post(&m_semaphore)) {
        fatal("sem post error");
        THROW_RUNTIME_ERROR_IF(1, "sem post error");
    }
}

class hyn::mutex::FiberSemaphore : boost::noncopyable {
public:
    typedef Spinlock MutexType;

    FiberSemaphore(size_t initial_concurrency = 0);

    ~FiberSemaphore();

    bool tryWait();

    void wait();

    void notify();

    size_t getConcurrency() const { return m_concurrency; }

    void reset() { m_concurrency = 0; }

private:
    MutexType m_mutex;
    std::list<std::pair<hyn::scheduler::Scheduler *, hyn::fiber::Fiber::ptr> > m_waiters;
    size_t m_concurrency;
};

hyn::mutex::FiberSemaphore::FiberSemaphore(size_t initial_concurrency) : m_concurrency(initial_concurrency) {

}

hyn::mutex::FiberSemaphore::~FiberSemaphore() {
    assert(m_waiters.empty());
}

bool hyn::mutex::FiberSemaphore::tryWait() {
    assert(hyn::scheduler::Scheduler::GetThis());
    {
        MutexType::Lock lock(m_mutex);
        if (m_concurrency > 0u) {
            --m_concurrency;
            return true;
        }
        return false;
    }
}

void hyn::mutex::FiberSemaphore::wait() {
    assert(hyn::scheduler::Scheduler::GetThis());
    {
        MutexType::Lock lock(m_mutex);
        if (m_concurrency > 0u) {
            --m_concurrency;
            return;
        }
        m_waiters.emplace_back(hyn::scheduler::Scheduler::GetThis(), hyn::fiber::Fiber::GetThis());
    }
    hyn::fiber::Fiber::YieldToHold();
}

void hyn::mutex::FiberSemaphore::notify() {
    MutexType::Lock lock(m_mutex);
    if (!m_waiters.empty()) {
        auto next = m_waiters.front();
        m_waiters.pop_front();
        next.first->schedule(next.second);
    } else {
        ++m_concurrency;
    }
}

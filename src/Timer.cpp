/**
  ******************************************************************************
  * @file           : Timer.cpp
  * @author         : hyn
  * @brief          : None
  * @attention      : None
  * @date           : 2023/4/9
  ******************************************************************************
  */
#include <utility>

#include "../include/Timer.h"
#include "../include/util.h"

namespace hyn {

Timer::Timer(uint64_t ms, std::function<void()> cb, bool recurring, TimerManager *manager) :
        m_recurring(recurring), m_ms(ms), m_cb(std::move(cb)), m_manager(manager), m_next(util::GetCurrentMS() + m_ms) {
}

Timer::Timer(uint64_t next) : m_next(next) {
}

bool Timer::Comparator::operator()(const Timer::ptr &lhs, const Timer::ptr &rhs) const {
    if (!lhs && !rhs)
        return false;
    if (!lhs)
        return true;
    if (!rhs)
        return false;
    if (lhs->m_next < rhs->m_next)
        return true;
    if (rhs->m_next < lhs->m_next)
        return false;
    return lhs.get() < rhs.get();
}


TimerManager::TimerManager() = default;

TimerManager::~TimerManager() = default;

Timer::ptr TimerManager::addTimer(uint64_t ms, std::function<void()> cb, bool recurring) {
    Timer::ptr timer(new Timer(ms, std::move(cb), recurring, this));
    RWMutexType::WriteLock lock(m_mutex);
    addTimer(timer, lock);
    return timer;
}

static void OnTimer(const std::weak_ptr<void> &weak_cond, const std::function<void()> &cb) {
    std::shared_ptr<void> tmp = weak_cond.lock();
    if (tmp)
        cb();
}

Timer::ptr
TimerManager::addConditionTimer(uint64_t ms, const std::function<void()> &cb, const std::weak_ptr<void> &weak_cond,
                                bool recurring) {
    return addTimer(ms, [weak_cond, cb] { return hyn::OnTimer(weak_cond, cb); }, recurring);
}

uint64_t TimerManager::getNextTimer() {
    RWMutexType::ReadLock lock(m_mutex);
    if (m_timers.empty())
        return ~0ull;
    const Timer::ptr &next = *m_timers.begin();
    uint64_t now_ms = util::GetCurrentMS();
    if (now_ms >= next->m_next) {
        return 0;
    } else {
        return next->m_next - now_ms;
    }
}

void TimerManager::listExpiredCb(std::vector<std::function<void()>> &cbs) {
    uint64_t now_ms = util::GetCurrentMS();
    std::vector<Timer::ptr> expired;
    {
        RWMutexType::ReadLock lock(m_mutex);
        if (m_timers.empty())
            return;

    }
    RWMutexType::WriteLock lock(m_mutex);
    Timer::ptr now_timer(new Timer(now_ms));
    auto it = m_timers.lower_bound(now_timer);
    while (it != m_timers.end() && (*it)->m_next == now_ms)
        ++it;
    expired.insert(expired.begin(), m_timers.begin(), it);
    m_timers.erase(m_timers.begin(), it);
    cbs.reserve(expired.size());
    
}

void TimerManager::addTimer(const Timer::ptr &val, RWMutexType::WriteLock &lock) {
    auto it = m_timers.insert(val).first;
    bool at_front = (it == m_timers.begin()) && !m_tickled;
    if (at_front) {
        m_tickled = true;
    }
    lock.unlock();

    if (at_front) {
        onTimerInsertedAtFront();
    }
}


}

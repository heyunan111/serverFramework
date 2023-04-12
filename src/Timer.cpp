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

#include "Timer.h"
#include "util.h"

namespace hyn {


bool Timer::cancel() {
    TimerManager::RWMutexType::WriteLock lock(m_manager->m_mutex);
    if (m_cb) {
        m_cb = nullptr;
        auto it = m_manager->m_timers.find(shared_from_this());
        if (it == m_manager->m_timers.end())
            return false;
        m_manager->m_timers.erase(it);
        return true;
    }
    return false;
}

bool Timer::refresh() {
    TimerManager::RWMutexType::WriteLock lock(m_manager->m_mutex);
    if (!m_cb)
        return false;
    auto it = m_manager->m_timers.find(shared_from_this());
    if (it == m_manager->m_timers.end())
        return false;
    m_manager->m_timers.erase(it);
    m_next = util::GetCurrentMS() + m_ms;
    m_manager->m_timers.insert(shared_from_this());
    return true;
}


bool Timer::reset(uint64_t ms, bool from_now) {
    if (m_ms == ms && !from_now)
        return true;
    TimerManager::RWMutexType::WriteLock lock(m_manager->m_mutex);
    if (!m_cb)
        return false;
    auto it = m_manager->m_timers.find(shared_from_this());
    if (it == m_manager->m_timers.end())
        return false;
    m_manager->m_timers.erase(it);
    uint64_t start{};
    if (from_now) {
        start = util::GetCurrentMS();
    } else {
        start = m_next - m_ms;
    }
    m_ms = ms;
    m_next = start + m_ms;
    m_manager->addTimer(shared_from_this(), lock);
    return true;
}

Timer::Timer(uint64_t ms, std::function<void()> cb, bool recurring, TimerManager *manager)
        : m_cb(std::move(cb)), m_ms(ms), m_recurring(recurring), m_manager(manager),
          m_next(util::GetCurrentMS() + m_ms) {}

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
    if (lhs->m_next > rhs->m_next)
        return false;
    return lhs.get() < rhs.get();
}


TimerManager::TimerManager() {
    m_previouseTime = util::GetCurrentMS();
}

TimerManager::~TimerManager() = default;

Timer::ptr TimerManager::addTimer(uint64_t ms, std::function<void()> cb, bool recurring) {
    Timer::ptr t(new Timer(ms, std::move(cb), recurring, this));
    RWMutexType::WriteLock lock(m_mutex);
    addTimer(t, lock);
    return t;
}

Timer::ptr
TimerManager::addConditionTimer(uint64_t ms, const std::function<void()> &cb, const std::weak_ptr<void> &weak_cond,
                                bool recurring) {
    return addTimer(ms, [weak_cond, cb] { return onTimer(weak_cond, cb); }, recurring);
}

uint64_t TimerManager::getNextTimer() {
    RWMutexType::ReadLock lock(m_mutex);
    m_tickled = false;
    if (m_timers.empty())
        return ~0ul;//最大值
    const Timer::ptr &next = *m_timers.begin();
    auto now_ms = util::GetCurrentMS();
    if (now_ms >= next->m_next) {
        return 0;
    } else {
        return next->m_next - now_ms;
    };
}

void TimerManager::addTimer(const Timer::ptr &val, mutex::RWMutex::WriteLock &lock) {
    auto it = m_timers.insert(val);
    if (it.first == m_timers.begin() && !m_tickled)
        m_tickled = true;
    lock.unlock();
    if (it.first == m_timers.begin() && !m_tickled)
        onTimerInsertedAtFront();
}

void TimerManager::listExpiredCb(std::vector<std::function<void()>> &cbs) {
    uint64_t now_time = util::GetCurrentMS();
    std::vector<Timer::ptr> expired;
    {
        RWMutexType::ReadLock lock(m_mutex);
        if (m_timers.empty())
            return;
    }
    RWMutexType::WriteLock lock1(m_mutex);
    if (m_timers.empty())
        return;
    bool rollover = detectClockRollover(now_time);
    if (!rollover && (*m_timers.begin())->m_next > now_time)
        return;
    Timer::ptr nowTimer(new Timer(now_time));
    auto it = rollover ? m_timers.end() : m_timers.lower_bound(nowTimer);
    while (it != m_timers.end() && (*it)->m_next == now_time)
        it++;
    expired.insert(expired.begin(), m_timers.begin(), it);
    m_timers.erase(m_timers.begin(), it);
    cbs.reserve(expired.size());
    for (auto &time: expired) {
        cbs.push_back(time->m_cb);
        if (time->m_recurring) {
            time->m_next = now_time + time->m_ms;
            m_timers.insert(time);
        } else {
            time->m_cb = nullptr;
        }
    }
}

bool TimerManager::hasTimer() {
    RWMutexType::ReadLock lock(m_mutex);
    return !m_timers.empty();
}

void TimerManager::onTimer(const std::weak_ptr<void> &weak_cond, const std::function<void()> &cb) {
    auto temp = weak_cond.lock();
    if (temp)
        cb();
}

/**
方法中通过比较当前时间 now_ms 和上一次记录的时间 m_previouseTime，如果 now_ms 小于 m_previouseTime 并且小于
 m_previouseTime - 60 * 60 * 1000（即上一次记录时间的一小时前），则认为时钟回滚了，返回 true。
在定时器中，时钟回滚会导致定时器失效，为了避免这种情况，detectClockRollover 方法会在检测到时钟回滚时，将所有定时
 器的下次触发时间都重置为当前时间，从而保证定时器正常工作。
 */
bool TimerManager::detectClockRollover(uint64_t now_ms) {
    bool rollover = false;
    if (now_ms < m_previouseTime &&
        now_ms < (m_previouseTime - 60 * 60 * 1000)) {
        rollover = true;
    }
    m_previouseTime = now_ms;
    return rollover;
}
}

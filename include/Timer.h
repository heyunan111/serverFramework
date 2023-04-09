/**
  ******************************************************************************
  * @file           : Timer.h
  * @author         : hyn
  * @brief          : None
  * @attention      : None
  * @date           : 2023/4/9
  ******************************************************************************
  */
#pragma once

#include <memory>
#include <set>
#include "thread.h"


namespace hyn {

class TimerManager;

class Timer : public std::enable_shared_from_this<Timer> {
    friend TimerManager;
public:
    typedef std::shared_ptr<Timer> ptr;

    /**
     *@作用：取消定时器
     *@参数：null
     *@返回值：是否成功
     */
    bool cancel();

    /**
     *@作用：刷新设置定时器的执行时间
     *@参数：null
     *@返回值：是否成功
     */
    bool refresh();

    /**
     *@作用：重置定时器时间
     *@参数：定时器执行间隔时间（毫秒）
     *@参数：是否从当前时间开始计算
     *@返回值：是否成功
     */
    bool reset(uint64_t ms, bool from_now);

private:
    /**
     *@作用：构造函数
     *@参数：定时器执行间隔时间
     *@参数：回调函数
     *@参数：是否循环
     *@参数：定时器管理器
     *@返回值：null
     */
    Timer(uint64_t ms, std::function<void()> cb, bool recurring, TimerManager *manager);

    explicit Timer(uint64_t next);

    ///是否循环定时器
    bool m_recurring{false};
    ///执行周期
    uint64_t m_ms{0};
    ///执行完成之后时间
    uint64_t m_next{0};
    ///回调函数
    std::function<void()> m_cb;
    ///定时器管理器
    TimerManager *m_manager{nullptr};

    ///定时器比较仿函数
    struct Comparator {
        /**
         *@作用：比较定时器的只能指针大小，按照执行时间排
         *@参数：定时器智能指针
         */
        bool operator()(const Timer::ptr &lhs, const Timer::ptr &rhs) const;
    };
};

class TimerManager {
    friend Timer;
public:
    typedef mutex::RWMutex RWMutexType;

    TimerManager();

    virtual ~TimerManager();

    /**
     *@作用：添加定时器
     *@参数：定时器执行间隔时间
     *@参数：回调函数
     *@参数：是否循环定时器
     */
    Timer::ptr addTimer(uint64_t ms, std::function<void()> cb, bool recurring = false);

    /**
     *@作用：添加条件定时器
     *@参数：定时器执行间隔时间
     *@参数：回调函数
     *@参数：条件
     *@参数：是否循环定时器
     */
    Timer::ptr
    addConditionTimer(uint64_t ms, const std::function<void()> &cb, const std::weak_ptr<void> &weak_cond,
                      bool recurring = false);

    /**
     *@作用：得到最近一个定时器执行的时间间隔
     */
    uint64_t getNextTimer();

    /**
     *@作用：在定时器超时后执行用户指定的回调函数
     */
    static void onTimer(const std::weak_ptr<void> &weak_cond, const std::function<void()> &cb);

protected:
    ///当有新的定时器插入到定时器的首部，执行
    virtual void onTimerInsertedAtFront() = 0;

    /**
     *@作用：将定时器添加到管理类中
     */
    void addTimer(const Timer::ptr &val, RWMutexType::WriteLock &lock);


    /**
     *@作用：获取需要执行的定时器的回调函数列表
     *@参数：[out]回调函数数组
     */
    void listExpiredCb(std::vector<std::function<void()>> &cbs);

    /**
     *@作用：是否有定时器
     */
    bool hasTimer();

private:
    /**
     *@作用：检测服务器是否被调后了
     *@参数：当前时间
     */
    bool detectClockRollover(uint64_t now_ms);

    /// Mutex
    RWMutexType m_mutex;
    /// 定时器集合
    std::set<Timer::ptr, Timer::Comparator> m_timers;
    /// 是否触发onTimerInsertedAtFront
    bool m_tickled = false;
    /// 上次执行时间
    uint64_t m_previouseTime = 0;
};
}
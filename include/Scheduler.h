/**
  ******************************************************************************
  * @file           : Scheduler.h
  * @author         : hyn
  * @brief          : None
  * @attention      : None
  * @date           : 2023/4/2
  ******************************************************************************
  */
#pragma once

#include <memory>
#include <list>
#include <boost/noncopyable.hpp>
#include <utility>
#include "mutex.h"
#include "fiber.h"
#include "thread.h"

namespace hyn::scheduler {

/**
 *@类名：Scheduler
 *@参数：mutexType m_mutex;
 *@参数：std::vector<thread::Thread::ptr> m_threads;     线程对象列表
 *@参数：std::list<Task> m_fibers;                       任务集合
 *@参数：std::string m_name;
 */

class Scheduler : boost::noncopyable {
public:
    typedef std::shared_ptr<Scheduler> ptr;
    typedef hyn::mutex::Mutex mutexType;

    explicit Scheduler(size_t thread = 1, bool use_caller = true, const std::string &name = "");

    virtual ~Scheduler();

    [[nodiscard]] const std::string &get_name() const { return m_name; }

    /**
     *@作用：启动写成调度器
     *@参数：null
     *@返回值：null
     */
    void start();

    /**
     *@作用：停止协调调度器
     *@参数：null
     *@返回值：null
     */
    void stop();

    /**
     *@作用：添加任务
     *@参数：fc：可执行函数
     *@参数：thread：绑定的线程id，默认-1
     *@返回值：null
     */
    template<typename fiber_or_callback>
    void Schedule(fiber_or_callback fc, int thread = -1) {
        bool need_tickle = false;
        {
            mutexType::Lock lock(m_mutex);
            need_tickle = scheduleNoLock(fc, thread);
        }
        if (need_tickle) {
            tickle();
        }
    }

    /**
     *@作用：添加多个任务
     *@参数：begin：单项迭代器，开始位置
     *@参数：end：结束位置
     *@返回值：null
     */
    template<typename InputIter>
    void schedule(InputIter begin, InputIter end) {
        bool need_tickle = false;
        {
            mutexType::Lock lock(m_mutex);
            while (begin != end) {
                need_tickle = scheduleNoLock(&(*begin) || need_tickle);
            }
        }
        if (need_tickle)
            tickle();
    }

public:
    static Scheduler *GetThis();

    static fiber::Fiber *GetMainFiber();

protected:
    /**
     *@作用：通知协程调度器有任务
     *@参数：null
     *@返回值：null
     */
    virtual void tickle();

    /**
     *@作用：协程调度器
     *@参数：null
     *@返回值：null
     */
    void run();

    /**
     *@作用：判断是否可以停止
     *@参数：null
     *@返回值：bool
     */
    virtual bool stopping();


    /**
     *@作用：设置当前协程调度器
     *@参数：null
     *@返回值：null
     */
    void SetThis();

    /**
     *@作用：协程无任务时执行idle
     *@参数：null
     *@返回值：null
     */
    virtual void idle();

private:
    /**
     *@作用：添加任务（无锁）
     *@参数：fc：可执行实例
     *@参数：thread：要绑定id
     *@返回值：null
     */
    template<typename fiber_or_callback>
    bool scheduleNoLock(fiber_or_callback fc, int thread) {
        bool need_tickle = m_fibers.empty();
        Task task(fc, thread);
        if (task.fiber || task.cb)
            m_fibers.push_back(task);
        return need_tickle;
    }

    struct Task {
        fiber::Fiber::ptr fiber;
        std::function<void()> cb;
        int thread;

        Task(fiber::Fiber::ptr f, int thr) : fiber(std::move(f)), thread(thr) {}

        Task(fiber::Fiber::ptr *f, int thr) : thread(thr) {
            fiber.swap(*f);
        }

        Task(std::function<void()> f, int thr) : cb(std::move(f)), thread(thr) {}

        Task(std::function<void()> *f, int thr) : thread(thr) {
            cb.swap(*f);
        }

        Task() : thread(-1) {}

        /**
         *@作用：重置
         *@参数：null
         *@返回值：null
         */
        void reset() {
            fiber = nullptr;
            cb = nullptr;
            thread = -1;
        }
    };

protected:
    std::vector<int> m_thread_ids;      //协程id数组
    size_t m_thread_count{0};           //线程数
    size_t m_active_thread_count{0};    //工作线程数
    size_t m_idle_thread_count{0};      //空闲线程数
    bool m_stopping{true};              //是否正在停止
    bool m_auto_stop{false};            //是否自动停止
    int m_root_thread_id{0};            //主线程id
private:
    mutexType m_mutex;
    std::vector<thread::Thread::ptr> m_threads;     //线程池
    std::list<Task> m_fibers;                       //待执行的协程队列
    std::string m_name;                             //协程调度器名称
    fiber::Fiber::ptr m_root_fiber;                 //use_caller为true有效，调度协程
};

} // Scheduler


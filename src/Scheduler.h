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
N-M协程调度器的基本思路是将协程分配给多个线程执行，每个线程都有自己的协程队列和空闲的线程栈。当一个线程中的所有协程都处于暂停状态时，
 该线程就会去获取其他线程的空闲协程来执行，从而实现了协程的并发执行。
N-M协程调度器的优点是可以充分利用多核处理器的性能，实现协程的并发执行，提高程序的运行效率。同时，由于协程的切换操作是由调度器控制的，
 因此可以避免线程切换时的开销，从而提高程序的响应速度。
 */

/**
 *@brief：Scheduler
 *@parma：mutexType m_mutex;
 *@parma：std::vector<thread::Thread::ptr> m_thread_pool;     线程对象列表
 *@parma：std::list<Task> m_task_queue;                       任务集合
 *@parma：std::string m_name;
 */

class Scheduler : boost::noncopyable {
public:
    typedef std::shared_ptr<Scheduler> ptr;
    typedef hyn::mutex::Mutex mutexType;

    /**
     *@brief：构造函数
     *@parma：创建的线程数
     *@parma：是否使用当前线程作为调度器的主线程
     *@parma：调度器的名称
     */
    explicit Scheduler(size_t thread = 1, bool use_caller = true, const std::string &name = "");

    virtual ~Scheduler();

    [[nodiscard]] const std::string &get_name() const { return m_name; }

    /**
     *@brife：启动调度器
     */
    void start();

    /**
     *@brief：停止协调调度器
     */
    void stop();

    /**
     *@brief：添加任务
     *@param：fc：可执行函数
     *@param：thread：绑定的线程id，默认-1
     */
    template<typename fiber_or_callback>
    void schedule(fiber_or_callback fc, int thread = -1) {
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
                ///全部添加完成之后是否需要tickle
                need_tickle = scheduleNoLock(&(*begin), -1) || need_tickle;
                ++begin;
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
     *@作用：判断是否已经停止
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

    bool has_idle_thread() {
        return m_idle_thread_count > 0;
    }

private:
    /**
     *@作用：添加任务（无锁）
     *@参数：fc：可执行实例
     *@参数：thread：要绑定id
     *@返回值：是否需要唤醒调度器
     */
    template<typename fiber_or_callback>
    bool scheduleNoLock(fiber_or_callback fc, int thread) {
        bool need_tickle = m_task_queue.empty();
        Task task(fc, thread);
        if (task.fiber || task.cb)
            m_task_queue.push_back(task);
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
    ///协程id数组
    std::vector<int> m_thread_id_vector;
    ///线程数
    size_t m_thread_count{0};
    ///工作线程数
    std::atomic<size_t> m_active_thread_count{0};
    ///空闲线程数
    std::atomic<size_t> m_idle_thread_count{0};
    ///是否处于停止状态
    bool m_stopping{true};
    ///是否自动停止
    bool m_auto_stop{false};
    ///主线程id
    int m_root_thread_id{0};
private:
    mutexType m_mutex;
    ///线程池
    std::vector<thread::Thread::ptr> m_thread_pool;
    ///待执行的协程队列
    std::list<Task> m_task_queue;
    ///协程调度器名称
    std::string m_name;
public:
    [[nodiscard]] const std::string &getMName() const;

private:
    ///use_caller为true有效，调度协程
    fiber::Fiber::ptr m_root_fiber;
};

} // Scheduler


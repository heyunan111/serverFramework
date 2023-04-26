/**
  ******************************************************************************
  * @file           : fiber.h
  * @author         : hyn
  * @brief          : None
  * @attention      : None
  * @date           : 2023/4/1
  ******************************************************************************
  */
#pragma once

#include <ucontext.h>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <memory>
#include <functional>

namespace hyn::fiber {

/**
uc_link主要指向的是当前上下文返回之后指向的上下文
uc_mcontext主要是保存一些寄存器
uc_sigmask主要保存阻塞信号量集
uc_stack主要是保存堆栈信息

getcontext : 将当前进程的上下文保存到ucp指向的数据结构里，为了后续的setcontext 调用
makecontext : 用以自己构造上下文，主要是需要引入自己的函数时调用（作用可能和pthread_create比较类似）。手册上说ucp所指向的结构之前必须
 需要被getcontext初始化，并且需要已经分配的栈空间。
setcontext : 用来保存当前进程的context。

如果ucp是由getcontext得到的话，那么setcontext的执行序列就如同调用getcontext之前一样。
如果ucp是由makecontext得到的话，那么当这个函数返回时，如果ucp指向的uc_link为空的话，进程退出；否则的话就直接指向ucp指向的上下文

swapconext : 相当于先调用getcontext到oucp，然后调用setconext将ucp所指向的上下文进行装载
*/



/**
*@brief：Fiber : public std::enable_shared_from_this<Fiber>
*@param：
*/
class Fiber : public std::enable_shared_from_this<Fiber> {
public:
    typedef std::shared_ptr<Fiber> ptr;
    //协程状态
    enum State {
        /// 初始化状态
        INIT,
        /// 暂停状态
        HOLD,
        /// 执行中状态
        EXEC,
        /// 结束状态
        TERM,
        /// 可执行状态
        READY,
        /// 异常状态
        EXCEPT
    };
private:
    /**
     *@brief：每个线程第一个协程的构造函数
     *@param：null
     *@return：null
     */
    Fiber();

public:

    /**
     *@brief：构造函数
     *@param：协程执行的函数
     *@param：协程栈的大小
     *@param：是否在MainFiber上调度
     */
    explicit Fiber(std::function<void()> cb, size_t stacksize = 0, bool use_caller = false);

    ~Fiber();

    /**
     *@brief：重置协程函数，并重置状态
     */
    void reset(const std::function<void()> &cb);

    /**
     *@brief：切换到当前协程
     */
    void swapIn();

    /**
     *@brief：切换到后台执行
     */
    void swapOut();

    /**
     *@brief：获取协程id
     */
    uint64_t getId() const {
        return m_id;
    }

    /**
     *@brief：获取协程状态
     */
    State getState() const {
        return m_state;
    }

    /**
     *@brief：换入协程。将调用时的上下挂起保存到线程局部变量中
     */
    void call();

    /**
     *@brief：挂起协程，保存当前上下文到协程对象中，从线程局部变量恢复执行上下文
     */
    void back();

public:
    /**
     *@brief：设置当前协程
     */
    static void SetThis(Fiber *f);

    /**
     *@brief：返回当前协程
     *@return：std::shared_ptr<Fiber>
     */
    static Fiber::ptr GetThis();

    /**
     *@brief：切换到后台，并设为ready
     */
    static void YieldToReady();

    /**
     *@brief：切换到后台并设置为hold
     */
    static void YieldToHold();

    /**
     *@brief：总协程数量
     */
    static uint64_t TotalFibers();

    /**
     *@brief：协程执行函数，执行完成返回到线程调度协程
     */
    static void MainFunc();

    /**
     *@brief：协程执行函数，执行完成返回到线程调度协程
     */
    static void CallerMainFunc();

    /**
     *@brief：获取协程id
     */
    static uint64_t GetFiberId();

    /**
     *@brief：设置state
     */
    void set_m_state(State mState);

private:
    ///id
    uint64_t m_id = 0;
    ///协程运行栈大小
    uint32_t m_stacksize = 0;
    /// 协程状态
    State m_state = INIT;
private:
    /// 协程上下文
    ucontext_t m_ctx{};
    /// 协程运行栈指针
    void *m_stack = nullptr;
    /// 协程运行函数
    std::function<void()> m_cb;
};
}
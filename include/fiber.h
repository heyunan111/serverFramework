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

/*
uc_link主要指向的是当前上下文返回之后指向的上下文
uc_mcontext主要是保存一些寄存器
uc_sigmask主要保存阻塞信号量集
uc_stack主要是保存堆栈信息

getcontext : 将当前进程的上下文保存到ucp指向的数据结构里，为了后续的setcontext 调用
makecontext : 用以自己构造上下文，主要是需要引入自己的函数时调用（作用可能和pthread_create比较类似）。手册上说ucp所指向的结构之前必须需要被getcontext初始化，并且需要已经分配的栈空间。
setcontext : 用来保存当前进程的context。

如果ucp是由getcontext得到的话，那么setcontext的执行序列就如同调用getcontext之前一样。
如果ucp是由makecontext得到的话，那么当这个函数返回时，如果ucp指向的uc_link为空的话，进程退出；否则的话就直接指向ucp指向的上下文

swapconext : 相当于先调用getcontext到oucp，然后调用setconext将ucp所指向的上下文进行装载
*/



/**
*@类名：Fiber : public std::enable_shared_from_this<Fiber>
*@参数：
*/
class Fiber : public std::enable_shared_from_this<Fiber> {
public:
    typedef std::shared_ptr<Fiber> ptr;
    //协程状态
    enum State {
        INIT,       // 初始化状态
        HOLD,       // 暂停状态
        EXEC,       // 执行中状态
        TERM,       // 结束状态
        READY,      // 可执行状态
        EXCEPT      // 异常状态
    };
public:
    Fiber();

    Fiber(std::function<void()> cb, size_t stacksize = 0);

    ~Fiber();

    /*
     *@作用：重置协程函数，并重置状态
     *@参数：null
     *@返回值：null
     */
    void reset(const std::function<void()> &cb);

    /*
     *@作用：切换到当前协程
     *@参数：null
     *@返回值：null
     */
    void swap_in();

    /*
     *@作用：切换到后台执行
     *@参数：null
     *@返回值：null
     */
    void swap_out();

    /*
     *@作用：获取协程id
     *@参数：null
     *@返回值：id
     */
    uint64_t get_id() const {
        return m_id;
    }

    /*
     *@作用：获取协程状态
     *@参数：null
     *@返回值：状态
     */
    State get_state() const {
        return m_state;
    }

public:
    /*
     *@作用：设置当前协程
     *@参数：null
     *@返回值：null
     */
    static void SetThis(Fiber *f);

    /*
     *@作用：返回当前协程
     *@参数：null
     *@返回值：std::shared_ptr<Fiber>
     */
    static Fiber::ptr GetThis();

    /*
     *@作用：切换到后台，并设为ready
     *@参数：null
     *@返回值：null
     */
    static void YieldToReady();

    /*
     *@作用：切换到后台并设置为hold
     *@参数：null
     *@返回值：null
     */
    static void YieldToHold();

    /*
     *@作用：总协程数量
     *@参数：null
     *@返回值：null
     */
    static uint64_t TotalFibers();

    /*
     *@作用：协程执行函数，执行完成返回到线程调度协程
     *@参数：null
     *@返回值：null
     */
    static void MainFunc();

    /*
     *@作用：获取协程id
     *@参数：null
     *@返回值：id
     */
    static uint64_t GetFiberId();

private:
    //id
    uint64_t m_id = 0;
    //协程运行栈大小
    uint32_t m_stacksize = 0;
    /// 协程状态
    State m_state = INIT;
    // 协程上下文
    ucontext_t m_ctx;
    // 协程运行栈指针
    void *m_stack = nullptr;
    // 协程运行函数
    std::function<void()> m_cb;

};
}
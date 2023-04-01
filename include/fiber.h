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
*@类名：Fiber : public std::enable_shared_from_this<Fiber>
*@参数：
*/
class Fiber : public std::enable_shared_from_this<Fiber> {
public:
    typedef std::shared_ptr<Fiber> ptr;
    //协程状态
    enum State {
        INIT, //初始化
        HOLD, //暂停
        EXEC, //执行中
        TERM, //结束
        READY //异常
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
    void reset(std::function<void()> cb);

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
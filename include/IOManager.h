/**
  ******************************************************************************
  * @file           : IOManager.h
  * @author         : hyn
  * @brief          : None
  * @attention      : None
  * @date           : 2023/4/3
  ******************************************************************************
  */
#pragma once

#include "Scheduler.h"

namespace hyn::iomanager {

class IOManager : public scheduler::Scheduler {
public:
    typedef std::shared_ptr<IOManager> ptr;
    typedef mutex::RWMutex RWMutexType;

    enum Event {
        ///无事件
        NONE = 0x0,
        ///读事件
        READ = 0x1,
        ///写事件
        WRITE = 0x4
    };

private:
    struct FdContext {
        typedef mutex::Mutex MutexType;
        struct EventContext {
            ///待执行scheduler
            scheduler::Scheduler *scheduler;
            ///事件协程
            fiber::Fiber::ptr fiber;
            ///事件回调函数
            std::function<void()> cb;
        };
        //事件关联句柄
        int m_fd;
        ///读事件
        EventContext read;
        ///写事件
        EventContext write;
        ///已经注册的事件
        Event m_event = NONE;
        MutexType mutex;
    };

public:
    /**
     *@作用：构造
     *@参数：线程数默认 1
     *@参数：是否将调用线程包含进去
     *@参数：调度器的名称
     *@返回值：null
     */
    IOManager(size_t thread = 1, bool use_call = true, const std::string &name = "");

    ~IOManager();

    /**
     *@作用：添加事件
     *@参数：socket句柄
     *@参数：事件类型
     *@参数：时间回调函数
     *@返回值：添加成功放回0，失败-1
     */
    int addEvent(int fd, Event event, std::function<void()> cb = nullptr);

    /**
     *@作用：删除事件
     *@参数：socket句柄
     *@参数：事件类型
     *@返回值：是否成功
     */
    bool delEvent(int fd, Event event);

    /**
     *@作用：取消事件
     *@参数：socket句柄
     *@参数：事件类型
     *@返回值：是否成功
     */
    bool cancelEvent(int fd, Event event);

    /**
     *@作用：全部取消
     *@参数：句柄
     *@返回值：是否成功
     */
    bool cancelAll(int fd);

    /**
     *@作用：返回当前的IOManager
     *@参数：null
     *@返回值：null
     */
    static IOManager *GetThis();

protected:
    void tickle() override;

    bool stopping() override;

    void idle() override;

private:
    ///epoll句柄
    int m_epfd{0};
    ///pipe句柄
    int m_tickleFds{2};
    ///当前等待执行的事件数量
    std::atomic<size_t> m_pendingEventCount{0};
    ///IOManger的mutex
    RWMutexType m_rw_mutex;
    ///socket事件上下文的容器
    std::vector<FdContext *> m_fdContexts;
};

} /// iomanager












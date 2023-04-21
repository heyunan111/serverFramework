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
#include "Timer.h"
namespace hyn::iomanager {

/**
 *@brief 这是一个基于 Epoll 的 IO 协程调度器实现。
 *@param 添加、删除、取消事件，并能触发事件的回调函数；
 *@param 支持读事件和写事件；
 *@param 取消所有事件；
 *@param 多线程并发执行；
 */
class IOManager : public scheduler::Scheduler, public TimerManager {
public:
    typedef std::shared_ptr<IOManager> ptr;
    typedef mutex::RWMutex RWMutexType;

    /**
     *@brief IO事件
     */
    enum Event {
        ///无事件
        NONE = 0x0,
        ///读事件
        READ = 0x00000001,      //EPOLLIN
        ///写事件
        WRITE = 0x00000004      //EPOLLOUT
    };

private:
    /**
     * @brife 存储了一个文件句柄的事件上下文，其中包括读和写两个事件的协程、调度器和回调函数。还包括事件关联的句柄、当前的事件和一个互斥锁。
     */
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

        /**
         *@brief：获取事件上下文
         *@parma：事件类型
         *@return：事件上下文
         */
        EventContext &get_context(Event event);

        /**
         *@brief：重置事件上下文
         *@parma：待重置的上下文类
         */
        void ResetContext(EventContext &ctx);

        /**
         *@brief：触发事件
         *@parma：事件类型
         */
        void triggerEvent(Event event);

        ///事件关联句柄
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
     *@brief：构造
     *@parma：线程数默认 1
     *@parma：是否将调用线程包含进去
     *@parma：调度器的名称
     */
    explicit IOManager(size_t thread = 1, bool use_call = true, const std::string &name = "");

    ~IOManager() override;

    /**
     *@brief：添加事件
     *@parma：socket句柄
     *@parma：事件类型
     *@parma：事件回调函数
     *@return：添加成功放回0，失败-1
     */
    int addEvent(int fd, Event event, std::function<void()> cb = nullptr);

    /**
     *@brief：删除事件
     *@parma：socket句柄
     *@parma：事件类型
     *@return：是否成功
     */
    bool delEvent(int fd, Event event);

    /**
     *@brief：取消事件,强制执行
     *@parma：socket句柄
     *@parma：事件类型
     *@return：是否成功
     */
    bool cancelEvent(int fd, Event event);

    /**
     *@brief：全部取消
     *@parma：句柄
     *@return：是否成功
     */
    bool cancelAll(int fd);

    /**
     *@brief：返回当前的IOManager
     */
    static IOManager *GetThis();

    bool stopping(uint64_t &timeout);

protected:
    void tickle() override;

    bool stopping() override;

    void idle() override;

    void onTimerInsertedAtFront() override;

    /**
     *@brief：重置socket上下文句柄容器大小
     *@param：大小
     *@return：null
     */
    void contextResize(size_t size);

private:
    ///epoll句柄
    int m_epfd;
    ///pipe句柄
    int m_tickleFds[2]{};
    ///当前等待执行的事件数量
    std::atomic<size_t> m_pendingEventCount{0};
    ///IOManger的mutex
    RWMutexType m_rw_mutex;
    ///socket事件上下文的容器
    std::vector<FdContext *> m_fdContexts_vertor;
};

} /// iomanager












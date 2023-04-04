/**
  ******************************************************************************
  * @file           : IOManager.cpp
  * @author         : hyn
  * @brief          : None
  * @attention      : None
  * @date           : 2023/4/3
  ******************************************************************************
  */


#include "../include/Logger.h"
#include "../include/IOManager.h"
#include <sys/epoll.h>
#include <cstring>
#include <fcntl.h>
#include <unistd.h>

namespace hyn::iomanager {
/*
管道是一个字节流的传输通道，它具有以下特点：
只能在具有亲缘关系的进程之间使用（如父子进程）。
管道是半双工的，即只能单向传输数据，一端只能写，另一端只能读。
管道中的数据是先进先出（FIFO）的。
在使用管道时，我们通常会使用pipe系统调用创建一个管道，这个系统调用会返回两个文件描述符，分别表示管道的读端和写端。在父进程中，我们通常会
 关闭管道的读端，只保留写端。在子进程中，我们通常会关闭管道的写端，只保留读端。这样，父进程就可以通过管道向子进程传递数据，而子进程则可以
 通过管道接收数据。
在Linux系统中，管道是一种非常常用的进程间通信机制，它不仅可以用于在同一台机器上的进程间通信，也可以用于不同机器之间的进程间通信，比如通过
 网络套接字实现的管道。管道不仅可以传输普通的文本数据，也可以传输二进制数据和文件等多种类型的数据。
 * */
IOManager::IOManager(size_t thread, bool use_call, const std::string &name) : Scheduler(thread, use_call, name) {
    m_epfd = epoll_create(11451);
    THROW_RUNTIME_ERROR_IF(m_epfd < 0, "epoll_create error");
    int rt = pipe(m_tickleFds);                 //创建管道
    THROW_RUNTIME_ERROR_IF(rt != 0, "pipe error");
    epoll_event ev{};
    memset(&ev, 0, sizeof(epoll_event));
    ev.events = EPOLLIN | EPOLLET;
    ev.data.fd = m_tickleFds[0];
    rt = fcntl(m_tickleFds[0], F_SETFL, O_NONBLOCK); //非阻塞IO
    assert(!rt);
    rt = epoll_ctl(m_epfd, EPOLL_CTL_ADD, m_tickleFds[0], &ev);
    assert(!rt);
    contextResize(32);
    start();
}

IOManager::~IOManager() {
    stop();
    close(m_epfd);
    close(m_tickleFds[0]);
    close(m_tickleFds[1]);
    for (auto &m_fdContext: m_fdContexts) {
        delete m_fdContext;
    }
}

//首先通过文件描述符fd找到对应的FdContext，如果找到则加锁并将fd对应的事件添加到epoll事件循环中，如果未找到则先扩展
//FdContext数组，再加锁后添加事件到epoll事件循环中。如果添加事件成功，将事件与回调函数绑定，以便在事件发生时回调。
int IOManager::addEvent(int fd, IOManager::Event event, std::function<void()> cb) {
    FdContext *fdContext;
    RWMutexType::ReadLock lock(m_rw_mutex);

    //通过文件描述符fd找到对应的FdContext，如果未找到则先扩展FdContext数组
    if (static_cast<int>(m_fdContexts.size()) > fd) {
        fdContext = m_fdContexts[fd];
        lock.unlock();
    } else {
        lock.unlock();
        RWMutexType::WriteLock writeLock(m_rw_mutex);
        contextResize(fd * 1.5);
        fdContext = m_fdContexts[fd];
    }

    //锁定FdContext并检查要添加的事件是否已经存在。
    FdContext::MutexType::Lock lock1(fdContext->mutex);

    if (fdContext->m_event & event) {
        error("addEvent error fd :%d", fd);
        assert(!(fdContext->m_event & event));
    }

    //添加事件到epoll事件循环中。
    int op = fdContext->m_event ? EPOLL_CTL_MOD : EPOLL_CTL_ADD;
    epoll_event epollEvent{};
    epollEvent.events = EPOLLET | fdContext->m_event | event;
    epollEvent.data.ptr = fdContext;
    int rt = epoll_ctl(m_epfd, op, fd, &epollEvent);

    if (rt) {
        error("epoll_ctl :%d,op:%d,fd:%d", m_epfd, op, fd);
        return -1;
    }

    //绑定事件与回调函数。
    ++m_pendingEventCount;
    fdContext->m_event = (Event) (fdContext->m_event | event);
    FdContext::EventContext &event_ctx = fdContext->get_context(event);
    assert(!event_ctx.scheduler && !event_ctx.fiber && !event_ctx.cb);
    event_ctx.scheduler = scheduler::Scheduler::GetThis();

    if (cb) {
        event_ctx.cb.swap(cb);
    } else {
        event_ctx.fiber = fiber::Fiber::GetThis();
        assert(event_ctx.fiber->get_state() == fiber::Fiber::EXEC);
    }

    return 0;
}

bool IOManager::delEvent(int fd, IOManager::Event event) {
    //判断fd是否存在
    RWMutexType::ReadLock lock(m_rw_mutex);
    if ((int) m_fdContexts.size() <= fd)
        return false;
    FdContext *fdContext = m_fdContexts[fd];
    lock.unlock();

    //判断event是否存在
    FdContext::MutexType::Lock lock1(fdContext->mutex);
    if (!(fdContext->m_event & event))
        return false;

    //构造一个新的事件列表 new_events，删除要删除的事件 event。
    //如果新的事件列表 new_events 不为空，就调用 epoll_ctl 修改事件列表为新的列表；否则，就调用 epoll_ctl 删除事件。
    auto new_event = (Event) (fdContext->m_event & ~event);
    int op = new_event ? EPOLL_CTL_MOD : EPOLL_CTL_DEL;
    

}

bool IOManager::cancelEvent(int fd, IOManager::Event event) {
    return false;
}

bool IOManager::cancelAll(int fd) {
    return false;
}

IOManager *IOManager::GetThis() {
    return nullptr;
}

void IOManager::tickle() {
    Scheduler::tickle();
}

bool IOManager::stopping() {
    return Scheduler::stopping();
}

void IOManager::idle() {
    Scheduler::idle();
}

void IOManager::contextResize(size_t size) {
    m_fdContexts.resize(size);
    for (int i = 0; i < size; ++i) {
        if (!m_fdContexts[i]) {
            m_fdContexts[i] = new FdContext;
            m_fdContexts[i]->m_fd = i;
        }
    }
}

IOManager::FdContext::EventContext &IOManager::FdContext::get_context(IOManager::Event event) {
    switch (event) {
        case READ:
            return read;
        case WRITE:
            return write;
        default:
            THROW_RUNTIME_ERROR_IF(1, "get context error");
    }
}

void IOManager::FdContext::ResetContext(IOManager::FdContext::EventContext &ctx) {
    ctx.fiber.reset();
    ctx.cb = nullptr;
    ctx.scheduler = nullptr;
}

void IOManager::FdContext::triggerEvent(IOManager::Event event) {
    assert(m_event & event); //当前事件是否已经注册
    m_event = (Event) (m_event & ~event);
    EventContext &ctx = get_context(event);

    if (ctx.cb) {
        ctx.scheduler->schedule(&ctx.cb);
    } else {
        ctx.scheduler->schedule(&ctx.fiber);
    }

    ctx.scheduler = nullptr;
}
} // iomanager
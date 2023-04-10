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
/**
管道是一个字节流的传输通道，它具有以下特点：
只能在具有亲缘关系的进程之间使用（如父子进程）。
管道是半双工的，即只能单向传输数据，一端只能写，另一端只能读。
管道中的数据是先进先出（FIFO）的。
在使用管道时，我们通常会使用pipe系统调用创建一个管道，这个系统调用会返回两个文件描述符，分别表示管道的读端和写端。在父进程中，我们通常会
 关闭管道的读端，只保留写端。在子进程中，我们通常会关闭管道的写端，只保留读端。这样，父进程就可以通过管道向子进程传递数据，而子进程则可以
 通过管道接收数据。
在Linux系统中，管道是一种非常常用的进程间通信机制，它不仅可以用于在同一台机器上的进程间通信，也可以用于不同机器之间的进程间通信，比如通过
 网络套接字实现的管道。管道不仅可以传输普通的文本数据，也可以传输二进制数据和文件等多种类型的数据。
 */
/**
 *pipe[0] 读 pipe[1]写
 */
IOManager::IOManager(size_t thread, bool use_call, const std::string &name) : Scheduler(thread, use_call, name) {
    int epfd = epoll_create(5000);
    THROW_RUNTIME_ERROR_IF(epfd < 0, "epoll create error");

    int rt = pipe(m_tickleFds);
    THROW_RUNTIME_ERROR_IF(rt, "pipe error");

    epoll_event ep_event{};
    ep_event.events = EPOLLIN | EPOLLET;
    ep_event.data.fd = m_tickleFds[0];

    rt = fcntl(m_tickleFds[0], F_SETFL, O_NONBLOCK);//设置非阻塞
    THROW_RUNTIME_ERROR_IF(rt, "fcntl error");

    rt = epoll_ctl(epfd, EPOLL_CTL_ADD, m_tickleFds[0], &ep_event);
    THROW_RUNTIME_ERROR_IF(rt, "epoll ctl error");

    contextResize(32);
    start();
}

IOManager::~IOManager() {
    stop();
    close(m_epfd);
    close(m_tickleFds[0]);
    close(m_tickleFds[1]);
    for (auto &m_fdContext: m_fdContexts_vertor) {
        delete m_fdContext;
    }
}

//首先通过文件描述符fd找到对应的FdContext，如果找到则加锁并将fd对应的事件添加到epoll事件循环中，如果未找到则先扩展
//FdContext数组，再加锁后添加事件到epoll事件循环中。如果添加事件成功，将事件与回调函数绑定，以便在事件发生时回调。
int IOManager::addEvent(int fd, IOManager::Event event, std::function<void()> cb) {
    FdContext *fdContext = nullptr;
    RWMutexType::ReadLock lock(m_rw_mutex);

    //通过文件描述符fd找到对应的FdContext，如果未找到则先扩展FdContext数组
    if (static_cast<int>(m_fdContexts_vertor.size()) > fd) {
        fdContext = m_fdContexts_vertor[fd];
        lock.unlock();
    } else {
        lock.unlock();
        RWMutexType::WriteLock writeLock(m_rw_mutex);
        contextResize(fd * 2);
        fdContext = m_fdContexts_vertor[fd];
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
    assert(event_ctx.scheduler == nullptr && !event_ctx.fiber && !event_ctx.cb);
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
    if ((int) m_fdContexts_vertor.size() <= fd)
        return false;
    FdContext *fdContext = m_fdContexts_vertor[fd];
    lock.unlock();

    //判断event是否存在
    FdContext::MutexType::Lock lock1(fdContext->mutex);
    if (!(fdContext->m_event & event))
        return false;

    //构造一个新的事件列表 new_events，删除要删除的事件 event。
    //如果新的事件列表 new_events 不为空，就调用 epoll_ctl 修改事件列表为新的列表；否则，就调用 epoll_ctl 删除事件。
    auto new_event = (Event) (fdContext->m_event & ~event);
    int op = new_event ? EPOLL_CTL_MOD : EPOLL_CTL_DEL;
    epoll_event ep_event{};
    ep_event.events = EPOLLET | new_event;
    ep_event.data.ptr = &fdContext;
    int rt = epoll_ctl(m_epfd, op, fd, &ep_event);

    if (rt) {
        error("epoll_ctl :%d,op:%d,fd:%d", m_epfd, op, fd);
        return false;
    }

    --m_pendingEventCount;
    fdContext->m_event = new_event;
    FdContext::EventContext &eventContext = fdContext->get_context(event);
    fdContext->ResetContext(eventContext);
    return true;
}

bool IOManager::cancelEvent(int fd, IOManager::Event event) {
    //判断fd是否存在
    RWMutexType::ReadLock lock(m_rw_mutex);
    if ((int) m_fdContexts_vertor.size() <= fd)
        return false;
    FdContext *fdContext = m_fdContexts_vertor[fd];
    lock.unlock();

    //判断event是否存在
    FdContext::MutexType::Lock lock1(fdContext->mutex);
    if (!(fdContext->m_event & event))
        return false;

    //构造一个新的事件列表 new_events，删除要删除的事件 event。
    //如果新的事件列表 new_events 不为空，就调用 epoll_ctl 修改事件列表为新的列表；否则，就调用 epoll_ctl 删除事件。
    auto new_event = (Event) (fdContext->m_event & ~event);
    int op = new_event ? EPOLL_CTL_MOD : EPOLL_CTL_DEL;
    epoll_event ep_event{};
    ep_event.events = EPOLLET | new_event;
    ep_event.data.ptr = &fdContext;
    int rt = epoll_ctl(m_epfd, op, fd, &ep_event);

    if (rt) {
        error("epoll_ctl :%d,op:%d,fd:%d", m_epfd, op, fd);
        return false;
    }

    ///仅仅从这开始与delEvent不同
    fdContext->triggerEvent(event);
    --m_pendingEventCount;
    return true;
}

bool IOManager::cancelAll(int fd) {
//判断fd是否存在
    RWMutexType::ReadLock lock(m_rw_mutex);
    if ((int) m_fdContexts_vertor.size() <= fd)
        return false;
    FdContext *fdContext = m_fdContexts_vertor[fd];
    lock.unlock();

    //判断event是否存在
    FdContext::MutexType::Lock lock1(fdContext->mutex);
    if (!fdContext->m_event)
        return false;

    //构造一个新的事件列表 new_events，删除要删除的事件 event。
    //如果新的事件列表 new_events 不为空，就调用 epoll_ctl 修改事件列表为新的列表；否则，就调用 epoll_ctl 删除
    epoll_event ep_event{};
    ep_event.events = 0;
    ep_event.data.ptr = &fdContext;
    int rt = epoll_ctl(m_epfd, EPOLL_CTL_DEL, fd, &ep_event);

    if (rt) {
        error("epoll_ctl :%d,op:%d,fd:%d", m_epfd, EPOLL_CTL_DEL, fd);
        return false;
    }

    if (fdContext->m_event & READ) {
        fdContext->triggerEvent(READ);
        --m_pendingEventCount;
    }

    if (fdContext->m_event & WRITE) {
        fdContext->triggerEvent(WRITE);
        --m_pendingEventCount;
    }

    return true;
}

IOManager *IOManager::GetThis() {
    return dynamic_cast<IOManager *>(scheduler::Scheduler::GetThis());
}

void IOManager::tickle() {
    if (!has_idle_thread()) {
        return;
    }
    auto rt = write(m_tickleFds[1], "T", 1);
    assert(rt == 1);
}

bool IOManager::stopping(uint64_t &timeout) {
    timeout = getNextTimer();
    return timeout == ~0ull
           && m_pendingEventCount == 0
           && Scheduler::stopping();

}

bool IOManager::stopping() {
    uint64_t timeout = 0;
    return stopping(timeout);
}

void IOManager::idle() {
    const uint64_t MAX_EVENTS = 256;
    auto *events = new epoll_event[MAX_EVENTS]();
    std::shared_ptr<epoll_event> shared_events(events, [](epoll_event *ptr) {
        delete[] ptr;
    });


    for (;;) {
        uint64_t next_timeout = 0;
        if (stopping(next_timeout)) {
            info("name = %s idle stopping exit", getMName().c_str());
            break;
        }
        int rt;
        do {
            static const int MAX_TIMEOUT = 3000;
            if (next_timeout != ~0ul) {
                next_timeout = (int) next_timeout > MAX_TIMEOUT ? MAX_TIMEOUT : next_timeout;
            } else {
                next_timeout = MAX_TIMEOUT;
            }
            rt = epoll_wait(m_epfd, events, MAX_EVENTS, (int) next_timeout);
            if (!(rt < 0 && errno == EINTR)) {
                break;
            }
        } while (true);

        std::vector<std::function<void()> > cbs;
        listExpiredCb(cbs);
        if (!cbs.empty()) {
            schedule(cbs.begin(), cbs.end());
            cbs.clear();
        }

        for (int i = 0; i < rt; ++i) {
            epoll_event &event = events[i];

            if (event.data.fd == m_tickleFds[0]) {
                uint8_t dummy[256];
                while (read(m_tickleFds[0], dummy, sizeof(dummy)) > 0);
                continue;
            }

            auto *fd_ctx = static_cast<FdContext *>(event.data.ptr);
            FdContext::MutexType::Lock lock(fd_ctx->mutex);

            if (event.events & (EPOLLERR | EPOLLHUP)) {
                event.events |= (EPOLLIN | EPOLLOUT) & fd_ctx->m_event;
            }

            int real_events = NONE;

            if (event.events & EPOLLIN) {
                real_events |= READ;
            }

            if (event.events & EPOLLOUT) {
                real_events |= WRITE;
            }

            if ((fd_ctx->m_event & real_events) == NONE)
                continue;

            int left_events = (fd_ctx->m_event & ~real_events);//剩余事件
            int op = left_events ? EPOLL_CTL_MOD : EPOLL_CTL_DEL;
            event.events = EPOLLET | left_events;

            int rt2 = epoll_ctl(m_epfd, op, fd_ctx->m_fd, &event);
            if (rt2) {
                error("epoll_ctl :%d,op:%d,fd:%d", m_epfd, op, fd_ctx->m_fd);
                continue;
            }

            if (real_events & READ) {
                fd_ctx->triggerEvent(READ);
                --m_pendingEventCount;
            }

            if (real_events & WRITE) {
                fd_ctx->triggerEvent(WRITE);
                --m_pendingEventCount;
            }
        }
        fiber::Fiber::ptr cur = fiber::Fiber::GetThis();
        auto raw_ptr = cur.get();
        cur.reset();
        raw_ptr->swap_out();
    }
}

void IOManager::contextResize(size_t size) {
    m_fdContexts_vertor.resize(size);
    for (int i = 0; i < size; ++i) {
        if (!m_fdContexts_vertor[i]) {
            m_fdContexts_vertor[i] = new FdContext;
            m_fdContexts_vertor[i]->m_fd = i;
        }
    }
}

void IOManager::onTimerInsertedAtFront() {
    tickle();
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
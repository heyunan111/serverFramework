/**
  ******************************************************************************
  * @file           : Hook.cpp
  * @author         : hyn
  * @brief          : None
  * @attention      : None
  * @date           : 2023/4/10
  ******************************************************************************
  */
#include "../include/Hook.h"
#include "../include/fiber.h"
#include "../include/IOManager.h"
#include "../include/FDManger.h"
#include "../include/Logger.h"
#include <dlfcn.h>


namespace hyn {

static thread_local bool s_hook_enable = false;

#define HOOK_FUN(XX) \
    XX(sleep)        \
    XX(usleep)       \
    XX(nanosleep)    \
    XX(socket)       \
    XX(connect)      \
    XX(accept)       \
    XX(read)         \
    XX(readv)        \
    XX(recv)         \
    XX(recvfrom)     \
    XX(recvmsg)      \
    XX(write)        \
    XX(writev)       \
    XX(send)         \
    XX(sendto)       \
    XX(sendmsg)      \
    XX(close)        \
    XX(fcntl)        \
    XX(ioctl)        \
    XX(getsockopt)   \
    XX(setsockopt)

struct time_info {
    int cancelled = 0;
};

/**
 *@作用：在进行非阻塞 IO 操作时，添加超时和事件通知等功能
 *@参数：fd
 *@参数：函数指针，指向原函数
 *@参数：被hook的函数名称
 *@参数：事件类型
 *@参数：超时时间类型(读还是写)
 *@参数：可变参数：函数参数
 *@返回值：操作的字节数，如果发生错误则返回-1。
 */
template<typename OriginFun, typename ...Args>
static ssize_t
do_io(int fd, OriginFun func, char *hook_fun_name, iomanager::IOManager::Event event, int timeout_so, Args ...args) {
    //首先判断是否开启了 hook，如果未开启则直接调用原始函数。
    if (!s_hook_enable)
        return func(fd, std::forward<Args>(args)...);

    //如果开启了 hook，则通过文件描述符从 FdMgr 中获取文件描述符上下文 FdCtx，如果不存在则直接调用原始函数。
    FdCtx::ptr fd_ctx = FdMgr::GetInstance()->get(fd);
    if (!fd_ctx)
        return func(fd, std::forward<Args>(args)...);

    //如果该文件描述符已经被关闭，则返回 EBADF 错误。
    if (fd_ctx->is_close()) {
        errno = EBADF;
        return -1;
    }

    //如果该文件描述符不是一个 socket，或者该文件描述符已经设置为用户自定义非阻塞，则直接调用原始函数。
    if (!(fd_ctx->is_socket()) || fd_ctx->is_usr_nonblock())
        return func(fd, std::forward<Args>(args)...);

    uint64_t timeout = fd_ctx->get_time(timeout_so);
    auto tinfo = std::make_shared<time_info>();

    retry:

    //使用 fun 执行一次 IO 操作，并检查返回值。如果返回值为 -1，且错误码为 EINTR，则说明 IO 操作被中断，需要再次执行 IO 操作。
    ssize_t n = func(fd, std::forward<Args>(args)...);
    while (n == -1 && errno == EINTR) {
        n = func(fd, std::forward<Args>(args)...);
    }
    //如果函数返回值为 -1 并且全局变量 errno 的值为 EAGAIN，则表示操作被阻塞，需要进行异步操作，否则直接返回函数返回值。
    if (n == -1 && errno == EAGAIN) {
        iomanager::IOManager *iom = iomanager::IOManager::GetThis();
        Timer::ptr timer;
        std::weak_ptr<time_info> weak_time_info(tinfo);


        //如果超时时间不为 -1，则使用 IO 线程添加一个条件定时器，到达超时时间时会取消当前事件的监听,强制执行，并将 timer_info 结构体的
        // cancelled 标志设置为 ETIMEDOUT。
        if (timeout != static_cast<uint64_t>(-1)) {
            timer = iom->addConditionTimer(timeout, [weak_time_info, fd, iom, event]() {
                auto t = weak_time_info.lock();
                if (t == nullptr || t->cancelled) {
                    return;
                }
                t->cancelled = ETIMEDOUT;
                iom->cancelEvent(fd, event);
            }, weak_time_info, false);
        }

        //将文件描述符 fd 加入到 IO 管理器的事件循环中，如果返回值不为 0，则表示出错，打印错误日志并返回 -1。否则，使用协程的
        // YieldToHold 函数将协程切换到后台运行
        int rt = iom->addEvent(fd, event);
        if (rt != 0) {
            error("%s addEvent error fd:%d", hook_fun_name, fd);
            if (timer)
                timer->cancel();
            return -1;
        } else {
            fiber::Fiber::YieldToHold();
            if (timer)
                timer->cancel();
            if (tinfo->cancelled) {
                errno = tinfo->cancelled;
                return -1;
            }
            goto retry;
        }
    }
    return n;
}

}//namespace hyn

extern "C" {
#define XX(name) name##_fun name##_f = nullptr;
HOOK_FUN(XX) ;
#undef XX
}

namespace hyn {

bool is_hook_enable() {
    return s_hook_enable;
}

void set_hook_enable(bool f) {
    s_hook_enable = f;
}

void hook_init() {
    static bool is_hook_init = false;
    if (is_hook_init)
        return;
    is_hook_init = true;
#define XX(name) name ## _f = (name ## _fun) dlsym (RTLD_NEXT, #name);
    HOOK_FUN(XX);
#undef XX
}

struct HookIniter {
    HookIniter() {
        hook_init();
    }
};

static HookIniter s_hook_init;

};

extern "C" {
unsigned int sleep(unsigned int seconds) {
    if (!hyn::s_hook_enable)
        return sleep_f(seconds);

    hyn::fiber::Fiber::ptr fiber = hyn::fiber::Fiber::GetThis();
    hyn::iomanager::IOManager *iom = hyn::iomanager::IOManager::GetThis();
    iom->addTimer(seconds * 1000, [fiber, iom]() { iom->schedule(fiber); });
    fiber->YieldToHold();
    return 0;
}

int usleep(useconds_t usec) {
    if (!hyn::s_hook_enable)
        return usleep_f(usec);

    hyn::fiber::Fiber::ptr fiber = hyn::fiber::Fiber::GetThis();
    hyn::iomanager::IOManager *iom = hyn::iomanager::IOManager::GetThis();
    iom->addTimer(usec / 1000, [fiber, iom]() { iom->schedule(fiber); });
    fiber->YieldToHold();
    return 0;
}

int nanosleep(const struct timespec *req, struct timespec *rem) {
    if (!hyn::s_hook_enable)
        return nanosleep_f(req, rem);

    uint64_t times = req->tv_sec * 1000 + req->tv_nsec / 1000 / 1000;
    hyn::fiber::Fiber::ptr fiber = hyn::fiber::Fiber::GetThis();
    hyn::iomanager::IOManager *iom = hyn::iomanager::IOManager::GetThis();
    iom->addTimer(times, [fiber, iom]() { iom->schedule(fiber); });
    fiber->YieldToHold();
    return 0;
}


}//extern "C"
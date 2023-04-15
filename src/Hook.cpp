/**
  ******************************************************************************
  * @file           : Hook.cpp
  * @author         : hyn
  * @brief          : None
  * @attention      : None
  * @date           : 2023/4/10
  ******************************************************************************
  */
#include "Hook.h"
#include "fiber.h"
#include "IOManager.h"
#include "FDManger.h"
#include "Logger.h"
#include "iniFile.h"

namespace hyn {

static uint64_t s_connect_timeout = -1;
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
do_io(int fd, OriginFun func, const char *hook_fun_name, iomanager::IOManager::Event event, int timeout_so,
      Args ...args) {
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

    ssize_t n = func(fd, std::forward<Args>(args)...);//如果n == 0，则读到数据了，可以直接返回

    while (n == -1 && errno == EINTR) {
        n = func(fd, std::forward<Args>(args)...);
    }
    //如果函数返回值为 -1 并且全局变量 errno 的值为 EAGAIN，则表示当前操作不能立即完成，需要等待一段时间后再次尝试。
    if (n == -1 && errno == EAGAIN) {
        //debug("do io");

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
HOOK_FUN(XX)
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
    HOOK_FUN(XX)
#undef XX
}

struct HookIniter {
    HookIniter() {
        ///FIXME :ini配置
        hook_init();
    }
};

static HookIniter s_hook_init;

} //namespace hyn

extern "C" {

//sleep
unsigned int sleep(unsigned int seconds) {
    if (!hyn::s_hook_enable) {
        return sleep_f(seconds);
    }

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

//socket
int socket(int domain, int type, int protocol) {
    if (!hyn::s_hook_enable)
        return socket_f(domain, type, protocol);
    int fd = socket_f(domain, type, protocol);
    if (fd == -1)
        return -1;
    hyn::FdMgr::GetInstance()->get(fd, true);
    return fd;
}

int connect_with_timeout(int fd, const struct sockaddr *addr, socklen_t addrlen, uint64_t timout_ms) {
    //首先判断hook是否启用，如果没有启用，则直接调用connect函数。
    if (!hyn::s_hook_enable)
        return connect_f(fd, addr, addrlen);

    //如果hook启用了，那么通过fd获取fd上下文，如果没有或者已经关闭则返回错误。
    auto ctx = hyn::FdMgr::GetInstance()->get(fd);
    if (!ctx || ctx->is_close()) {
        errno = EBADF;
        return -1;
    }

    //判断是否为socket以及是否为非阻塞模式，如果不是socket或者用户设置非阻塞，则直接调用connect函数。
    if (!ctx->is_socket() || ctx->is_usr_nonblock())
        return connect_f(fd, addr, addrlen);

    // 调用conn如果连接成功，则返回0；       EINPROGRESS是一个系统错误码，表示一个非阻塞的套接字正在进行连接操作，因此连接操作正在进行中（in progress），不是阻塞的。
    //如果连接出错且错误不是EINPROGRESS，则返回错误代码；
    int n = connect_f(fd, addr, addrlen);

    if (n == 0) {
        return 0;
    } else if (n != -1 || errno != EINPROGRESS) {
        return n;
    }
    //如果返回-1且错误代码是EINPROGRESS，则进行以下操作：
    //创建一个定时器。
    hyn::iomanager::IOManager *iom = hyn::iomanager::IOManager::GetThis();
    hyn::Timer::ptr timer;
    auto tinfo = std::make_shared<hyn::time_info>();
    std::weak_ptr<hyn::time_info> winfo(tinfo);

    //如果timeout_ms不为默认值（-1），则创建一个定时器。定时器回调函数会将超时标记设置为ETIMEDOUT，并取消等待事件的IO事件。
    if (timout_ms != static_cast<uint64_t>(-1)) {
        timer = iom->addConditionTimer(timout_ms, [winfo, fd, iom] {
            auto t = winfo.lock();
            if (!t || t->cancelled)
                return;
            t->cancelled = ETIMEDOUT;
            iom->cancelEvent(fd, hyn::iomanager::IOManager::WRITE);
        }, winfo, false);
    }

    //使用IOManager的addEvent函数向事件循环中添加WRITE事件。如果添加成功，则调用Fiber::YieldToHold函数切换到其他协程执行，
    // 等待IO事件被触发或者超时定时器超时。
    int rt = iom->addEvent(fd, hyn::iomanager::IOManager::WRITE);
    if (rt == 0) {
        hyn::fiber::Fiber::YieldToHold();
        if (timer)
            timer->cancel();
        if (tinfo) {
            errno = tinfo->cancelled;
            return -1;
        }
    } else {
        if (timer)
            timer->cancel();
        error("connect addEvent error fd = %d,WRITE", fd);
    }

    //当WRITE事件被触发或者定时器超时时，使用getsockopt函数获取套接字选项值。如果选项值为0，则表示连接成功，返回0；否则，
    // 将errno设置为获取的错误码，返回-1。
    int error{0};
    socklen_t len = sizeof(int);
    if (-1 == getsockopt(fd, SOL_SOCKET, SO_ERROR, &error, &len)) {
        return -1;
    }
    if (!error) {
        return 0;
    } else {
        errno = error;
        return -1;
    }
}

int connect(int fd, const struct sockaddr *addr, socklen_t addrlen) {
    return connect_with_timeout(fd, addr, addrlen, hyn::s_connect_timeout);
}

int accept(int sockfd, struct sockaddr *addr, socklen_t *addrlen) {
    ssize_t fd = hyn::do_io(sockfd, accept_f, "accept", hyn::iomanager::IOManager::READ, SO_RCVTIMEO, addr, addrlen);
    if (fd >= 0)
        hyn::FdMgr::GetInstance()->get(static_cast<int>(fd), true);
    return static_cast<int>(fd);
}

int getsockopt(int sockfd, int level, int optname, void *optval, socklen_t *optlen) {
    return getsockopt_f(sockfd, level, optname, optval, optlen);
}

/*setsockopt，用于设置socket选项的当前值。*/
/*在 Linux 中，每个套接字（socket）都有一些可选项（options），它们可以设置或获取。这些选项可以控制套接字的行为和属性，例如超时时间、
 * 缓冲区大小、是否允许地址重用等等。getsockopt 函数可以获取套接字的选项值，这个值可以帮助我们了解套接字的当前状态和属性。*/
int setsockopt(int sockfd, int level, int optname, const void *optval, socklen_t optlen) {
    if (!hyn::s_hook_enable)
        return setsockopt_f(sockfd, level, optname, optval, optlen);
    if (level == SOL_SOCKET) {
        if (optname == SO_RCVTIMEO || optname == SO_SNDTIMEO) {
            auto ctx = hyn::FdMgr::GetInstance()->get(sockfd);
            if (ctx) {
                auto *v = static_cast<const timeval *>(optval);
                ctx->set_time(optname, v->tv_sec * 1000 + v->tv_usec / 1000);
            }
        }
    }
    return setsockopt_f(sockfd, level, optname, optval, optlen);
}

//read
ssize_t read(int fd, void *buf, size_t count) {
    return hyn::do_io(fd, read_f, "read", hyn::iomanager::IOManager::READ, SO_RCVTIMEO, buf, count);
}

ssize_t readv(int fd, const struct iovec *iov, int iovcnt) {
    return hyn::do_io(fd, readv_f, "readv", hyn::iomanager::IOManager::READ, SO_RCVTIMEO, iov, iovcnt);
}

ssize_t recv(int sockfd, void *buf, size_t len, int flags) {
    return hyn::do_io(sockfd, recv_f, "recv", hyn::iomanager::IOManager::READ, SO_RCVTIMEO, buf, len, flags);
}

ssize_t recvfrom(int sockfd, void *buf, size_t len, int flags, struct sockaddr *src_addr, socklen_t *addrlen) {
    return hyn::do_io(sockfd, recvfrom_f, "recvfrom", hyn::iomanager::IOManager::READ, SO_RCVTIMEO, buf, len, flags,
                      src_addr, addrlen);
}

ssize_t recvmsg(int sockfd, struct msghdr *msg, int flags) {
    return hyn::do_io(sockfd, recvmsg_f, "recvmsg", hyn::iomanager::IOManager::READ, SO_RCVTIMEO, msg, flags);
}

//write
ssize_t write(int fd, const void *buf, size_t count) {
    return hyn::do_io(fd, write_f, "write", hyn::iomanager::IOManager::WRITE, SO_SNDTIMEO, buf, count);
}

ssize_t writev(int fd, const struct iovec *iov, int iovcnt) {
    return hyn::do_io(fd, writev_f, "writev", hyn::iomanager::IOManager::WRITE, SO_SNDTIMEO, iov, iovcnt);
}

//send
ssize_t send(int sockfd, const void *buf, size_t len, int flags) {
    return hyn::do_io(sockfd, send_f, "send", hyn::iomanager::IOManager::WRITE, SO_SNDTIMEO, buf, len, flags);
}

ssize_t
sendto(int sockfd, const void *buf, size_t len, int flags, const struct sockaddr *dest_addr, socklen_t addrlen) {
    return hyn::do_io(sockfd, sendto_f, "sendto", hyn::iomanager::IOManager::WRITE, SO_SNDTIMEO, buf, len, flags,
                      dest_addr, addrlen);
}

ssize_t sendmsg(int sockfd, const struct msghdr *msg, int flags) {
    return hyn::do_io(sockfd, sendmsg_f, "sendmsg", hyn::iomanager::IOManager::WRITE, SO_SNDTIMEO, msg, flags);
}

//close
int close(int fd) {
    if (!hyn::s_hook_enable)
        return close_f(fd);
    auto ctx = hyn::FdMgr::GetInstance()->get(fd);
    if (ctx) {
        auto iom = hyn::iomanager::IOManager::GetThis();
        if (iom)
            iom->cancelAll(fd);
        hyn::FdMgr::GetInstance()->del(fd);
    }
    return close_f(fd);
}

int fcntl(int fd, int cmd, ... /* arg */ ) {
    va_list va;
    va_start(va, cmd);
    switch (cmd) {
        case F_SETFL: {
            int arg = va_arg(va, int);
            va_end(va);
            auto ctx = hyn::FdMgr::GetInstance()->get(fd);
            if (!ctx || ctx->is_close() || !ctx->is_socket())
                return fcntl_f(fd, cmd, arg);
            //将 arg 参数的 O_NONBLOCK 位设置为 ctx->is_sys_nonblock() 的值
            ctx->set_is_usr_nonblock(arg & O_NONBLOCK);
            if (ctx->is_sys_nonblock()) {
                arg |= O_NONBLOCK;
            } else {
                arg &= ~O_NONBLOCK;
            }
            return fcntl_f(fd, cmd, arg);
        }
        case F_GETFL: {
            va_end(va);
            int arg = fcntl_f(fd, cmd);
            auto ctx = hyn::FdMgr::GetInstance()->get(fd);
            if (!ctx || ctx->is_close() || !ctx->is_socket())
                return arg;
            if (ctx->is_usr_nonblock()) {
                return arg | O_NONBLOCK;
            } else {
                return arg & ~O_NONBLOCK;
            }
        }
        case F_DUPFD:
        case F_DUPFD_CLOEXEC:
        case F_SETFD:
        case F_SETOWN:
        case F_SETSIG:
        case F_SETLEASE:
        case F_NOTIFY:
#ifdef F_SETPIPE_SZ
        case F_SETPIPE_SZ:
#endif
        {
            int arg = va_arg(va, int);
            va_end(va);
            return fcntl_f(fd, cmd, arg);
        }
        case F_GETFD:
        case F_GETOWN:
        case F_GETSIG:
        case F_GETLEASE:
#ifdef F_GETPIPE_SZ
        case F_GETPIPE_SZ:
#endif
        {
            va_end(va);
            return fcntl_f(fd, cmd);
        }
        case F_SETLK:
        case F_SETLKW:
        case F_GETLK: {
            struct flock *arg = va_arg(va, struct flock*);
            va_end(va);
            return fcntl_f(fd, cmd, arg);
        }
        case F_GETOWN_EX:
        case F_SETOWN_EX: {
            struct f_owner_exlock *arg = va_arg(va, struct f_owner_exlock*);
            va_end(va);
            return fcntl_f(fd, cmd, arg);
        }
        default:
            va_end(va);
            return fcntl_f(fd, cmd);
    }
}

int ioctl(int fd, unsigned long request, ...) {
    va_list va;
    va_start(va, request);
    void *arg = va_arg(va, void*);
    va_end(va);
    //FIONBIO用于控制非阻塞I/O
    if (request == FIONBIO) {
        /*将一个整数类型的指针arg解引用，然后通过取反和再取反操作将其转化为bool类型，并将其赋值给了user_nonblock变量。
        * !!是C/C++中的一种惯用写法，表示将一个值转换为bool类型。第一个感叹号将其转换为bool类型，
        * 然后再通过第二个感叹号将其再次转换为bool类型并取反，从而保证了最终结果为true或false。*/
        bool user_nonblock = !!*(int *) arg;
        auto ctx = hyn::FdMgr::GetInstance()->get(fd);
        if (!ctx || !ctx->is_socket() || ctx->is_close())
            return ioctl_f(fd, request, arg);
        ctx->set_is_usr_nonblock(user_nonblock);
    }
    return ioctl(fd, request, arg);
}

}//extern "C"
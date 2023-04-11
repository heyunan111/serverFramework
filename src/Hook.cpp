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

}

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

int socket(int domain, int type, int protocol) {

}
}//extern "C"
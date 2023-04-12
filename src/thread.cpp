/**
  ******************************************************************************
  * @file           : thread.cpp
  * @author         : hyn
  * @brief          : None
  * @attention      : None
  * @date           : 2023/4/1
  ******************************************************************************
  */
#include <utility>

#include "thread.h"
#include "Logger.h"
#include "util.h"

static thread_local hyn::thread::Thread *t_thread = nullptr;
static thread_local std::string t_thread_name = "UNKNOW";

hyn::thread::Thread::Thread(std::function<void()> cb, const std::string &name) : m_cb(std::move(cb)), m_name(name) {
    if (name.empty())
        m_name = "UNKNOW";
    int rt = pthread_create(&m_thread, nullptr, &Thread::run, this);
    if (rt) {
        error("thread.cpp Thread(std::function<void()> cb, const std::string &name) pthread_create thread fail,rt = %d,name = %s",
              rt, name.c_str());
        THROW_RUNTIME_ERROR_IF(1, "pthread_create error");
    }
    m_semaphore.wait();
}

hyn::thread::Thread::~Thread() {
    if (m_thread) {
        pthread_detach(m_thread);
    }
}

void hyn::thread::Thread::join() {
    if (m_thread) {
        int rt = pthread_join(m_thread, nullptr);
        if (rt) {
            error("thread.cpp void hyn::thread::Thread::join() thread join fail,rt = %d,name = %s", rt, m_name.c_str());
            THROW_RUNTIME_ERROR_IF(1, "pthread_join error");
        }
        m_thread = 0;
    }
}

hyn::thread::Thread *hyn::thread::Thread::GetThis() {
    return t_thread;
}

const std::string &hyn::thread::Thread::GetName() {
    return t_thread_name;
}

void hyn::thread::Thread::SetName(const std::string &name) {
    if (name.empty())
        return;
    if (t_thread)
        t_thread->m_name = name;
    t_thread_name = name;
}

void *hyn::thread::Thread::run(void *arg) {
    auto *thread = static_cast<Thread *>(arg);
    t_thread = thread;
    t_thread_name = thread->m_name;
    thread->m_id = hyn::util::GetThreadId();
    pthread_setname_np(pthread_self(), thread->m_name.substr(0, 15).c_str());
    std::function<void()> cb;
    cb.swap(thread->m_cb);
    thread->m_semaphore.notify();
    cb();
    return nullptr;
}


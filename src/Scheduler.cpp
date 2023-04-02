/**
  ******************************************************************************
  * @file           : Scheduler.cpp
  * @author         : hyn
  * @brief          : None
  * @attention      : None
  * @date           : 2023/4/2
  ******************************************************************************
  */



#include <cassert>
#include <utility>
#include "../include/Scheduler.h"
#include "../include/util.h"
#include "../include/Logger.h"

namespace hyn::scheduler {
static thread_local Scheduler *t_scheduler = nullptr;
static thread_local fiber::Fiber *t_fiber = nullptr;

Scheduler::Scheduler(size_t thread, bool use_caller, const std::string &name) : m_name(name) {
    assert(thread > 0);
    if (use_caller) {
        fiber::Fiber::GetThis();
        --thread;
        assert(GetThis() == nullptr);
        t_scheduler = this;
        m_root_fiber.reset(new fiber::Fiber([this] { run(); }));
        thread::Thread::SetName(name);
        t_fiber = m_root_fiber.get();
        m_root_thread_id = util::GetThreadId();
        m_thread_ids.emplace_back(m_root_thread_id);
    } else {
        m_root_thread_id = -1;
    }
    m_thread_count = thread;
}

Scheduler::~Scheduler() {
    assert(m_stopping);
    if (GetThis() == this) {
        t_scheduler = nullptr;
    }
}

void Scheduler::start() {
    mutexType::Lock lock(m_mutex);
    if (!m_stopping) {
        return;
    }
    m_stopping = false;
    assert(m_threads.empty());
    m_threads.resize(m_thread_count);
    for (int i = 0; i < m_thread_count; ++i) {
        m_threads[i].reset(new thread::Thread([this] { run(); }, m_name + '_' + std::to_string(i)));
        m_thread_ids.push_back(m_threads[i]->get_id());
    }
}

void Scheduler::stop() {
    m_auto_stop = true;
    if (m_root_fiber && m_thread_count == 0 &&
        (m_root_fiber->get_state() == fiber::Fiber::TERM || m_root_fiber->get_state() == fiber::Fiber::INIT)) {
        info("fiber stop");
        m_stopping = true;
    }
    if (stopping()) {
        return;
    }
    bool exit_on_this_fiber = false;
    if (m_root_thread_id != -1) {
        assert(GetThis() == this);
    } else {
        assert(GetThis() != this);
    }
    m_stopping = true;
    for (int i = 0; i < m_thread_count; ++i) {
        tickle();
    }

}

Scheduler *Scheduler::GetThis() {
    return t_scheduler;
}

fiber::Fiber *Scheduler::GetMainFiber() {
    return t_fiber;
}

void Scheduler::tickle() {

}

void Scheduler::run() {

}

bool Scheduler::stopping() {

}
} // Scheduler
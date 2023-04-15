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
#include "Scheduler.h"
#include "util.h"
#include "Logger.h"
#include "Hook.h"

namespace hyn::scheduler {

///记录当前线程所属的调度器对象指针，用于协程的调度和切换
static thread_local Scheduler *t_scheduler = nullptr;
///记录当前线程正在执行的协程对象指针，用于协程的切换和状态保存
static thread_local fiber::Fiber *t_fiber = nullptr;

Scheduler::Scheduler(size_t thread, bool use_caller, const std::string &name) : m_name(name) {
    assert(thread > 0);
    if (use_caller) {
        fiber::Fiber::GetThis();//确保当前线程中有协程
        --thread;

        assert(GetThis() == nullptr);
        t_scheduler = this;

        m_root_fiber.reset(new fiber::Fiber([this] { run(); }, 0, true));
        thread::Thread::SetName(name);
        t_fiber = m_root_fiber.get();
        m_root_thread_id = util::GetThreadId();
        m_thread_id_vector.emplace_back(m_root_thread_id);
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
    debug("scheduler start");
    mutexType::Lock lock(m_mutex);
    if (!m_stopping) {
        return;
    }
    m_stopping = false;
    assert(m_thread_pool.empty());
    m_thread_pool.resize(m_thread_count);
    for (int i = 0; i < m_thread_count; ++i) {
        m_thread_pool[i].reset(new thread::Thread([this] { run(); }, m_name + '_' + std::to_string(i)));
        m_thread_id_vector.push_back(m_thread_pool[i]->get_id());
    }
    lock.unlock();
}

void Scheduler::stop() {
   // debug("scheduler stop");
    m_auto_stop = true;
    if (m_root_fiber && m_thread_count == 0 &&
        (m_root_fiber->get_state() == fiber::Fiber::TERM || m_root_fiber->get_state() == fiber::Fiber::INIT)) {
        info("fiber stop");
        m_stopping = true;
    }

    if (stopping()) {
        return;
    }

    if (m_root_thread_id != -1) {
        assert(GetThis() == this);
    } else {
        assert(GetThis() != this);
    }

    m_stopping = true;
    for (int i = 0; i < m_thread_count; ++i) {
        tickle();
    }

    if (m_root_fiber) {
        tickle();
    }

    if (m_root_fiber) {
        if (!stopping()) {
            m_root_fiber->call();
        }
    }

    std::vector<thread::Thread::ptr> thrs;

    {
        mutexType::Lock lock(m_mutex);
        thrs.swap(m_thread_pool);
    }

    for (auto &i: thrs) {
        i->join();
    }

}

Scheduler *Scheduler::GetThis() {
    return t_scheduler;
}

fiber::Fiber *Scheduler::GetMainFiber() {
    return t_fiber;
}

void Scheduler::tickle() {
    debug("tickle");
}

/**
 * 设置当前线程的scheduler
 * 设置当前线程的run,fiber
 * 协程调度循环while(true)
 *      协程消息队列是否有任务
 *      无任务，执行idle
 * */
void Scheduler::run() {
    debug("scheduler run name:%s", m_name.c_str());
    set_hook_enable(true);
    SetThis();  //把当前线程的schedule置为他自己
    if (util::GetThreadId() != m_root_thread_id) {
        //如果线程id != 主线程id，协程就等于主线程的协程
        t_fiber = fiber::Fiber::GetThis().get();
    }
    fiber::Fiber::ptr idle_fiber(new fiber::Fiber([this] { idle(); })); //当调度任务都完成之后去做
    fiber::Fiber::ptr cb_fiber;
    Task task;
    while (true) {
        task.reset();
        bool tickle_me = false;
        bool is_active = false;
        {
            mutexType::Lock lock(m_mutex);
            auto it = m_task_queue.begin();
            while (it != m_task_queue.end()) {
                if (it->thread != -1 && it->thread != util::GetThreadId()) {
                    //当前执行run的线程id不等于期望的线程id，不能处理，通知别人处理
                    ++it;
                    tickle_me = true;
                    continue;
                }

                assert(it->fiber || it->cb);

                if (it->fiber && it->fiber->get_state() == fiber::Fiber::EXEC) {
                    //如果是fiber ,并且正在执行，不处理
                    ++it;
                    continue;
                }

                //把任务拿出来
                task = *it;
                m_task_queue.erase(it++);
                ++m_active_thread_count;
                is_active = true;
                break;
            }
            tickle_me |= it != m_task_queue.end();

        }

        if (tickle_me) {
            tickle();
        }

        if (task.fiber && task.fiber->get_state() != fiber::Fiber::TERM &&
            task.fiber->get_state() != fiber::Fiber::EXCEPT) {
            //如果是协程
            task.fiber->swap_in();
            --m_active_thread_count;

            if (task.fiber->get_state() == fiber::Fiber::READY) {
                //fiber可执行，放入消息队列去执行
                schedule(task.fiber);
            } else if (task.fiber->get_state() != fiber::Fiber::TERM &&
                       task.fiber->get_state() != fiber::Fiber::EXCEPT) {
                task.fiber->set_m_state(fiber::Fiber::HOLD);
            }

            task.reset();
        } else if (task.cb) {

            if (cb_fiber) {
                cb_fiber->reset(task.cb);
            } else {
                cb_fiber.reset(new fiber::Fiber(task.cb));
            }

            task.reset();
            cb_fiber->swap_in();
            --m_active_thread_count;

            if (cb_fiber->get_state() == fiber::Fiber::READY) {
                schedule(cb_fiber);
                cb_fiber.reset();
            } else if (cb_fiber->get_state() == fiber::Fiber::EXCEPT ||
                       cb_fiber->get_state() == fiber::Fiber::TERM) {
                cb_fiber->reset(nullptr);
            } else {
                cb_fiber->set_m_state(fiber::Fiber::HOLD);
                cb_fiber.reset();
            }

        } else {

            if (is_active) {
                --m_active_thread_count;
                continue;
            }

            if (idle_fiber->get_state() == fiber::Fiber::TERM) {
                info("idle fiber term");
                break;
            }

            ++m_idle_thread_count;
            idle_fiber->swap_in();
            --m_idle_thread_count;

            if (idle_fiber->get_state() != fiber::Fiber::TERM &&
                idle_fiber->get_state() != fiber::Fiber::EXCEPT) {
                idle_fiber->set_m_state(fiber::Fiber::HOLD);
            }
        }
    }
}

bool Scheduler::stopping() {
    mutexType::Lock lock(m_mutex);
    return m_auto_stop && m_stopping && m_task_queue.empty() && m_active_thread_count == 0;
}

void Scheduler::SetThis() {
    t_scheduler = this;
}

void Scheduler::idle() {
    debug("idle");
    while (!stopping()) {
        fiber::Fiber::YieldToHold();
    }
}

const std::string &Scheduler::getMName() const {
    return m_name;
}


} // Scheduler
/**
  ******************************************************************************
  * @file           : fiber.cpp
  * @author         : hyn
  * @brief          : None
  * @attention      : None
  * @date           : 2023/4/1
  ******************************************************************************
  */
#include <atomic>
#include <utility>

#include "../include/fiber.h"
#include "../include/Logger.h"
#include "../include/iniFile.h"

static std::atomic<uint64_t> s_fiber_id{0};
static std::atomic<uint64_t> s_fiber_count{0};
static thread_local hyn::fiber::Fiber *t_fiber = nullptr;
static thread_local hyn::fiber::Fiber::ptr t_threadFiber = nullptr;


/**
*@类名：MallocStackAllocator
*@参数：null
*/
class MallocStackAllocator {
public:
    /*
     *@作用：分配内存
     *@参数：分配大小
     *@返回值：void*
     */
    static void *Alloc(size_t size) {
        return malloc(size);
    }

    /*
     *@作用：释放内存
     *@参数：要释放的内存，大小
     *@返回值：null
     */
    static void Dealloc(void *vp, size_t size) {
        free(vp);
    }
};

using StackAlloc = MallocStackAllocator;

hyn::fiber::Fiber::Fiber() {
    m_state = EXEC;
    SetThis(this);
    if (getcontext(&m_ctx)) {
        error("getcontext error");
        THROW_RUNTIME_ERROR_IF(1, "getcontext");
    }
    ++s_fiber_count;
}

hyn::fiber::Fiber::Fiber(std::function<void()> cb, size_t stacksize) : m_id(++s_fiber_id), m_cb(std::move(cb)) {
    m_stacksize = stacksize ? stacksize : 131072;
    m_stack = StackAlloc::Alloc(m_stacksize);
    if (getcontext(&m_ctx)) {
        error("getcontext error");
        THROW_RUNTIME_ERROR_IF(1, "getcontext");
    }
    //uc link 关联上下文
    m_ctx.uc_link = nullptr;
    m_ctx.uc_stack.ss_sp = m_stack;
    m_ctx.uc_stack.ss_size = m_stacksize;
    makecontext(&m_ctx, &Fiber::MainFunc, 0);
}

hyn::fiber::Fiber::~Fiber() {
    --s_fiber_count;
    if (m_stack) {
        assert(m_state == INIT || m_state == TERM || m_state == EXCEPT);
        StackAlloc::Dealloc(m_stack, m_stacksize);
    } else {
        assert(!m_cb);
        assert(m_state == EXEC);
        Fiber *cur = t_fiber;
        if (cur == this) {
            SetThis(nullptr);
        }
    }
}

void hyn::fiber::Fiber::reset(const std::function<void()> &cb) {
    assert(m_stack);
    assert(m_state == TERM || m_state == INIT || m_state == EXCEPT);
    m_cb = cb;
    THROW_RUNTIME_ERROR_IF(getcontext(&m_ctx), "getcontext error");
    m_ctx.uc_link = nullptr;
    m_ctx.uc_stack.ss_sp = m_stack;
    m_ctx.uc_stack.ss_size = m_stacksize;
    makecontext(&m_ctx, &Fiber::MainFunc, 0);
    m_state = INIT;
}

void hyn::fiber::Fiber::swap_in() {
    SetThis(this);
    THROW_RUNTIME_ERROR_IF(m_state != EXEC, "swap_in error");
    if (swapcontext(&t_threadFiber->m_ctx, &m_ctx)) {
        THROW_RUNTIME_ERROR_IF(1, "swap context");
        error("swap context");
    }
}

void hyn::fiber::Fiber::swap_out() {
    SetThis(t_threadFiber.get());
    if (swapcontext(&m_ctx, &t_threadFiber->m_ctx)) {
        THROW_RUNTIME_ERROR_IF(1, "swap out");
        error("swap out");
    }
}

hyn::fiber::Fiber::ptr hyn::fiber::Fiber::GetThis() {
    if (t_fiber) {
        return t_fiber->shared_from_this();
    }
    Fiber::ptr main_fiber(new Fiber);
    assert(t_fiber == main_fiber.get());
    t_threadFiber = main_fiber;
    return t_fiber->shared_from_this();
}

void hyn::fiber::Fiber::YieldToReady() {
    Fiber::ptr cur = GetThis();
    cur->m_state = READY;
    cur->swap_out();
}

uint64_t hyn::fiber::Fiber::TotalFibers() {
    return s_fiber_count;
}

void hyn::fiber::Fiber::MainFunc() {
    Fiber::ptr cur = GetThis();
    assert(cur);
    try {
        cur->m_cb();
        cur->m_cb = nullptr;
        cur->m_state = TERM;
    } catch (std::exception &exception) {
        cur->m_state = EXCEPT;
        info("Fiber EXCEPT %s : ", exception.what());
    } catch (...) {
        cur->m_state = EXCEPT;
        info("Fiber EXCEPT");
    }
}

void hyn::fiber::Fiber::SetThis(Fiber *f) {
    t_fiber = f;
}

void hyn::fiber::Fiber::YieldToHold() {
    Fiber::ptr cur = GetThis();
    cur->m_state = HOLD;
    cur->swap_out();
}

uint64_t hyn::fiber::Fiber::GetFiberId() {
    if (t_fiber)
        return t_fiber->get_id();
    return 0;
}




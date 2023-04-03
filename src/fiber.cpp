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
    static void Dealloc(void *vp) {
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

    debug("Fiber::Fiber");
}
/*
    typedef struct ucontext_t
    {
    unsigned long int __ctx(uc_flags);
    struct ucontext_t *uc_link;
    stack_t uc_stack;
    mcontext_t uc_mcontext;
    sigset_t uc_sigmask;
    struct _libc_fpstate __fpregs_mem;
    __extension__ unsigned long long int __ssp[4];
    } ucontext_t;
*/
hyn::fiber::Fiber::Fiber(std::function<void()> cb, size_t stacksize) : m_id(++s_fiber_id), m_cb(std::move(cb)),
                                                                       m_ctx(), m_state(INIT), m_stack(nullptr) {
    m_stacksize = stacksize ? stacksize : 131072;
    m_stack = StackAlloc::Alloc(m_stacksize);
    // 获取上下文对象的副本
    if (getcontext(&m_ctx)) {
        error("getcontext error");
        THROW_RUNTIME_ERROR_IF(1, "getcontext");
    }
    //uc link 关联上下文
    m_ctx.uc_link = nullptr;
    m_ctx.uc_stack.ss_sp = m_stack;
    m_ctx.uc_stack.ss_size = m_stacksize;
    makecontext(&m_ctx, &Fiber::MainFunc, 0);

    debug("Fiber::Fiber id : ", m_id);
}

hyn::fiber::Fiber::~Fiber() {
    --s_fiber_count;
    if (m_stack) {
        assert(m_state == INIT || m_state == TERM || m_state == EXCEPT);
        StackAlloc::Dealloc(m_stack);
    } else {
        assert(!m_cb);
        assert(m_state == EXEC);
        if (t_fiber == this) {
            SetThis(nullptr);
        }
    }
    debug("Fiber:~Fiber");
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
    assert(m_state == INIT || m_state == READY || m_state == HOLD);
    m_state = EXEC;
    if (swapcontext(&t_threadFiber->m_ctx, &m_ctx)) {
        error("swap context");
        THROW_RUNTIME_ERROR_IF(1, "swap context");
    }
}

void hyn::fiber::Fiber::swap_out() {
    SetThis(t_threadFiber.get());
    if (swapcontext(&m_ctx, &t_threadFiber->m_ctx)) {
        error("swap out");
        THROW_RUNTIME_ERROR_IF(1, "swap out");
    }
}

hyn::fiber::Fiber::ptr hyn::fiber::Fiber::GetThis() {
    if (t_fiber != nullptr) {
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
    auto raw_ptr = cur.get();
    cur.reset();
    raw_ptr->swap_out();
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

void hyn::fiber::Fiber::call() {
    assert(t_threadFiber && "当前不存在主协程");
    assert(m_state == INIT || m_state == READY || m_state == HOLD);
    SetThis(this);
    m_state = EXEC;
    if (swapcontext(&(t_threadFiber->m_ctx), &m_ctx)) {
        info("call error");
        THROW_RUNTIME_ERROR_IF(1, "call error");
    }
}

void hyn::fiber::Fiber::back() {
    assert(t_threadFiber && "当前线程不存在主协程");
    assert(m_stack);
    SetThis(t_threadFiber.get());
    if (swapcontext(&m_ctx, &(t_threadFiber->m_ctx))) {
        info("back error");
        THROW_RUNTIME_ERROR_IF(1, "back error");
    }
}

void hyn::fiber::Fiber::swap_in(const hyn::fiber::Fiber::ptr &fiber) {
    assert(m_state == INIT || m_state == READY || m_state == HOLD);
    SetThis(this);
    m_state = EXEC;
    if (swapcontext(&(fiber->m_ctx), &m_ctx)) {
        info("swap_in (fiber) error");
        THROW_RUNTIME_ERROR_IF(1, "swap_in (fiber) error");
    }
}

void hyn::fiber::Fiber::swap_out(const hyn::fiber::Fiber::ptr &fiber1) {
    assert(m_state);
    SetThis(fiber1.get());
    if (swapcontext(&m_ctx, &(fiber1->m_ctx))) {
        info("swap_out (fiber) error");
        THROW_RUNTIME_ERROR_IF(1, "swap_out (fiber) error");
    }
}

void hyn::fiber::Fiber::set_m_state(State mState) {
    m_state = mState;
}




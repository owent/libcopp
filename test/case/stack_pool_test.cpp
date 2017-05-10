#include <cstdio>
#include <cstring>
#include <iostream>
#include <vector>

#include <libcopp/stack/stack_pool.h>
#include <libcotask/task.h>

#include "frame/test_macros.h"

typedef copp::stack_pool<copp::allocator::stack_allocator_malloc> stack_pool_t;
struct stack_pool_test_macro_coroutine {
    typedef copp::detail::coroutine_context_base coroutine_t;
    typedef copp::allocator::stack_allocator_pool<stack_pool_t> stack_allocator_t;

    typedef copp::detail::coroutine_context_container<coroutine_t, stack_allocator_t> coroutine_container_t;
};
static stack_pool_t::ptr_t global_stack_pool;

struct stack_pool_test_task_allocator : public cotask::macro_task::task_allocator_t {
public:
#if defined(COPP_MACRO_ENABLE_VARIADIC_TEMPLATE) && COPP_MACRO_ENABLE_VARIADIC_TEMPLATE
    template <typename Ty, typename... TARGS>
    static std::shared_ptr<Ty> allocate(Ty *t, TARGS COPP_MACRO_RV_REF... args) {
        std::shared_ptr<Ty> ret = cotask::macro_task::task_allocator_t::allocate(t, COPP_MACRO_STD_FORWARD(TARGS, args)...);
        ret->get_coroutine_context().get_allocator().attach(global_stack_pool);
        return COPP_MACRO_STD_MOVE(ret);
    }
#else
    template <typename Ty>
    static std::shared_ptr<Ty> allocate(Ty *t) {
        std::shared_ptr<Ty> ret = cotask::macro_task::task_allocator_t::allocate(t);
        ret->get_coroutine_context().get_allocator().attach(global_stack_pool);
        return COPP_MACRO_STD_MOVE(ret);
    }
#endif
};

struct stack_pool_test_macro_task {
    typedef cotask::macro_task::id_t id_t;
    typedef cotask::macro_task::id_allocator_t id_allocator_t;

    typedef cotask::macro_task::action_allocator_t action_allocator_t;
    typedef stack_pool_test_task_allocator task_allocator_t;
};

typedef cotask::task<stack_pool_test_macro_coroutine, stack_pool_test_macro_task> stack_pool_test_task_t;

static int stack_pool_test_task_action() { return 0; }

CASE_TEST(stack_pool_test, basic) {
    global_stack_pool = stack_pool_t::create();
    std::vector<stack_pool_test_task_t::ptr_t> task_arr;
    const size_t task_arr_sz = 64;

    // alloc
    for (size_t i = 0; i < task_arr_sz; ++i) {
        stack_pool_test_task_t::ptr_t tp = stack_pool_test_task_t::create(stack_pool_test_task_action);
        task_arr.push_back(tp);
    }
    CASE_EXPECT_EQ(task_arr_sz, global_stack_pool->get_limit().used_stack_number);
    CASE_EXPECT_EQ(task_arr_sz * (global_stack_pool->get_stack_size() + global_stack_pool->get_stack_size_offset()),
                   global_stack_pool->get_limit().used_stack_size);

    CASE_EXPECT_EQ(0, global_stack_pool->get_limit().free_stack_number);
    CASE_EXPECT_EQ(0, global_stack_pool->get_limit().free_stack_size);

    // recycle to free list
    for (size_t i = 0; i < task_arr_sz / 2; ++i) {
        task_arr.pop_back();
    }
    CASE_EXPECT_EQ(task_arr_sz - task_arr_sz / 2, global_stack_pool->get_limit().used_stack_number);
    CASE_EXPECT_EQ(task_arr_sz / 2, global_stack_pool->get_limit().free_stack_number);
    CASE_EXPECT_EQ((task_arr_sz - task_arr_sz / 2) * (global_stack_pool->get_stack_size() + global_stack_pool->get_stack_size_offset()),
                   global_stack_pool->get_limit().used_stack_size);
    CASE_EXPECT_EQ((task_arr_sz / 2) * (global_stack_pool->get_stack_size() + global_stack_pool->get_stack_size_offset()),
                   global_stack_pool->get_limit().free_stack_size);

    // auto_gc
    task_arr.pop_back();
    CASE_EXPECT_EQ(task_arr_sz / 4, global_stack_pool->get_limit().free_stack_number);
    CASE_EXPECT_EQ((task_arr_sz / 4) * (global_stack_pool->get_stack_size() + global_stack_pool->get_stack_size_offset()),
                   global_stack_pool->get_limit().free_stack_size);

    global_stack_pool.reset();
}

CASE_TEST(stack_pool_test, full_number) {
    global_stack_pool = stack_pool_t::create();
    // full

    std::vector<stack_pool_test_task_t::ptr_t> task_arr;
    const size_t task_arr_sz = 64;
    global_stack_pool->set_max_stack_number(task_arr_sz);
    // alloc
    for (size_t i = 0; i < task_arr_sz; ++i) {
        stack_pool_test_task_t::ptr_t tp = stack_pool_test_task_t::create(stack_pool_test_task_action);
        CASE_EXPECT_TRUE(!!tp);
        task_arr.push_back(tp);
    }

    {
        stack_pool_test_task_t::ptr_t tp = stack_pool_test_task_t::create(stack_pool_test_task_action);
        CASE_EXPECT_TRUE(!tp);
    }

    global_stack_pool.reset();
}

CASE_TEST(stack_pool_test, custom_gc) {
    global_stack_pool = stack_pool_t::create();
    std::vector<stack_pool_test_task_t::ptr_t> task_arr;
    const size_t task_arr_sz = 64;

    global_stack_pool->set_min_stack_number(task_arr_sz);

    // alloc
    for (size_t i = 0; i < task_arr_sz; ++i) {
        stack_pool_test_task_t::ptr_t tp = stack_pool_test_task_t::create(stack_pool_test_task_action);
        task_arr.push_back(tp);
    }

    CASE_EXPECT_EQ(task_arr_sz, global_stack_pool->get_limit().used_stack_number);
    CASE_EXPECT_EQ(task_arr_sz * (global_stack_pool->get_stack_size() + global_stack_pool->get_stack_size_offset()),
                   global_stack_pool->get_limit().used_stack_size);

    CASE_EXPECT_EQ(0, global_stack_pool->get_limit().free_stack_number);
    CASE_EXPECT_EQ(0, global_stack_pool->get_limit().free_stack_size);

    for (size_t i = 0; i < task_arr_sz; ++i) {
        task_arr.pop_back();
    }

    CASE_EXPECT_EQ(0, global_stack_pool->get_limit().used_stack_number);
    CASE_EXPECT_EQ(0, global_stack_pool->get_limit().used_stack_size);

    CASE_EXPECT_EQ(task_arr_sz, global_stack_pool->get_limit().free_stack_number);
    CASE_EXPECT_EQ(task_arr_sz * (global_stack_pool->get_stack_size() + global_stack_pool->get_stack_size_offset()),
                   global_stack_pool->get_limit().free_stack_size);

    global_stack_pool->set_min_stack_number(0);
    global_stack_pool.reset();
}

CASE_TEST(stack_pool_test, gc_once) {
    global_stack_pool = stack_pool_t::create();
    // gc_once
    std::vector<stack_pool_test_task_t::ptr_t> task_arr;
    const size_t task_arr_sz = 64;

    global_stack_pool->set_auto_gc(false);
    global_stack_pool->set_gc_once_number(10);

    // alloc
    for (size_t i = 0; i < task_arr_sz; ++i) {
        stack_pool_test_task_t::ptr_t tp = stack_pool_test_task_t::create(stack_pool_test_task_action);
        task_arr.push_back(tp);
    }

    CASE_EXPECT_EQ(task_arr_sz, global_stack_pool->get_limit().used_stack_number);
    CASE_EXPECT_EQ(task_arr_sz * (global_stack_pool->get_stack_size() + global_stack_pool->get_stack_size_offset()),
                   global_stack_pool->get_limit().used_stack_size);

    CASE_EXPECT_EQ(0, global_stack_pool->get_limit().free_stack_number);
    CASE_EXPECT_EQ(0, global_stack_pool->get_limit().free_stack_size);

    for (size_t i = 0; i < task_arr_sz; ++i) {
        task_arr.pop_back();
    }

    CASE_EXPECT_EQ(0, global_stack_pool->get_limit().used_stack_number);
    CASE_EXPECT_EQ(0, global_stack_pool->get_limit().used_stack_size);

    CASE_EXPECT_EQ(task_arr_sz, global_stack_pool->get_limit().free_stack_number);
    CASE_EXPECT_EQ(task_arr_sz * (global_stack_pool->get_stack_size() + global_stack_pool->get_stack_size_offset()),
                   global_stack_pool->get_limit().free_stack_size);

    global_stack_pool->gc();

    CASE_EXPECT_EQ(task_arr_sz - 10, global_stack_pool->get_limit().free_stack_number);
    CASE_EXPECT_EQ((task_arr_sz - 10) * (global_stack_pool->get_stack_size() + global_stack_pool->get_stack_size_offset()),
                   global_stack_pool->get_limit().free_stack_size);

    global_stack_pool.reset();
}

CASE_TEST(stack_pool_test, full_size) {
    global_stack_pool = stack_pool_t::create();
    // full size

    std::vector<stack_pool_test_task_t::ptr_t> task_arr;
    stack_pool_test_task_t::ptr_t tp1;
    stack_pool_test_task_t::ptr_t tp2;

    global_stack_pool->set_max_stack_size(128 * 1024);
    global_stack_pool->set_stack_size(100 * 1024);
    // alloc
    tp1 = stack_pool_test_task_t::create(stack_pool_test_task_action);
    CASE_EXPECT_TRUE(!!tp1);

    tp2 = stack_pool_test_task_t::create(stack_pool_test_task_action);
    CASE_EXPECT_TRUE(!tp2);

    global_stack_pool.reset();
}
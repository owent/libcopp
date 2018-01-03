#ifdef COTASK_MACRO_ENABLED

#include <cstdio>
#include <cstring>
#include <iostream>

#include <libcopp/utils/std/smart_ptr.h>

#include "frame/test_macros.h"
#include <libcotask/task.h>

static int g_test_coroutine_task_status = 0;
static int g_test_coroutine_task_on_finished = 0;
class test_context_task_action_base : public cotask::impl::task_action_impl {
public:
    // add a same name function to find the type detection error
    virtual int operator()() = 0;

    int operator()(void *priv_data) {
        ++g_test_coroutine_task_status;

        CASE_EXPECT_EQ(&g_test_coroutine_task_status, priv_data);

        CASE_EXPECT_EQ(cotask::EN_TS_RUNNING, cotask::this_task::get_task()->get_status());
        cotask::this_task::get_task()->yield(&priv_data);
        CASE_EXPECT_EQ(cotask::EN_TS_RUNNING, cotask::this_task::get_task()->get_status());

        CASE_EXPECT_EQ(cotask::this_task::get_task(), priv_data);

        ++g_test_coroutine_task_status;

        return 0;
    }

    virtual int on_finished(cotask::impl::task_impl &t) {
        ++g_test_coroutine_task_on_finished;
        return 0;
    }
};

class test_context_task_action : public test_context_task_action_base {
public:
    using test_context_task_action_base::operator();

    // add a same name function to find the type detection error
    virtual int operator()() { return 0; }
};


CASE_TEST(coroutine_task, custom_action) {
    typedef cotask::task<>::ptr_t task_ptr_type;
    g_test_coroutine_task_status = 0;
    g_test_coroutine_task_on_finished = 0;

    {
        task_ptr_type co_task = cotask::task<>::create(test_context_task_action());
        task_ptr_type co_another_task = cotask::task<>::create(test_context_task_action()); // share action

        CASE_EXPECT_EQ(cotask::EN_TS_CREATED, co_task->get_status());
        CASE_EXPECT_EQ(cotask::EN_TS_CREATED, co_another_task->get_status());

        CASE_EXPECT_EQ(0, co_task->start(&g_test_coroutine_task_status));

        CASE_EXPECT_EQ(g_test_coroutine_task_status, 1);
        CASE_EXPECT_FALSE(co_task->is_completed());

        CASE_EXPECT_EQ(0, co_another_task->start(&g_test_coroutine_task_status));
        CASE_EXPECT_EQ(g_test_coroutine_task_status, 2);

        CASE_EXPECT_EQ(cotask::EN_TS_WAITING, co_task->get_status());
        CASE_EXPECT_EQ(cotask::EN_TS_WAITING, co_another_task->get_status());

        CASE_EXPECT_EQ(0, co_task->resume(co_task.get()));
        CASE_EXPECT_EQ(g_test_coroutine_task_status, 3);

        CASE_EXPECT_EQ(0, co_another_task->resume(co_another_task.get()));
        CASE_EXPECT_EQ(g_test_coroutine_task_status, 4);

        CASE_EXPECT_TRUE(co_task->is_completed());
        CASE_EXPECT_TRUE(co_another_task->is_completed());

        CASE_EXPECT_GT(0, co_another_task->resume(co_another_task.get()));
        CASE_EXPECT_EQ(g_test_coroutine_task_status, 4);

        ++g_test_coroutine_task_status;
        CASE_EXPECT_EQ(g_test_coroutine_task_status, 5);

        CASE_EXPECT_NE(co_task->get_id(), 0);
    }

    CASE_EXPECT_EQ(g_test_coroutine_task_on_finished, 2);
}


struct test_context_task_functor {
public:
    int operator()(void *) const {
        ++g_test_coroutine_task_status;
        CASE_EXPECT_EQ(g_test_coroutine_task_status, 1);

        cotask::this_task::get_task()->yield();

        ++g_test_coroutine_task_status;
        CASE_EXPECT_EQ(g_test_coroutine_task_status, 3);

        return 0;
    }
};

CASE_TEST(coroutine_task, functor_action) {
    typedef cotask::task<>::ptr_t task_ptr_type;
    task_ptr_type co_task = cotask::task<>::create(test_context_task_functor());
    g_test_coroutine_task_status = 0;

    CASE_EXPECT_EQ(0, co_task->start());

    ++g_test_coroutine_task_status;
    CASE_EXPECT_EQ(g_test_coroutine_task_status, 2);

    CASE_EXPECT_FALSE(co_task->is_completed());
    CASE_EXPECT_EQ(0, co_task->resume());

    CASE_EXPECT_TRUE(co_task->is_completed());

    ++g_test_coroutine_task_status;
    CASE_EXPECT_EQ(g_test_coroutine_task_status, 4);

    CASE_EXPECT_NE(co_task->get_id(), 0);
}


static int test_context_task_function_1(void *) {
    ++g_test_coroutine_task_status;
    CASE_EXPECT_EQ(g_test_coroutine_task_status, 1);

    cotask::this_task::get_task()->yield();

    ++g_test_coroutine_task_status;
    CASE_EXPECT_EQ(g_test_coroutine_task_status, 3);

    return 100;
}

static void test_context_task_function_2(void *) {
    ++g_test_coroutine_task_status;
    CASE_EXPECT_EQ(g_test_coroutine_task_status, 1);

    cotask::this_task::get_task()->yield();

    ++g_test_coroutine_task_status;
    CASE_EXPECT_EQ(g_test_coroutine_task_status, 3);
}

CASE_TEST(coroutine_task, function_action) {
    {
        typedef cotask::task<>::ptr_t task_ptr_type;
        task_ptr_type co_task = cotask::task<>::create(test_context_task_function_1);
        g_test_coroutine_task_status = 0;

        CASE_EXPECT_EQ(0, co_task->start());

        ++g_test_coroutine_task_status;
        CASE_EXPECT_EQ(g_test_coroutine_task_status, 2);

        CASE_EXPECT_FALSE(co_task->is_completed());
        CASE_EXPECT_EQ(0, co_task->resume());

        CASE_EXPECT_TRUE(co_task->is_completed());

        ++g_test_coroutine_task_status;
        CASE_EXPECT_EQ(g_test_coroutine_task_status, 4);
        CASE_EXPECT_EQ(co_task->get_coroutine_context()->get_ret_code(), 100);
    }

    {
        typedef cotask::task<>::ptr_t task_ptr_type;
        task_ptr_type co_task = cotask::task<>::create(test_context_task_function_2);
        g_test_coroutine_task_status = 0;

        CASE_EXPECT_EQ(0, co_task->start());

        ++g_test_coroutine_task_status;
        CASE_EXPECT_EQ(g_test_coroutine_task_status, 2);

        CASE_EXPECT_FALSE(co_task->is_completed());
        CASE_EXPECT_EQ(0, co_task->resume());

        CASE_EXPECT_TRUE(co_task->is_completed());

        ++g_test_coroutine_task_status;
        CASE_EXPECT_EQ(g_test_coroutine_task_status, 4);
    }
}

// task start and coroutine context yield
static void test_context_task_function_3(void *) {
    ++g_test_coroutine_task_status;
    CASE_EXPECT_EQ(g_test_coroutine_task_status, 1);

    CASE_EXPECT_EQ(cotask::EN_TS_RUNNING, cotask::this_task::get_task()->get_status());

    copp::this_coroutine::yield();

    CASE_EXPECT_EQ(cotask::EN_TS_RUNNING, cotask::this_task::get<cotask::task<> >()->get_status());

    ++g_test_coroutine_task_status;
    CASE_EXPECT_EQ(g_test_coroutine_task_status, 3);
}

CASE_TEST(coroutine_task, coroutine_context_yield) {
    typedef cotask::task<>::ptr_t task_ptr_type;
    task_ptr_type co_task = cotask::task<>::create(test_context_task_function_3);
    g_test_coroutine_task_status = 0;

    CASE_EXPECT_EQ(0, co_task->start());

    ++g_test_coroutine_task_status;
    CASE_EXPECT_EQ(g_test_coroutine_task_status, 2);

    CASE_EXPECT_FALSE(co_task->is_completed());
    CASE_EXPECT_EQ(cotask::EN_TS_WAITING, co_task->get_status());
    CASE_EXPECT_EQ(0, co_task->resume());

    CASE_EXPECT_TRUE(co_task->is_completed());

    ++g_test_coroutine_task_status;
    CASE_EXPECT_EQ(g_test_coroutine_task_status, 4);

    CASE_EXPECT_NE(co_task->get_id(), 0);
}


struct test_context_task_mem_function {
    cotask::task<>::id_t task_id_;

    int real_run(void *) {
        ++g_test_coroutine_task_status;
        CASE_EXPECT_EQ(g_test_coroutine_task_status, 1);

        CASE_EXPECT_EQ(task_id_, cotask::task<>::this_task()->get_id());
        cotask::task<>::this_task()->yield();
        CASE_EXPECT_EQ(task_id_, cotask::task<>::this_task()->get_id());

        ++g_test_coroutine_task_status;
        CASE_EXPECT_EQ(g_test_coroutine_task_status, 3);

        return -1;
    }
};

CASE_TEST(coroutine_task, mem_function_action) {
    typedef cotask::task<>::ptr_t task_ptr_type;
    test_context_task_mem_function obj;
    task_ptr_type co_task = cotask::task<>::create(&test_context_task_mem_function::real_run, &obj);
    g_test_coroutine_task_status = 0;
    obj.task_id_ = co_task->get_id();

    CASE_EXPECT_EQ(0, co_task->start());

    ++g_test_coroutine_task_status;
    CASE_EXPECT_EQ(g_test_coroutine_task_status, 2);

    CASE_EXPECT_FALSE(co_task->is_completed());
    CASE_EXPECT_EQ(0, co_task->resume());

    CASE_EXPECT_TRUE(co_task->is_completed());

    ++g_test_coroutine_task_status;
    CASE_EXPECT_EQ(g_test_coroutine_task_status, 4);

    CASE_EXPECT_NE(co_task->get_coroutine_context()->get_ret_code(), -1);
}

CASE_TEST(coroutine_task, auto_finish) {
    typedef cotask::task<>::ptr_t task_ptr_type;
    {
        test_context_task_mem_function obj;
        task_ptr_type co_task = cotask::task<>::create(&test_context_task_mem_function::real_run, &obj);
        g_test_coroutine_task_status = 0;
        obj.task_id_ = co_task->get_id();
    }
    CASE_EXPECT_EQ(0, g_test_coroutine_task_status);

    {
        test_context_task_mem_function obj;
        task_ptr_type co_task = cotask::task<>::create(&test_context_task_mem_function::real_run, &obj);
        g_test_coroutine_task_status = 0;
        obj.task_id_ = co_task->get_id();

        CASE_EXPECT_EQ(0, co_task->start());

        ++g_test_coroutine_task_status;
        CASE_EXPECT_EQ(g_test_coroutine_task_status, 2);

        CASE_EXPECT_FALSE(co_task->is_completed());
    }

    ++g_test_coroutine_task_status;
    CASE_EXPECT_EQ(g_test_coroutine_task_status, 4);
}

struct test_context_task_next_action : public cotask::impl::task_action_impl {
    int set_;
    int check_;
    test_context_task_next_action(int s, int c) : cotask::impl::task_action_impl(), set_(s), check_(c) {}

    int operator()(void *) {
        CASE_EXPECT_EQ(g_test_coroutine_task_status, check_);
        g_test_coroutine_task_status = set_;

        CASE_EXPECT_EQ(copp::COPP_EC_IS_RUNNING, cotask::this_task::get_task()->start());
        return 0;
    }
};

CASE_TEST(coroutine_task, next) {
    typedef cotask::task<>::ptr_t task_ptr_type;

    task_ptr_type co_task = cotask::task<>::create(test_context_task_next_action(15, 0));
    co_task->next(test_context_task_next_action(7, 15))
        ->next(test_context_task_next_action(99, 7))
        ->next(test_context_task_next_action(1023, 99))
        ->next(test_context_task_next_action(5, 1023));


    g_test_coroutine_task_status = 0;
    CASE_EXPECT_EQ(0, co_task->start());
    CASE_EXPECT_EQ(g_test_coroutine_task_status, 5);

    CASE_EXPECT_EQ(copp::COPP_EC_ALREADY_FINISHED, co_task->start());
}

#if defined(UTIL_CONFIG_COMPILER_CXX_VARIADIC_TEMPLATES) && UTIL_CONFIG_COMPILER_CXX_VARIADIC_TEMPLATES

struct test_context_task_functor_drived : public cotask::impl::task_action_impl {
public:
    int a_;
    int b_;
    test_context_task_functor_drived(int a, int b) : a_(a), b_(b) {}

    virtual int operator()(void *) {
        CASE_EXPECT_EQ(a_, 1);
        CASE_EXPECT_EQ(3, b_);

        return 0;
    }
};

CASE_TEST(coroutine_task, functor_drived_action) {
    typedef cotask::task<>::ptr_t task_ptr_type;
    cotask::task<>::coroutine_t::allocator_type alloc;
    task_ptr_type co_task = cotask::task<>::create_with<test_context_task_functor_drived>(alloc, 0, 0, 1, 3);
    CASE_EXPECT_EQ(0, co_task->start());
}

#endif


static int test_context_task_priavte_buffer(void *) {

    void *priv_data = cotask::task<>::this_task()->get_private_buffer();
    size_t priv_sz = cotask::task<>::this_task()->get_private_buffer_size();


    CASE_EXPECT_GE(priv_sz, 256);

    if (priv_sz >= 256) {
        char checked_data[256];
        memset(checked_data, 0x5e, 256);

        CASE_EXPECT_EQ(0, memcmp(priv_data, checked_data, 256));
    }

    return 0;
}

CASE_TEST(coroutine_task, priavte_buffer) {
    typedef cotask::task<>::ptr_t task_ptr_type;
    task_ptr_type co_task = cotask::task<>::create(test_context_task_priavte_buffer, 16384, 256);

    void *priv_data = co_task->get_private_buffer();
    memset(priv_data, 0x5e, 256);

    CASE_EXPECT_EQ(0, co_task->start());
}


#endif

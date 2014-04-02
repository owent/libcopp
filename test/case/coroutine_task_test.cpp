#ifdef COTASK_MACRO_ENABLED

#include <iostream>
#include <cstdio>
#include <cstring>

#include <libcopp/utils/std/smart_ptr.h>

#include <libcotask/task.h>
#include "frame/test_macros.h"

static int g_test_coroutine_task_status = 0;
class test_context_task_action : public cotask::impl::task_action_impl
{
public:
    int operator()() {
        ++ g_test_coroutine_task_status;
        CASE_EXPECT_EQ(g_test_coroutine_task_status, 1);

        get_task()->yield();

        ++ g_test_coroutine_task_status;
        CASE_EXPECT_EQ(g_test_coroutine_task_status, 3);

        return 0;
    }
};


CASE_TEST(coroutine_task, custom_action)
{
    typedef std::shared_ptr< cotask::task<> > task_ptr_type;
    test_context_task_action* action = new test_context_task_action();
    task_ptr_type co_task = task_ptr_type(new cotask::task<>(action));
    g_test_coroutine_task_status = 0;

    CASE_EXPECT_EQ(0, co_task->start());

    ++ g_test_coroutine_task_status;
    CASE_EXPECT_EQ(g_test_coroutine_task_status, 2);

    CASE_EXPECT_FALSE(co_task->is_completed());
    CASE_EXPECT_EQ(0, co_task->resume());

    CASE_EXPECT_TRUE(co_task->is_completed());

    ++ g_test_coroutine_task_status;
    CASE_EXPECT_EQ(g_test_coroutine_task_status, 4);
}


struct test_context_task_functor
{
public:
    int operator()() {
        ++ g_test_coroutine_task_status;
        CASE_EXPECT_EQ(g_test_coroutine_task_status, 1);

        cotask::this_task::get_task()->yield();

        ++ g_test_coroutine_task_status;
        CASE_EXPECT_EQ(g_test_coroutine_task_status, 3);

        return 0;
    }
};

CASE_TEST(coroutine_task, functor_action)
{
    typedef std::shared_ptr< cotask::task<> > task_ptr_type;
    task_ptr_type co_task = task_ptr_type(new cotask::task<>(test_context_task_functor()));
    g_test_coroutine_task_status = 0;

    CASE_EXPECT_EQ(0, co_task->start());

    ++ g_test_coroutine_task_status;
    CASE_EXPECT_EQ(g_test_coroutine_task_status, 2);

    CASE_EXPECT_FALSE(co_task->is_completed());
    CASE_EXPECT_EQ(0, co_task->resume());

    CASE_EXPECT_TRUE(co_task->is_completed());

    ++ g_test_coroutine_task_status;
    CASE_EXPECT_EQ(g_test_coroutine_task_status, 4);
}


static int test_context_task_function_1() {
    ++ g_test_coroutine_task_status;
    CASE_EXPECT_EQ(g_test_coroutine_task_status, 1);

    cotask::this_task::get_task()->yield();

    ++ g_test_coroutine_task_status;
    CASE_EXPECT_EQ(g_test_coroutine_task_status, 3);

    return 100;
}

static void test_context_task_function_2() {
    ++ g_test_coroutine_task_status;
    CASE_EXPECT_EQ(g_test_coroutine_task_status, 1);

    cotask::this_task::get_task()->yield();

    ++ g_test_coroutine_task_status;
    CASE_EXPECT_EQ(g_test_coroutine_task_status, 3);
}

CASE_TEST(coroutine_task, function_action)
{
    {
        typedef std::shared_ptr< cotask::task<> > task_ptr_type;
        task_ptr_type co_task = task_ptr_type(new cotask::task<>(test_context_task_function_1));
        g_test_coroutine_task_status = 0;

        CASE_EXPECT_EQ(0, co_task->start());

        ++ g_test_coroutine_task_status;
        CASE_EXPECT_EQ(g_test_coroutine_task_status, 2);

        CASE_EXPECT_FALSE(co_task->is_completed());
        CASE_EXPECT_EQ(0, co_task->resume());

        CASE_EXPECT_TRUE(co_task->is_completed());

        ++ g_test_coroutine_task_status;
        CASE_EXPECT_EQ(g_test_coroutine_task_status, 4);
        CASE_EXPECT_EQ(co_task->get_coroutine_context().get_ret_code(), 100);
    }

    {
        typedef std::shared_ptr< cotask::task<> > task_ptr_type;
        task_ptr_type co_task = task_ptr_type(new cotask::task<>(test_context_task_function_2));
        g_test_coroutine_task_status = 0;

        CASE_EXPECT_EQ(0, co_task->start());

        ++ g_test_coroutine_task_status;
        CASE_EXPECT_EQ(g_test_coroutine_task_status, 2);

        CASE_EXPECT_FALSE(co_task->is_completed());
        CASE_EXPECT_EQ(0, co_task->resume());

        CASE_EXPECT_TRUE(co_task->is_completed());

        ++ g_test_coroutine_task_status;
        CASE_EXPECT_EQ(g_test_coroutine_task_status, 4);
    }
}

// task start and coroutine context yield
static void test_context_task_function_3() {
    ++ g_test_coroutine_task_status;
    CASE_EXPECT_EQ(g_test_coroutine_task_status, 1);

    CASE_EXPECT_EQ(cotask::EN_TS_RUNNING, cotask::this_task::get_task()->get_status());

    copp::this_coroutine::yield();

    CASE_EXPECT_EQ(cotask::EN_TS_RUNNING, cotask::this_task::get_task()->get_status());

    ++ g_test_coroutine_task_status;
    CASE_EXPECT_EQ(g_test_coroutine_task_status, 3);
}

CASE_TEST(coroutine_task, coroutine_context_yield)
{
    typedef std::shared_ptr< cotask::task<> > task_ptr_type;
    task_ptr_type co_task = task_ptr_type(new cotask::task<>(test_context_task_function_3));
    g_test_coroutine_task_status = 0;

    CASE_EXPECT_EQ(0, co_task->start());

    ++ g_test_coroutine_task_status;
    CASE_EXPECT_EQ(g_test_coroutine_task_status, 2);

    CASE_EXPECT_FALSE(co_task->is_completed());
    CASE_EXPECT_EQ(cotask::EN_TS_WAITING, co_task->get_status());
    CASE_EXPECT_EQ(0, co_task->resume());

    CASE_EXPECT_TRUE(co_task->is_completed());

    ++ g_test_coroutine_task_status;
    CASE_EXPECT_EQ(g_test_coroutine_task_status, 4);
}

#endif


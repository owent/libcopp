#include <iostream>
#include <cstdio>
#include <cstring>

#include <libcopp/coroutine/coroutine_context_container.h>
#include "frame/test_macros.h"


typedef copp::detail::coroutine_context_container<
    copp::detail::coroutine_context_base,
    copp::allocator::stack_allocator_memory
> test_context_base_coroutine_context_test_type;

static int g_test_coroutine_base_status = 0;

class test_context_base_foo_runner : public copp::coroutine_runnable_base
{
public:
    int call_times;
    int operator()() {
        ++ call_times;
        ++ g_test_coroutine_base_status;
        copp::this_coroutine::get<test_context_base_coroutine_context_test_type>()->yield();

        ++ g_test_coroutine_base_status;
        return 0;
    }
};

CASE_TEST(coroutine, context_base)
{
    char* stack_buff = new char[128 * 1024];
    g_test_coroutine_base_status = 0;
    ++ g_test_coroutine_base_status;
    CASE_EXPECT_EQ(g_test_coroutine_base_status, 1);

    test_context_base_coroutine_context_test_type co;
    test_context_base_foo_runner runner;
    runner.call_times = 0;

    co.get_allocator().attach(stack_buff, 128 * 1024);
    co.create(&runner, 128 * 1024);
    int res = co.create(&runner, 128 * 1024); // can not be created muli times
    CASE_EXPECT_EQ(res, copp::COPP_EC_ALREADY_INITED);

    co.start();

    ++ g_test_coroutine_base_status;
    CASE_EXPECT_EQ(g_test_coroutine_base_status, 3);
    co.resume();

    ++ g_test_coroutine_base_status;
    CASE_EXPECT_EQ(g_test_coroutine_base_status, 5);

    delete []stack_buff;
}


CASE_TEST(coroutine, shared_runner)
{
    const int stack_len = 128 * 1024;
    char* stack_buff = new char[4 * stack_len];
    g_test_coroutine_base_status = 0;
    ++ g_test_coroutine_base_status;
    CASE_EXPECT_EQ(g_test_coroutine_base_status, 1);

    test_context_base_coroutine_context_test_type co[4];
    test_context_base_foo_runner runner;
    runner.call_times = 0;

    for (int i = 0; i < 4; ++ i) {
        co[i].get_allocator().attach(stack_buff + i * stack_len, stack_len);
        co[i].create(&runner, stack_len);
        co[i].start();
    }

    CASE_EXPECT_EQ(g_test_coroutine_base_status, 5);

    for (int i = 0; i < 4; ++ i)
        co[i].resume();

    CASE_EXPECT_EQ(g_test_coroutine_base_status, 9);

    CASE_EXPECT_EQ(runner.call_times, 4);

    delete []stack_buff;
}

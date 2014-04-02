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
    int operator()() {
        ++ g_test_coroutine_base_status;
        CASE_EXPECT_EQ(g_test_coroutine_base_status, 2);

        get_coroutine_context<test_context_base_coroutine_context_test_type>()->yield();

        ++ g_test_coroutine_base_status;
        CASE_EXPECT_EQ(g_test_coroutine_base_status, 4);

        return 0;
    }
};

CASE_TEST(coroutine, context_base)
{
    char* stack_buff = new char[64 * 1024];
    g_test_coroutine_base_status = 0;
    ++ g_test_coroutine_base_status;
    CASE_EXPECT_EQ(g_test_coroutine_base_status, 1);

    test_context_base_coroutine_context_test_type co;
    test_context_base_foo_runner runner;
    co.get_allocator().attach(stack_buff, 64 * 1024);
    co.create(&runner, 64 * 1024);
    co.start();

    ++ g_test_coroutine_base_status;
    CASE_EXPECT_EQ(g_test_coroutine_base_status, 3);
    co.resume();

    ++ g_test_coroutine_base_status;
    CASE_EXPECT_EQ(g_test_coroutine_base_status, 5);

    delete []stack_buff;
}

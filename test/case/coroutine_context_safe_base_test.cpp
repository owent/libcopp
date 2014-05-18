#include <iostream>
#include <cstdio>
#include <cstring>

#include <libcopp/coroutine/coroutine_context_container.h>
#include "frame/test_macros.h"

typedef copp::detail::coroutine_context_container<
    copp::detail::coroutine_context_safe_base,
    copp::allocator::default_statck_allocator
    > test_context_safe_base_context_type;

static int g_test_coroutine_safe_base_status = 0;

class test_context_safe_base_foo_runner : public copp::coroutine_runnable_base
{
public:
    int operator()()
    {
        ++ g_test_coroutine_safe_base_status;
        CASE_EXPECT_EQ(g_test_coroutine_safe_base_status, 2);

        copp::this_coroutine::get<test_context_safe_base_context_type>()->resume();
        ++ g_test_coroutine_safe_base_status;
        CASE_EXPECT_EQ(g_test_coroutine_safe_base_status, 3);

        copp::this_coroutine::get<test_context_safe_base_context_type>()->yield();
        ++ g_test_coroutine_safe_base_status;
        CASE_EXPECT_EQ(g_test_coroutine_safe_base_status, 5);

        return 0;
    }
};

CASE_TEST(coroutine, context_safe_base)
{
    g_test_coroutine_safe_base_status = 0;

    ++ g_test_coroutine_safe_base_status;
    CASE_EXPECT_EQ(g_test_coroutine_safe_base_status, 1);

    test_context_safe_base_context_type co;
    test_context_safe_base_foo_runner runner;
    co.create(&runner);
    co.start();

    ++ g_test_coroutine_safe_base_status;
    CASE_EXPECT_EQ(g_test_coroutine_safe_base_status, 4);
    co.resume();

    ++ g_test_coroutine_safe_base_status;
    CASE_EXPECT_EQ(g_test_coroutine_safe_base_status, 6);
}

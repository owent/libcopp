#include <cstdio>
#include <cstring>
#include <iostream>

#include "frame/test_macros.h"
#include <libcopp/coroutine/coroutine_context_container.h>


typedef copp::coroutine_context_container<copp::allocator::stack_allocator_memory>
    test_context_base_coroutine_context_test_type;

static int g_test_coroutine_base_status = 0;

class test_context_base_foo_runner {
public:
    int call_times;
    int operator()(void*) {
        ++call_times;
        ++g_test_coroutine_base_status;
        copp::this_coroutine::get<test_context_base_coroutine_context_test_type>()->yield();

        ++g_test_coroutine_base_status;
        return 0;
    }
};

CASE_TEST(coroutine, context_base) {
    char *stack_buff = new char[128 * 1024];
    g_test_coroutine_base_status = 0;
    ++g_test_coroutine_base_status;
    CASE_EXPECT_EQ(g_test_coroutine_base_status, 1);

    test_context_base_foo_runner runner;
    {
        copp::allocator::stack_allocator_memory alloc(stack_buff, 128 * 1024);
        test_context_base_coroutine_context_test_type::ptr_t co = test_context_base_coroutine_context_test_type::create(&runner, alloc);
        runner.call_times = 0;

        CASE_EXPECT_TRUE(!!co);

        co->start();

        ++g_test_coroutine_base_status;
        CASE_EXPECT_EQ(g_test_coroutine_base_status, 3);
        co->resume();

        ++g_test_coroutine_base_status;
        CASE_EXPECT_EQ(g_test_coroutine_base_status, 5);

        CASE_EXPECT_EQ(::copp::COPP_EC_NOT_READY, co->resume());
    }

    delete[] stack_buff;
}


CASE_TEST(coroutine, shared_runner) {
    const int stack_len = 128 * 1024;
    char *stack_buff = new char[4 * stack_len];
    g_test_coroutine_base_status = 0;
    ++g_test_coroutine_base_status;
    CASE_EXPECT_EQ(g_test_coroutine_base_status, 1);

    test_context_base_foo_runner runner;
    runner.call_times = 0;

    {
        test_context_base_coroutine_context_test_type::ptr_t co[4];
        for (int i = 0; i < 4; ++i) {
            copp::allocator::stack_allocator_memory alloc(stack_buff + i * stack_len, stack_len);
            co[i] = test_context_base_coroutine_context_test_type::create(&runner, alloc);
            co[i]->start();
        }

        CASE_EXPECT_EQ(g_test_coroutine_base_status, 5);

        for (int i = 0; i < 4; ++i) {
            co[i]->resume();
        }

        CASE_EXPECT_EQ(g_test_coroutine_base_status, 9);

        CASE_EXPECT_EQ(runner.call_times, 4);
    }

    delete[] stack_buff;
}

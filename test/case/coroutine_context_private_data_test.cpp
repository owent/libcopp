#include <cstdio>
#include <cstring>
#include <iostream>

#include "frame/test_macros.h"
#include <libcopp/coroutine/coroutine_context_container.h>

typedef copp::detail::coroutine_context_container<copp::detail::coroutine_context_base, copp::allocator::default_statck_allocator>
    test_context_private_data_context_type;

static int g_test_coroutine_private_data_status = 0;

class test_context_private_data_foo_runner : public copp::coroutine_runnable_base {
public:
    int operator()(void *priv_data) {
        ++g_test_coroutine_private_data_status;
        CASE_EXPECT_EQ(g_test_coroutine_private_data_status, 2);
        CASE_EXPECT_EQ(&g_test_coroutine_private_data_status, priv_data);

        copp::this_coroutine::get<test_context_private_data_context_type>()->resume();
        ++g_test_coroutine_private_data_status;
        CASE_EXPECT_EQ(g_test_coroutine_private_data_status, 3);

        copp::this_coroutine::get<test_context_private_data_context_type>()->yield(&priv_data);
        ++g_test_coroutine_private_data_status;

        CASE_EXPECT_EQ(copp::this_coroutine::get<test_context_private_data_context_type>(), priv_data);
        CASE_EXPECT_EQ(g_test_coroutine_private_data_status, 5);

        return 0;
    }
};

CASE_TEST(coroutine, context_private_data) {
    g_test_coroutine_private_data_status = 0;

    ++g_test_coroutine_private_data_status;
    CASE_EXPECT_EQ(g_test_coroutine_private_data_status, 1);

    test_context_private_data_context_type co;
    test_context_private_data_foo_runner runner;
    co.create(&runner);
    co.start(&g_test_coroutine_private_data_status);

    ++g_test_coroutine_private_data_status;
    CASE_EXPECT_EQ(g_test_coroutine_private_data_status, 4);
    co.resume(&co);

    ++g_test_coroutine_private_data_status;
    CASE_EXPECT_EQ(g_test_coroutine_private_data_status, 6);
}

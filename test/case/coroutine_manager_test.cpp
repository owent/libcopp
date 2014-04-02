#include <iostream>
#include <cstdio>
#include <cstring>

#include <libcopp/coroutine/coroutine_manager.h>
#include "frame/test_macros.h"

static int g_test_coroutine_manager_status = 0;
class test_context_manager_runner1 : public copp::coroutine_manager_runner_base
{
public:
    typedef copp::default_coroutine_manager::value_type value_type;
public:
    int operator()() {
        ++ g_test_coroutine_manager_status;
        CASE_EXPECT_EQ(g_test_coroutine_manager_status, 3);

        get_coroutine_context<value_type>()->yield();

        ++ g_test_coroutine_manager_status;
        CASE_EXPECT_EQ(g_test_coroutine_manager_status, 6);

        return 0;
    }
};

class test_context_manager_runner2 : public copp::coroutine_manager_runner_base
{
public:
    typedef copp::default_coroutine_manager::value_type value_type;

    test_context_manager_runner2(uint64_t key, copp::default_coroutine_manager* mgr):
        other_co_key(key), mgr_(mgr){}

public:
    int operator()() {
        ++ g_test_coroutine_manager_status;
        CASE_EXPECT_EQ(g_test_coroutine_manager_status, 2);

        mgr_->start(other_co_key);

        ++ g_test_coroutine_manager_status;
        CASE_EXPECT_EQ(g_test_coroutine_manager_status, 4);

        get_coroutine_context<value_type>()->yield();

        ++ g_test_coroutine_manager_status;
        CASE_EXPECT_EQ(g_test_coroutine_manager_status, 8);

        return 0;
    }

private:
    uint64_t other_co_key;
    copp::default_coroutine_manager* mgr_;
};

CASE_TEST(coroutine_manager, default_operation)
{
    copp::default_coroutine_manager g_co_mgr;
    g_test_coroutine_manager_status = 0;
    typedef copp::default_coroutine_manager::value_type co_type;

    co_type* pco1 = g_co_mgr.create();
    co_type* pco2 = g_co_mgr.create();
    pco1->create_runner<test_context_manager_runner1>(64 * 1024);
    pco2->create_runner<test_context_manager_runner2>(64 * 1024, pco1->get_key(), &g_co_mgr);

    CASE_EXPECT_EQ(g_co_mgr.size(), static_cast<std::size_t>(2));
    ++ g_test_coroutine_manager_status;
    CASE_EXPECT_EQ(g_test_coroutine_manager_status, 1);

    g_co_mgr.start(pco2->get_key());
    ++ g_test_coroutine_manager_status;
    CASE_EXPECT_EQ(g_test_coroutine_manager_status, 5);

    g_co_mgr.start(pco1->get_key());
    ++ g_test_coroutine_manager_status;
    CASE_EXPECT_EQ(g_test_coroutine_manager_status, 7);

    g_co_mgr.start(pco2->get_key());
    ++ g_test_coroutine_manager_status;
    CASE_EXPECT_EQ(g_test_coroutine_manager_status, 9);

    g_co_mgr.remove(pco1->get_key());
    CASE_EXPECT_EQ(g_co_mgr.size(), static_cast<std::size_t>(1));
}

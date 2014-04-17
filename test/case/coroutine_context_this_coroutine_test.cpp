/*
 * couroutine_context_this_coroutine_test.cpp
 *
 *  Created on: 2014年3月17日
 *      Author: owent
 *
 *  Released under the MIT license
 */

#if (defined(__cplusplus) && __cplusplus >= 201103L) || (defined(_MSC_VER) && _MSC_VER >= 1800)

#include <assert.h>
#include <iostream>
#include <cstdio>
#include <cstring>
#include <algorithm>

#include <thread>
#include <atomic>
#include <chrono>
#include <memory>
#include <functional>
#include <vector>

#include <libcopp/coroutine/coroutine_context_container.h>
#include "frame/test_macros.h"

class test_this_context_get_cotoutine_runner : public copp::coroutine_runnable_base
{
public:
    typedef copp::coroutine_context_default value_type;
    typedef value_type* value_ptr_type;

public:
    test_this_context_get_cotoutine_runner(): addr_(NULL), run_(false) {}
    int operator()() {
        ++ cur_thd_count;

        value_ptr_type this_co = dynamic_cast<value_ptr_type>(copp::this_coroutine::get_coroutine());
        CASE_EXPECT_EQ(addr_, this_co);
        run_ = true;

        std::chrono::milliseconds dura( 4 );
        std::this_thread::sleep_for( dura );

        max_thd_count = std::max(max_thd_count, cur_thd_count.load());

        -- cur_thd_count;
        return 0;
    }

    void set_co_obj(value_ptr_type addr) {
        addr_ = addr;
    }
    bool is_run() {
        return run_;
    }

    static int get_max_thd_count() {
        return max_thd_count;
    }
private:
    value_ptr_type addr_;
    bool run_;
    static std::atomic_int cur_thd_count;
    static int max_thd_count;
};

std::atomic_int test_this_context_get_cotoutine_runner::cur_thd_count;
int test_this_context_get_cotoutine_runner::max_thd_count = 0;

static void test_this_context_thread_func(copp::coroutine_context_default& co) {

    test_this_context_get_cotoutine_runner* runner = dynamic_cast<test_this_context_get_cotoutine_runner*>(co.get_runner());
    runner->set_co_obj(&co);

    CASE_EXPECT_FALSE(runner->is_run());

    co.start();

    CASE_EXPECT_TRUE(runner->is_run());
}

CASE_TEST(this_context, get_coroutine) {
    typedef copp::coroutine_context_default co_type;

    std::vector<std::thread> th_pool;

    co_type co_arr[5];
    for(co_type& co: co_arr) {
        co.create(new test_this_context_get_cotoutine_runner(), 32 * 1024);

        th_pool.push_back(std::thread(std::bind(
            test_this_context_thread_func,
            std::ref(co)
        )));
    }

    for(std::thread& th: th_pool) {
        th.join();
    }

    CASE_EXPECT_LT(1, test_this_context_get_cotoutine_runner::get_max_thd_count());

    for(co_type& co: co_arr) {
        assert(co.get_runner());
        delete dynamic_cast<test_this_context_get_cotoutine_runner*>(co.get_runner());
    }
}


class test_this_context_yield_runner : public copp::coroutine_runnable_base
{
public:
    typedef copp::coroutine_context_default value_type;
    typedef value_type* value_ptr_type;

public:
    test_this_context_yield_runner(): run_(false), finished_(false) {}
    int operator()() {
        run_ = true;
        copp::this_coroutine::yield();
        finished_ = true;
        return 0;
    }

    bool is_run() {
        return run_;
    }

    bool is_finished() {
        return finished_;
    }

private:
    bool run_;
    bool finished_;
};

CASE_TEST(this_context, yield) {
    typedef copp::coroutine_context_default co_type;

    co_type co;
    co.create(new test_this_context_yield_runner(), 32 * 1024);

    co.start();

    test_this_context_yield_runner* runner = dynamic_cast<test_this_context_yield_runner*>(co.get_runner());
    CASE_EXPECT_TRUE(runner->is_run());
    CASE_EXPECT_FALSE(runner->is_finished());

    assert(co.get_runner());
    delete runner;
}


#endif


/*
 * couroutine_context_this_coroutine_test.cpp
 *
 *  Created on: 2014年3月17日
 *      Author: owent
 *
 *  Released under the MIT license
 */

#if (defined(__cplusplus) && __cplusplus >= 201103L) || (defined(_MSC_VER) && _MSC_VER >= 1800)

#include <algorithm>
#include <assert.h>
#include <cstdio>
#include <cstring>
#include <iostream>

#include <atomic>
#include <chrono>
#include <functional>
#include <memory>
#include <thread>
#include <vector>

#include "frame/test_macros.h"
#include <libcopp/coroutine/coroutine_context_container.h>

class test_this_context_get_cotoutine_runner : public copp::coroutine_runnable_base {
public:
    typedef copp::coroutine_context_default value_type;
    typedef value_type *value_ptr_type;

public:
    test_this_context_get_cotoutine_runner() : addr_(NULL), run_(false) {}
    int operator()() {
        ++cur_thd_count;

        value_ptr_type this_co = dynamic_cast<value_ptr_type>(copp::this_coroutine::get_coroutine());
        CASE_EXPECT_EQ(addr_, this_co);
        run_ = true;

        std::chrono::milliseconds dura(4);
        std::this_thread::sleep_for(dura);

        using std::max;
        max_thd_count = max(max_thd_count, cur_thd_count.load());

        --cur_thd_count;
        return 0;
    }

    void set_co_obj(value_ptr_type addr) { addr_ = addr; }
    bool is_run() { return run_; }

    static int get_max_thd_count() { return max_thd_count; }

private:
    value_ptr_type addr_;
    bool run_;
    static std::atomic_int cur_thd_count;
    static int max_thd_count;
};

std::atomic_int test_this_context_get_cotoutine_runner::cur_thd_count;
int test_this_context_get_cotoutine_runner::max_thd_count = 0;

static void test_this_context_thread_func(copp::coroutine_context_default &co) {

    test_this_context_get_cotoutine_runner *runner = dynamic_cast<test_this_context_get_cotoutine_runner *>(co.get_runner());
    runner->set_co_obj(&co);

    CASE_EXPECT_FALSE(runner->is_run());

    CASE_EXPECT_EQ(NULL, copp::this_coroutine::get_coroutine());
    co.start();
    CASE_EXPECT_EQ(NULL, copp::this_coroutine::get_coroutine());

    CASE_EXPECT_TRUE(runner->is_run());
}

CASE_TEST(this_context, get_coroutine) {
    typedef copp::coroutine_context_default co_type;

    std::vector<std::thread> th_pool;

    co_type co_arr[5];
    for (co_type &co : co_arr) {
        co.create(new test_this_context_get_cotoutine_runner(), 32 * 1024);

        th_pool.push_back(std::thread(std::bind(test_this_context_thread_func, std::ref(co))));
    }

    for (std::thread &th : th_pool) {
        th.join();
    }

    CASE_EXPECT_LT(1, test_this_context_get_cotoutine_runner::get_max_thd_count());

    for (co_type &co : co_arr) {
        assert(co.get_runner());
        delete dynamic_cast<test_this_context_get_cotoutine_runner *>(co.get_runner());
    }
}


class test_this_context_yield_runner : public copp::coroutine_runnable_base {
public:
    typedef copp::coroutine_context_default value_type;
    typedef value_type *value_ptr_type;

public:
    test_this_context_yield_runner() : run_(false), finished_(false) {}
    int operator()() {
        run_ = true;

        copp::detail::coroutine_context_base *ptr = copp::this_coroutine::get_coroutine();

        CASE_EXPECT_EQ(ptr, copp::this_coroutine::get_coroutine()->get_private_data());

        copp::this_coroutine::yield();
        finished_ = true;

        CASE_EXPECT_EQ(ptr, copp::this_coroutine::get_coroutine());
        return 0;
    }

    bool is_run() { return run_; }

    bool is_finished() { return finished_; }

private:
    bool run_;
    bool finished_;
};

CASE_TEST(this_context, yield_) {
    typedef copp::coroutine_context_default co_type;

    co_type co;
    co.create(new test_this_context_yield_runner(), 32 * 1024);
    co.set_private_data(&co);

    CASE_EXPECT_EQ(NULL, copp::this_coroutine::get_coroutine());
    co.start();
    CASE_EXPECT_EQ(NULL, copp::this_coroutine::get_coroutine());

    test_this_context_yield_runner *runner = dynamic_cast<test_this_context_yield_runner *>(co.get_runner());
    CASE_EXPECT_TRUE(runner->is_run());
    CASE_EXPECT_FALSE(runner->is_finished());

    assert(co.get_runner());
    delete runner;
}


struct test_this_context_rec_runner : public copp::coroutine_runnable_base {
    typedef copp::coroutine_context_default value_type;
    typedef value_type *value_ptr_type;

    test_this_context_rec_runner(value_ptr_type p) : jump_to(NULL), owner(p), has_yield(false), has_resume(false) {}
    int operator()() {
        copp::detail::coroutine_context_base *ptr = copp::this_coroutine::get_coroutine();

        value_ptr_type *co_startup = reinterpret_cast<value_ptr_type *>(ptr->get_private_data());
        if (NULL == co_startup[0]) {
            co_startup[0] = dynamic_cast<value_ptr_type>(ptr);
        } else {
            co_startup[1] = dynamic_cast<value_ptr_type>(ptr);
        }

        CASE_EXPECT_EQ(ptr, owner);

        if (NULL != jump_to) {
            CASE_EXPECT_EQ(false, jump_to->has_yield);
            CASE_EXPECT_EQ(false, jump_to->has_resume);
            int res = jump_to->owner->start();
            CASE_EXPECT_EQ(true, jump_to->has_yield);
            CASE_EXPECT_EQ(false, jump_to->has_resume);

            jump_to->has_resume = true;
            jump_to->owner->resume();
            CASE_EXPECT_EQ(0, res);
        }

        ptr = copp::this_coroutine::get_coroutine();
        CASE_EXPECT_EQ(ptr, owner);

        CASE_EXPECT_EQ(false, has_yield);
        CASE_EXPECT_EQ(false, has_resume);

        has_yield = true;
        owner->yield();

        CASE_EXPECT_EQ(true, has_yield);
        CASE_EXPECT_EQ(true, has_resume);
        return 0;
    }

    test_this_context_rec_runner *jump_to;
    value_ptr_type owner;
    bool has_yield;
    bool has_resume;
};

CASE_TEST(this_context, start_in_co) {
    typedef test_this_context_rec_runner::value_type co_type;

    co_type *co_startup[2] = {NULL};
    co_type co1, co2;
    test_this_context_rec_runner cor1(&co1), cor2(&co2);

    co1.create(&cor1, 32 * 1024);
    co2.create(&cor2, 32 * 1024);
    co1.set_private_data(co_startup);
    co2.set_private_data(co_startup);

    CASE_EXPECT_EQ(NULL, copp::this_coroutine::get_coroutine());
    cor1.jump_to = &cor2;

    CASE_EXPECT_EQ(false, cor1.has_yield);
    CASE_EXPECT_EQ(false, cor1.has_resume);
    co1.start();
    CASE_EXPECT_EQ(true, cor1.has_yield);
    CASE_EXPECT_EQ(false, cor1.has_resume);
    cor1.has_resume = true;
    co1.resume();

    CASE_EXPECT_EQ(NULL, copp::this_coroutine::get_coroutine());

    CASE_EXPECT_EQ(&co1, co_startup[0]);
    CASE_EXPECT_EQ(&co2, co_startup[1]);
}


#endif

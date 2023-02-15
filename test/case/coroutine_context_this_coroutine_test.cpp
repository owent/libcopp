// Copyright 2023 owent

#include "frame/test_macros.h"

#include <assert.h>
#include <algorithm>
#include <cstdio>
#include <cstring>
#include <iostream>

#include <atomic>
#include <chrono>
#include <functional>
#include <memory>
#include <thread>
#include <vector>

#include <libcopp/coroutine/coroutine_context_container.h>
#include <libcotask/task.h>

class test_this_context_get_cotoutine_runner {
 public:
  typedef copp::coroutine_context_default value_type;
  typedef value_type *value_ptr_type;

 public:
  test_this_context_get_cotoutine_runner() : addr_(nullptr), run_(false) {}
  int operator()(void *passed) {
    CASE_EXPECT_EQ(this, passed);

    ++cur_thd_count;

    value_ptr_type this_co = static_cast<value_ptr_type>(copp::this_coroutine::get_coroutine());
    CASE_EXPECT_EQ(addr_, this_co);
#ifdef LIBCOTASK_MACRO_ENABLED
    CASE_EXPECT_EQ(nullptr, cotask::this_task::get_task());
#endif
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

static void test_this_context_thread_func(copp::coroutine_context_default::ptr_t co,
                                          test_this_context_get_cotoutine_runner *runner) {
  runner->set_co_obj(co.get());

  CASE_EXPECT_FALSE(runner->is_run());

  CASE_EXPECT_EQ(nullptr, copp::this_coroutine::get_coroutine());
  co->start(runner);
  CASE_EXPECT_EQ(nullptr, copp::this_coroutine::get_coroutine());

  CASE_EXPECT_TRUE(runner->is_run());
}

CASE_TEST(this_context, get_coroutine) {
  typedef copp::coroutine_context_default co_type;

  test_this_context_get_cotoutine_runner runners[5];
  {
    std::vector<std::thread> th_pool;
    co_type::ptr_t co_arr[5];
    for (int i = 0; i < 5; ++i) {
      co_arr[i] = co_type::create(&runners[i], 128 * 1024);

      th_pool.push_back(
          std::thread([&co_arr, &runners, i]() { test_this_context_thread_func(co_arr[i], &runners[i]); }));
    }

    for (std::thread &th : th_pool) {
      th.join();
    }

    CASE_EXPECT_LT(1, test_this_context_get_cotoutine_runner::get_max_thd_count());
  }
}

class test_this_context_yield_runner {
 public:
  typedef copp::coroutine_context_default value_type;
  typedef value_type *value_ptr_type;

 public:
  test_this_context_yield_runner() : run_(false), finished_(false) {}
  int operator()(void *) {
    run_ = true;
    copp::this_coroutine::yield();
    finished_ = true;
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

  test_this_context_yield_runner runner;
  {
    co_type::ptr_t co = co_type::create(&runner, 128 * 1024);

    CASE_EXPECT_EQ(nullptr, copp::this_coroutine::get_coroutine());
    co->start();
    CASE_EXPECT_EQ(nullptr, copp::this_coroutine::get_coroutine());

    CASE_EXPECT_TRUE(runner.is_run());
    CASE_EXPECT_FALSE(runner.is_finished());
  }
}

struct test_this_context_rec_runner {
  typedef copp::coroutine_context_default value_type;
  typedef value_type *value_ptr_type;

  test_this_context_rec_runner() : jump_to(nullptr), owner(nullptr), has_yield(false), has_resume(false) {}
  int operator()(void *co_startup_raw) {
    copp::coroutine_context *ptr = copp::this_coroutine::get_coroutine();

    value_ptr_type *co_startup = reinterpret_cast<value_ptr_type *>(co_startup_raw);
    if (nullptr == co_startup[0]) {
      co_startup[0] = static_cast<value_ptr_type>(ptr);
    } else {
      co_startup[1] = static_cast<value_ptr_type>(ptr);
    }

    CASE_EXPECT_EQ(ptr, owner);

    if (nullptr != jump_to && nullptr != jump_to->owner) {
      CASE_EXPECT_EQ(false, jump_to->has_yield);
      CASE_EXPECT_EQ(false, jump_to->has_resume);
      int res = jump_to->owner->start(co_startup_raw);
      CASE_EXPECT_EQ(true, jump_to->has_yield);
      CASE_EXPECT_EQ(false, jump_to->has_resume);

      jump_to->has_resume = true;
      jump_to->owner->resume(co_startup_raw);
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

  co_type *co_startup[2] = {nullptr};

  test_this_context_rec_runner cor1, cor2;

  {
    co_type::ptr_t co1, co2;
    co1 = co_type::create(&cor1, 128 * 1024);
    co2 = co_type::create(&cor2, 128 * 1024);
    cor1.owner = co1.get();
    cor2.owner = co2.get();

    CASE_EXPECT_EQ(nullptr, copp::this_coroutine::get_coroutine());
    cor1.jump_to = &cor2;

    CASE_EXPECT_EQ(false, cor1.has_yield);
    CASE_EXPECT_EQ(false, cor1.has_resume);
    co1->start(co_startup);
    CASE_EXPECT_EQ(true, cor1.has_yield);
    CASE_EXPECT_EQ(false, cor1.has_resume);
    cor1.has_resume = true;
    co1->resume(co_startup);

    CASE_EXPECT_EQ(nullptr, copp::this_coroutine::get_coroutine());

    CASE_EXPECT_EQ(co1.get(), co_startup[0]);
    CASE_EXPECT_EQ(co2.get(), co_startup[1]);
  }
}

struct test_this_context_start_failed_when_running {
  typedef copp::coroutine_context_default value_type;

  test_this_context_start_failed_when_running() : is_start(false) {}
  int operator()(void *pco2) {
    value_type *ptr = copp::this_coroutine::get<value_type>();

    value_type *co_jump = reinterpret_cast<value_type *>(pco2);

    if (is_start) {
      CASE_EXPECT_NE(nullptr, co_jump);
      co_jump->start(ptr);
    } else {
      CASE_EXPECT_NE(nullptr, co_jump);
      int res = co_jump->start(ptr);  // this should be COPP_EC_IS_RUNNING
      CASE_EXPECT_EQ(copp::COPP_EC_IS_RUNNING, res);
    }

    // finished and return to caller
    return 0;
  }

  bool is_start;
};

CASE_TEST(this_context, start_failed_when_running) {
  typedef test_this_context_start_failed_when_running::value_type co_type;

  test_this_context_start_failed_when_running cor1, cor2;

  {
    co_type::ptr_t co1, co2;
    co1 = co_type::create(&cor1, 128 * 1024);
    co2 = co_type::create(&cor2, 128 * 1024);

    cor1.is_start = true;
    co1->start(co2.get());
  }
}

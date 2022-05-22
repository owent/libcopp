// Copyright 2022 owent

#include <libcopp/coroutine/callable_promise.h>
#include <libcopp/coroutine/generator_promise.h>

#include <cstdio>
#include <cstring>
#include <iostream>
#include <list>
#include <string>

#include "frame/test_macros.h"

#if defined(LIBCOPP_MACRO_ENABLE_STD_COROUTINE) && LIBCOPP_MACRO_ENABLE_STD_COROUTINE

using generator_promise_future_int_type = copp::generator_future<int>;
using generator_promise_future_void_type = copp::generator_future<void>;

namespace {
std::list<generator_promise_future_int_type::context_pointer_type> g_pending_int_contexts;
std::list<generator_promise_future_void_type::context_pointer_type> g_pending_void_contexts;
size_t g_resume_generator_count = 0;
size_t g_suspend_generator_count = 0;

void resume_pending_contexts(std::list<int> values) {
  while (!g_pending_int_contexts.empty() || !g_pending_void_contexts.empty()) {
    if (!g_pending_int_contexts.empty()) {
      auto ctx = *g_pending_int_contexts.begin();
      g_pending_int_contexts.pop_front();

      if (!values.empty()) {
        int val = values.front();
        values.pop_front();
        ctx->set_value(val);
      } else {
        ctx->set_value(0);
      }
    }

    if (!g_pending_void_contexts.empty()) {
      auto ctx = *g_pending_void_contexts.begin();
      g_pending_void_contexts.pop_front();
      ctx->set_value();
    }
  }
}

}  // namespace

static copp::callable_future<int> callable_func_await_int() {
  generator_promise_future_int_type gen_left_value{
      [](generator_promise_future_int_type::context_pointer_type ctx) {
        ++g_suspend_generator_count;
        g_pending_int_contexts.push_back(ctx);
      },
      [](const generator_promise_future_int_type::context_type&) { ++g_resume_generator_count; }};

  // await left value
  CASE_EXPECT_FALSE(gen_left_value.is_ready());
  CASE_EXPECT_TRUE(gen_left_value.is_pending());
  CASE_EXPECT_TRUE(gen_left_value.get_status() == copp::promise_status::kRunning);
  int x1 = co_await gen_left_value;
  CASE_EXPECT_TRUE(gen_left_value.is_ready());
  CASE_EXPECT_FALSE(gen_left_value.is_pending());
  CASE_EXPECT_TRUE(gen_left_value.get_status() == copp::promise_status::kDone);

  // Await a ready generator will be ignored and will not incease suspend count
  CASE_EXPECT_EQ(x1, co_await gen_left_value);

  // await right value
  int x2 = co_await generator_promise_future_int_type{
      [](generator_promise_future_int_type::context_pointer_type ctx) {
        ++g_suspend_generator_count;
        g_pending_int_contexts.push_back(ctx);
      },
      [](const generator_promise_future_int_type::context_type&) { ++g_resume_generator_count; }};

  generator_promise_future_void_type gen_left_void{
      [](generator_promise_future_void_type::context_pointer_type ctx) {
        ++g_suspend_generator_count;
        g_pending_void_contexts.push_back(ctx);
      },
      [](const generator_promise_future_void_type::context_type&) { ++g_resume_generator_count; }};

  // await left value
  CASE_EXPECT_FALSE(gen_left_void.is_ready());
  CASE_EXPECT_TRUE(gen_left_void.is_pending());
  CASE_EXPECT_TRUE(gen_left_void.get_status() == copp::promise_status::kRunning);
  co_await gen_left_void;
  CASE_EXPECT_TRUE(gen_left_void.is_ready());
  CASE_EXPECT_FALSE(gen_left_void.is_pending());
  CASE_EXPECT_TRUE(gen_left_void.get_status() == copp::promise_status::kDone);

  // Await a ready generator will be ignored and will not incease suspend count
  co_await gen_left_void;

  // await right value
  co_await generator_promise_future_void_type{
      [](generator_promise_future_void_type::context_pointer_type ctx) {
        ++g_suspend_generator_count;
        g_pending_void_contexts.push_back(ctx);
      },
      [](const generator_promise_future_void_type::context_type&) { ++g_resume_generator_count; }};

  co_return x1 + x2;
}

CASE_TEST(generator_promise, basic_int_generator) {
  size_t old_resume_generator_count = g_resume_generator_count;
  size_t old_suspend_generator_count = g_suspend_generator_count;

  copp::callable_future<int> f = callable_func_await_int();
  CASE_EXPECT_EQ(static_cast<int>(copp::promise_status::kCreated), static_cast<int>(f.get_status()));
  // Start
  f.start();

  CASE_EXPECT_NE(static_cast<int>(copp::promise_status::kDone), static_cast<int>(f.get_status()));
  CASE_EXPECT_FALSE(f.is_ready());

  resume_pending_contexts({13100, 13});

  CASE_EXPECT_TRUE(f.is_ready());
  CASE_EXPECT_EQ(13113, f.get_internal_promise().data());

  CASE_EXPECT_EQ(old_resume_generator_count + 4, g_resume_generator_count);
  CASE_EXPECT_EQ(old_suspend_generator_count + 4, g_suspend_generator_count);
}

static copp::callable_future<int> callable_func_await_int_killed() {
  generator_promise_future_int_type gen_left_value{[](generator_promise_future_int_type::context_pointer_type ctx) {
                                                     ++g_suspend_generator_count;
                                                     g_pending_int_contexts.push_back(ctx);
                                                   },
                                                   [](const generator_promise_future_int_type::context_type& ctx) {
                                                     CASE_EXPECT_FALSE(ctx.is_ready());
                                                     CASE_EXPECT_TRUE(ctx.is_pending());
                                                     ++g_resume_generator_count;
                                                   }};

  // await left value
  CASE_EXPECT_FALSE(gen_left_value.is_ready());
  CASE_EXPECT_TRUE(gen_left_value.is_pending());
  CASE_EXPECT_TRUE(gen_left_value.get_status() == copp::promise_status::kRunning);
  int x1 = co_await gen_left_value;
  CASE_EXPECT_FALSE(gen_left_value.is_ready());
  CASE_EXPECT_TRUE(gen_left_value.is_pending());
  CASE_EXPECT_TRUE(gen_left_value.get_status() == copp::promise_status::kRunning);

  auto current_callable_status = co_yield copp::callable_future<int>::yield_status();
  CASE_EXPECT_TRUE(current_callable_status == copp::promise_status::kKilled);

  // All co_await will be ignored after caller is killed
  co_await gen_left_value;

  generator_promise_future_void_type gen_left_void{[](generator_promise_future_void_type::context_pointer_type ctx) {
                                                     ++g_suspend_generator_count;
                                                     g_pending_void_contexts.push_back(ctx);
                                                   },
                                                   [](const generator_promise_future_void_type::context_type& ctx) {
                                                     CASE_EXPECT_FALSE(ctx.is_ready());
                                                     CASE_EXPECT_TRUE(ctx.is_pending());
                                                     ++g_resume_generator_count;
                                                   }};

  // All co_await will be ignored after caller is killed
  CASE_EXPECT_FALSE(gen_left_void.is_ready());
  CASE_EXPECT_TRUE(gen_left_void.is_pending());
  CASE_EXPECT_TRUE(gen_left_void.get_status() == copp::promise_status::kRunning);
  co_await gen_left_void;
  CASE_EXPECT_FALSE(gen_left_void.is_ready());
  CASE_EXPECT_TRUE(gen_left_void.is_pending());
  CASE_EXPECT_TRUE(gen_left_void.get_status() == copp::promise_status::kRunning);

  co_return x1;
}

static copp::callable_future<void> callable_func_await_void_killed() {
  generator_promise_future_void_type gen_left_void{[](generator_promise_future_void_type::context_pointer_type ctx) {
                                                     ++g_suspend_generator_count;
                                                     g_pending_void_contexts.push_back(ctx);
                                                   },
                                                   [](const generator_promise_future_void_type::context_type& ctx) {
                                                     CASE_EXPECT_FALSE(ctx.is_ready());
                                                     CASE_EXPECT_TRUE(ctx.is_pending());
                                                     ++g_resume_generator_count;
                                                   }};

  // await left value
  CASE_EXPECT_FALSE(gen_left_void.is_ready());
  CASE_EXPECT_TRUE(gen_left_void.is_pending());
  CASE_EXPECT_TRUE(gen_left_void.get_status() == copp::promise_status::kRunning);
  co_await gen_left_void;
  CASE_EXPECT_FALSE(gen_left_void.is_ready());
  CASE_EXPECT_TRUE(gen_left_void.is_pending());
  CASE_EXPECT_TRUE(gen_left_void.get_status() == copp::promise_status::kRunning);

  // All co_await will be ignored after caller is killed
  co_await gen_left_void;

  generator_promise_future_int_type gen_left_value{[](generator_promise_future_int_type::context_pointer_type ctx) {
                                                     ++g_suspend_generator_count;
                                                     g_pending_int_contexts.push_back(ctx);
                                                   },
                                                   [](const generator_promise_future_int_type::context_type& ctx) {
                                                     CASE_EXPECT_FALSE(ctx.is_ready());
                                                     CASE_EXPECT_TRUE(ctx.is_pending());
                                                     ++g_resume_generator_count;
                                                   }};

  // All co_await will be ignored after caller is killed
  CASE_EXPECT_FALSE(gen_left_value.is_ready());
  CASE_EXPECT_TRUE(gen_left_value.is_pending());
  CASE_EXPECT_TRUE(gen_left_value.get_status() == copp::promise_status::kRunning);
  co_await gen_left_value;
  CASE_EXPECT_FALSE(gen_left_value.is_ready());
  CASE_EXPECT_TRUE(gen_left_value.is_pending());
  CASE_EXPECT_TRUE(gen_left_value.get_status() == copp::promise_status::kRunning);

  auto current_callable_status = co_yield copp::callable_future<int>::yield_status();
  CASE_EXPECT_TRUE(current_callable_status == copp::promise_status::kKilled);

  co_return;
}

CASE_TEST(generator_promise, caller_killed) {
  {
    size_t old_resume_generator_count = g_resume_generator_count;
    size_t old_suspend_generator_count = g_suspend_generator_count;

    copp::callable_future<int> f = callable_func_await_int_killed();
    CASE_EXPECT_EQ(static_cast<int>(copp::promise_status::kCreated), static_cast<int>(f.get_status()));
    // Start
    f.start();

    // Mock to kill by caller
    f.get_internal_promise().set_status(copp::promise_status::kKilled);
    f.start();
    CASE_EXPECT_TRUE(f.is_ready());

    CASE_EXPECT_EQ(old_resume_generator_count + 1, g_resume_generator_count);
    CASE_EXPECT_EQ(old_suspend_generator_count + 1, g_suspend_generator_count);
  }

  {
    size_t old_resume_generator_count = g_resume_generator_count;
    size_t old_suspend_generator_count = g_suspend_generator_count;

    copp::callable_future<void> f = callable_func_await_void_killed();
    CASE_EXPECT_EQ(static_cast<int>(copp::promise_status::kCreated), static_cast<int>(f.get_status()));
    // Start
    f.start();

    // Mock to kill by caller
    f.get_internal_promise().set_status(copp::promise_status::kKilled);
    f.start();
    CASE_EXPECT_TRUE(f.is_ready());

    CASE_EXPECT_EQ(old_resume_generator_count + 1, g_resume_generator_count);
    CASE_EXPECT_EQ(old_suspend_generator_count + 1, g_suspend_generator_count);
  }

  resume_pending_contexts({});
}

#else
CASE_TEST(generator_promise, disabled) {}
#endif
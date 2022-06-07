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

size_t resume_pending_contexts(std::list<int> values) {
  size_t ret = 0;
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

      ++ret;
    }

    if (!g_pending_void_contexts.empty()) {
      auto ctx = *g_pending_void_contexts.begin();
      g_pending_void_contexts.pop_front();
      ctx->set_value();

      ++ret;
    }
  }

  return ret;
}

}  // namespace

static copp::callable_future<int> callable_func_await_int() {
  generator_promise_future_int_type gen_left_value{
      [](generator_promise_future_int_type::context_pointer_type ctx) {
        ++g_suspend_generator_count;
        g_pending_int_contexts.push_back(ctx);
      },
      [](const generator_promise_future_int_type::context_type &) { ++g_resume_generator_count; }};

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
      [](const generator_promise_future_int_type::context_type &) { ++g_resume_generator_count; }};

  generator_promise_future_void_type gen_left_void{
      [](generator_promise_future_void_type::context_pointer_type ctx) {
        ++g_suspend_generator_count;
        g_pending_void_contexts.push_back(ctx);
      },
      [](const generator_promise_future_void_type::context_type &) { ++g_resume_generator_count; }};

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
      [](const generator_promise_future_void_type::context_type &) { ++g_resume_generator_count; }};

  co_return x1 + x2;
}

CASE_TEST(generator_promise, basic_int_generator) {
  size_t old_resume_generator_count = g_resume_generator_count;
  size_t old_suspend_generator_count = g_suspend_generator_count;

  copp::callable_future<int> f = callable_func_await_int();

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
                                                   [](const generator_promise_future_int_type::context_type &ctx) {
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
                                                   [](const generator_promise_future_void_type::context_type &ctx) {
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
                                                   [](const generator_promise_future_void_type::context_type &ctx) {
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
                                                   [](const generator_promise_future_int_type::context_type &ctx) {
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

    // Mock to kill by caller
    f.kill(copp::promise_status::kKilled, true);
    CASE_EXPECT_TRUE(f.is_ready());

    CASE_EXPECT_EQ(old_resume_generator_count + 1, g_resume_generator_count);
    CASE_EXPECT_EQ(old_suspend_generator_count + 1, g_suspend_generator_count);
  }

  {
    size_t old_resume_generator_count = g_resume_generator_count;
    size_t old_suspend_generator_count = g_suspend_generator_count;

    copp::callable_future<void> f = callable_func_await_void_killed();

    // Mock to kill by caller
    f.kill(copp::promise_status::kKilled, true);
    CASE_EXPECT_TRUE(f.is_ready());

    CASE_EXPECT_EQ(old_resume_generator_count + 1, g_resume_generator_count);
    CASE_EXPECT_EQ(old_suspend_generator_count + 1, g_suspend_generator_count);
  }

  resume_pending_contexts({});
}

class test_context_transform_error_code_type {
 public:
  int code;

  test_context_transform_error_code_type() : code(0) {}
  test_context_transform_error_code_type(int c) : code(c) {}
  test_context_transform_error_code_type(const test_context_transform_error_code_type &) = default;
  test_context_transform_error_code_type &operator=(const test_context_transform_error_code_type &) = default;
  test_context_transform_error_code_type(test_context_transform_error_code_type &&) = default;
  test_context_transform_error_code_type &operator=(test_context_transform_error_code_type &&) = default;
  ~test_context_transform_error_code_type() {}
};

namespace {
using test_context_transform_error_code_generator = copp::generator_future<test_context_transform_error_code_type>;
std::list<test_context_transform_error_code_generator::context_pointer_type>
    g_test_context_transform_error_code_generator_executor;

static copp::callable_future<int> test_context_transform_error_code_generator_l2() {
  auto result = co_await test_context_transform_error_code_generator{
      [](test_context_transform_error_code_generator::context_pointer_type ctx) {
        g_test_context_transform_error_code_generator_executor.emplace_back(std::move(ctx));
      }};
  co_return result.code;
}

static copp::callable_future<void> test_context_transform_error_code_generator_l1() {
  auto result = co_await test_context_transform_error_code_generator_l2();
  CASE_EXPECT_EQ(result, -500);
  co_return;
}
}  // namespace

LIBCOPP_COPP_NAMESPACE_BEGIN
template <>
struct std_coroutine_default_error_transform<test_context_transform_error_code_type> {
  using type = test_context_transform_error_code_type;
  type operator()(promise_status in) const {
    if (in == promise_status::kTimeout) {
      return test_context_transform_error_code_type{-500};
    }
    return test_context_transform_error_code_type{static_cast<int>(in)};
  }
};
LIBCOPP_COPP_NAMESPACE_END

CASE_TEST(generator_promise, transform_error_code) {
  auto f2 = test_context_transform_error_code_generator_l1();
  f2.kill(copp::promise_status::kTimeout, true);

  g_test_context_transform_error_code_generator_executor.clear();
}

static copp::callable_future<int> callable_func_multiple_await_int(
    generator_promise_future_int_type &shared_generator) {
  auto result = co_await shared_generator;
  co_return result;
}

CASE_TEST(generator_promise, miltiple_wait_int_generator) {
  size_t old_resume_generator_count = g_resume_generator_count;
  size_t old_suspend_generator_count = g_suspend_generator_count;

  generator_promise_future_int_type int_generator{
      [](generator_promise_future_int_type::context_pointer_type ctx) {
        ++g_suspend_generator_count;
        if (g_pending_int_contexts.end() ==
            std::find(g_pending_int_contexts.begin(), g_pending_int_contexts.end(), ctx)) {
          g_pending_int_contexts.push_back(ctx);
        }
      },
      [](const generator_promise_future_int_type::context_type &) { ++g_resume_generator_count; }};

  copp::callable_future<int> f1 = callable_func_multiple_await_int(int_generator);
  copp::callable_future<int> f2 = callable_func_multiple_await_int(int_generator);

  CASE_EXPECT_NE(static_cast<int>(copp::promise_status::kDone), static_cast<int>(f1.get_status()));
  CASE_EXPECT_FALSE(f1.is_ready());
  CASE_EXPECT_NE(static_cast<int>(copp::promise_status::kDone), static_cast<int>(f2.get_status()));
  CASE_EXPECT_FALSE(f2.is_ready());

  CASE_EXPECT_EQ(1, resume_pending_contexts({143}));

  CASE_EXPECT_TRUE(f1.is_ready());
  CASE_EXPECT_EQ(143, f1.get_internal_promise().data());
  CASE_EXPECT_TRUE(f2.is_ready());
  CASE_EXPECT_EQ(143, f2.get_internal_promise().data());

  CASE_EXPECT_EQ(old_resume_generator_count + 2, g_resume_generator_count);
  CASE_EXPECT_EQ(old_suspend_generator_count + 2, g_suspend_generator_count);
}

#else
CASE_TEST(generator_promise, disabled) {}
#endif
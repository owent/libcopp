// Copyright 2022 owent

#include <libcopp/coroutine/callable_promise.h>
#include <libcopp/coroutine/generator_promise.h>
#include <libcotask/task_promise.h>

#include <cstdio>
#include <cstring>
#include <iostream>
#include <list>
#include <string>

#include "frame/test_macros.h"

#if defined(LIBCOPP_MACRO_ENABLE_STD_COROUTINE) && LIBCOPP_MACRO_ENABLE_STD_COROUTINE

struct task_future_private_data {
  int data;
};

using task_future_int_type = cotask::task_future<int, task_future_private_data>;
using task_future_void_type = cotask::task_future<void, task_future_private_data>;
using task_future_int_type_no_private_data = cotask::task_future<int, void>;
using task_future_void_type_no_private_data = cotask::task_future<void, void>;
using callable_future_int_type = copp::callable_future<int>;
using callable_future_void_type = copp::callable_future<void>;
using generator_future_int_type = copp::generator_future<int>;
using generator_future_void_type = copp::generator_future<void>;

namespace {
std::list<generator_future_int_type::context_pointer_type> g_task_future_pending_int_contexts;
std::list<generator_future_void_type::context_pointer_type> g_task_future_pending_void_contexts;
size_t g_task_future_resume_generator_count = 0;
size_t g_task_future_suspend_generator_count = 0;

static size_t resume_pending_contexts(std::list<int> values) {
  size_t ret = 0;
  while (!g_task_future_pending_int_contexts.empty() || !g_task_future_pending_void_contexts.empty()) {
    if (!g_task_future_pending_int_contexts.empty()) {
      auto ctx = *g_task_future_pending_int_contexts.begin();
      g_task_future_pending_int_contexts.pop_front();

      if (!values.empty()) {
        int val = values.front();
        values.pop_front();
        ctx->set_value(val);
      } else {
        ctx->set_value(0);
      }

      ++ret;
    }

    if (!g_task_future_pending_void_contexts.empty()) {
      auto ctx = *g_task_future_pending_void_contexts.begin();
      g_task_future_pending_void_contexts.pop_front();
      ctx->set_value();

      ++ret;
    }
  }

  return ret;
}

static void generator_int_suspend_callback(generator_future_int_type::context_pointer_type ctx) {
  ++g_task_future_suspend_generator_count;
  g_task_future_pending_int_contexts.push_back(ctx);
}
static void generator_int_resume_callback(const generator_future_int_type::context_type &) {
  ++g_task_future_resume_generator_count;
}

static callable_future_int_type callable_func_int_l1(int inout) {
  CASE_MSG_INFO() << "callable inner future ready int: " << inout << std::endl;
  co_return inout;
}

static callable_future_int_type callable_func_int_l2(int inout) {
  generator_future_int_type generator{generator_int_suspend_callback, generator_int_resume_callback};
  auto gen_res = co_await generator;
  if (gen_res < 0) {
    co_return gen_res;
  }
  co_return inout + gen_res;
}

static callable_future_int_type callable_func_await_int() {
  auto v = callable_func_int_l1(3);
  auto u = callable_func_int_l2(11);
  int x = (co_await v + co_await u);
  co_return x;
}

static task_future_int_type task_func_await_int() {
  auto current_status = co_yield task_future_void_type::yield_status();
  CASE_EXPECT_EQ(static_cast<int>(copp::promise_status::kRunning), static_cast<int>(current_status));
  auto private_data = co_yield cotask::task_private_data<task_future_private_data>();
  private_data->data = 121000;

  CASE_MSG_INFO() << "task await int" << std::endl;
  auto v = callable_func_await_int();
  int x = co_await v;
  CASE_MSG_INFO() << "task return int" << std::endl;
  co_return x;
}

static callable_future_int_type callable_func_no_wait_int() {
  auto v = callable_func_int_l1(7);
  auto u = callable_func_int_l1(19);
  int x = (co_await v + co_await u);
  co_return x;
}

static task_future_int_type task_func_no_wait_int() {
  auto current_status = co_yield task_future_void_type::yield_status();
  CASE_EXPECT_EQ(static_cast<int>(copp::promise_status::kRunning), static_cast<int>(current_status));
  auto private_data = co_yield cotask::task_private_data<task_future_private_data>();
  private_data->data = 121000;

  CASE_MSG_INFO() << "task await int" << std::endl;
  auto v = callable_func_no_wait_int();
  int x = co_await v;
  CASE_MSG_INFO() << "task return int" << std::endl;
  co_return x;
}

static void generator_void_suspend_callback(generator_future_void_type::context_pointer_type ctx) {
  ++g_task_future_suspend_generator_count;
  g_task_future_pending_void_contexts.push_back(ctx);
}
static void generator_void_resume_callback(const generator_future_void_type::context_type &) {
  ++g_task_future_resume_generator_count;
}

static callable_future_void_type callable_func_void_l1() {
  CASE_MSG_INFO() << "callable inner future ready void" << std::endl;
  co_return;
}

static callable_future_void_type callable_func_void_l2() {
  generator_future_void_type generator{generator_void_suspend_callback, generator_void_resume_callback};
  co_await generator;
  co_return;
}

static callable_future_void_type callable_func_await_void() {
  auto v = callable_func_void_l1();
  auto u = callable_func_void_l2();
  co_await v;
  co_await u;
  co_return;
}

static task_future_void_type task_func_await_void() {
  auto current_status = co_yield task_future_void_type::yield_status();
  CASE_EXPECT_EQ(static_cast<int>(copp::promise_status::kRunning), static_cast<int>(current_status));
  auto private_data = co_yield cotask::task_private_data<task_future_private_data>();
  private_data->data = 123000;

  CASE_MSG_INFO() << "task await void" << std::endl;
  auto v = callable_func_await_void();
  co_await v;
  CASE_MSG_INFO() << "task return void" << std::endl;
  co_return;
}

static callable_future_void_type callable_func_no_wait_void() {
  auto v = callable_func_void_l1();
  auto u = callable_func_void_l1();
  co_await v;
  co_await u;
  co_return;
}

static task_future_void_type task_func_no_wait_void() {
  auto current_status = co_yield task_future_void_type::yield_status();
  CASE_EXPECT_EQ(static_cast<int>(copp::promise_status::kRunning), static_cast<int>(current_status));
  auto private_data = co_yield cotask::task_private_data<task_future_private_data>();
  private_data->data = 123000;

  CASE_MSG_INFO() << "task await void" << std::endl;
  auto v = callable_func_no_wait_void();
  co_await v;
  CASE_MSG_INFO() << "task return void" << std::endl;
  co_return;
}

}  // namespace

CASE_TEST(task_promise, task_future_integer_need_resume) {
  size_t old_resume_generator_count = g_task_future_resume_generator_count;
  size_t old_suspend_generator_count = g_task_future_suspend_generator_count;

  task_future_int_type t = task_func_await_int();
  CASE_EXPECT_EQ(static_cast<int>(copp::promise_status::kCreated), static_cast<int>(t.get_status()));
  CASE_EXPECT_FALSE(t.is_canceled());
  CASE_EXPECT_FALSE(t.is_faulted());
  CASE_EXPECT_FALSE(t.is_timeout());
  CASE_EXPECT_FALSE(t.is_exiting());
  CASE_EXPECT_FALSE(t.is_completed());

  CASE_EXPECT_TRUE(t.start());
  CASE_EXPECT_EQ(121000, t.get_private_data()->data);
  CASE_EXPECT_EQ(static_cast<int>(copp::promise_status::kRunning), static_cast<int>(t.get_status()));
  CASE_EXPECT_FALSE(t.is_canceled());
  CASE_EXPECT_FALSE(t.is_faulted());
  CASE_EXPECT_FALSE(t.is_timeout());
  CASE_EXPECT_FALSE(t.is_exiting());
  CASE_EXPECT_FALSE(t.is_completed());

  resume_pending_contexts({100});
  CASE_EXPECT_EQ(static_cast<int>(copp::promise_status::kDone), static_cast<int>(t.get_status()));
  CASE_EXPECT_FALSE(t.is_canceled());
  CASE_EXPECT_FALSE(t.is_faulted());
  CASE_EXPECT_FALSE(t.is_timeout());
  CASE_EXPECT_TRUE(t.is_exiting());
  CASE_EXPECT_TRUE(t.is_completed());
  CASE_EXPECT_EQ(114, *t.get_context()->data());

  CASE_EXPECT_EQ(old_resume_generator_count + 1, g_task_future_resume_generator_count);
  CASE_EXPECT_EQ(old_suspend_generator_count + 1, g_task_future_suspend_generator_count);
}

CASE_TEST(task_promise, task_future_void_need_resume) {
  size_t old_resume_generator_count = g_task_future_resume_generator_count;
  size_t old_suspend_generator_count = g_task_future_suspend_generator_count;

  task_future_void_type t = task_func_await_void();
  CASE_EXPECT_EQ(static_cast<int>(copp::promise_status::kCreated), static_cast<int>(t.get_status()));
  CASE_EXPECT_FALSE(t.is_canceled());
  CASE_EXPECT_FALSE(t.is_faulted());
  CASE_EXPECT_FALSE(t.is_timeout());
  CASE_EXPECT_FALSE(t.is_exiting());
  CASE_EXPECT_FALSE(t.is_completed());

  CASE_EXPECT_TRUE(t.start());
  CASE_EXPECT_EQ(123000, t.get_private_data()->data);
  CASE_EXPECT_EQ(static_cast<int>(copp::promise_status::kRunning), static_cast<int>(t.get_status()));
  CASE_EXPECT_FALSE(t.is_canceled());
  CASE_EXPECT_FALSE(t.is_faulted());
  CASE_EXPECT_FALSE(t.is_timeout());
  CASE_EXPECT_FALSE(t.is_exiting());
  CASE_EXPECT_FALSE(t.is_completed());

  resume_pending_contexts({100});
  CASE_EXPECT_EQ(static_cast<int>(copp::promise_status::kDone), static_cast<int>(t.get_status()));
  CASE_EXPECT_FALSE(t.is_canceled());
  CASE_EXPECT_FALSE(t.is_faulted());
  CASE_EXPECT_FALSE(t.is_timeout());
  CASE_EXPECT_TRUE(t.is_exiting());
  CASE_EXPECT_TRUE(t.is_completed());

  CASE_EXPECT_EQ(old_resume_generator_count + 1, g_task_future_resume_generator_count);
  CASE_EXPECT_EQ(old_suspend_generator_count + 1, g_task_future_suspend_generator_count);
}

CASE_TEST(task_promise, task_future_integer_no_resume) {
  size_t old_resume_generator_count = g_task_future_resume_generator_count;
  size_t old_suspend_generator_count = g_task_future_suspend_generator_count;

  task_future_int_type t = task_func_no_wait_int();
  CASE_EXPECT_EQ(static_cast<int>(copp::promise_status::kCreated), static_cast<int>(t.get_status()));
  CASE_EXPECT_FALSE(t.is_canceled());
  CASE_EXPECT_FALSE(t.is_faulted());
  CASE_EXPECT_FALSE(t.is_timeout());
  CASE_EXPECT_FALSE(t.is_exiting());
  CASE_EXPECT_FALSE(t.is_completed());

  CASE_EXPECT_TRUE(t.start());
  CASE_EXPECT_EQ(121000, t.get_private_data()->data);
  CASE_EXPECT_EQ(static_cast<int>(copp::promise_status::kDone), static_cast<int>(t.get_status()));
  CASE_EXPECT_FALSE(t.is_canceled());
  CASE_EXPECT_FALSE(t.is_faulted());
  CASE_EXPECT_FALSE(t.is_timeout());
  CASE_EXPECT_TRUE(t.is_exiting());
  CASE_EXPECT_TRUE(t.is_completed());
  CASE_EXPECT_EQ(26, *t.get_context()->data());

  CASE_EXPECT_EQ(old_resume_generator_count, g_task_future_resume_generator_count);
  CASE_EXPECT_EQ(old_suspend_generator_count, g_task_future_suspend_generator_count);
}

CASE_TEST(task_promise, task_future_void_no_resume) {
  size_t old_resume_generator_count = g_task_future_resume_generator_count;
  size_t old_suspend_generator_count = g_task_future_suspend_generator_count;

  task_future_void_type t = task_func_no_wait_void();
  CASE_EXPECT_EQ(static_cast<int>(copp::promise_status::kCreated), static_cast<int>(t.get_status()));
  CASE_EXPECT_FALSE(t.is_canceled());
  CASE_EXPECT_FALSE(t.is_faulted());
  CASE_EXPECT_FALSE(t.is_timeout());
  CASE_EXPECT_FALSE(t.is_exiting());
  CASE_EXPECT_FALSE(t.is_completed());

  CASE_EXPECT_TRUE(t.start());
  CASE_EXPECT_EQ(123000, t.get_private_data()->data);
  CASE_EXPECT_EQ(static_cast<int>(copp::promise_status::kDone), static_cast<int>(t.get_status()));
  CASE_EXPECT_FALSE(t.is_canceled());
  CASE_EXPECT_FALSE(t.is_faulted());
  CASE_EXPECT_FALSE(t.is_timeout());
  CASE_EXPECT_TRUE(t.is_exiting());
  CASE_EXPECT_TRUE(t.is_completed());

  CASE_EXPECT_EQ(old_resume_generator_count, g_task_future_resume_generator_count);
  CASE_EXPECT_EQ(old_suspend_generator_count, g_task_future_suspend_generator_count);
}

namespace {
static task_future_int_type task_func_await_int_generator() {
  generator_future_int_type generator{generator_int_suspend_callback, generator_int_resume_callback};
  auto res = co_await generator;
  co_return 235 + res;
}

static callable_future_int_type callable_func_await_int_task() {
  task_future_int_type t = task_func_await_int_generator();
  t.start();
  auto res = co_await t;
  co_return res;
}

static task_future_int_type task_func_await_int_task() {
  task_future_int_type t = task_func_await_int_generator();
  t.start();
  auto res = co_await t;
  co_return res;
}
}  // namespace

CASE_TEST(task_promise, callable_future_await_task) {
  size_t old_resume_generator_count = g_task_future_resume_generator_count;
  size_t old_suspend_generator_count = g_task_future_suspend_generator_count;

  callable_future_int_type f = callable_func_await_int_task();
  CASE_EXPECT_EQ(static_cast<int>(copp::promise_status::kRunning), static_cast<int>(f.get_status()));

  CASE_EXPECT_FALSE(f.is_ready());

  resume_pending_contexts({1000});
  CASE_EXPECT_EQ(static_cast<int>(copp::promise_status::kDone), static_cast<int>(f.get_status()));
  CASE_EXPECT_TRUE(f.is_ready());
  CASE_EXPECT_EQ(1235, f.get_internal_promise().data());

  CASE_EXPECT_EQ(old_resume_generator_count + 1, g_task_future_resume_generator_count);
  CASE_EXPECT_EQ(old_suspend_generator_count + 1, g_task_future_suspend_generator_count);
}

CASE_TEST(task_promise, task_future_await_task) {
  size_t old_resume_generator_count = g_task_future_resume_generator_count;
  size_t old_suspend_generator_count = g_task_future_suspend_generator_count;

  task_future_int_type t = task_func_await_int_task();
  CASE_EXPECT_EQ(static_cast<int>(copp::promise_status::kCreated), static_cast<int>(t.get_status()));
  CASE_EXPECT_FALSE(t.is_canceled());
  CASE_EXPECT_FALSE(t.is_faulted());
  CASE_EXPECT_FALSE(t.is_timeout());
  CASE_EXPECT_FALSE(t.is_exiting());
  CASE_EXPECT_FALSE(t.is_completed());

  CASE_EXPECT_TRUE(t.start());
  CASE_EXPECT_EQ(static_cast<int>(copp::promise_status::kRunning), static_cast<int>(t.get_status()));
  CASE_EXPECT_FALSE(t.is_canceled());
  CASE_EXPECT_FALSE(t.is_faulted());
  CASE_EXPECT_FALSE(t.is_timeout());
  CASE_EXPECT_FALSE(t.is_exiting());
  CASE_EXPECT_FALSE(t.is_completed());

  resume_pending_contexts({2000});
  CASE_EXPECT_EQ(static_cast<int>(copp::promise_status::kDone), static_cast<int>(t.get_status()));
  CASE_EXPECT_FALSE(t.is_canceled());
  CASE_EXPECT_FALSE(t.is_faulted());
  CASE_EXPECT_FALSE(t.is_timeout());
  CASE_EXPECT_TRUE(t.is_exiting());
  CASE_EXPECT_TRUE(t.is_completed());
  CASE_EXPECT_EQ(2235, *t.get_context()->data());

  CASE_EXPECT_EQ(old_resume_generator_count + 1, g_task_future_resume_generator_count);
  CASE_EXPECT_EQ(old_suspend_generator_count + 1, g_task_future_suspend_generator_count);
}

namespace {
static callable_future_int_type task_func_await_callable_and_be_killed() {
  auto u = callable_func_int_l2(13);
  int x = co_await u;
  co_return x;
}

static task_future_int_type task_func_await_and_be_killed_by_caller() {
  auto v = task_func_await_callable_and_be_killed();
  int x = co_await v;
  co_return x;
}

std::shared_ptr<task_future_int_type> g_task_promise_killed_by_callee;
static task_future_int_type task_func_await_and_be_killed_by_callee(task_future_int_type::task_status_type status) {
  g_task_promise_killed_by_callee->kill(status);
  auto v = task_func_await_callable_and_be_killed();
  int x = co_await v;
  co_return x;
}

}  // namespace

// kill
CASE_TEST(task_promise, killed_by_caller) {
  size_t old_resume_generator_count = g_task_future_resume_generator_count;
  size_t old_suspend_generator_count = g_task_future_suspend_generator_count;

  task_future_int_type t = task_func_await_and_be_killed_by_caller();
  CASE_EXPECT_EQ(static_cast<int>(copp::promise_status::kCreated), static_cast<int>(t.get_status()));
  CASE_EXPECT_FALSE(t.is_canceled());
  CASE_EXPECT_FALSE(t.is_faulted());
  CASE_EXPECT_FALSE(t.is_timeout());
  CASE_EXPECT_FALSE(t.is_exiting());
  CASE_EXPECT_FALSE(t.is_completed());

  CASE_EXPECT_TRUE(t.start());
  CASE_EXPECT_EQ(static_cast<int>(copp::promise_status::kRunning), static_cast<int>(t.get_status()));
  CASE_EXPECT_FALSE(t.is_canceled());
  CASE_EXPECT_FALSE(t.is_faulted());
  CASE_EXPECT_FALSE(t.is_timeout());
  CASE_EXPECT_FALSE(t.is_exiting());
  CASE_EXPECT_FALSE(t.is_completed());

  CASE_EXPECT_TRUE(t.kill(task_future_int_type::task_status_type::kKilled));
  CASE_EXPECT_EQ(static_cast<int>(copp::promise_status::kKilled), static_cast<int>(t.get_status()));
  CASE_EXPECT_FALSE(t.is_canceled());
  CASE_EXPECT_TRUE(t.is_faulted());
  CASE_EXPECT_FALSE(t.is_timeout());
  CASE_EXPECT_TRUE(t.is_exiting());
  CASE_EXPECT_TRUE(t.is_completed());
  CASE_EXPECT_EQ(-static_cast<int>(copp::promise_status::kKilled), *t.get_context()->data());

  CASE_EXPECT_EQ(old_resume_generator_count + 1, g_task_future_resume_generator_count);
  CASE_EXPECT_EQ(old_suspend_generator_count + 1, g_task_future_suspend_generator_count);

  resume_pending_contexts({100});
  CASE_EXPECT_EQ(-static_cast<int>(copp::promise_status::kKilled), *t.get_context()->data());
  CASE_EXPECT_FALSE(t.is_canceled());
  CASE_EXPECT_TRUE(t.is_faulted());
  CASE_EXPECT_FALSE(t.is_timeout());
  CASE_EXPECT_TRUE(t.is_exiting());
  CASE_EXPECT_TRUE(t.is_completed());

  CASE_EXPECT_EQ(old_resume_generator_count + 1, g_task_future_resume_generator_count);
  CASE_EXPECT_EQ(old_suspend_generator_count + 1, g_task_future_suspend_generator_count);
}

CASE_TEST(task_promise, killed_by_callee) {
  size_t old_resume_generator_count = g_task_future_resume_generator_count;
  size_t old_suspend_generator_count = g_task_future_suspend_generator_count;

  task_future_int_type t = task_func_await_and_be_killed_by_callee(copp::promise_status::kKilled);
  CASE_EXPECT_EQ(static_cast<int>(copp::promise_status::kCreated), static_cast<int>(t.get_status()));
  CASE_EXPECT_FALSE(t.is_canceled());
  CASE_EXPECT_FALSE(t.is_faulted());
  CASE_EXPECT_FALSE(t.is_timeout());
  CASE_EXPECT_FALSE(t.is_exiting());
  CASE_EXPECT_FALSE(t.is_completed());

  g_task_promise_killed_by_callee = std::make_shared<task_future_int_type>(t);
  CASE_EXPECT_TRUE(t.start());
  g_task_promise_killed_by_callee.reset();

  CASE_EXPECT_EQ(static_cast<int>(copp::promise_status::kKilled), static_cast<int>(t.get_status()));
  CASE_EXPECT_FALSE(t.is_canceled());
  CASE_EXPECT_TRUE(t.is_faulted());
  CASE_EXPECT_FALSE(t.is_timeout());
  CASE_EXPECT_TRUE(t.is_exiting());
  CASE_EXPECT_TRUE(t.is_completed());
  CASE_EXPECT_EQ(-static_cast<int>(copp::promise_status::kKilled), *t.get_context()->data());

  CASE_EXPECT_EQ(old_resume_generator_count + 1, g_task_future_resume_generator_count);
  CASE_EXPECT_EQ(old_suspend_generator_count + 1, g_task_future_suspend_generator_count);

  resume_pending_contexts({100});
  CASE_EXPECT_EQ(-static_cast<int>(copp::promise_status::kKilled), *t.get_context()->data());
  CASE_EXPECT_FALSE(t.is_canceled());
  CASE_EXPECT_TRUE(t.is_faulted());
  CASE_EXPECT_FALSE(t.is_timeout());
  CASE_EXPECT_TRUE(t.is_exiting());
  CASE_EXPECT_TRUE(t.is_completed());

  CASE_EXPECT_EQ(old_resume_generator_count + 1, g_task_future_resume_generator_count);
  CASE_EXPECT_EQ(old_suspend_generator_count + 1, g_task_future_suspend_generator_count);
}

// Cancel
CASE_TEST(task_promise, cancel_by_caller) {
  size_t old_resume_generator_count = g_task_future_resume_generator_count;
  size_t old_suspend_generator_count = g_task_future_suspend_generator_count;

  task_future_int_type t = task_func_await_and_be_killed_by_caller();
  CASE_EXPECT_EQ(static_cast<int>(copp::promise_status::kCreated), static_cast<int>(t.get_status()));
  CASE_EXPECT_FALSE(t.is_canceled());
  CASE_EXPECT_FALSE(t.is_faulted());
  CASE_EXPECT_FALSE(t.is_timeout());
  CASE_EXPECT_FALSE(t.is_exiting());
  CASE_EXPECT_FALSE(t.is_completed());

  CASE_EXPECT_TRUE(t.start());
  CASE_EXPECT_EQ(static_cast<int>(copp::promise_status::kRunning), static_cast<int>(t.get_status()));
  CASE_EXPECT_FALSE(t.is_canceled());
  CASE_EXPECT_FALSE(t.is_faulted());
  CASE_EXPECT_FALSE(t.is_timeout());
  CASE_EXPECT_FALSE(t.is_exiting());
  CASE_EXPECT_FALSE(t.is_completed());

  CASE_EXPECT_TRUE(t.kill(task_future_int_type::task_status_type::kCancle));
  CASE_EXPECT_EQ(static_cast<int>(copp::promise_status::kCancle), static_cast<int>(t.get_status()));
  CASE_EXPECT_TRUE(t.is_canceled());
  CASE_EXPECT_FALSE(t.is_faulted());
  CASE_EXPECT_FALSE(t.is_timeout());
  CASE_EXPECT_TRUE(t.is_exiting());
  CASE_EXPECT_TRUE(t.is_completed());
  CASE_EXPECT_EQ(-static_cast<int>(copp::promise_status::kCancle), *t.get_context()->data());

  CASE_EXPECT_EQ(old_resume_generator_count + 1, g_task_future_resume_generator_count);
  CASE_EXPECT_EQ(old_suspend_generator_count + 1, g_task_future_suspend_generator_count);

  resume_pending_contexts({100});
  CASE_EXPECT_EQ(-static_cast<int>(copp::promise_status::kCancle), *t.get_context()->data());
  CASE_EXPECT_TRUE(t.is_canceled());
  CASE_EXPECT_FALSE(t.is_faulted());
  CASE_EXPECT_FALSE(t.is_timeout());
  CASE_EXPECT_TRUE(t.is_exiting());
  CASE_EXPECT_TRUE(t.is_completed());

  CASE_EXPECT_EQ(old_resume_generator_count + 1, g_task_future_resume_generator_count);
  CASE_EXPECT_EQ(old_suspend_generator_count + 1, g_task_future_suspend_generator_count);
}

// Timeout
CASE_TEST(task_promise, timeout_by_caller) {
  size_t old_resume_generator_count = g_task_future_resume_generator_count;
  size_t old_suspend_generator_count = g_task_future_suspend_generator_count;

  task_future_int_type t = task_func_await_and_be_killed_by_caller();
  CASE_EXPECT_EQ(static_cast<int>(copp::promise_status::kCreated), static_cast<int>(t.get_status()));
  CASE_EXPECT_FALSE(t.is_canceled());
  CASE_EXPECT_FALSE(t.is_faulted());
  CASE_EXPECT_FALSE(t.is_timeout());
  CASE_EXPECT_FALSE(t.is_exiting());
  CASE_EXPECT_FALSE(t.is_completed());

  CASE_EXPECT_TRUE(t.start());
  CASE_EXPECT_EQ(static_cast<int>(copp::promise_status::kRunning), static_cast<int>(t.get_status()));
  CASE_EXPECT_FALSE(t.is_canceled());
  CASE_EXPECT_FALSE(t.is_faulted());
  CASE_EXPECT_FALSE(t.is_timeout());
  CASE_EXPECT_FALSE(t.is_exiting());
  CASE_EXPECT_FALSE(t.is_completed());

  CASE_EXPECT_TRUE(t.kill(task_future_int_type::task_status_type::kTimeout));
  CASE_EXPECT_EQ(static_cast<int>(copp::promise_status::kTimeout), static_cast<int>(t.get_status()));
  CASE_EXPECT_FALSE(t.is_canceled());
  CASE_EXPECT_TRUE(t.is_faulted());
  CASE_EXPECT_TRUE(t.is_timeout());
  CASE_EXPECT_TRUE(t.is_exiting());
  CASE_EXPECT_TRUE(t.is_completed());
  CASE_EXPECT_EQ(-static_cast<int>(copp::promise_status::kTimeout), *t.get_context()->data());

  CASE_EXPECT_EQ(old_resume_generator_count + 1, g_task_future_resume_generator_count);
  CASE_EXPECT_EQ(old_suspend_generator_count + 1, g_task_future_suspend_generator_count);

  resume_pending_contexts({100});
  CASE_EXPECT_EQ(-static_cast<int>(copp::promise_status::kTimeout), *t.get_context()->data());
  CASE_EXPECT_FALSE(t.is_canceled());
  CASE_EXPECT_TRUE(t.is_faulted());
  CASE_EXPECT_TRUE(t.is_timeout());
  CASE_EXPECT_TRUE(t.is_exiting());
  CASE_EXPECT_TRUE(t.is_completed());

  CASE_EXPECT_EQ(old_resume_generator_count + 1, g_task_future_resume_generator_count);
  CASE_EXPECT_EQ(old_suspend_generator_count + 1, g_task_future_suspend_generator_count);
}

CASE_TEST(task_promise, timeout_by_callee) {
  size_t old_resume_generator_count = g_task_future_resume_generator_count;
  size_t old_suspend_generator_count = g_task_future_suspend_generator_count;

  task_future_int_type t = task_func_await_and_be_killed_by_callee(copp::promise_status::kTimeout);
  CASE_EXPECT_EQ(static_cast<int>(copp::promise_status::kCreated), static_cast<int>(t.get_status()));
  CASE_EXPECT_FALSE(t.is_canceled());
  CASE_EXPECT_FALSE(t.is_faulted());
  CASE_EXPECT_FALSE(t.is_timeout());
  CASE_EXPECT_FALSE(t.is_exiting());
  CASE_EXPECT_FALSE(t.is_completed());

  g_task_promise_killed_by_callee = std::make_shared<task_future_int_type>(t);
  CASE_EXPECT_TRUE(t.start());
  g_task_promise_killed_by_callee.reset();

  CASE_EXPECT_EQ(static_cast<int>(copp::promise_status::kTimeout), static_cast<int>(t.get_status()));
  CASE_EXPECT_FALSE(t.is_canceled());
  CASE_EXPECT_TRUE(t.is_faulted());
  CASE_EXPECT_TRUE(t.is_timeout());
  CASE_EXPECT_TRUE(t.is_exiting());
  CASE_EXPECT_TRUE(t.is_completed());
  CASE_EXPECT_EQ(-static_cast<int>(copp::promise_status::kTimeout), *t.get_context()->data());

  CASE_EXPECT_EQ(old_resume_generator_count + 1, g_task_future_resume_generator_count);
  CASE_EXPECT_EQ(old_suspend_generator_count + 1, g_task_future_suspend_generator_count);

  resume_pending_contexts({100});
  CASE_EXPECT_EQ(-static_cast<int>(copp::promise_status::kTimeout), *t.get_context()->data());
  CASE_EXPECT_FALSE(t.is_canceled());
  CASE_EXPECT_TRUE(t.is_faulted());
  CASE_EXPECT_TRUE(t.is_timeout());
  CASE_EXPECT_TRUE(t.is_exiting());
  CASE_EXPECT_TRUE(t.is_completed());

  CASE_EXPECT_EQ(old_resume_generator_count + 1, g_task_future_resume_generator_count);
  CASE_EXPECT_EQ(old_suspend_generator_count + 1, g_task_future_suspend_generator_count);
}

namespace {
static task_future_int_type task_func_await_child_task_child() {
  auto u = callable_func_int_l2(53);
  int x = co_await u;
  co_return x;
}

static task_future_int_type task_func_await_child_task_parent(task_future_int_type &out) {
  out = task_func_await_child_task_child();
  int x = co_await out;
  co_return x;
}

static task_future_int_type task_func_await_empty() {
  task_future_int_type empty;
  int x = co_await empty;
  CASE_EXPECT_EQ(-1, x);
  co_return x;
}
}  // namespace

// kill parent task and the child task will not be effected
CASE_TEST(task_promise, kill_parent_only) {
  size_t old_resume_generator_count = g_task_future_resume_generator_count;
  size_t old_suspend_generator_count = g_task_future_suspend_generator_count;

  task_future_int_type child;
  task_future_int_type parent = task_func_await_child_task_parent(child);
  CASE_EXPECT_EQ(static_cast<int>(copp::promise_status::kCreated), static_cast<int>(parent.get_status()));
  CASE_EXPECT_FALSE(parent.is_canceled());
  CASE_EXPECT_FALSE(parent.is_faulted());
  CASE_EXPECT_FALSE(parent.is_timeout());
  CASE_EXPECT_FALSE(parent.is_exiting());
  CASE_EXPECT_FALSE(parent.is_completed());

  CASE_EXPECT_TRUE(parent.start());
  CASE_EXPECT_EQ(static_cast<int>(copp::promise_status::kRunning), static_cast<int>(parent.get_status()));
  CASE_EXPECT_FALSE(parent.is_canceled());
  CASE_EXPECT_FALSE(parent.is_faulted());
  CASE_EXPECT_FALSE(parent.is_timeout());
  CASE_EXPECT_FALSE(parent.is_exiting());
  CASE_EXPECT_FALSE(parent.is_completed());

  CASE_EXPECT_EQ(static_cast<int>(copp::promise_status::kCreated), static_cast<int>(child.get_status()));
  CASE_EXPECT_EQ(old_resume_generator_count, g_task_future_resume_generator_count);
  CASE_EXPECT_EQ(old_suspend_generator_count, g_task_future_suspend_generator_count);
  child.start();
  CASE_EXPECT_EQ(old_resume_generator_count, g_task_future_resume_generator_count);
  CASE_EXPECT_EQ(old_suspend_generator_count + 1, g_task_future_suspend_generator_count);

  CASE_EXPECT_EQ(static_cast<int>(copp::promise_status::kRunning), static_cast<int>(child.get_status()));
  CASE_EXPECT_FALSE(child.is_canceled());
  CASE_EXPECT_FALSE(child.is_faulted());
  CASE_EXPECT_FALSE(child.is_timeout());
  CASE_EXPECT_FALSE(child.is_exiting());
  CASE_EXPECT_FALSE(child.is_completed());

  CASE_EXPECT_TRUE(parent.kill(task_future_int_type::task_status_type::kTimeout));
  CASE_EXPECT_EQ(static_cast<int>(copp::promise_status::kTimeout), static_cast<int>(parent.get_status()));
  CASE_EXPECT_FALSE(parent.is_canceled());
  CASE_EXPECT_TRUE(parent.is_faulted());
  CASE_EXPECT_TRUE(parent.is_timeout());
  CASE_EXPECT_TRUE(parent.is_exiting());
  CASE_EXPECT_TRUE(parent.is_completed());
  CASE_EXPECT_EQ(-static_cast<int>(copp::promise_status::kTimeout), *parent.get_context()->data());

  CASE_EXPECT_EQ(static_cast<int>(copp::promise_status::kRunning), static_cast<int>(child.get_status()));
  CASE_EXPECT_FALSE(child.is_canceled());
  CASE_EXPECT_FALSE(child.is_faulted());
  CASE_EXPECT_FALSE(child.is_timeout());
  CASE_EXPECT_FALSE(child.is_exiting());
  CASE_EXPECT_FALSE(child.is_completed());

  resume_pending_contexts({1000});
  CASE_EXPECT_EQ(-static_cast<int>(copp::promise_status::kTimeout), *parent.get_context()->data());
  CASE_EXPECT_FALSE(parent.is_canceled());
  CASE_EXPECT_TRUE(parent.is_faulted());
  CASE_EXPECT_TRUE(parent.is_timeout());
  CASE_EXPECT_TRUE(parent.is_exiting());
  CASE_EXPECT_TRUE(parent.is_completed());

  CASE_EXPECT_EQ(static_cast<int>(copp::promise_status::kDone), static_cast<int>(child.get_status()));
  CASE_EXPECT_FALSE(child.is_canceled());
  CASE_EXPECT_FALSE(child.is_faulted());
  CASE_EXPECT_FALSE(child.is_timeout());
  CASE_EXPECT_TRUE(child.is_exiting());
  CASE_EXPECT_TRUE(child.is_completed());
  CASE_EXPECT_EQ(1053, *child.get_context()->data());

  CASE_EXPECT_EQ(old_resume_generator_count + 1, g_task_future_resume_generator_count);
  CASE_EXPECT_EQ(old_suspend_generator_count + 1, g_task_future_suspend_generator_count);
}

CASE_TEST(task_promise, empty_task) {
  task_future_int_type t = task_func_await_empty();
  t.start();

  task_future_int_type empty;
  CASE_EXPECT_FALSE(empty.start());
  CASE_EXPECT_FALSE(empty.kill(task_future_int_type::task_status_type::kKilled));

  CASE_EXPECT_EQ(static_cast<int>(copp::promise_status::kInvalid), static_cast<int>(empty.get_status()));
  CASE_EXPECT_EQ(nullptr, empty.get_private_data());
  CASE_EXPECT_EQ(0, empty.get_id());
  CASE_EXPECT_EQ(0, empty.get_ref_future_count());
  CASE_EXPECT_EQ(nullptr, empty.get_context().get());
}

// task destroy and auto resume
CASE_TEST(task_promise, task_destroy_and_auto_resume) {
  task_future_int_type parent;
  {
    task_future_int_type child;
    parent = task_func_await_child_task_parent(child);
    CASE_EXPECT_TRUE(parent.start());
    child.start();

    CASE_EXPECT_EQ(1, parent.get_ref_future_count());
    CASE_EXPECT_EQ(1, child.get_ref_future_count());

    task_future_int_type move_child_assign = std::move(child);
    CASE_EXPECT_EQ(0, child.get_ref_future_count());
    CASE_EXPECT_EQ(1, move_child_assign.get_ref_future_count());
    task_future_int_type move_child_ctor{std::move(move_child_assign)};
    CASE_EXPECT_EQ(0, move_child_assign.get_ref_future_count());
    CASE_EXPECT_EQ(1, move_child_ctor.get_ref_future_count());

    task_future_int_type copy_child_assign = move_child_ctor;
    task_future_int_type copy_child_ctor{move_child_ctor};

    CASE_EXPECT_EQ(3, copy_child_ctor.get_ref_future_count());
    CASE_EXPECT_EQ(3, copy_child_assign.get_ref_future_count());
    CASE_EXPECT_EQ(3, move_child_ctor.get_ref_future_count());

    CASE_EXPECT_EQ(copy_child_assign.get_context(), copy_child_ctor.get_context());
    CASE_EXPECT_EQ(copy_child_assign.get_context(), move_child_ctor.get_context());

    CASE_EXPECT_EQ(copy_child_assign.get_id(), copy_child_ctor.get_id());
    CASE_EXPECT_EQ(copy_child_assign.get_id(), move_child_ctor.get_id());

    CASE_EXPECT_FALSE(parent.is_exiting());
    CASE_EXPECT_FALSE(parent.is_completed());
  }

  CASE_EXPECT_TRUE(parent.is_exiting());
  CASE_EXPECT_TRUE(parent.is_completed());
  CASE_EXPECT_EQ(-static_cast<int>(copp::promise_status::kKilled), *parent.get_context()->data());

  resume_pending_contexts({});
}

namespace {
static task_future_int_type task_func_await_int_simple() {
  auto u = callable_func_int_l2(91);
  int x = co_await u;
  co_return x;
}

static task_future_void_type task_func_await_void_simple() {
  auto u = callable_func_void_l2();
  co_await u;
  co_return;
}

}  // namespace

CASE_TEST(task_promise, then_12_1_task_return_int_and_thenable_return_normal_int) {
  auto t = task_func_await_int_simple();
  auto f = t.then([](task_future_int_type::context_pointer_type, task_future_int_type::value_type value) {
    CASE_EXPECT_EQ(2091, value);
    CASE_MSG_INFO() << "thenable return int" << std::endl;
    return value;
  });

  CASE_EXPECT_FALSE(f.is_ready());
  t.start();
  CASE_EXPECT_FALSE(f.is_ready());

  resume_pending_contexts({2000});
  CASE_EXPECT_TRUE(f.is_ready());

  CASE_EXPECT_EQ(2091, f.get_internal_promise().data());
}

CASE_TEST(task_promise, then_12_2_task_return_int_and_thenable_return_normal_void) {
  auto t = task_func_await_int_simple();
  auto f = t.then([](const task_future_int_type::context_pointer_type &, task_future_int_type::value_type value) {
    CASE_EXPECT_EQ(2091, value);
    CASE_MSG_INFO() << "thenable return void" << std::endl;
  });

  CASE_EXPECT_FALSE(f.is_ready());
  t.start();
  CASE_EXPECT_FALSE(f.is_ready());

  resume_pending_contexts({2000});
  CASE_EXPECT_TRUE(f.is_ready());
}

CASE_TEST(task_promise, then_12_3_task_return_int_and_thenable_return_callable_int) {
  auto t = task_func_await_int_simple();
  auto f = t.then([](task_future_int_type::context_pointer_type &&,
                     task_future_int_type::value_type value) -> copp::callable_future<int> {
    CASE_EXPECT_EQ(2091, value);
    CASE_MSG_INFO() << "thenable return callable_future<int>" << std::endl;
    co_return value;
  });

  CASE_EXPECT_FALSE(f.is_ready());
  t.start();
  CASE_EXPECT_FALSE(f.is_ready());

  resume_pending_contexts({2000});
  CASE_EXPECT_TRUE(f.is_ready());

  CASE_EXPECT_EQ(2091, f.get_internal_promise().data());
}

CASE_TEST(task_promise, then_12_4_task_return_int_and_thenable_return_callable_void) {
  auto t = task_func_await_int_simple();
  auto f = t.then([](const task_future_int_type::context_pointer_type &,
                     task_future_int_type::value_type value) -> copp::callable_future<void> {
    CASE_EXPECT_EQ(2091, value);
    CASE_MSG_INFO() << "thenable return callable_future<void>" << std::endl;
    co_return;
  });

  CASE_EXPECT_FALSE(f.is_ready());
  t.start();
  CASE_EXPECT_FALSE(f.is_ready());

  resume_pending_contexts({2000});
  CASE_EXPECT_TRUE(f.is_ready());
}

CASE_TEST(task_promise, then_12_5_task_return_int_and_thenable_return_callable_int) {
  auto t = task_func_await_int_simple();
  auto f = t.then([](task_future_int_type::context_pointer_type,
                     task_future_int_type::value_type value) -> cotask::task_future<int, void> {
    CASE_EXPECT_EQ(2091, value);
    CASE_MSG_INFO() << "thenable return task_future<int>" << std::endl;
    co_return value;
  });

  CASE_EXPECT_FALSE(f.is_ready());
  t.start();
  CASE_EXPECT_FALSE(f.is_ready());

  resume_pending_contexts({2000});
  CASE_EXPECT_TRUE(f.is_ready());

  CASE_EXPECT_EQ(2091, f.get_internal_promise().data());
}

CASE_TEST(task_promise, then_12_6_task_return_int_and_thenable_return_callable_void) {
  auto t = task_func_await_int_simple();
  auto f = t.then([](task_future_int_type::context_pointer_type &&,
                     task_future_int_type::value_type value) -> cotask::task_future<void, void> {
    CASE_EXPECT_EQ(2091, value);
    CASE_MSG_INFO() << "thenable return task_future<void>" << std::endl;
    co_return;
  });

  CASE_EXPECT_FALSE(f.is_ready());
  t.start();
  CASE_EXPECT_FALSE(f.is_ready());

  resume_pending_contexts({2000});
  CASE_EXPECT_TRUE(f.is_ready());
}

CASE_TEST(task_promise, then_12_7_task_return_void_and_thenable_return_normal_int) {
  auto t = task_func_await_void_simple();
  auto f = t.then([](task_future_void_type::context_pointer_type) {
    CASE_MSG_INFO() << "thenable return int" << std::endl;
    return 2111;
  });

  CASE_EXPECT_FALSE(f.is_ready());
  t.start();
  CASE_EXPECT_FALSE(f.is_ready());

  resume_pending_contexts({});
  CASE_EXPECT_TRUE(f.is_ready());

  CASE_EXPECT_EQ(2111, f.get_internal_promise().data());
}

CASE_TEST(task_promise, then_12_8_task_return_void_and_thenable_return_normal_void) {
  auto t = task_func_await_void_simple();
  auto f = t.then([](const task_future_void_type::context_pointer_type &) {
    CASE_MSG_INFO() << "thenable return void" << std::endl;
  });

  CASE_EXPECT_FALSE(f.is_ready());
  t.start();
  CASE_EXPECT_FALSE(f.is_ready());

  resume_pending_contexts({});
  CASE_EXPECT_TRUE(f.is_ready());
}

CASE_TEST(task_promise, then_12_9_task_return_void_and_thenable_return_callable_int) {
  auto t = task_func_await_void_simple();
  auto f = t.then([](task_future_void_type::context_pointer_type) -> copp::callable_future<int> {
    CASE_MSG_INFO() << "thenable return callable_future<int>" << std::endl;
    co_return 2111;
  });

  CASE_EXPECT_FALSE(f.is_ready());
  t.start();
  CASE_EXPECT_FALSE(f.is_ready());

  resume_pending_contexts({});
  CASE_EXPECT_TRUE(f.is_ready());

  CASE_EXPECT_EQ(2111, f.get_internal_promise().data());
}

CASE_TEST(task_promise, then_12_10_task_return_void_and_thenable_return_callable_void) {
  auto t = task_func_await_void_simple();
  auto f = t.then([](const task_future_void_type::context_pointer_type &) -> copp::callable_future<void> {
    CASE_MSG_INFO() << "thenable return callable_future<void>" << std::endl;
    co_return;
  });

  CASE_EXPECT_FALSE(f.is_ready());
  t.start();
  CASE_EXPECT_FALSE(f.is_ready());

  resume_pending_contexts({});
  CASE_EXPECT_TRUE(f.is_ready());
}

CASE_TEST(task_promise, then_12_11_task_return_void_and_thenable_return_callable_int) {
  auto t = task_func_await_void_simple();
  auto f = t.then([](task_future_void_type::context_pointer_type) -> cotask::task_future<int, void> {
    CASE_MSG_INFO() << "thenable return task_future<int>" << std::endl;
    co_return 2111;
  });

  CASE_EXPECT_FALSE(f.is_ready());
  t.start();
  CASE_EXPECT_FALSE(f.is_ready());

  resume_pending_contexts({});
  CASE_EXPECT_TRUE(f.is_ready());

  CASE_EXPECT_EQ(2111, f.get_internal_promise().data());
}

CASE_TEST(task_promise, then_12_12_task_return_void_and_thenable_return_callable_void) {
  auto t = task_func_await_void_simple();
  auto f = t.then([](const task_future_void_type::context_pointer_type &) -> cotask::task_future<void, void> {
    CASE_MSG_INFO() << "thenable return task_future<void>" << std::endl;
    co_return;
  });

  CASE_EXPECT_FALSE(f.is_ready());
  t.start();
  CASE_EXPECT_FALSE(f.is_ready());

  resume_pending_contexts({});
  CASE_EXPECT_TRUE(f.is_ready());
}

CASE_TEST(task_promise, then_todo_kill_task) {}

CASE_TEST(task_promise, then_todo_empty_task) {}

CASE_TEST(task_promise, then_todo_exiting_task) {}

#else
CASE_TEST(task_promise, disabled) {}
#endif

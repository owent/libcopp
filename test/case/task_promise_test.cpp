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
  CASE_MSG_INFO() << "callable inner future async int: " << inout << std::endl;
  generator_future_int_type generator{generator_int_suspend_callback, generator_int_resume_callback};
  auto gen_res = co_await generator;
  CASE_MSG_INFO() << "callable inner future return int: " << inout << std::endl;
  if (gen_res < 0) {
    co_return gen_res;
  }
  co_return inout + gen_res;
}

static callable_future_int_type callable_func_await_int() {
  auto v = callable_func_int_l1(3);
  auto u = callable_func_int_l2(11);
  CASE_MSG_INFO() << "callable await int" << std::endl;
  int x = (co_await v + co_await u);
  CASE_MSG_INFO() << "callable return int" << std::endl;
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
  CASE_MSG_INFO() << "callable await int" << std::endl;
  int x = (co_await v + co_await u);
  CASE_MSG_INFO() << "callable return int" << std::endl;
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
  CASE_MSG_INFO() << "callable inner future async void" << std::endl;
  generator_future_void_type generator{generator_void_suspend_callback, generator_void_resume_callback};
  co_await generator;
  CASE_MSG_INFO() << "callable inner future return void" << std::endl;
  co_return;
}

static callable_future_void_type callable_func_await_void() {
  auto v = callable_func_void_l1();
  auto u = callable_func_void_l2();
  CASE_MSG_INFO() << "callable await void" << std::endl;
  co_await v;
  co_await u;
  CASE_MSG_INFO() << "callable return void" << std::endl;
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
  CASE_MSG_INFO() << "callable await void" << std::endl;
  co_await v;
  co_await u;
  CASE_MSG_INFO() << "callable return void" << std::endl;
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
static callable_future_int_type task_func_await_and_be_killed() {
  auto u = callable_func_int_l2(13);
  int x = co_await u;
  co_return x;
}

static task_future_int_type task_func_await_and_be_killed_by_caller() {
  auto v = task_func_await_and_be_killed();
  int x = co_await v;
  co_return x;
}

std::shared_ptr<task_future_int_type> g_task_promise_killed_by_callee;
static task_future_int_type task_func_await_and_be_killed_by_callee(task_future_int_type::task_status_type status) {
  g_task_promise_killed_by_callee->kill(status);
  auto v = task_func_await_and_be_killed();
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

// TODO kill parent task and the child task will not be effected
// TODO task destroy and auto resume

#else
CASE_TEST(task_promise, disabled) {}
#endif

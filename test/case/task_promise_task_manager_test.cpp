// Copyright 2023 owent

#include <libcopp/coroutine/generator_promise.h>
#include <libcotask/task_manager.h>
#include <libcotask/task_promise.h>

#include <cstdio>
#include <cstring>
#include <iostream>
#include <memory>

#include "frame/test_macros.h"

#ifdef LIBCOTASK_MACRO_ENABLED

#  if defined(LIBCOPP_MACRO_ENABLE_STD_COROUTINE) && LIBCOPP_MACRO_ENABLE_STD_COROUTINE

namespace {
struct task_manager_future_private_data {};

using task_future_int_type = cotask::task_future<int, task_manager_future_private_data>;
using generator_future_int_type = copp::generator_future<int>;

std::list<generator_future_int_type::context_pointer_type> g_task_manager_future_pending_int_contexts;
size_t g_task_manager_future_resume_generator_count = 0;
size_t g_task_manager_future_suspend_generator_count = 0;

static size_t task_manager_resume_pending_contexts(std::list<int> values, int max_count = 32767) {
  size_t ret = 0;
  while (max_count > 0 && !g_task_manager_future_pending_int_contexts.empty()) {
    --max_count;
    if (!g_task_manager_future_pending_int_contexts.empty()) {
      auto ctx = *g_task_manager_future_pending_int_contexts.begin();
      g_task_manager_future_pending_int_contexts.pop_front();

      if (!values.empty()) {
        int val = values.front();
        values.pop_front();
        ctx->set_value(val);
      } else {
        ctx->set_value(0);
      }

      ++ret;
    }
  }

  return ret;
}

static void task_manager_generator_int_suspend_callback(generator_future_int_type::context_pointer_type ctx) {
  ++g_task_manager_future_suspend_generator_count;
  g_task_manager_future_pending_int_contexts.push_back(ctx);
}
static void task_manager_generator_int_resume_callback(const generator_future_int_type::context_type&) {
  ++g_task_manager_future_resume_generator_count;
}

static task_future_int_type task_func_await_int() {
  generator_future_int_type generator{task_manager_generator_int_suspend_callback,
                                      task_manager_generator_int_resume_callback};
  auto gen_res = co_await generator;
  co_return gen_res;
}

}  // namespace

CASE_TEST(task_promise_task_manager, tickspec_t) {
  cotask::detail::tickspec_t l;
  cotask::detail::tickspec_t r;

  l.tv_sec = 123;
  l.tv_nsec = 456;

  r.tv_sec = 123;
  r.tv_nsec = 456;

  CASE_EXPECT_TRUE(l == r);
  CASE_EXPECT_FALSE(l != r);
  CASE_EXPECT_FALSE(l < r);
  CASE_EXPECT_TRUE(l <= r);

  r.tv_sec = 456;
  r.tv_nsec = 123;

  CASE_EXPECT_FALSE(l == r);
  CASE_EXPECT_TRUE(l != r);
  CASE_EXPECT_TRUE(l < r);
  CASE_EXPECT_TRUE(l <= r);

  r.tv_sec = 45;
  r.tv_nsec = 999;

  CASE_EXPECT_FALSE(l == r);
  CASE_EXPECT_TRUE(l != r);
  CASE_EXPECT_FALSE(l < r);
  CASE_EXPECT_FALSE(l <= r);
}

CASE_TEST(task_promise_task_manager, task_timer_node) {
  cotask::detail::task_timer_node<task_future_int_type::id_type> l;
  cotask::detail::task_timer_node<task_future_int_type::id_type> r;

  l.expired_time.tv_sec = 123;
  l.expired_time.tv_nsec = 456;
  l.task_id = 10;

  r.expired_time.tv_sec = 123;
  r.expired_time.tv_nsec = 456;
  r.task_id = 10;

  CASE_EXPECT_TRUE(l == r);
  CASE_EXPECT_FALSE(l != r);
  CASE_EXPECT_FALSE(l < r);
  CASE_EXPECT_TRUE(l <= r);

  r.task_id = 5;

  CASE_EXPECT_FALSE(l == r);
  CASE_EXPECT_TRUE(l != r);
  CASE_EXPECT_FALSE(l < r);
  CASE_EXPECT_FALSE(l <= r);

  r.expired_time.tv_nsec = 45;
  r.task_id = 10;

  CASE_EXPECT_FALSE(l == r);
  CASE_EXPECT_TRUE(l != r);
  CASE_EXPECT_FALSE(l < r);
  CASE_EXPECT_FALSE(l <= r);

  r.expired_time.tv_nsec = 456;
  r.task_id = 15;

  CASE_EXPECT_FALSE(l == r);
  CASE_EXPECT_TRUE(l != r);
  CASE_EXPECT_TRUE(l < r);
  CASE_EXPECT_TRUE(l <= r);

  r.expired_time.tv_nsec = 4567;
  r.task_id = 10;

  CASE_EXPECT_FALSE(l == r);
  CASE_EXPECT_TRUE(l != r);
  CASE_EXPECT_TRUE(l < r);
  CASE_EXPECT_TRUE(l <= r);
}

CASE_TEST(task_promise_task_manager, add_and_timeout) {
  {
    task_future_int_type co_task = task_func_await_int();
    task_future_int_type co_another_task = task_func_await_int();

    using mgr_t = cotask::task_manager<task_future_int_type>;
    mgr_t::ptr_type task_mgr = mgr_t::create();

    CASE_EXPECT_EQ(0, (int)task_mgr->get_task_size());
    size_t old_resume_generator_count = g_task_manager_future_resume_generator_count;
    size_t old_suspend_generator_count = g_task_manager_future_suspend_generator_count;

    task_mgr->add_task(co_task, 5, 0);
    task_mgr->add_task(co_another_task);

    CASE_EXPECT_EQ(2, (int)task_mgr->get_task_size());
    CASE_EXPECT_EQ(1, (int)task_mgr->get_tick_checkpoint_size());

    CASE_EXPECT_TRUE(co_task == *task_mgr->find_task(co_task.get_id()));
    CASE_EXPECT_TRUE(co_another_task == *task_mgr->find_task(co_another_task.get_id()));

    CASE_EXPECT_EQ(0, (int)task_mgr->get_last_tick_time().tv_sec);
    task_mgr->tick(3);
    CASE_EXPECT_EQ(2, (int)task_mgr->get_task_size());
    CASE_EXPECT_EQ(1, (int)task_mgr->get_tick_checkpoint_size());

    co_task.start();

    CASE_EXPECT_EQ(3, (int)task_mgr->get_last_tick_time().tv_sec);
    task_mgr->tick(8);
    CASE_EXPECT_EQ(8, (int)task_mgr->get_last_tick_time().tv_sec);
    // tick reset timeout: 3 + 5 = 8
    CASE_EXPECT_EQ(8, (int)task_mgr->get_container().find(co_task.get_id())->second.timer_node->expired_time.tv_sec);
    CASE_EXPECT_EQ(2, (int)task_mgr->get_task_size());
    CASE_EXPECT_EQ(1, (int)task_mgr->get_tick_checkpoint_size());
    CASE_EXPECT_EQ(1, (int)task_mgr->get_checkpoints().size());
    CASE_EXPECT_FALSE(task_future_int_type::task_status_type::kTimeout == co_task.get_status());

    task_mgr->tick(9);
    CASE_EXPECT_EQ(9, (int)task_mgr->get_last_tick_time().tv_sec);
    CASE_EXPECT_EQ(1, (int)task_mgr->get_task_size());
    CASE_EXPECT_EQ(1, (int)task_mgr->get_container().size());
    CASE_EXPECT_EQ(0, (int)task_mgr->get_tick_checkpoint_size());
    CASE_EXPECT_TRUE(task_future_int_type::task_status_type::kTimeout == co_task.get_status());

    CASE_EXPECT_EQ(nullptr, task_mgr->find_task(co_task.get_id()));
    CASE_EXPECT_TRUE(co_another_task == *task_mgr->find_task(co_another_task.get_id()));

    CASE_EXPECT_EQ(old_resume_generator_count + 1, g_task_manager_future_resume_generator_count);
    CASE_EXPECT_EQ(old_suspend_generator_count + 1, g_task_manager_future_suspend_generator_count);

    CASE_EXPECT_EQ(0, task_mgr->start(co_another_task.get_id()));

    CASE_EXPECT_TRUE(task_future_int_type::task_status_type::kRunning == co_another_task.get_status());

    CASE_EXPECT_EQ(old_resume_generator_count + 1, g_task_manager_future_resume_generator_count);
    CASE_EXPECT_EQ(old_suspend_generator_count + 2, g_task_manager_future_suspend_generator_count);

    task_manager_resume_pending_contexts({901, 902});

    CASE_EXPECT_EQ(old_resume_generator_count + 2, g_task_manager_future_resume_generator_count);
    CASE_EXPECT_EQ(old_suspend_generator_count + 2, g_task_manager_future_suspend_generator_count);

    CASE_EXPECT_TRUE(task_future_int_type::task_status_type::kTimeout == co_task.get_status());
    CASE_EXPECT_TRUE(task_future_int_type::task_status_type::kDone == co_another_task.get_status());

    CASE_EXPECT_EQ(0, (int)task_mgr->get_task_size());
    CASE_EXPECT_EQ(0, (int)task_mgr->get_tick_checkpoint_size());

    CASE_EXPECT_EQ(copp::COPP_EC_TASK_IS_EXITING, task_mgr->add_task(co_another_task));

    task_mgr.reset();
  }
  task_manager_resume_pending_contexts({});
}

CASE_TEST(task_promise_task_manager, add_and_timeout_no_start) {
  {
    task_future_int_type co_task = task_func_await_int();
    task_future_int_type co_another_task = task_func_await_int();

    using mgr_t = cotask::task_manager<task_future_int_type>;
    mgr_t::ptr_type task_mgr = mgr_t::create();

    CASE_EXPECT_EQ(0, (int)task_mgr->get_task_size());
    size_t old_resume_generator_count = g_task_manager_future_resume_generator_count;
    size_t old_suspend_generator_count = g_task_manager_future_suspend_generator_count;

    task_mgr->add_task(co_task, 5, 0);
    task_mgr->add_task(co_another_task);

    CASE_EXPECT_EQ(2, (int)task_mgr->get_task_size());
    CASE_EXPECT_EQ(1, (int)task_mgr->get_tick_checkpoint_size());

    CASE_EXPECT_TRUE(co_task == *task_mgr->find_task(co_task.get_id()));
    CASE_EXPECT_TRUE(co_another_task == *task_mgr->find_task(co_another_task.get_id()));

    CASE_EXPECT_EQ(0, (int)task_mgr->get_last_tick_time().tv_sec);
    task_mgr->tick(3);
    CASE_EXPECT_EQ(3, (int)task_mgr->get_last_tick_time().tv_sec);
    task_mgr->tick(8);
    CASE_EXPECT_EQ(8, (int)task_mgr->get_last_tick_time().tv_sec);
    // tick reset timeout: 3 + 5 = 8
    CASE_EXPECT_EQ(8, (int)task_mgr->get_container().find(co_task.get_id())->second.timer_node->expired_time.tv_sec);
    CASE_EXPECT_EQ(2, (int)task_mgr->get_task_size());
    CASE_EXPECT_EQ(1, (int)task_mgr->get_tick_checkpoint_size());
    CASE_EXPECT_EQ(1, (int)task_mgr->get_checkpoints().size());
    CASE_EXPECT_FALSE(task_future_int_type::task_status_type::kTimeout == co_task.get_status());

    task_mgr->tick(9);
    CASE_EXPECT_EQ(9, (int)task_mgr->get_last_tick_time().tv_sec);
    CASE_EXPECT_EQ(1, (int)task_mgr->get_task_size());
    CASE_EXPECT_EQ(1, (int)task_mgr->get_container().size());
    CASE_EXPECT_EQ(0, (int)task_mgr->get_tick_checkpoint_size());
    CASE_EXPECT_TRUE(task_future_int_type::task_status_type::kTimeout == co_task.get_status());

    CASE_EXPECT_EQ(nullptr, task_mgr->find_task(co_task.get_id()));
    CASE_EXPECT_TRUE(co_another_task == *task_mgr->find_task(co_another_task.get_id()));

    CASE_EXPECT_EQ(old_resume_generator_count, g_task_manager_future_resume_generator_count);
    CASE_EXPECT_EQ(old_suspend_generator_count, g_task_manager_future_suspend_generator_count);

    CASE_EXPECT_EQ(0, task_mgr->start(co_another_task.get_id()));

    CASE_EXPECT_TRUE(task_future_int_type::task_status_type::kRunning == co_another_task.get_status());

    CASE_EXPECT_EQ(old_resume_generator_count, g_task_manager_future_resume_generator_count);
    CASE_EXPECT_EQ(old_suspend_generator_count + 1, g_task_manager_future_suspend_generator_count);

    task_manager_resume_pending_contexts({901, 902});

    CASE_EXPECT_EQ(old_resume_generator_count + 1, g_task_manager_future_resume_generator_count);
    CASE_EXPECT_EQ(old_suspend_generator_count + 1, g_task_manager_future_suspend_generator_count);

    CASE_EXPECT_TRUE(task_future_int_type::task_status_type::kTimeout == co_task.get_status());
    CASE_EXPECT_TRUE(task_future_int_type::task_status_type::kDone == co_another_task.get_status());

    CASE_EXPECT_EQ(0, (int)task_mgr->get_task_size());
    CASE_EXPECT_EQ(0, (int)task_mgr->get_tick_checkpoint_size());

    CASE_EXPECT_EQ(copp::COPP_EC_TASK_IS_EXITING, task_mgr->add_task(co_another_task));

    task_mgr.reset();
  }
  task_manager_resume_pending_contexts({});
}

CASE_TEST(task_promise_task_manager, add_and_timeout_last_reference) {
  {
    size_t old_resume_generator_count = g_task_manager_future_resume_generator_count;
    size_t old_suspend_generator_count = g_task_manager_future_suspend_generator_count;
    using mgr_t = cotask::task_manager<task_future_int_type>;
    mgr_t::ptr_type task_mgr = mgr_t::create();

    task_future_int_type::id_type another_task_id;
    task_future_int_type::id_type task_id;
    {
      task_future_int_type co_task = task_func_await_int();
      task_future_int_type co_another_task = task_func_await_int();

      CASE_EXPECT_EQ(0, (int)task_mgr->get_task_size());

      co_task.start();
      another_task_id = co_another_task.get_id();
      task_id = co_task.get_id();

      task_mgr->add_task(std::move(co_task), 5, 0);
      task_mgr->add_task(std::move(co_another_task));

      CASE_EXPECT_EQ(2, (int)task_mgr->get_task_size());
      CASE_EXPECT_EQ(1, (int)task_mgr->get_tick_checkpoint_size());
    }

    CASE_EXPECT_EQ(0, (int)task_mgr->get_last_tick_time().tv_sec);
    task_mgr->tick(3);
    CASE_EXPECT_EQ(3, (int)task_mgr->get_last_tick_time().tv_sec);
    task_mgr->tick(8);
    CASE_EXPECT_EQ(8, (int)task_mgr->get_last_tick_time().tv_sec);
    // tick reset timeout: 3 + 5 = 8
    CASE_EXPECT_EQ(8, (int)task_mgr->get_container().find(task_id)->second.timer_node->expired_time.tv_sec);
    CASE_EXPECT_EQ(2, (int)task_mgr->get_task_size());
    CASE_EXPECT_EQ(1, (int)task_mgr->get_tick_checkpoint_size());
    CASE_EXPECT_EQ(1, (int)task_mgr->get_checkpoints().size());
    CASE_EXPECT_FALSE(nullptr == task_mgr->find_task(task_id));

    task_mgr->tick(9);
    CASE_EXPECT_EQ(9, (int)task_mgr->get_last_tick_time().tv_sec);
    CASE_EXPECT_EQ(1, (int)task_mgr->get_task_size());
    CASE_EXPECT_EQ(1, (int)task_mgr->get_container().size());
    CASE_EXPECT_EQ(0, (int)task_mgr->get_tick_checkpoint_size());
    CASE_EXPECT_EQ(nullptr, task_mgr->find_task(task_id));

    CASE_EXPECT_EQ(old_resume_generator_count + 1, g_task_manager_future_resume_generator_count);
    CASE_EXPECT_EQ(old_suspend_generator_count + 1, g_task_manager_future_suspend_generator_count);

    CASE_EXPECT_EQ(0, task_mgr->start(another_task_id));

    CASE_EXPECT_EQ(old_resume_generator_count + 1, g_task_manager_future_resume_generator_count);
    CASE_EXPECT_EQ(old_suspend_generator_count + 2, g_task_manager_future_suspend_generator_count);

    task_manager_resume_pending_contexts({901, 902});

    CASE_EXPECT_EQ(old_resume_generator_count + 2, g_task_manager_future_resume_generator_count);
    CASE_EXPECT_EQ(old_suspend_generator_count + 2, g_task_manager_future_suspend_generator_count);

    CASE_EXPECT_EQ(0, (int)task_mgr->get_task_size());
    CASE_EXPECT_EQ(0, (int)task_mgr->get_tick_checkpoint_size());
  }
  task_manager_resume_pending_contexts({});
}

CASE_TEST(task_promise_task_manager, kill) {
  {
    size_t old_resume_generator_count = g_task_manager_future_resume_generator_count;
    size_t old_suspend_generator_count = g_task_manager_future_suspend_generator_count;
    using mgr_t = cotask::task_manager<task_future_int_type>;
    mgr_t::ptr_type task_mgr = mgr_t::create();

    task_future_int_type co_task = task_func_await_int();
    task_future_int_type co_another_task = task_func_await_int();

    CASE_EXPECT_EQ(0, (int)task_mgr->get_task_size());
    task_mgr->tick(10, 0);

    task_mgr->add_task(co_task, 10, 0);
    task_mgr->add_task(co_another_task, 10, 0);

    task_mgr->start(co_task.get_id());
    task_mgr->start(co_another_task.get_id());

    CASE_EXPECT_EQ(2, (int)task_mgr->get_task_size());
    CASE_EXPECT_EQ(2, (int)task_mgr->get_tick_checkpoint_size());

    task_mgr->kill(co_task.get_id());
    task_mgr->cancel(co_another_task.get_id());

    CASE_EXPECT_EQ(old_resume_generator_count + 2, g_task_manager_future_resume_generator_count);
    CASE_EXPECT_EQ(old_suspend_generator_count + 2, g_task_manager_future_suspend_generator_count);

    CASE_EXPECT_EQ(0, (int)task_mgr->get_task_size());
    CASE_EXPECT_EQ(0, (int)task_mgr->get_tick_checkpoint_size());

    CASE_EXPECT_TRUE(task_future_int_type::task_status_type::kKilled == co_task.get_status());
    CASE_EXPECT_TRUE(task_future_int_type::task_status_type::kCancle == co_another_task.get_status());
  }

  task_manager_resume_pending_contexts({});
}

CASE_TEST(task_promise_task_manager, duplicated_checkpoints) {
  {
    size_t old_resume_generator_count = g_task_manager_future_resume_generator_count;
    size_t old_suspend_generator_count = g_task_manager_future_suspend_generator_count;
    using mgr_t = cotask::task_manager<task_future_int_type>;
    mgr_t::ptr_type task_mgr = mgr_t::create();

    task_future_int_type co_task = task_func_await_int();

    CASE_EXPECT_EQ(0, (int)task_mgr->get_task_size());

    task_mgr->tick(4);
    co_task.start();

    task_mgr->add_task(co_task, 10, 0);
    CASE_EXPECT_EQ(1, (int)task_mgr->get_task_size());
    CASE_EXPECT_EQ(1, (int)task_mgr->get_tick_checkpoint_size());

    task_mgr->tick(5);

    CASE_EXPECT_EQ(1, (int)task_mgr->get_task_size());
    CASE_EXPECT_EQ(1, (int)task_mgr->get_tick_checkpoint_size());

    task_mgr->tick(15, 1);

    CASE_EXPECT_EQ(0, (int)task_mgr->get_task_size());
    CASE_EXPECT_EQ(0, (int)task_mgr->get_tick_checkpoint_size());

    CASE_EXPECT_EQ(old_resume_generator_count + 1, g_task_manager_future_resume_generator_count);
    CASE_EXPECT_EQ(old_suspend_generator_count + 1, g_task_manager_future_suspend_generator_count);
  }
  task_manager_resume_pending_contexts({});
}

CASE_TEST(task_promise_task_manager, update_timeout) {
  {
    size_t old_resume_generator_count = g_task_manager_future_resume_generator_count;
    size_t old_suspend_generator_count = g_task_manager_future_suspend_generator_count;
    using mgr_t = cotask::task_manager<task_future_int_type>;
    mgr_t::ptr_type task_mgr = mgr_t::create();

    task_future_int_type co_task = task_func_await_int();

    CASE_EXPECT_EQ(0, (int)task_mgr->get_task_size());

    task_mgr->add_task(co_task, 10, 0);
    co_task.start();
    CASE_EXPECT_EQ(1, (int)task_mgr->get_task_size());
    CASE_EXPECT_EQ(1, (int)task_mgr->get_tick_checkpoint_size());

    task_mgr->tick(5);
    CASE_EXPECT_EQ(copp::COPP_EC_SUCCESS, task_mgr->set_timeout(co_task.get_id(), 20, 0));

    CASE_EXPECT_EQ(1, (int)task_mgr->get_task_size());
    CASE_EXPECT_EQ(1, (int)task_mgr->get_tick_checkpoint_size());

    task_mgr->tick(24, 1);

    CASE_EXPECT_EQ(1, (int)task_mgr->get_task_size());
    CASE_EXPECT_EQ(1, (int)task_mgr->get_tick_checkpoint_size());

    task_mgr->tick(25, 1);

    CASE_EXPECT_EQ(0, (int)task_mgr->get_task_size());
    CASE_EXPECT_EQ(0, (int)task_mgr->get_tick_checkpoint_size());

    CASE_EXPECT_EQ(old_resume_generator_count + 1, g_task_manager_future_resume_generator_count);
    CASE_EXPECT_EQ(old_suspend_generator_count + 1, g_task_manager_future_suspend_generator_count);
  }
  task_manager_resume_pending_contexts({});
}

// TODO: thread safety check

#    if defined(LIBCOTASK_MACRO_AUTO_CLEANUP_MANAGER) && LIBCOTASK_MACRO_AUTO_CLEANUP_MANAGER
CASE_TEST(task_promise_task_manager, auto_cleanup_for_manager) {
  {
    size_t old_resume_generator_count = g_task_manager_future_resume_generator_count;
    size_t old_suspend_generator_count = g_task_manager_future_suspend_generator_count;
    using mgr_t = cotask::task_manager<task_future_int_type>;

    task_future_int_type co_task = task_func_await_int();

    mgr_t::ptr_type task_mgr1 = mgr_t::create();
    mgr_t::ptr_type task_mgr2 = mgr_t::create();

    CASE_EXPECT_EQ(0, task_mgr1->add_task(co_task, 5, 0));
    CASE_EXPECT_EQ(copp::COPP_EC_TASK_ALREADY_IN_ANOTHER_MANAGER, task_mgr2->add_task(co_task, 5, 0));

    CASE_EXPECT_EQ(1, task_mgr1->get_task_size());
    CASE_EXPECT_EQ(1, task_mgr1->get_tick_checkpoint_size());

    co_task.start();
    task_manager_resume_pending_contexts({903});

    CASE_EXPECT_EQ(0, task_mgr1->get_task_size());
    CASE_EXPECT_EQ(0, task_mgr1->get_tick_checkpoint_size());

    CASE_EXPECT_EQ(old_resume_generator_count + 1, g_task_manager_future_resume_generator_count);
    CASE_EXPECT_EQ(old_suspend_generator_count + 1, g_task_manager_future_suspend_generator_count);
  }
  task_manager_resume_pending_contexts({});
}

CASE_TEST(task_promise_task_manager, remove_last_task_in_manager) {
  {
    size_t old_resume_generator_count = g_task_manager_future_resume_generator_count;
    size_t old_suspend_generator_count = g_task_manager_future_suspend_generator_count;
    using mgr_t = cotask::task_manager<task_future_int_type>;

    mgr_t::ptr_type task_mgr1 = mgr_t::create();

    {
      task_future_int_type co_task = task_func_await_int();

      CASE_EXPECT_EQ(0, task_mgr1->add_task(co_task, 5, 0));

      CASE_EXPECT_EQ(1, task_mgr1->get_task_size());
      CASE_EXPECT_EQ(1, task_mgr1->get_tick_checkpoint_size());

      co_task.start();
    }

    CASE_EXPECT_EQ(1, task_mgr1->get_task_size());
    CASE_EXPECT_EQ(1, task_mgr1->get_tick_checkpoint_size());

    task_manager_resume_pending_contexts({903});

    CASE_EXPECT_EQ(0, task_mgr1->get_task_size());
    CASE_EXPECT_EQ(0, task_mgr1->get_tick_checkpoint_size());

    CASE_EXPECT_EQ(old_resume_generator_count + 1, g_task_manager_future_resume_generator_count);
    CASE_EXPECT_EQ(old_suspend_generator_count + 1, g_task_manager_future_suspend_generator_count);
  }
  task_manager_resume_pending_contexts({});
}
#    endif

// rethrow a exception in c++20 coroutine will crash when using MSVC now(VS2022)
// We may enable exception in the future
#    if 0 && defined(LIBCOPP_MACRO_ENABLE_STD_EXCEPTION_PTR) && LIBCOPP_MACRO_ENABLE_STD_EXCEPTION_PTR
namespace {
static task_future_int_type task_func_await_int_start_exception() {
  generator_future_int_type generator{task_manager_generator_int_suspend_callback,
                                      task_manager_generator_int_resume_callback};

  throw std::exception("test-exception-start");

  auto gen_res = co_await generator;
  co_return gen_res;
}

static task_future_int_type task_func_await_int_resume_exception() {
  generator_future_int_type generator{task_manager_generator_int_suspend_callback,
                                      task_manager_generator_int_resume_callback};

  auto gen_res = co_await generator;

  throw std::exception("test-exception-resume");

  co_return gen_res;
}

}  // namespace

CASE_TEST(task_promise_task_manager, start_exception_safe) {
  {
    size_t old_resume_generator_count = g_task_manager_future_resume_generator_count;
    size_t old_suspend_generator_count = g_task_manager_future_suspend_generator_count;
    using mgr_t = cotask::task_manager<task_future_int_type>;

    task_future_int_type co_task = task_func_await_int_start_exception();
    mgr_t::ptr_type task_mgr = mgr_t::create();

    CASE_EXPECT_EQ(0, task_mgr->add_task(co_task, 5, 0));

    CASE_EXPECT_EQ(1, task_mgr->get_task_size());
    CASE_EXPECT_EQ(1, task_mgr->get_tick_checkpoint_size());

    try {
      co_task.start();
      task_manager_resume_pending_contexts({907});
    } catch (const std::exception& e) {
      CASE_MSG_INFO() << "Catch a exception: " << e.what() << std::endl;
    } catch (...) {
      CASE_MSG_INFO() << "Catch a exception" << std::endl;
    }

    CASE_EXPECT_EQ(0, task_mgr->get_task_size());
    CASE_EXPECT_EQ(0, task_mgr->get_tick_checkpoint_size());
    CASE_EXPECT_TRUE(co_task.is_completed());

    CASE_EXPECT_EQ(old_resume_generator_count, g_task_manager_future_resume_generator_count);
    CASE_EXPECT_EQ(old_suspend_generator_count, g_task_manager_future_suspend_generator_count);
  }
  task_manager_resume_pending_contexts({});
}

CASE_TEST(task_promise_task_manager, resume_exception_safe) {
  {
    size_t old_resume_generator_count = g_task_manager_future_resume_generator_count;
    size_t old_suspend_generator_count = g_task_manager_future_suspend_generator_count;
    using mgr_t = cotask::task_manager<task_future_int_type>;

    task_future_int_type co_task = task_func_await_int_resume_exception();
    mgr_t::ptr_type task_mgr = mgr_t::create();

    CASE_EXPECT_EQ(0, task_mgr->add_task(co_task, 5, 0));

    CASE_EXPECT_EQ(1, task_mgr->get_task_size());
    CASE_EXPECT_EQ(1, task_mgr->get_tick_checkpoint_size());

    try {
      co_task.start();
      task_manager_resume_pending_contexts({907});
    } catch (const std::exception& e) {
      CASE_MSG_INFO() << "Catch a exception: " << e.what() << std::endl;
    } catch (...) {
      CASE_MSG_INFO() << "Catch a exception" << std::endl;
    }

    CASE_EXPECT_EQ(0, task_mgr->get_task_size());
    CASE_EXPECT_EQ(0, task_mgr->get_tick_checkpoint_size());
    CASE_EXPECT_TRUE(co_task.is_completed());

    CASE_EXPECT_EQ(old_resume_generator_count, g_task_manager_future_resume_generator_count);
    CASE_EXPECT_EQ(old_suspend_generator_count, g_task_manager_future_suspend_generator_count);
  }
  task_manager_resume_pending_contexts({});
}

#    endif

#  else
CASE_TEST(task_promise_task_manager, disabled) {}
#  endif
#else
CASE_TEST(task_promise_task_manager, disabled) {}
#endif

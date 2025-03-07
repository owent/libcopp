// Copyright 2025 owent

#include <libcopp/coroutine/callable_promise.h>
#include <libcopp/coroutine/stackful_channel.h>
#include <libcopp/stack/stack_pool.h>
#include <libcotask/task.h>

#include <cstdio>
#include <cstring>
#include <iostream>
#include <memory>
#include <set>
#include <vector>

#include "frame/test_macros.h"

#ifdef LIBCOTASK_MACRO_ENABLED
static int g_test_coroutine_task_channel_status = 0;
static int g_test_coroutine_task_channel_on_finished = 0;
class test_context_task_channel_action : public cotask::impl::task_action_impl {
 public:
  test_context_task_channel_action(bool transform_error = false) : last_value(0), transform_error_(transform_error) {}

  copp::stackful_channel_receiver<int> create_channel() {
    auto ret = copp::make_stackful_channel<int>();
    sender = copp::memory::make_strong_rc<copp::stackful_channel_sender<int>>(ret.second);
    return std::move(ret.first);
  }

  int operator()(void *priv_data) override {
    ++g_test_coroutine_task_channel_status;

    CASE_EXPECT_EQ(&g_test_coroutine_task_channel_status, priv_data);

    if (transform_error_) {
      last_value =
          cotask::task<>::this_task()->await_value(create_channel(), [](copp::copp_error_code) { return -17; });
    } else {
      last_value = cotask::task<>::this_task()->await_value(create_channel());
    }

    ++g_test_coroutine_task_channel_status;

    return 0;
  }

  int on_finished(cotask::impl::task_impl &) override {
    ++g_test_coroutine_task_channel_on_finished;
    return 0;
  }

  copp::memory::strong_rc_ptr<copp::stackful_channel_sender<int>> sender;
  int last_value;
  bool transform_error_;
};

CASE_TEST(coroutine_task_channel, basic_int_channel) {
  typedef cotask::task<>::ptr_t task_ptr_type;
  g_test_coroutine_task_channel_status = 0;
  g_test_coroutine_task_channel_on_finished = 0;

  {
    task_ptr_type co_task = cotask::task<>::create(test_context_task_channel_action());
    task_ptr_type co_another_task = cotask::task<>::create(test_context_task_channel_action(true));

    CASE_EXPECT_EQ(cotask::EN_TS_CREATED, co_task->get_status());
    CASE_EXPECT_EQ(cotask::EN_TS_CREATED, co_another_task->get_status());

    CASE_EXPECT_EQ(0, co_task->start(&g_test_coroutine_task_channel_status));

    CASE_EXPECT_EQ(g_test_coroutine_task_channel_status, 1);
    CASE_EXPECT_FALSE(co_task->is_completed());
    CASE_EXPECT_FALSE(co_task->is_canceled());
    CASE_EXPECT_FALSE(co_task->is_faulted());

    CASE_EXPECT_EQ(0, co_another_task->start(&g_test_coroutine_task_channel_status));
    CASE_EXPECT_EQ(g_test_coroutine_task_channel_status, 2);

    CASE_EXPECT_EQ(cotask::EN_TS_WAITING, co_task->get_status());
    CASE_EXPECT_EQ(cotask::EN_TS_WAITING, co_another_task->get_status());

    static_cast<test_context_task_channel_action *>(co_task->get_raw_action())->sender->set_value(357);
    CASE_EXPECT_EQ(g_test_coroutine_task_channel_status, 3);
    CASE_EXPECT_EQ(static_cast<test_context_task_channel_action *>(co_task->get_raw_action())->last_value, 357);

    static_cast<test_context_task_channel_action *>(co_another_task->get_raw_action())->sender->reset_value();
    static_cast<test_context_task_channel_action *>(co_another_task->get_raw_action())->sender->set_value(579);
    CASE_EXPECT_EQ(g_test_coroutine_task_channel_status, 4);
    CASE_EXPECT_EQ(static_cast<test_context_task_channel_action *>(co_another_task->get_raw_action())->last_value, 579);

    CASE_EXPECT_TRUE(co_task->is_completed());
    CASE_EXPECT_TRUE(co_another_task->is_completed());
    CASE_EXPECT_FALSE(co_task->is_canceled());
    CASE_EXPECT_FALSE(co_task->is_faulted());

    CASE_EXPECT_EQ(co_task->get_status(), cotask::EN_TS_DONE);
    CASE_EXPECT_EQ(co_another_task->get_status(), cotask::EN_TS_DONE);
  }

  CASE_EXPECT_EQ(g_test_coroutine_task_channel_on_finished, 2);
}

CASE_TEST(coroutine_task_channel, transform_error_for_int_channel) {
  typedef cotask::task<>::ptr_t task_ptr_type;
  g_test_coroutine_task_channel_status = 0;
  g_test_coroutine_task_channel_on_finished = 0;

  {
    task_ptr_type co_task = cotask::task<>::create(test_context_task_channel_action());
    task_ptr_type co_another_task = cotask::task<>::create(test_context_task_channel_action(true));

    CASE_EXPECT_EQ(cotask::EN_TS_CREATED, co_task->get_status());
    CASE_EXPECT_EQ(cotask::EN_TS_CREATED, co_another_task->get_status());

    CASE_EXPECT_EQ(0, co_task->start(&g_test_coroutine_task_channel_status));

    CASE_EXPECT_EQ(g_test_coroutine_task_channel_status, 1);
    CASE_EXPECT_FALSE(co_task->is_completed());
    CASE_EXPECT_FALSE(co_task->is_canceled());
    CASE_EXPECT_FALSE(co_task->is_faulted());

    CASE_EXPECT_EQ(0, co_another_task->start(&g_test_coroutine_task_channel_status));
    CASE_EXPECT_EQ(g_test_coroutine_task_channel_status, 2);

    CASE_EXPECT_EQ(cotask::EN_TS_WAITING, co_task->get_status());
    CASE_EXPECT_EQ(cotask::EN_TS_WAITING, co_another_task->get_status());

    co_task->kill();
    CASE_EXPECT_EQ(g_test_coroutine_task_channel_status, 3);
    CASE_EXPECT_EQ(static_cast<test_context_task_channel_action *>(co_task->get_raw_action())->last_value,
                   copp::COPP_EC_OPERATION_CANCLE);

    static_cast<test_context_task_channel_action *>(co_another_task->get_raw_action())->sender->reset_value();
    co_another_task->kill();

    CASE_EXPECT_EQ(g_test_coroutine_task_channel_status, 4);
    CASE_EXPECT_EQ(static_cast<test_context_task_channel_action *>(co_another_task->get_raw_action())->last_value, -17);

    CASE_EXPECT_TRUE(co_task->is_completed());
    CASE_EXPECT_TRUE(co_another_task->is_completed());
    CASE_EXPECT_FALSE(co_task->is_canceled());
    CASE_EXPECT_TRUE(co_task->is_faulted());

    CASE_EXPECT_EQ(co_task->get_status(), cotask::EN_TS_KILLED);
    CASE_EXPECT_EQ(co_another_task->get_status(), cotask::EN_TS_KILLED);
  }

  CASE_EXPECT_EQ(g_test_coroutine_task_channel_on_finished, 2);
}

#endif

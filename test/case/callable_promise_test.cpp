// Copyright 2022 owent

#include <libcopp/coroutine/callable_promise.h>

#include <cstdio>
#include <cstring>
#include <iostream>
#include <list>
#include <string>

#include "frame/test_macros.h"

#if defined(LIBCOPP_MACRO_ENABLE_STD_COROUTINE) && LIBCOPP_MACRO_ENABLE_STD_COROUTINE

struct callable_promise_test_pending_awaitable {
  bool await_ready() noexcept { return false; }

  void await_resume() noexcept { detach(); }

#  if defined(LIBCOPP_MACRO_ENABLE_CONCEPTS) && LIBCOPP_MACRO_ENABLE_CONCEPTS
  template <copp::DerivedPromiseBaseType TPROMISE>
#  else
  template <class TPROMISE, typename = std::enable_if_t<std::is_base_of<copp::promise_base_type, TPROMISE>::value> >
#  endif
  void await_suspend(LIBCOPP_MACRO_STD_COROUTINE_NAMESPACE coroutine_handle<TPROMISE> caller) noexcept {
    pending.push_back(caller);
    current = caller;
  }

  void detach() noexcept {
    if (!current) {
      return;
    }

    for (auto iter = pending.begin(); iter != pending.end(); ++iter) {
      if (*iter == current) {
        pending.erase(iter);
        current = nullptr;
        break;
      }
    }
  }

  callable_promise_test_pending_awaitable() {}
  ~callable_promise_test_pending_awaitable() {
    // detach when destroyed
    detach();
  }

  LIBCOPP_MACRO_STD_COROUTINE_NAMESPACE coroutine_handle<> current = nullptr;

  static std::list<LIBCOPP_MACRO_STD_COROUTINE_NAMESPACE coroutine_handle<> > pending;
  static void resume_all() {
    while (!pending.empty()) {
      pending.front().resume();
    }
  }
};
std::list<LIBCOPP_MACRO_STD_COROUTINE_NAMESPACE coroutine_handle<> > callable_promise_test_pending_awaitable::pending;

static copp::callable_future<int> callable_func_int_l1(int inout) {
  CASE_MSG_INFO() << "callable inner future ready int: " << inout << std::endl;
  co_return inout;
}

static copp::callable_future<int> callable_func_int_l2(int inout) {
  CASE_MSG_INFO() << "callable inner future async int: " << inout << std::endl;
  co_await callable_promise_test_pending_awaitable();
  CASE_MSG_INFO() << "callable inner future return int: " << inout << std::endl;
  co_return inout;
}

static copp::callable_future<int> callable_func_await_int() {
  auto v = callable_func_int_l1(3);
  auto u = callable_func_int_l2(11);
  CASE_MSG_INFO() << "callable await int" << std::endl;
  int x = (co_await v + co_await u);
  CASE_MSG_INFO() << "callable return int" << std::endl;
  co_return x;
}

CASE_TEST(callable_promise, callable_future_integer_need_resume) {
  copp::callable_future<int> f = callable_func_await_int();
  CASE_EXPECT_EQ(static_cast<int>(copp::promise_status::kCreated), static_cast<int>(f.get_status()));
  // Start
  f.start();

  CASE_EXPECT_NE(static_cast<int>(copp::promise_status::kDone), static_cast<int>(f.get_status()));
  CASE_EXPECT_FALSE(f.is_ready());

  callable_promise_test_pending_awaitable::resume_all();
  CASE_EXPECT_EQ(static_cast<int>(copp::promise_status::kDone), static_cast<int>(f.get_status()));
  CASE_EXPECT_TRUE(f.is_ready());
  CASE_EXPECT_EQ(14, f.get_internal_promise().data());
}

static copp::callable_future<int> callable_func_await_int_ready() {
  auto v = callable_func_int_l1(33);
  auto u = callable_func_int_l1(31);
  CASE_MSG_INFO() << "callable await int no wait" << std::endl;
  int x = (co_await v + co_await u);
  CASE_MSG_INFO() << "callable return int no wait" << std::endl;
  co_return x;
}

CASE_TEST(callable_promise, callable_future_integer_ready) {
  copp::callable_future<int> f = callable_func_await_int_ready();
  CASE_EXPECT_EQ(static_cast<int>(copp::promise_status::kCreated), static_cast<int>(f.get_status()));
  // Start
  f.start();

  CASE_EXPECT_EQ(static_cast<int>(copp::promise_status::kDone), static_cast<int>(f.get_status()));
  CASE_EXPECT_TRUE(f.is_ready());
  CASE_EXPECT_EQ(64, f.get_internal_promise().data());
}

static copp::callable_future<void> callable_func_void_l1() {
  CASE_MSG_INFO() << "callable inner future ready void" << std::endl;
  co_return;
}

static copp::callable_future<void> callable_func_void_l2() {
  CASE_MSG_INFO() << "callable inner future async void" << std::endl;
  co_await callable_promise_test_pending_awaitable();
  CASE_MSG_INFO() << "callable inner future return void" << std::endl;
  co_return;
}

static copp::callable_future<void> callable_func_await_void() {
  CASE_MSG_INFO() << "callable await void" << std::endl;
  co_await callable_func_void_l1();
  co_await callable_func_void_l2();
  CASE_MSG_INFO() << "callable return void" << std::endl;
  co_return;
}

CASE_TEST(callable_promise, callable_future_void_need_resume) {
  copp::callable_future<void> f = callable_func_await_void();
  CASE_EXPECT_EQ(static_cast<int>(copp::promise_status::kCreated), static_cast<int>(f.get_status()));
  // Start
  f.start();

  CASE_EXPECT_NE(static_cast<int>(copp::promise_status::kDone), static_cast<int>(f.get_status()));
  CASE_EXPECT_FALSE(f.is_ready());
  callable_promise_test_pending_awaitable::resume_all();
  CASE_EXPECT_EQ(static_cast<int>(copp::promise_status::kDone), static_cast<int>(f.get_status()));
  CASE_EXPECT_TRUE(f.is_ready());
}

static copp::callable_future<void> callable_func_await_void_ready() {
  CASE_MSG_INFO() << "callable await void no wait" << std::endl;
  co_await callable_func_void_l1();
  co_await callable_func_void_l1();
  CASE_MSG_INFO() << "callable return void no wait" << std::endl;
  co_return;
}

CASE_TEST(callable_promise, callable_future_void_ready) {
  copp::callable_future<void> f = callable_func_await_void_ready();
  CASE_EXPECT_EQ(static_cast<int>(copp::promise_status::kCreated), static_cast<int>(f.get_status()));
  // Start
  f.start();

  callable_promise_test_pending_awaitable::resume_all();
  CASE_EXPECT_EQ(static_cast<int>(copp::promise_status::kDone), static_cast<int>(f.get_status()));
  CASE_EXPECT_TRUE(f.is_ready());
}

static copp::callable_future<int> callable_func_int_await_void() {
  CASE_MSG_INFO() << "callable int await void: start" << std::endl;
  co_await callable_func_void_l1();
  co_await callable_func_void_l2();
  auto v = callable_func_int_l1(17);
  auto u = callable_func_int_l2(23);
  int x = (co_await v + co_await u);
  CASE_MSG_INFO() << "callable int await void: return" << std::endl;
  co_return x;
}

CASE_TEST(callable_promise, callable_future_int_await_void) {
  copp::callable_future<int> f = callable_func_int_await_void();
  CASE_EXPECT_EQ(static_cast<int>(copp::promise_status::kCreated), static_cast<int>(f.get_status()));
  // Start
  f.start();

  CASE_EXPECT_NE(static_cast<int>(copp::promise_status::kDone), static_cast<int>(f.get_status()));
  CASE_EXPECT_FALSE(f.is_ready());
  callable_promise_test_pending_awaitable::resume_all();
  CASE_EXPECT_EQ(static_cast<int>(copp::promise_status::kDone), static_cast<int>(f.get_status()));
  CASE_EXPECT_TRUE(f.is_ready());
  CASE_EXPECT_EQ(40, f.get_internal_promise().data());
}

static copp::callable_future<int> callable_func_killed_by_caller_l3() {
  co_await callable_promise_test_pending_awaitable();
  auto current_status = co_yield copp::callable_future<int>::yield_status();
  CASE_EXPECT_TRUE(copp::promise_status::kKilled == current_status);

  // await again and return immdiately
  // co_await callable_promise_test_pending_awaitable();
  co_return -static_cast<int>(current_status);
}

static copp::callable_future<int> callable_func_killed_by_caller_l2() {
  int result = co_await callable_func_killed_by_caller_l3();
  co_return result;
}

static copp::callable_future<int> callable_func_killed_by_caller_l1() {
  int result = co_await callable_func_killed_by_caller_l2();
  co_return result;
}

CASE_TEST(callable_promise, killed_by_caller_resume_waiting) {
  copp::callable_future<int> f = callable_func_killed_by_caller_l1();
  CASE_EXPECT_EQ(static_cast<int>(copp::promise_status::kCreated), static_cast<int>(f.get_status()));
  // Start
  f.start();

  CASE_EXPECT_NE(static_cast<int>(copp::promise_status::kDone), static_cast<int>(f.get_status()));
  CASE_EXPECT_FALSE(f.is_ready());

  // Mock to kill by caller
  f.kill(copp::promise_status::kKilled);
  CASE_EXPECT_TRUE(f.is_ready());
  CASE_EXPECT_EQ(f.get_internal_promise().data(), -static_cast<int>(copp::promise_status::kKilled));

  // cleanup
  callable_promise_test_pending_awaitable::resume_all();
  CASE_EXPECT_EQ(static_cast<int>(copp::promise_status::kKilled), static_cast<int>(f.get_status()));
}

CASE_TEST(callable_promise, killed_by_caller_drop_generator) {
  copp::callable_future<int> f = callable_func_killed_by_caller_l2();
  CASE_EXPECT_EQ(static_cast<int>(copp::promise_status::kCreated), static_cast<int>(f.get_status()));
  // Start
  f.start();

  CASE_EXPECT_NE(static_cast<int>(copp::promise_status::kDone), static_cast<int>(f.get_status()));
  CASE_EXPECT_FALSE(f.is_ready());

  // Mock to kill by caller
  f.kill(copp::promise_status::kKilled);
  CASE_EXPECT_TRUE(f.is_ready());
  CASE_EXPECT_EQ(f.get_internal_promise().data(), -static_cast<int>(copp::promise_status::kKilled));

  // cleanup
  callable_promise_test_pending_awaitable::resume_all();
  CASE_EXPECT_EQ(static_cast<int>(copp::promise_status::kKilled), static_cast<int>(f.get_status()));
}

#else
CASE_TEST(callable_promise, disabled) {}
#endif
// Copyright 2023 owent

#include <libcopp/coroutine/algorithm.h>
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
  template <class TPROMISE, typename = std::enable_if_t<std::is_base_of<copp::promise_base_type, TPROMISE>::value>>
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

  static std::list<LIBCOPP_MACRO_STD_COROUTINE_NAMESPACE coroutine_handle<>> pending;
  static void resume_all() {
    while (!pending.empty()) {
      pending.front().resume();
    }
  }

  static void resume_some(int max_count) {
    while (!pending.empty() && max_count-- > 0) {
      pending.front().resume();
    }
  }
};
std::list<LIBCOPP_MACRO_STD_COROUTINE_NAMESPACE coroutine_handle<>> callable_promise_test_pending_awaitable::pending;

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
  CASE_EXPECT_EQ(static_cast<int>(copp::promise_status::kRunning), static_cast<int>(f.get_status()));

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
  CASE_EXPECT_EQ(static_cast<int>(copp::promise_status::kRunning), static_cast<int>(f.get_status()));

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
  CASE_EXPECT_EQ(static_cast<int>(copp::promise_status::kDone), static_cast<int>(f.get_status()));
  CASE_EXPECT_TRUE(f.is_ready());

  callable_promise_test_pending_awaitable::resume_all();
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
  CASE_EXPECT_EQ(static_cast<int>(copp::promise_status::kRunning), static_cast<int>(f.get_status()));

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
  CASE_EXPECT_EQ(static_cast<int>(copp::promise_status::kRunning), static_cast<int>(f.get_status()));

  CASE_EXPECT_FALSE(f.is_ready());

  // Mock to kill by caller
  f.kill(copp::promise_status::kKilled, true);
  CASE_EXPECT_TRUE(f.is_ready());
  CASE_EXPECT_EQ(f.get_internal_promise().data(), -static_cast<int>(copp::promise_status::kKilled));

  // cleanup
  callable_promise_test_pending_awaitable::resume_all();
  CASE_EXPECT_EQ(static_cast<int>(copp::promise_status::kKilled), static_cast<int>(f.get_status()));
}

CASE_TEST(callable_promise, killed_by_caller_drop_generator) {
  copp::callable_future<int> f = callable_func_killed_by_caller_l2();
  CASE_EXPECT_EQ(static_cast<int>(copp::promise_status::kRunning), static_cast<int>(f.get_status()));

  CASE_EXPECT_FALSE(f.is_ready());

  // Mock to kill by caller
  f.kill(copp::promise_status::kKilled, true);
  CASE_EXPECT_TRUE(f.is_ready());
  CASE_EXPECT_EQ(f.get_internal_promise().data(), -static_cast<int>(copp::promise_status::kKilled));

  // cleanup
  callable_promise_test_pending_awaitable::resume_all();
  CASE_EXPECT_EQ(static_cast<int>(copp::promise_status::kKilled), static_cast<int>(f.get_status()));
}

static copp::callable_future<int> callable_func_some_any_all_callable_suspend(int value) {
  co_await callable_promise_test_pending_awaitable();
  co_return value;
}

static copp::callable_future<int> callable_func_some_callable_in_container(size_t expect_ready_count,
                                                                           copp::promise_status expect_status) {
  size_t resume_ready_count = 0;

  std::vector<copp::callable_future<int>> callables;
  callables.emplace_back(callable_func_some_any_all_callable_suspend(471));
  callables.emplace_back(callable_func_some_any_all_callable_suspend(473));
  callables.emplace_back(callable_func_some_any_all_callable_suspend(477));

  copp::some_ready<copp::callable_future<int>>::type readys;
  auto some_result = co_await copp::some(readys, 2, copp::gsl::make_span(callables));
  CASE_EXPECT_EQ(static_cast<int>(expect_status), static_cast<int>(some_result));

  int result = 1;
  for (auto &ready_callable : readys) {
    if (ready_callable->is_ready()) {
      result += ready_callable->get_internal_promise().data();
      ++resume_ready_count;
    }
  }

  CASE_EXPECT_EQ(expect_ready_count, resume_ready_count);

  // Nothing happend here if we await the callables again.
  some_result = co_await copp::some(readys, 2, callables);
  CASE_EXPECT_EQ(static_cast<int>(expect_status), static_cast<int>(some_result));

  // If it's killed, await will trigger suspend and resume again, or it will return directly.
  CASE_EXPECT_EQ(expect_ready_count, resume_ready_count);

  co_return result;
}

CASE_TEST(callable_promise, finish_some_in_container) {
  auto f = callable_func_some_callable_in_container(2, copp::promise_status::kDone);

  CASE_EXPECT_FALSE(f.is_ready());

  // partly resume
  callable_promise_test_pending_awaitable::resume_some(1);
  CASE_EXPECT_FALSE(f.is_ready());
  callable_promise_test_pending_awaitable::resume_some(1);

  CASE_EXPECT_TRUE(f.is_ready());
  CASE_EXPECT_EQ(945, f.get_internal_promise().data());

  callable_promise_test_pending_awaitable::resume_all();
}

CASE_TEST(callable_promise, kill_some_in_container) {
  auto f = callable_func_some_callable_in_container(0, copp::promise_status::kKilled);

  CASE_EXPECT_FALSE(f.is_ready());

  // partly resume
  f.kill();

  CASE_EXPECT_TRUE(f.is_ready());
  CASE_EXPECT_EQ(1, f.get_internal_promise().data());

  callable_promise_test_pending_awaitable::resume_all();
}

static copp::callable_future<int> callable_func_some_callable_in_initialize_list(size_t expect_ready_count,
                                                                                 copp::promise_status expect_status) {
  size_t resume_ready_count = 0;

  copp::callable_future<int> callable1 = callable_func_some_any_all_callable_suspend(471);
  copp::callable_future<int> callable2 = callable_func_some_any_all_callable_suspend(473);
  copp::callable_future<int> callable3 = callable_func_some_any_all_callable_suspend(477);

  copp::some_ready<copp::callable_future<int>>::type readys;
  std::reference_wrapper<copp::callable_future<int>> pending[] = {callable1, callable2, callable3};
  auto some_result = co_await copp::some(readys, 2, copp::gsl::make_span(pending));
  CASE_EXPECT_EQ(static_cast<int>(expect_status), static_cast<int>(some_result));

  int result = 1;
  for (auto &ready_callable : readys) {
    if (ready_callable->is_ready()) {
      result += ready_callable->get_internal_promise().data();
      ++resume_ready_count;
    }
  }

  CASE_EXPECT_EQ(expect_ready_count, resume_ready_count);

  co_return result;
}

CASE_TEST(callable_promise, finish_some_in_initialize_list) {
  auto f = callable_func_some_callable_in_initialize_list(2, copp::promise_status::kDone);

  CASE_EXPECT_FALSE(f.is_ready());

  // partly resume
  callable_promise_test_pending_awaitable::resume_some(1);
  CASE_EXPECT_FALSE(f.is_ready());
  callable_promise_test_pending_awaitable::resume_some(1);

  CASE_EXPECT_TRUE(f.is_ready());
  CASE_EXPECT_EQ(945, f.get_internal_promise().data());

  callable_promise_test_pending_awaitable::resume_all();
}

static copp::callable_future<int> callable_func_any_callable_in_container(size_t expect_ready_count,
                                                                          copp::promise_status expect_status) {
  size_t resume_ready_count = 0;

  std::vector<copp::callable_future<int>> callables;
  callables.emplace_back(callable_func_some_any_all_callable_suspend(671));
  callables.emplace_back(callable_func_some_any_all_callable_suspend(673));
  callables.emplace_back(callable_func_some_any_all_callable_suspend(677));

  copp::any_ready<copp::callable_future<int>>::type readys;
  auto any_result = co_await copp::any(readys, copp::gsl::make_span(&callables[0], &callables[0] + callables.size()));
  CASE_EXPECT_EQ(static_cast<int>(expect_status), static_cast<int>(any_result));

  int result = 1;
  for (auto &ready_callable : readys) {
    if (ready_callable->is_ready()) {
      result += ready_callable->get_internal_promise().data();
      ++resume_ready_count;
    }
  }

  CASE_EXPECT_EQ(expect_ready_count, resume_ready_count);

  // Nothing happend here if we await the callables again.
  any_result = co_await copp::any(readys, callables);
  CASE_EXPECT_EQ(static_cast<int>(expect_status), static_cast<int>(any_result));

  // If it's killed, await will trigger suspend and resume again, or it will return directly.
  CASE_EXPECT_EQ(expect_ready_count, resume_ready_count);

  co_return result;
}

CASE_TEST(callable_promise, finish_any_in_container) {
  auto f = callable_func_any_callable_in_container(1, copp::promise_status::kDone);

  CASE_EXPECT_FALSE(f.is_ready());

  // partly resume
  callable_promise_test_pending_awaitable::resume_some(1);

  CASE_EXPECT_TRUE(f.is_ready());
  CASE_EXPECT_EQ(672, f.get_internal_promise().data());

  callable_promise_test_pending_awaitable::resume_all();
}

static copp::callable_future<int> callable_func_all_callable_in_container(size_t expect_ready_count,
                                                                          copp::promise_status expect_status) {
  size_t resume_ready_count = 0;

  std::vector<copp::callable_future<int>> callables;
  callables.emplace_back(callable_func_some_any_all_callable_suspend(671));
  callables.emplace_back(callable_func_some_any_all_callable_suspend(791));
  callables.emplace_back(callable_func_some_any_all_callable_suspend(793));

  copp::all_ready<copp::callable_future<int>>::type readys;
  auto all_result = co_await copp::all(readys, copp::gsl::make_span(callables.data(), callables.size()));
  CASE_EXPECT_EQ(static_cast<int>(expect_status), static_cast<int>(all_result));

  int result = 1;
  for (auto &ready_callable : readys) {
    if (ready_callable->is_ready()) {
      result += ready_callable->get_internal_promise().data();
      ++resume_ready_count;
    }
  }

  CASE_EXPECT_EQ(expect_ready_count, resume_ready_count);

  // Nothing happend here if we await the callables again.
  all_result = co_await copp::all(readys, callables);
  CASE_EXPECT_EQ(static_cast<int>(expect_status), static_cast<int>(all_result));

  // If it's killed, await will trigger suspend and resume again, or it will return directly.
  CASE_EXPECT_EQ(expect_ready_count, resume_ready_count);

  co_return result;
}

CASE_TEST(callable_promise, finish_all_in_container) {
  auto f = callable_func_all_callable_in_container(3, copp::promise_status::kDone);

  CASE_EXPECT_FALSE(f.is_ready());

  // partly resume
  callable_promise_test_pending_awaitable::resume_some(1);
  CASE_EXPECT_FALSE(f.is_ready());

  callable_promise_test_pending_awaitable::resume_all();

  CASE_EXPECT_TRUE(f.is_ready());
  CASE_EXPECT_EQ(2256, f.get_internal_promise().data());

  callable_promise_test_pending_awaitable::resume_all();
}

#else
CASE_TEST(callable_promise, disabled) {}
#endif
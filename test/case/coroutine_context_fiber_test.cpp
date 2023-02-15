// Copyright 2023 owent

#include <libcopp/coroutine/coroutine_context_fiber_container.h>

#include <cstdio>
#include <cstring>
#include <iostream>

#include "frame/test_macros.h"

#if defined(LIBCOPP_MACRO_ENABLE_WIN_FIBER) && LIBCOPP_MACRO_ENABLE_WIN_FIBER

typedef copp::coroutine_fiber_context_default test_coroutine_context_fiber_test_type;

static int g_test_coroutine_fiber_status = 0;

class test_context_fiber_foo_runner {
 public:
  test_context_fiber_foo_runner() : call_times(0) {}
  int call_times;
  int operator()(void *) {
    ++call_times;
    ++g_test_coroutine_fiber_status;
    copp::this_fiber::get<test_coroutine_context_fiber_test_type>()->yield();

    ++g_test_coroutine_fiber_status;
    return 0;
  }
};

CASE_TEST(coroutine_fiber, context_base) {
  g_test_coroutine_fiber_status = 0;
  ++g_test_coroutine_fiber_status;
  CASE_EXPECT_EQ(g_test_coroutine_fiber_status, 1);

  test_context_fiber_foo_runner runner;
  {
    copp::stack_context test_move_alloc;

    test_coroutine_context_fiber_test_type::ptr_t co = test_coroutine_context_fiber_test_type::create(&runner);
    runner.call_times = 0;

    CASE_EXPECT_TRUE(!!co);

    CASE_EXPECT_EQ(LIBCOPP_COPP_NAMESPACE_ID::COPP_EC_NOT_RUNNING, co->yield());

    co->start();

    ++g_test_coroutine_fiber_status;
    CASE_EXPECT_EQ(g_test_coroutine_fiber_status, 3);
    co->resume();

    ++g_test_coroutine_fiber_status;
    CASE_EXPECT_EQ(g_test_coroutine_fiber_status, 5);

    CASE_EXPECT_EQ(LIBCOPP_COPP_NAMESPACE_ID::COPP_EC_NOT_READY, co->resume());
    CASE_EXPECT_EQ(LIBCOPP_COPP_NAMESPACE_ID::COPP_EC_ALREADY_EXIST, co->yield());
  }

  {
    g_test_coroutine_fiber_status = 1;
    copp::stack_context test_move_alloc;

    test_coroutine_context_fiber_test_type::ptr_t co = test_coroutine_context_fiber_test_type::create(&runner);
    runner.call_times = 0;

    CASE_EXPECT_TRUE(!!co);

    CASE_EXPECT_EQ(LIBCOPP_COPP_NAMESPACE_ID::COPP_EC_NOT_RUNNING, co->yield());

    co->start();

    ++g_test_coroutine_fiber_status;
    CASE_EXPECT_EQ(g_test_coroutine_fiber_status, 3);
    co->resume();

    ++g_test_coroutine_fiber_status;
    CASE_EXPECT_EQ(g_test_coroutine_fiber_status, 5);

    CASE_EXPECT_EQ(LIBCOPP_COPP_NAMESPACE_ID::COPP_EC_NOT_READY, co->resume());
    CASE_EXPECT_EQ(LIBCOPP_COPP_NAMESPACE_ID::COPP_EC_ALREADY_EXIST, co->yield());
  }
}

CASE_TEST(coroutine_fiber, shared_runner) {
  g_test_coroutine_fiber_status = 0;
  ++g_test_coroutine_fiber_status;
  CASE_EXPECT_EQ(g_test_coroutine_fiber_status, 1);

  test_context_fiber_foo_runner runner;
  runner.call_times = 0;

  {
    test_coroutine_context_fiber_test_type::ptr_t co[4];
    for (int i = 0; i < 4; ++i) {
      co[i] = test_coroutine_context_fiber_test_type::create(&runner);
      co[i]->start();
    }

    CASE_EXPECT_EQ(g_test_coroutine_fiber_status, 5);

    for (int i = 0; i < 4; ++i) {
      co[i]->resume();
    }

    CASE_EXPECT_EQ(g_test_coroutine_fiber_status, 9);

    CASE_EXPECT_EQ(runner.call_times, 4);
  }
}

#  if defined(LIBCOPP_MACRO_ENABLE_STD_EXCEPTION_PTR) && LIBCOPP_MACRO_ENABLE_STD_EXCEPTION_PTR
static int test_context_fiber_foo_runner_throw_exception(void *) { return static_cast<int>(std::string().at(1)); }

CASE_TEST(coroutine_fiber, coroutine_context_throw_exception) {
  try {
    copp::stack_context test_move_alloc;

    test_coroutine_context_fiber_test_type::ptr_t co =
        test_coroutine_context_fiber_test_type::create(test_context_fiber_foo_runner_throw_exception);

    CASE_EXPECT_TRUE(!!co);

    CASE_EXPECT_EQ(LIBCOPP_COPP_NAMESPACE_ID::COPP_EC_NOT_RUNNING, co->yield());

    co->start();
  } catch (const std::exception &e) {
    CASE_MSG_INFO() << "Caught exception \"" << e.what() << "\"" << std::endl;
  }
}

#  endif

#endif

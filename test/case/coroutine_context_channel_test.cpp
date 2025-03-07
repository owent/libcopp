// Copyright 2025 owent

#include <libcopp/coroutine/coroutine_context_container.h>
#include <libcopp/coroutine/coroutine_context_fiber_container.h>
#include <libcopp/coroutine/stackful_channel.h>

#include <cstdio>
#include <cstring>
#include <iostream>

#include "frame/test_macros.h"

using test_context_channel_coroutine_context_test_type =
    copp::coroutine_context_container<copp::allocator::stack_allocator_memory>;

#if defined(LIBCOPP_MACRO_ENABLE_WIN_FIBER) && LIBCOPP_MACRO_ENABLE_WIN_FIBER
using test_context_channel_coroutine_context_fiber_test_type =
    copp::coroutine_context_fiber_container<copp::allocator::stack_allocator_memory>;
#endif

static int g_test_coroutine_channel_status = 0;

template <class T>
struct test_context_channel_foo_error_transform;

template <>
struct test_context_channel_foo_error_transform<int> {
  int operator()(copp::copp_error_code err_code) { return static_cast<int>(err_code) - 10000; }
};

template <>
struct test_context_channel_foo_error_transform<int *> {
  int *operator()(copp::copp_error_code /*err_code*/) { return &g_test_coroutine_channel_status; }
};

template <class TCTX>
struct test_context_channel_this_coroutine;

template <>
struct test_context_channel_this_coroutine<test_context_channel_coroutine_context_test_type> {
  static test_context_channel_coroutine_context_test_type *get() {
    return copp::this_coroutine::get<test_context_channel_coroutine_context_test_type>();
  }
};

#if defined(LIBCOPP_MACRO_ENABLE_WIN_FIBER) && LIBCOPP_MACRO_ENABLE_WIN_FIBER
template <>
struct test_context_channel_this_coroutine<test_context_channel_coroutine_context_fiber_test_type> {
  static test_context_channel_coroutine_context_fiber_test_type *get() {
    return copp::this_fiber::get<test_context_channel_coroutine_context_fiber_test_type>();
  }
};
#endif

template <class TCTX, class T>
class test_context_channel_foo_runner {
 public:
  test_context_channel_foo_runner() : use_error_transform(false), call_times(0), last_value(0) {}

  bool use_error_transform;
  int call_times;
  copp::memory::strong_rc_ptr<copp::stackful_channel_sender<T>> sender;
  T last_value;

  copp::stackful_channel_receiver<T> create_channel() {
    auto ret = copp::make_stackful_channel<T>();
    sender = copp::memory::make_strong_rc<copp::stackful_channel_sender<T>>(ret.second);
    return std::move(ret.first);
  }

  int operator()(void *) {
    ++call_times;
    ++g_test_coroutine_channel_status;

    if (use_error_transform) {
      last_value = test_context_channel_this_coroutine<TCTX>::get()->await_value(
          create_channel(), test_context_channel_foo_error_transform<T>());
    } else {
      last_value = test_context_channel_this_coroutine<TCTX>::get()->await_value(create_channel());
    }

    ++g_test_coroutine_channel_status;
    return 0;
  }
};

CASE_TEST(coroutine_channel, channel_int) {
  unsigned char *stack_buff = new unsigned char[128 * 1024];

  test_context_channel_foo_runner<test_context_channel_coroutine_context_test_type, int> runner;
  // Normal
  g_test_coroutine_channel_status = 0;
  ++g_test_coroutine_channel_status;
  {
    copp::stack_context test_move_alloc;

    copp::allocator::stack_allocator_memory alloc_created(stack_buff, 128 * 1024);
    copp::allocator::stack_allocator_memory alloc(alloc_created);

    alloc_created.allocate(test_move_alloc, 64 * 1024);
    CASE_EXPECT_EQ(nullptr, test_move_alloc.sp);

    test_context_channel_coroutine_context_test_type::ptr_t co =
        test_context_channel_coroutine_context_test_type::create(&runner, alloc);
    runner.call_times = 0;

    CASE_EXPECT_TRUE(!!co);

    co->start();

    CASE_EXPECT_EQ(g_test_coroutine_channel_status, 2);

    runner.sender->set_value(137);

    CASE_EXPECT_EQ(g_test_coroutine_channel_status, 3);
    CASE_EXPECT_EQ(runner.last_value, 137);

    CASE_EXPECT_EQ(LIBCOPP_COPP_NAMESPACE_ID::COPP_EC_NOT_READY, co->resume());
    CASE_EXPECT_EQ(LIBCOPP_COPP_NAMESPACE_ID::COPP_EC_ALREADY_EXIST, co->yield());
  }

  // auto transform
  g_test_coroutine_channel_status = 0;
  ++g_test_coroutine_channel_status;
  {
    copp::stack_context test_move_alloc;

    copp::allocator::stack_allocator_memory alloc_created(stack_buff, 128 * 1024);
    copp::allocator::stack_allocator_memory alloc(alloc_created);

    alloc_created.allocate(test_move_alloc, 64 * 1024);
    CASE_EXPECT_EQ(nullptr, test_move_alloc.sp);

    test_context_channel_coroutine_context_test_type::ptr_t co =
        test_context_channel_coroutine_context_test_type::create(&runner, alloc);
    runner.call_times = 0;

    CASE_EXPECT_TRUE(!!co);

    co->start();

    CASE_EXPECT_EQ(g_test_coroutine_channel_status, 2);

    co->resume(nullptr);

    CASE_EXPECT_EQ(g_test_coroutine_channel_status, 3);
    CASE_EXPECT_EQ(runner.last_value, copp::COPP_EC_OPERATION_CANCLE);

    CASE_EXPECT_EQ(LIBCOPP_COPP_NAMESPACE_ID::COPP_EC_NOT_READY, co->resume());
    CASE_EXPECT_EQ(LIBCOPP_COPP_NAMESPACE_ID::COPP_EC_ALREADY_EXIST, co->yield());
  }

  // custom transform
  g_test_coroutine_channel_status = 0;
  ++g_test_coroutine_channel_status;
  {
    copp::stack_context test_move_alloc;

    copp::allocator::stack_allocator_memory alloc_created(stack_buff, 128 * 1024);
    copp::allocator::stack_allocator_memory alloc(alloc_created);

    alloc_created.allocate(test_move_alloc, 64 * 1024);
    CASE_EXPECT_EQ(nullptr, test_move_alloc.sp);

    test_context_channel_coroutine_context_test_type::ptr_t co =
        test_context_channel_coroutine_context_test_type::create(&runner, alloc);
    runner.call_times = 0;
    runner.use_error_transform = true;

    CASE_EXPECT_TRUE(!!co);

    co->start();

    CASE_EXPECT_EQ(g_test_coroutine_channel_status, 2);

    co->resume(nullptr);

    CASE_EXPECT_EQ(g_test_coroutine_channel_status, 3);
    CASE_EXPECT_EQ(runner.last_value, copp::COPP_EC_OPERATION_CANCLE - 10000);

    CASE_EXPECT_EQ(LIBCOPP_COPP_NAMESPACE_ID::COPP_EC_NOT_READY, co->resume());
    CASE_EXPECT_EQ(LIBCOPP_COPP_NAMESPACE_ID::COPP_EC_ALREADY_EXIST, co->yield());
  }

  delete[] stack_buff;
}

CASE_TEST(coroutine_channel, channel_pointer) {
  unsigned char *stack_buff = new unsigned char[128 * 1024];

  test_context_channel_foo_runner<test_context_channel_coroutine_context_test_type, int *> runner;
  // Normal
  g_test_coroutine_channel_status = 0;
  ++g_test_coroutine_channel_status;
  {
    copp::stack_context test_move_alloc;

    copp::allocator::stack_allocator_memory alloc_created(stack_buff, 128 * 1024);
    copp::allocator::stack_allocator_memory alloc(alloc_created);

    alloc_created.allocate(test_move_alloc, 64 * 1024);
    CASE_EXPECT_EQ(nullptr, test_move_alloc.sp);

    test_context_channel_coroutine_context_test_type::ptr_t co =
        test_context_channel_coroutine_context_test_type::create(&runner, alloc);
    runner.call_times = 0;

    CASE_EXPECT_TRUE(!!co);

    co->start();

    CASE_EXPECT_EQ(g_test_coroutine_channel_status, 2);

    runner.sender->set_value(reinterpret_cast<int *>(stack_buff));

    CASE_EXPECT_EQ(g_test_coroutine_channel_status, 3);
    CASE_EXPECT_EQ(runner.last_value, reinterpret_cast<int *>(stack_buff));

    CASE_EXPECT_EQ(LIBCOPP_COPP_NAMESPACE_ID::COPP_EC_NOT_READY, co->resume());
    CASE_EXPECT_EQ(LIBCOPP_COPP_NAMESPACE_ID::COPP_EC_ALREADY_EXIST, co->yield());
  }

  // auto transform
  g_test_coroutine_channel_status = 0;
  ++g_test_coroutine_channel_status;
  {
    copp::stack_context test_move_alloc;

    copp::allocator::stack_allocator_memory alloc_created(stack_buff, 128 * 1024);
    copp::allocator::stack_allocator_memory alloc(alloc_created);

    alloc_created.allocate(test_move_alloc, 64 * 1024);
    CASE_EXPECT_EQ(nullptr, test_move_alloc.sp);

    test_context_channel_coroutine_context_test_type::ptr_t co =
        test_context_channel_coroutine_context_test_type::create(&runner, alloc);
    runner.call_times = 0;

    CASE_EXPECT_TRUE(!!co);

    co->start();

    CASE_EXPECT_EQ(g_test_coroutine_channel_status, 2);

    co->resume(nullptr);

    CASE_EXPECT_EQ(g_test_coroutine_channel_status, 3);
    CASE_EXPECT_EQ(runner.last_value, nullptr);

    CASE_EXPECT_EQ(LIBCOPP_COPP_NAMESPACE_ID::COPP_EC_NOT_READY, co->resume());
    CASE_EXPECT_EQ(LIBCOPP_COPP_NAMESPACE_ID::COPP_EC_ALREADY_EXIST, co->yield());
  }

  // custom transform
  g_test_coroutine_channel_status = 0;
  ++g_test_coroutine_channel_status;
  {
    copp::stack_context test_move_alloc;

    copp::allocator::stack_allocator_memory alloc_created(stack_buff, 128 * 1024);
    copp::allocator::stack_allocator_memory alloc(alloc_created);

    alloc_created.allocate(test_move_alloc, 64 * 1024);
    CASE_EXPECT_EQ(nullptr, test_move_alloc.sp);

    test_context_channel_coroutine_context_test_type::ptr_t co =
        test_context_channel_coroutine_context_test_type::create(&runner, alloc);
    runner.call_times = 0;
    runner.use_error_transform = true;

    CASE_EXPECT_TRUE(!!co);

    co->start();

    CASE_EXPECT_EQ(g_test_coroutine_channel_status, 2);

    co->resume(nullptr);

    CASE_EXPECT_EQ(g_test_coroutine_channel_status, 3);
    CASE_EXPECT_EQ(runner.last_value, &g_test_coroutine_channel_status);

    CASE_EXPECT_EQ(LIBCOPP_COPP_NAMESPACE_ID::COPP_EC_NOT_READY, co->resume());
    CASE_EXPECT_EQ(LIBCOPP_COPP_NAMESPACE_ID::COPP_EC_ALREADY_EXIST, co->yield());
  }

  delete[] stack_buff;
}

#if defined(LIBCOPP_MACRO_ENABLE_WIN_FIBER) && LIBCOPP_MACRO_ENABLE_WIN_FIBER

CASE_TEST(coroutine_channel, fiber_channel_int) {
  unsigned char *stack_buff = new unsigned char[128 * 1024];

  test_context_channel_foo_runner<test_context_channel_coroutine_context_fiber_test_type, int> runner;
  // Normal
  g_test_coroutine_channel_status = 0;
  ++g_test_coroutine_channel_status;
  {
    copp::stack_context test_move_alloc;

    copp::allocator::stack_allocator_memory alloc_created(stack_buff, 128 * 1024);
    copp::allocator::stack_allocator_memory alloc(alloc_created);

    alloc_created.allocate(test_move_alloc, 64 * 1024);
    CASE_EXPECT_EQ(nullptr, test_move_alloc.sp);

    test_context_channel_coroutine_context_fiber_test_type::ptr_t co =
        test_context_channel_coroutine_context_fiber_test_type::create(&runner, alloc);
    runner.call_times = 0;

    CASE_EXPECT_TRUE(!!co);

    co->start();

    CASE_EXPECT_EQ(g_test_coroutine_channel_status, 2);

    runner.sender->set_value(157);

    CASE_EXPECT_EQ(g_test_coroutine_channel_status, 3);
    CASE_EXPECT_EQ(runner.last_value, 157);

    CASE_EXPECT_EQ(LIBCOPP_COPP_NAMESPACE_ID::COPP_EC_NOT_READY, co->resume());
    CASE_EXPECT_EQ(LIBCOPP_COPP_NAMESPACE_ID::COPP_EC_ALREADY_EXIST, co->yield());
  }

  // auto transform
  g_test_coroutine_channel_status = 0;
  ++g_test_coroutine_channel_status;
  {
    copp::stack_context test_move_alloc;

    copp::allocator::stack_allocator_memory alloc_created(stack_buff, 128 * 1024);
    copp::allocator::stack_allocator_memory alloc(alloc_created);

    alloc_created.allocate(test_move_alloc, 64 * 1024);
    CASE_EXPECT_EQ(nullptr, test_move_alloc.sp);

    test_context_channel_coroutine_context_fiber_test_type::ptr_t co =
        test_context_channel_coroutine_context_fiber_test_type::create(&runner, alloc);
    runner.call_times = 0;

    CASE_EXPECT_TRUE(!!co);

    co->start();

    CASE_EXPECT_EQ(g_test_coroutine_channel_status, 2);

    co->resume(nullptr);

    CASE_EXPECT_EQ(g_test_coroutine_channel_status, 3);
    CASE_EXPECT_EQ(runner.last_value, copp::COPP_EC_OPERATION_CANCLE);

    CASE_EXPECT_EQ(LIBCOPP_COPP_NAMESPACE_ID::COPP_EC_NOT_READY, co->resume());
    CASE_EXPECT_EQ(LIBCOPP_COPP_NAMESPACE_ID::COPP_EC_ALREADY_EXIST, co->yield());
  }

  // custom transform
  g_test_coroutine_channel_status = 0;
  ++g_test_coroutine_channel_status;
  {
    copp::stack_context test_move_alloc;

    copp::allocator::stack_allocator_memory alloc_created(stack_buff, 128 * 1024);
    copp::allocator::stack_allocator_memory alloc(alloc_created);

    alloc_created.allocate(test_move_alloc, 64 * 1024);
    CASE_EXPECT_EQ(nullptr, test_move_alloc.sp);

    test_context_channel_coroutine_context_fiber_test_type::ptr_t co =
        test_context_channel_coroutine_context_fiber_test_type::create(&runner, alloc);
    runner.call_times = 0;
    runner.use_error_transform = true;

    CASE_EXPECT_TRUE(!!co);

    co->start();

    CASE_EXPECT_EQ(g_test_coroutine_channel_status, 2);

    co->resume(nullptr);

    CASE_EXPECT_EQ(g_test_coroutine_channel_status, 3);
    CASE_EXPECT_EQ(runner.last_value, copp::COPP_EC_OPERATION_CANCLE - 10000);

    CASE_EXPECT_EQ(LIBCOPP_COPP_NAMESPACE_ID::COPP_EC_NOT_READY, co->resume());
    CASE_EXPECT_EQ(LIBCOPP_COPP_NAMESPACE_ID::COPP_EC_ALREADY_EXIST, co->yield());
  }

  delete[] stack_buff;
}

CASE_TEST(coroutine_channel, fiber_channel_pointer) {
  unsigned char *stack_buff = new unsigned char[128 * 1024];

  test_context_channel_foo_runner<test_context_channel_coroutine_context_fiber_test_type, int *> runner;
  // Normal
  g_test_coroutine_channel_status = 0;
  ++g_test_coroutine_channel_status;
  {
    copp::stack_context test_move_alloc;

    copp::allocator::stack_allocator_memory alloc_created(stack_buff, 128 * 1024);
    copp::allocator::stack_allocator_memory alloc(alloc_created);

    alloc_created.allocate(test_move_alloc, 64 * 1024);
    CASE_EXPECT_EQ(nullptr, test_move_alloc.sp);

    test_context_channel_coroutine_context_fiber_test_type::ptr_t co =
        test_context_channel_coroutine_context_fiber_test_type::create(&runner, alloc);
    runner.call_times = 0;

    CASE_EXPECT_TRUE(!!co);

    co->start();

    CASE_EXPECT_EQ(g_test_coroutine_channel_status, 2);

    runner.sender->set_value(reinterpret_cast<int *>(stack_buff));

    CASE_EXPECT_EQ(g_test_coroutine_channel_status, 3);
    CASE_EXPECT_EQ(runner.last_value, reinterpret_cast<int *>(stack_buff));

    CASE_EXPECT_EQ(LIBCOPP_COPP_NAMESPACE_ID::COPP_EC_NOT_READY, co->resume());
    CASE_EXPECT_EQ(LIBCOPP_COPP_NAMESPACE_ID::COPP_EC_ALREADY_EXIST, co->yield());
  }

  // auto transform
  g_test_coroutine_channel_status = 0;
  ++g_test_coroutine_channel_status;
  {
    copp::stack_context test_move_alloc;

    copp::allocator::stack_allocator_memory alloc_created(stack_buff, 128 * 1024);
    copp::allocator::stack_allocator_memory alloc(alloc_created);

    alloc_created.allocate(test_move_alloc, 64 * 1024);
    CASE_EXPECT_EQ(nullptr, test_move_alloc.sp);

    test_context_channel_coroutine_context_fiber_test_type::ptr_t co =
        test_context_channel_coroutine_context_fiber_test_type::create(&runner, alloc);
    runner.call_times = 0;

    CASE_EXPECT_TRUE(!!co);

    co->start();

    CASE_EXPECT_EQ(g_test_coroutine_channel_status, 2);

    co->resume(nullptr);

    CASE_EXPECT_EQ(g_test_coroutine_channel_status, 3);
    CASE_EXPECT_EQ(runner.last_value, nullptr);

    CASE_EXPECT_EQ(LIBCOPP_COPP_NAMESPACE_ID::COPP_EC_NOT_READY, co->resume());
    CASE_EXPECT_EQ(LIBCOPP_COPP_NAMESPACE_ID::COPP_EC_ALREADY_EXIST, co->yield());
  }

  // custom transform
  g_test_coroutine_channel_status = 0;
  ++g_test_coroutine_channel_status;
  {
    copp::stack_context test_move_alloc;

    copp::allocator::stack_allocator_memory alloc_created(stack_buff, 128 * 1024);
    copp::allocator::stack_allocator_memory alloc(alloc_created);

    alloc_created.allocate(test_move_alloc, 64 * 1024);
    CASE_EXPECT_EQ(nullptr, test_move_alloc.sp);

    test_context_channel_coroutine_context_fiber_test_type::ptr_t co =
        test_context_channel_coroutine_context_fiber_test_type::create(&runner, alloc);
    runner.call_times = 0;
    runner.use_error_transform = true;

    CASE_EXPECT_TRUE(!!co);

    co->start();

    CASE_EXPECT_EQ(g_test_coroutine_channel_status, 2);

    co->resume(nullptr);

    CASE_EXPECT_EQ(g_test_coroutine_channel_status, 3);
    CASE_EXPECT_EQ(runner.last_value, &g_test_coroutine_channel_status);

    CASE_EXPECT_EQ(LIBCOPP_COPP_NAMESPACE_ID::COPP_EC_NOT_READY, co->resume());
    CASE_EXPECT_EQ(LIBCOPP_COPP_NAMESPACE_ID::COPP_EC_ALREADY_EXIST, co->yield());
  }

  delete[] stack_buff;
}
#endif

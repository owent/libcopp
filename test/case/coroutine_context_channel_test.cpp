// Copyright 2026 owent

#include <libcopp/coroutine/coroutine_context_container.h>
#include <libcopp/coroutine/coroutine_context_fiber_container.h>
#include <libcopp/coroutine/stackful_channel.h>

#include <cstdio>
#include <cstring>
#include <iostream>
#include <vector>

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

namespace {
struct test_context_channel_fifo_context : public copp::stackful_channel_context_base {};

std::vector<int> g_test_coroutine_channel_fifo_order;

int test_context_channel_fifo_resume(void *handle_data, copp::stackful_channel_context_base *) {
  g_test_coroutine_channel_fifo_order.push_back(*reinterpret_cast<int *>(handle_data));
  return 0;
}
}  // namespace

CASE_TEST(coroutine_channel, multiple_callers_resume_fifo) {
  constexpr int k_caller_count = 8;
  int caller_indexes[k_caller_count];
  copp::stackful_channel_handle_delegate callers[k_caller_count];
  test_context_channel_fifo_context context;

  g_test_coroutine_channel_fifo_order.clear();
  for (int index = 0; index < k_caller_count; ++index) {
    caller_indexes[index] = index;
    callers[index].handle_data = &caller_indexes[index];
    callers[index].resume_handle = &test_context_channel_fifo_resume;
    context.add_caller(callers[index]);
  }
  // A repeated registration of the same handle is ignored.
  context.add_caller(callers[3]);

  CASE_EXPECT_TRUE(context.has_multiple_callers());
  CASE_EXPECT_EQ(k_caller_count, static_cast<int>(context.resume_callers()));
  CASE_EXPECT_EQ(k_caller_count, static_cast<int>(g_test_coroutine_channel_fifo_order.size()));
  for (int index = 0; index < k_caller_count; ++index) {
    CASE_EXPECT_EQ(index, g_test_coroutine_channel_fifo_order[static_cast<size_t>(index)]);
  }

  // A handle removed and registered again joins the back of the queue.
  g_test_coroutine_channel_fifo_order.clear();
  for (int index = 0; index < k_caller_count; ++index) {
    context.add_caller(callers[index]);
  }
  CASE_EXPECT_TRUE(context.remove_caller(callers[2]));
  context.add_caller(callers[2]);

  const int expected_order[k_caller_count] = {0, 1, 3, 4, 5, 6, 7, 2};
  CASE_EXPECT_EQ(k_caller_count, static_cast<int>(context.resume_callers()));
  CASE_EXPECT_EQ(k_caller_count, static_cast<int>(g_test_coroutine_channel_fifo_order.size()));
  if (k_caller_count == static_cast<int>(g_test_coroutine_channel_fifo_order.size())) {
    for (int index = 0; index < k_caller_count; ++index) {
      CASE_EXPECT_EQ(expected_order[index], g_test_coroutine_channel_fifo_order[static_cast<size_t>(index)]);
    }
  }
}

CASE_TEST(coroutine_channel, multiple_callers_resume_fifo_after_first_removed) {
  int caller_indexes[3] = {0, 1, 2};
  copp::stackful_channel_handle_delegate callers[3];
  test_context_channel_fifo_context context;
  for (int index = 0; index < 3; ++index) {
    callers[index].handle_data = &caller_indexes[index];
    callers[index].resume_handle = &test_context_channel_fifo_resume;
  }

  context.add_caller(callers[0]);
  context.add_caller(callers[1]);
  CASE_EXPECT_TRUE(context.remove_caller(callers[0]));
  CASE_EXPECT_FALSE(context.has_multiple_callers());

  // New and re-registered callers must stay behind the remaining waiter.
  context.add_caller(callers[2]);
  context.add_caller(callers[0]);
  context.add_caller(callers[1]);
  CASE_EXPECT_TRUE(context.has_multiple_callers());

  g_test_coroutine_channel_fifo_order.clear();
  CASE_EXPECT_EQ(3, static_cast<int>(context.resume_callers()));
  CASE_EXPECT_EQ(3, static_cast<int>(g_test_coroutine_channel_fifo_order.size()));
  if (g_test_coroutine_channel_fifo_order.size() == 3) {
    CASE_EXPECT_EQ(1, g_test_coroutine_channel_fifo_order[0]);
    CASE_EXPECT_EQ(2, g_test_coroutine_channel_fifo_order[1]);
    CASE_EXPECT_EQ(0, g_test_coroutine_channel_fifo_order[2]);
  }
  CASE_EXPECT_FALSE(context.has_multiple_callers());
  CASE_EXPECT_EQ(0, static_cast<int>(context.resume_callers()));
}

CASE_TEST(coroutine_channel, multiple_callers_add_remove_edge_cases) {
  int caller_indexes[3] = {0, 1, 2};
  copp::stackful_channel_handle_delegate first_caller;
  copp::stackful_channel_handle_delegate second_caller;
  copp::stackful_channel_handle_delegate unregistered_caller;
  first_caller.handle_data = &caller_indexes[0];
  first_caller.resume_handle = &test_context_channel_fifo_resume;
  second_caller.handle_data = &caller_indexes[1];
  second_caller.resume_handle = &test_context_channel_fifo_resume;
  unregistered_caller.handle_data = &caller_indexes[2];
  unregistered_caller.resume_handle = &test_context_channel_fifo_resume;

  test_context_channel_fifo_context context;

  // Re-adding the same handle while it's the only caller keeps a single caller.
  context.add_caller(first_caller);
  context.add_caller(first_caller);
  CASE_EXPECT_FALSE(context.has_multiple_callers());

  // Removing a handle that was never registered returns false while single caller.
  CASE_EXPECT_FALSE(context.remove_caller(second_caller));

  // Convert to multiple callers, then a repeated registration is still ignored.
  context.add_caller(second_caller);
  context.add_caller(first_caller);
  CASE_EXPECT_TRUE(context.has_multiple_callers());

  // Removing a handle not present in multi-caller mode returns false.
  CASE_EXPECT_FALSE(context.remove_caller(unregistered_caller));

  // Removing the registered handles returns true and empties the caller set.
  CASE_EXPECT_TRUE(context.remove_caller(first_caller));
  // Re-adding the remaining caller must not duplicate it into the vacant single-caller slot.
  context.add_caller(second_caller);
  CASE_EXPECT_FALSE(context.has_multiple_callers());
  CASE_EXPECT_TRUE(context.remove_caller(second_caller));
  CASE_EXPECT_FALSE(context.remove_caller(second_caller));

  g_test_coroutine_channel_fifo_order.clear();
  CASE_EXPECT_EQ(0, static_cast<int>(context.resume_callers()));
  CASE_EXPECT_TRUE(g_test_coroutine_channel_fifo_order.empty());

  context.add_caller(first_caller);
  CASE_EXPECT_FALSE(context.has_multiple_callers());
  CASE_EXPECT_EQ(1, static_cast<int>(context.resume_callers()));
  CASE_EXPECT_EQ(1, static_cast<int>(g_test_coroutine_channel_fifo_order.size()));
}

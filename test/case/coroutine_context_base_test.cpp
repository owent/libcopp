// Copyright 2023 owent

#include <libcopp/coroutine/coroutine_context_container.h>

#include <cstdio>
#include <cstring>
#include <iostream>

#include "frame/test_macros.h"

typedef copp::coroutine_context_container<copp::allocator::stack_allocator_memory>
    test_context_base_coroutine_context_test_type;

static int g_test_coroutine_base_status = 0;

class test_context_base_foo_runner {
 public:
  test_context_base_foo_runner() : call_times(0) {}
  int call_times;
  int operator()(void *) {
    ++call_times;
    ++g_test_coroutine_base_status;
    copp::this_coroutine::get<test_context_base_coroutine_context_test_type>()->yield();

    ++g_test_coroutine_base_status;
    return 0;
  }
};

CASE_TEST(coroutine, context_base) {
  unsigned char *stack_buff = new unsigned char[128 * 1024];
  g_test_coroutine_base_status = 0;
  ++g_test_coroutine_base_status;
  CASE_EXPECT_EQ(g_test_coroutine_base_status, 1);

  test_context_base_foo_runner runner;
  {
    copp::stack_context test_move_alloc;

    copp::allocator::stack_allocator_memory alloc_created(stack_buff, 128 * 1024);
    copp::allocator::stack_allocator_memory alloc(alloc_created);

    alloc_created.allocate(test_move_alloc, 64 * 1024);
    CASE_EXPECT_EQ(nullptr, test_move_alloc.sp);

    test_context_base_coroutine_context_test_type::ptr_t co =
        test_context_base_coroutine_context_test_type::create(&runner, alloc);
    runner.call_times = 0;

    CASE_EXPECT_TRUE(!!co);

    CASE_EXPECT_EQ(LIBCOPP_COPP_NAMESPACE_ID::COPP_EC_NOT_RUNNING, co->yield());

    co->start();

    ++g_test_coroutine_base_status;
    CASE_EXPECT_EQ(g_test_coroutine_base_status, 3);
    co->resume();

    ++g_test_coroutine_base_status;
    CASE_EXPECT_EQ(g_test_coroutine_base_status, 5);

    CASE_EXPECT_EQ(LIBCOPP_COPP_NAMESPACE_ID::COPP_EC_NOT_READY, co->resume());
    CASE_EXPECT_EQ(LIBCOPP_COPP_NAMESPACE_ID::COPP_EC_ALREADY_EXIST, co->yield());
  }

  {
    g_test_coroutine_base_status = 1;
    copp::stack_context test_move_alloc;

    copp::allocator::stack_allocator_memory alloc_created;
    alloc_created.attach(stack_buff, 128 * 1024);
    copp::allocator::stack_allocator_memory alloc;
    alloc = alloc_created;

    alloc_created.allocate(test_move_alloc, 64 * 1024);
    CASE_EXPECT_EQ(nullptr, test_move_alloc.sp);

    test_context_base_coroutine_context_test_type::ptr_t co =
        test_context_base_coroutine_context_test_type::create(&runner, alloc);
    runner.call_times = 0;

    CASE_EXPECT_TRUE(!!co);

    CASE_EXPECT_EQ(LIBCOPP_COPP_NAMESPACE_ID::COPP_EC_NOT_RUNNING, co->yield());

    co->start();

    ++g_test_coroutine_base_status;
    CASE_EXPECT_EQ(g_test_coroutine_base_status, 3);
    co->resume();

    ++g_test_coroutine_base_status;
    CASE_EXPECT_EQ(g_test_coroutine_base_status, 5);

    CASE_EXPECT_EQ(LIBCOPP_COPP_NAMESPACE_ID::COPP_EC_NOT_READY, co->resume());
    CASE_EXPECT_EQ(LIBCOPP_COPP_NAMESPACE_ID::COPP_EC_ALREADY_EXIST, co->yield());
  }

  delete[] stack_buff;
}

CASE_TEST(coroutine, shared_runner) {
  const int stack_len = 128 * 1024;
  unsigned char *stack_buff = new unsigned char[4 * stack_len];
  g_test_coroutine_base_status = 0;
  ++g_test_coroutine_base_status;
  CASE_EXPECT_EQ(g_test_coroutine_base_status, 1);

  test_context_base_foo_runner runner;
  runner.call_times = 0;

  {
    test_context_base_coroutine_context_test_type::ptr_t co[4];
    for (int i = 0; i < 4; ++i) {
      copp::allocator::stack_allocator_memory alloc(stack_buff + i * stack_len, stack_len);
      co[i] = test_context_base_coroutine_context_test_type::create(&runner, alloc);
      co[i]->start();
    }

    CASE_EXPECT_EQ(g_test_coroutine_base_status, 5);

    for (int i = 0; i < 4; ++i) {
      co[i]->resume();
    }

    CASE_EXPECT_EQ(g_test_coroutine_base_status, 9);

    CASE_EXPECT_EQ(runner.call_times, 4);
  }

  delete[] stack_buff;
}

CASE_TEST(coroutine, coroutine_context_container_create_failed) {
  unsigned char *stack_buff = new unsigned char[128 * 1024];

  test_context_base_foo_runner runner;
  {
    copp::stack_context test_move_alloc;

    copp::allocator::stack_allocator_memory alloc_created(stack_buff, 128 * 1024);
    copp::allocator::stack_allocator_memory alloc(alloc_created);

    alloc_created.allocate(test_move_alloc, 64 * 1024);
    CASE_EXPECT_EQ(nullptr, test_move_alloc.sp);

    test_context_base_coroutine_context_test_type::ptr_t co =
        test_context_base_coroutine_context_test_type::create(&runner, alloc, 32 * 1024, 16 * 1024, 16 * 1024);
    CASE_EXPECT_TRUE(!co);
  }

  delete[] stack_buff;
}

CASE_TEST(coroutine, coroutine_context_create_failed) {
  unsigned char *stack_buff = new unsigned char[128 * 1024];

  copp::coroutine_context *placement_new_addr = reinterpret_cast<copp::coroutine_context *>(stack_buff + 112 * 1024);
  test_context_base_foo_runner runner;
  {
    copp::stack_context callee_stack;
    callee_stack.sp = reinterpret_cast<void *>(stack_buff + 120 * 1024);
    callee_stack.size = 120 * 1024;

    CASE_EXPECT_EQ(copp::COPP_EC_ARGS_ERROR,
                   copp::coroutine_context::create(nullptr, std::move(runner), callee_stack, 4096, 4096));
  }

  {
    copp::stack_context callee_stack;
    callee_stack.sp = reinterpret_cast<void *>(stack_buff + 120 * 1024);
    callee_stack.size = 120 * 1024;

    CASE_EXPECT_EQ(copp::COPP_EC_ARGS_ERROR,
                   copp::coroutine_context::create(placement_new_addr, std::move(runner), callee_stack, 4096, 4095));
  }

  {
    copp::stack_context callee_stack;
    callee_stack.sp = reinterpret_cast<void *>(stack_buff + 120 * 1024);
    callee_stack.size = 120 * 1024;

    CASE_EXPECT_EQ(copp::COPP_EC_ARGS_ERROR,
                   copp::coroutine_context::create(placement_new_addr, std::move(runner), callee_stack, 4095, 4096));
  }

  {
    copp::stack_context callee_stack;
    callee_stack.sp = nullptr;
    callee_stack.size = 120 * 1024;

    CASE_EXPECT_EQ(copp::COPP_EC_ARGS_ERROR,
                   copp::coroutine_context::create(placement_new_addr, std::move(runner), callee_stack, 4096, 4096));
  }

  {
    copp::stack_context callee_stack;
    callee_stack.sp = reinterpret_cast<void *>(stack_buff + 120 * 1024);
    callee_stack.size = 8192;

    CASE_EXPECT_EQ(copp::COPP_EC_ARGS_ERROR,
                   copp::coroutine_context::create(placement_new_addr, std::move(runner), callee_stack, 4096, 4096));
  }

  {
    copp::stack_context callee_stack;
    callee_stack.sp = reinterpret_cast<void *>(stack_buff + 120 * 1024);
    callee_stack.size = 120 * 1024;

    copp::coroutine_context *placement_invalid_addr =
        reinterpret_cast<copp::coroutine_context *>(stack_buff + 116 * 1024);
    CASE_EXPECT_EQ(copp::COPP_EC_ARGS_ERROR, copp::coroutine_context::create(placement_invalid_addr, std::move(runner),
                                                                             callee_stack, 4096, 4096));
  }

  delete[] stack_buff;
}

#if defined(LIBCOPP_MACRO_ENABLE_STD_EXCEPTION_PTR) && LIBCOPP_MACRO_ENABLE_STD_EXCEPTION_PTR
static int test_context_base_foo_runner_throw_exception(void *) { return static_cast<int>(std::string().at(1)); }

CASE_TEST(coroutine, coroutine_context_throw_exception) {
  unsigned char *stack_buff = new unsigned char[128 * 1024];

  try {
    copp::stack_context test_move_alloc;

    copp::allocator::stack_allocator_memory alloc_created(stack_buff, 128 * 1024);
    copp::allocator::stack_allocator_memory alloc(alloc_created);

    alloc_created.allocate(test_move_alloc, 64 * 1024);
    CASE_EXPECT_EQ(nullptr, test_move_alloc.sp);

    test_context_base_coroutine_context_test_type::ptr_t co =
        test_context_base_coroutine_context_test_type::create(test_context_base_foo_runner_throw_exception, alloc);

    CASE_EXPECT_TRUE(!!co);

    CASE_EXPECT_EQ(LIBCOPP_COPP_NAMESPACE_ID::COPP_EC_NOT_RUNNING, co->yield());

    co->start();
  } catch (const std::exception &e) {
    CASE_MSG_INFO() << "Caught exception \"" << e.what() << "\"" << std::endl;
  }

  delete[] stack_buff;
}

#endif
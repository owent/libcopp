// Copyright 2023 owent

#include <libcopp/stack/stack_pool.h>
#include <libcotask/task.h>

#include <cstdio>
#include <cstring>
#include <iostream>
#include <vector>

#include "frame/test_macros.h"

typedef copp::stack_pool<copp::allocator::stack_allocator_malloc> stack_pool_t;
struct stack_pool_test_macro_coroutine {
  using stack_allocator_type = copp::allocator::stack_allocator_pool<stack_pool_t>;
  using coroutine_type = copp::coroutine_context_container<stack_allocator_type>;
  using value_type = int;
};
static stack_pool_t::ptr_t global_stack_pool;

typedef cotask::task<stack_pool_test_macro_coroutine> stack_pool_test_task_t;

static int stack_pool_test_task_action(void *) { return 0; }

CASE_TEST(stack_pool_test, stack_context) {
  copp::stack_context ctx;
  CASE_EXPECT_EQ(nullptr, ctx.sp);
  CASE_EXPECT_EQ(0, ctx.size);

  unsigned char test_buffer[256];
  ctx.sp = reinterpret_cast<void *>(test_buffer + 256);
  ctx.size = sizeof(test_buffer);
  ctx.reset();

  CASE_EXPECT_EQ(nullptr, ctx.sp);
  CASE_EXPECT_EQ(0, ctx.size);
}

CASE_TEST(stack_pool_test, basic) {
  global_stack_pool = stack_pool_t::create();
  std::vector<stack_pool_test_task_t::ptr_t> task_arr;
  const size_t task_arr_sz = 64;

  // alloc
  for (size_t i = 0; i < task_arr_sz; ++i) {
    copp::allocator::stack_allocator_pool<stack_pool_t> alloc(global_stack_pool);
    stack_pool_test_task_t::ptr_t tp = stack_pool_test_task_t::create(stack_pool_test_task_action, alloc);
    task_arr.push_back(tp);
  }
  CASE_EXPECT_EQ(task_arr_sz, global_stack_pool->get_limit().used_stack_number);
  CASE_EXPECT_EQ(task_arr_sz * (global_stack_pool->get_stack_size() + global_stack_pool->get_stack_size_offset()),
                 global_stack_pool->get_limit().used_stack_size);

  CASE_EXPECT_EQ(0, global_stack_pool->get_limit().free_stack_number);
  CASE_EXPECT_EQ(0, global_stack_pool->get_limit().free_stack_size);

  // recycle to free list
  for (size_t i = 0; i < task_arr_sz / 2; ++i) {
    task_arr.pop_back();
  }
  CASE_EXPECT_EQ(task_arr_sz - task_arr_sz / 2, global_stack_pool->get_limit().used_stack_number);
  CASE_EXPECT_EQ(task_arr_sz / 2, global_stack_pool->get_limit().free_stack_number);
  CASE_EXPECT_EQ((task_arr_sz - task_arr_sz / 2) *
                     (global_stack_pool->get_stack_size() + global_stack_pool->get_stack_size_offset()),
                 global_stack_pool->get_limit().used_stack_size);
  CASE_EXPECT_EQ((task_arr_sz / 2) * (global_stack_pool->get_stack_size() + global_stack_pool->get_stack_size_offset()),
                 global_stack_pool->get_limit().free_stack_size);

  // auto_gc
  task_arr.pop_back();
  CASE_EXPECT_EQ(task_arr_sz / 4, global_stack_pool->get_limit().free_stack_number);
  CASE_EXPECT_EQ((task_arr_sz / 4) * (global_stack_pool->get_stack_size() + global_stack_pool->get_stack_size_offset()),
                 global_stack_pool->get_limit().free_stack_size);

  global_stack_pool.reset();
}

CASE_TEST(stack_pool_test, full_number) {
  global_stack_pool = stack_pool_t::create();
  // full

  std::vector<stack_pool_test_task_t::ptr_t> task_arr;
  const size_t task_arr_sz = 64;
  global_stack_pool->set_max_stack_number(task_arr_sz);
  // alloc
  for (size_t i = 0; i < task_arr_sz; ++i) {
    copp::allocator::stack_allocator_pool<stack_pool_t> alloc(global_stack_pool);
    stack_pool_test_task_t::ptr_t tp = stack_pool_test_task_t::create(stack_pool_test_task_action, alloc);
    CASE_EXPECT_TRUE(!!tp);
    task_arr.push_back(tp);
  }

  {
    copp::allocator::stack_allocator_pool<stack_pool_t> alloc(global_stack_pool);
    stack_pool_test_task_t::ptr_t tp = stack_pool_test_task_t::create(stack_pool_test_task_action, alloc);
    CASE_EXPECT_TRUE(!tp);
  }

  global_stack_pool.reset();
}

CASE_TEST(stack_pool_test, custom_gc) {
  global_stack_pool = stack_pool_t::create();
  std::vector<stack_pool_test_task_t::ptr_t> task_arr;
  const size_t task_arr_sz = 64;

  global_stack_pool->set_min_stack_number(task_arr_sz);

  // alloc
  for (size_t i = 0; i < task_arr_sz; ++i) {
    copp::allocator::stack_allocator_pool<stack_pool_t> alloc(global_stack_pool);
    stack_pool_test_task_t::ptr_t tp = stack_pool_test_task_t::create(stack_pool_test_task_action, alloc);
    task_arr.push_back(tp);
  }

  CASE_EXPECT_EQ(task_arr_sz, global_stack_pool->get_limit().used_stack_number);
  CASE_EXPECT_EQ(task_arr_sz * (global_stack_pool->get_stack_size() + global_stack_pool->get_stack_size_offset()),
                 global_stack_pool->get_limit().used_stack_size);

  CASE_EXPECT_EQ(0, global_stack_pool->get_limit().free_stack_number);
  CASE_EXPECT_EQ(0, global_stack_pool->get_limit().free_stack_size);

  for (size_t i = 0; i < task_arr_sz; ++i) {
    task_arr.pop_back();
  }

  CASE_EXPECT_EQ(0, global_stack_pool->get_limit().used_stack_number);
  CASE_EXPECT_EQ(0, global_stack_pool->get_limit().used_stack_size);

  CASE_EXPECT_EQ(task_arr_sz, global_stack_pool->get_limit().free_stack_number);
  CASE_EXPECT_EQ(task_arr_sz * (global_stack_pool->get_stack_size() + global_stack_pool->get_stack_size_offset()),
                 global_stack_pool->get_limit().free_stack_size);

  global_stack_pool->set_min_stack_number(0);
  global_stack_pool.reset();
}

CASE_TEST(stack_pool_test, gc_once) {
  global_stack_pool = stack_pool_t::create();
  // gc_once
  std::vector<stack_pool_test_task_t::ptr_t> task_arr;
  const size_t task_arr_sz = 64;

  global_stack_pool->set_auto_gc(false);
  global_stack_pool->set_gc_once_number(10);

  // alloc
  for (size_t i = 0; i < task_arr_sz; ++i) {
    copp::allocator::stack_allocator_pool<stack_pool_t> alloc(global_stack_pool);
    stack_pool_test_task_t::ptr_t tp = stack_pool_test_task_t::create(stack_pool_test_task_action, alloc);
    task_arr.push_back(tp);
  }

  CASE_EXPECT_EQ(task_arr_sz, global_stack_pool->get_limit().used_stack_number);
  CASE_EXPECT_EQ(task_arr_sz * (global_stack_pool->get_stack_size() + global_stack_pool->get_stack_size_offset()),
                 global_stack_pool->get_limit().used_stack_size);

  CASE_EXPECT_EQ(0, global_stack_pool->get_limit().free_stack_number);
  CASE_EXPECT_EQ(0, global_stack_pool->get_limit().free_stack_size);

  for (size_t i = 0; i < task_arr_sz; ++i) {
    task_arr.pop_back();
  }

  CASE_EXPECT_EQ(0, global_stack_pool->get_limit().used_stack_number);
  CASE_EXPECT_EQ(0, global_stack_pool->get_limit().used_stack_size);

  CASE_EXPECT_EQ(task_arr_sz, global_stack_pool->get_limit().free_stack_number);
  CASE_EXPECT_EQ(task_arr_sz * (global_stack_pool->get_stack_size() + global_stack_pool->get_stack_size_offset()),
                 global_stack_pool->get_limit().free_stack_size);

  global_stack_pool->gc();

  CASE_EXPECT_EQ(task_arr_sz - 10, global_stack_pool->get_limit().free_stack_number);
  CASE_EXPECT_EQ(
      (task_arr_sz - 10) * (global_stack_pool->get_stack_size() + global_stack_pool->get_stack_size_offset()),
      global_stack_pool->get_limit().free_stack_size);

  global_stack_pool.reset();
}

CASE_TEST(stack_pool_test, full_size) {
  global_stack_pool = stack_pool_t::create();
  // full size

  std::vector<stack_pool_test_task_t::ptr_t> task_arr;
  stack_pool_test_task_t::ptr_t tp1;
  stack_pool_test_task_t::ptr_t tp2;

  global_stack_pool->set_max_stack_size(128 * 1024);
  global_stack_pool->set_stack_size(100 * 1024);
  // alloc
  copp::allocator::stack_allocator_pool<stack_pool_t> alloc1(global_stack_pool);
  copp::allocator::stack_allocator_pool<stack_pool_t> alloc2(global_stack_pool);
  tp1 = stack_pool_test_task_t::create(stack_pool_test_task_action, alloc1);
  CASE_EXPECT_TRUE(!!tp1);

  tp2 = stack_pool_test_task_t::create(stack_pool_test_task_action, alloc2);
  CASE_EXPECT_TRUE(!tp2);

  global_stack_pool.reset();
}
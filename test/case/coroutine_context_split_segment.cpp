// Copyright 2023 owent

#include <libcopp/coroutine/coroutine_context_container.h>
#include <libcopp/stack/allocator/stack_allocator_split_segment.h>

#include <cstdio>
#include <cstring>
#include <iostream>

#include "frame/test_macros.h"
#ifdef LIBCOPP_MACRO_USE_SEGMENTED_STACKS

typedef copp::coroutine_context_container<copp::allocator::stack_allocator_split_segment> test_split_segment_context;

class test_split_segment_foo_runner {
 private:
  void stack_test(int loop) {
    char a = 100;
    char b[1024 * 1024] = {0};
    char c = 200;
    CASE_EXPECT_TRUE((&a > &c) == (&a > b));
    end_addr_ = (intptr_t)(&c);
    if (loop > 0) stack_test(loop - 1);
  }

 public:
  int operator()(void *) {
    int a = 0;
    begin_addr_ = (intptr_t)(&a);
    end_addr_ = begin_addr_;

    stack_test(4);

    copp::this_coroutine::get<test_split_segment_context>()->yield();

    stack_test(20);
    return 0;
  }

  intptr_t begin_addr_;
  intptr_t end_addr_;
};

CASE_TEST(coroutine, context_split_segment_stack) {
  test_split_segment_foo_runner runner;
  {
    test_split_segment_context::ptr_t co = test_split_segment_context::create(&runner, 2 * 1024 * 1024);

    co->start();
    co->resume();

    intptr_t dis = (runner.end_addr_ > runner.begin_addr_) ? (runner.end_addr_ - runner.begin_addr_)
                                                           : (runner.begin_addr_ - runner.end_addr_);

    CASE_EXPECT_GE(dis, 4 * 1024 * 1024);
  }
}

#endif

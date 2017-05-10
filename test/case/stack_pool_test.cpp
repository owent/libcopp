#include <cstdio>
#include <cstring>
#include <iostream>

#include "frame/test_macros.h"
#include "libcopp/fcontext/all.hpp"
#include "libcopp/stack/stack_allocator.h"
#include "libcopp/stack/stack_pool.h"

typedef copp::stack_pool<copp::allocator::default_statck_allocator> stack_pool_t;
CASE_TEST(stack_pool_test, basic) {
    stack_pool_t::ptr_t stack_pool = stack_pool_t::create();
    // alloc

    // recycle to free list

    // auto_gc
}

CASE_TEST(stack_pool_test, full) {
    stack_pool_t::ptr_t stack_pool = stack_pool_t::create();
    // full
}

CASE_TEST(stack_pool_test, no_gc) {
    stack_pool_t::ptr_t stack_pool = stack_pool_t::create();
    // no_gc
}

CASE_TEST(stack_pool_test, gc_once) {
    stack_pool_t::ptr_t stack_pool = stack_pool_t::create();
    // gc_once
}
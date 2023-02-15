// Copyright 2023 owent

#include <libcopp/coroutine/coroutine_context_container.h>

#include <cstdio>
#include <cstring>
#include <iostream>

#include "frame/test_macros.h"

typedef copp::coroutine_context_container<copp::allocator::default_statck_allocator>
    test_context_private_data_context_type;

static int g_test_coroutine_private_data_status = 0;

class test_context_private_data_foo_runner {
 public:
  int operator()(void *priv_data) {
    ++g_test_coroutine_private_data_status;
    CASE_EXPECT_EQ(g_test_coroutine_private_data_status, 2);
    CASE_EXPECT_EQ(&g_test_coroutine_private_data_status, priv_data);

    copp::this_coroutine::get<test_context_private_data_context_type>()->resume();
    ++g_test_coroutine_private_data_status;
    CASE_EXPECT_EQ(g_test_coroutine_private_data_status, 3);

    copp::this_coroutine::get<test_context_private_data_context_type>()->yield(&priv_data);
    ++g_test_coroutine_private_data_status;

    CASE_EXPECT_EQ(copp::this_coroutine::get<test_context_private_data_context_type>(), priv_data);
    CASE_EXPECT_EQ(g_test_coroutine_private_data_status, 5);

    return 0;
  }
};

CASE_TEST(coroutine, context_private_data) {
  g_test_coroutine_private_data_status = 0;

  ++g_test_coroutine_private_data_status;
  CASE_EXPECT_EQ(g_test_coroutine_private_data_status, 1);

  test_context_private_data_context_type::ptr_t co =
      test_context_private_data_context_type::create(test_context_private_data_foo_runner());
  co->start(&g_test_coroutine_private_data_status);

  ++g_test_coroutine_private_data_status;
  CASE_EXPECT_EQ(g_test_coroutine_private_data_status, 4);
  co->resume(co.get());

  ++g_test_coroutine_private_data_status;
  CASE_EXPECT_EQ(g_test_coroutine_private_data_status, 6);
}

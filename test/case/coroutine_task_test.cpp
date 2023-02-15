// Copyright 2023 owent

#include <libcopp/coroutine/callable_promise.h>
#include <libcopp/stack/stack_pool.h>
#include <libcotask/task.h>

#include <cstdio>
#include <cstring>
#include <iostream>
#include <memory>
#include <set>
#include <vector>

#include "frame/test_macros.h"

#ifdef LIBCOTASK_MACRO_ENABLED
static int g_test_coroutine_task_status = 0;
static int g_test_coroutine_task_on_finished = 0;
class test_context_task_action_base : public cotask::impl::task_action_impl {
 public:
  // add a same name function to find the type detection error
  virtual int operator()() = 0;

  int operator()(void *priv_data) {
    ++g_test_coroutine_task_status;

    CASE_EXPECT_EQ(&g_test_coroutine_task_status, priv_data);

    CASE_EXPECT_EQ(cotask::EN_TS_RUNNING, cotask::this_task::get_task()->get_status());
    cotask::this_task::get_task()->yield(&priv_data);
    CASE_EXPECT_EQ(cotask::EN_TS_RUNNING, cotask::this_task::get_task()->get_status());

    CASE_EXPECT_EQ(cotask::this_task::get_task(), priv_data);

    ++g_test_coroutine_task_status;

    return 0;
  }

  virtual int on_finished(cotask::impl::task_impl &) {
    ++g_test_coroutine_task_on_finished;
    return 0;
  }
};

class test_context_task_action : public test_context_task_action_base {
 public:
  using test_context_task_action_base::operator();

  // add a same name function to find the type detection error
  virtual int operator()() { return 0; }
};

CASE_TEST(coroutine_task, custom_action) {
  typedef cotask::task<>::ptr_t task_ptr_type;
  g_test_coroutine_task_status = 0;
  g_test_coroutine_task_on_finished = 0;

  {
    task_ptr_type co_task = cotask::task<>::create(test_context_task_action());
    task_ptr_type co_another_task = cotask::task<>::create(test_context_task_action());  // share action

    CASE_EXPECT_EQ(cotask::EN_TS_CREATED, co_task->get_status());
    CASE_EXPECT_EQ(cotask::EN_TS_CREATED, co_another_task->get_status());

    CASE_EXPECT_EQ(0, co_task->start(&g_test_coroutine_task_status));

    CASE_EXPECT_EQ(g_test_coroutine_task_status, 1);
    CASE_EXPECT_FALSE(co_task->is_completed());
    CASE_EXPECT_FALSE(co_task->is_canceled());
    CASE_EXPECT_FALSE(co_task->is_faulted());

    CASE_EXPECT_EQ(0, co_another_task->start(&g_test_coroutine_task_status));
    CASE_EXPECT_EQ(g_test_coroutine_task_status, 2);

    CASE_EXPECT_EQ(cotask::EN_TS_WAITING, co_task->get_status());
    CASE_EXPECT_EQ(cotask::EN_TS_WAITING, co_another_task->get_status());

    CASE_EXPECT_EQ(0, co_task->resume(co_task.get()));
    CASE_EXPECT_EQ(g_test_coroutine_task_status, 3);

    CASE_EXPECT_EQ(0, co_another_task->resume(co_another_task.get()));
    CASE_EXPECT_EQ(g_test_coroutine_task_status, 4);

    CASE_EXPECT_TRUE(co_task->is_completed());
    CASE_EXPECT_TRUE(co_another_task->is_completed());
    CASE_EXPECT_FALSE(co_task->is_canceled());
    CASE_EXPECT_FALSE(co_task->is_faulted());

    CASE_EXPECT_GT(0, co_another_task->resume(co_another_task.get()));
    CASE_EXPECT_EQ(g_test_coroutine_task_status, 4);

    ++g_test_coroutine_task_status;
    CASE_EXPECT_EQ(g_test_coroutine_task_status, 5);

    CASE_EXPECT_NE(co_task->get_id(), 0);
  }

  CASE_EXPECT_EQ(g_test_coroutine_task_on_finished, 2);
}

struct test_context_task_functor {
 public:
  int operator()(void *) const {
    ++g_test_coroutine_task_status;
    CASE_EXPECT_EQ(g_test_coroutine_task_status, 1);

    cotask::this_task::get_task()->yield();

    ++g_test_coroutine_task_status;
    CASE_EXPECT_EQ(g_test_coroutine_task_status, 3);

    return 0;
  }
};

CASE_TEST(coroutine_task, functor_action) {
  typedef cotask::task<>::ptr_t task_ptr_type;
  task_ptr_type co_task = cotask::task<>::create(test_context_task_functor());
  g_test_coroutine_task_status = 0;

  CASE_EXPECT_EQ(0, co_task->start());

  ++g_test_coroutine_task_status;
  CASE_EXPECT_EQ(g_test_coroutine_task_status, 2);

  CASE_EXPECT_FALSE(co_task->is_completed());
  CASE_EXPECT_EQ(0, co_task->resume());

  CASE_EXPECT_TRUE(co_task->is_completed());

  ++g_test_coroutine_task_status;
  CASE_EXPECT_EQ(g_test_coroutine_task_status, 4);

  CASE_EXPECT_NE(co_task->get_id(), 0);
}

static int test_context_task_function_1(void *) {
  ++g_test_coroutine_task_status;
  CASE_EXPECT_EQ(g_test_coroutine_task_status, 1);

  cotask::this_task::get_task()->yield();

  ++g_test_coroutine_task_status;
  CASE_EXPECT_EQ(g_test_coroutine_task_status, 3);

  return 100;
}

static void test_context_task_function_2(void *) {
  ++g_test_coroutine_task_status;
  CASE_EXPECT_EQ(g_test_coroutine_task_status, 1);

  cotask::this_task::get_task()->yield();

  ++g_test_coroutine_task_status;
  CASE_EXPECT_EQ(g_test_coroutine_task_status, 3);
}

CASE_TEST(coroutine_task, function_action) {
  {
    typedef cotask::task<>::ptr_t task_ptr_type;
    task_ptr_type co_task = cotask::task<>::create(test_context_task_function_1);
    g_test_coroutine_task_status = 0;

    CASE_EXPECT_EQ(0, co_task->start());

    ++g_test_coroutine_task_status;
    CASE_EXPECT_EQ(g_test_coroutine_task_status, 2);

    CASE_EXPECT_FALSE(co_task->is_completed());
    CASE_EXPECT_EQ(0, co_task->resume());

    CASE_EXPECT_TRUE(co_task->is_completed());

    ++g_test_coroutine_task_status;
    CASE_EXPECT_EQ(g_test_coroutine_task_status, 4);
    CASE_EXPECT_EQ(co_task->get_coroutine_context()->get_ret_code(), 100);
  }

  {
    typedef cotask::task<>::ptr_t task_ptr_type;
    task_ptr_type co_task = cotask::task<>::create(test_context_task_function_2);
    g_test_coroutine_task_status = 0;

    CASE_EXPECT_EQ(0, co_task->start());

    ++g_test_coroutine_task_status;
    CASE_EXPECT_EQ(g_test_coroutine_task_status, 2);

    CASE_EXPECT_FALSE(co_task->is_completed());
    CASE_EXPECT_EQ(0, co_task->resume());

    CASE_EXPECT_TRUE(co_task->is_completed());

    ++g_test_coroutine_task_status;
    CASE_EXPECT_EQ(g_test_coroutine_task_status, 4);
  }
}

CASE_TEST(coroutine_task, cancel) {
  typedef cotask::task<>::ptr_t task_ptr_type;
  task_ptr_type co_task = cotask::task<>::create(test_context_task_function_1);
  g_test_coroutine_task_status = 0;

  CASE_EXPECT_EQ(0, co_task->start());

  ++g_test_coroutine_task_status;
  CASE_EXPECT_EQ(g_test_coroutine_task_status, 2);

  CASE_EXPECT_FALSE(co_task->is_completed());
  CASE_EXPECT_EQ(0, co_task->cancel(nullptr));

  CASE_EXPECT_TRUE(co_task->is_completed());
}

// task start and coroutine context yield
static void test_context_task_function_3(void *) {
  ++g_test_coroutine_task_status;
  CASE_EXPECT_EQ(g_test_coroutine_task_status, 1);

  CASE_EXPECT_EQ(cotask::EN_TS_RUNNING, cotask::this_task::get_task()->get_status());

  copp::this_coroutine::yield();

  CASE_EXPECT_EQ(cotask::EN_TS_RUNNING, cotask::this_task::get<cotask::task<> >()->get_status());

  ++g_test_coroutine_task_status;
  CASE_EXPECT_EQ(g_test_coroutine_task_status, 3);
}

CASE_TEST(coroutine_task, coroutine_context_yield) {
  typedef cotask::task<>::ptr_t task_ptr_type;
  task_ptr_type co_task = cotask::task<>::create(test_context_task_function_3);
  g_test_coroutine_task_status = 0;

  CASE_EXPECT_EQ(0, co_task->start());

  ++g_test_coroutine_task_status;
  CASE_EXPECT_EQ(g_test_coroutine_task_status, 2);

  CASE_EXPECT_FALSE(co_task->is_completed());
  CASE_EXPECT_EQ(cotask::EN_TS_WAITING, co_task->get_status());
  CASE_EXPECT_EQ(0, co_task->resume());

  CASE_EXPECT_TRUE(co_task->is_completed());

  ++g_test_coroutine_task_status;
  CASE_EXPECT_EQ(g_test_coroutine_task_status, 4);

  CASE_EXPECT_NE(co_task->get_id(), 0);
}

struct test_context_task_mem_function {
  cotask::task<>::id_t task_id_;

  int real_run(void *) {
    ++g_test_coroutine_task_status;
    CASE_EXPECT_EQ(g_test_coroutine_task_status, 1);

    CASE_EXPECT_EQ(task_id_, cotask::task<>::this_task()->get_id());
    cotask::task<>::this_task()->yield();
    CASE_EXPECT_EQ(task_id_, cotask::task<>::this_task()->get_id());

    ++g_test_coroutine_task_status;
    CASE_EXPECT_EQ(g_test_coroutine_task_status, 3);

    return -1;
  }
};

CASE_TEST(coroutine_task, mem_function_action) {
  typedef cotask::task<>::ptr_t task_ptr_type;
  test_context_task_mem_function obj;
  task_ptr_type co_task = cotask::task<>::create(&test_context_task_mem_function::real_run, &obj);
  g_test_coroutine_task_status = 0;
  obj.task_id_ = co_task->get_id();

  CASE_EXPECT_EQ(0, co_task->start());

  ++g_test_coroutine_task_status;
  CASE_EXPECT_EQ(g_test_coroutine_task_status, 2);

  CASE_EXPECT_FALSE(co_task->is_completed());
  CASE_EXPECT_EQ(0, co_task->resume());

  CASE_EXPECT_TRUE(co_task->is_completed());

  ++g_test_coroutine_task_status;
  CASE_EXPECT_EQ(g_test_coroutine_task_status, 4);

  CASE_EXPECT_NE(co_task->get_coroutine_context()->get_ret_code(), -1);
}

CASE_TEST(coroutine_task, auto_finish) {
  typedef cotask::task<>::ptr_t task_ptr_type;
  {
    test_context_task_mem_function obj;
    task_ptr_type co_task = cotask::task<>::create(&test_context_task_mem_function::real_run, &obj);
    g_test_coroutine_task_status = 0;
    obj.task_id_ = co_task->get_id();
  }
  CASE_EXPECT_EQ(0, g_test_coroutine_task_status);

  {
    test_context_task_mem_function obj;
    task_ptr_type co_task = cotask::task<>::create(&test_context_task_mem_function::real_run, &obj);
    g_test_coroutine_task_status = 0;
    obj.task_id_ = co_task->get_id();

    CASE_EXPECT_EQ(0, co_task->start());

    ++g_test_coroutine_task_status;
    CASE_EXPECT_EQ(g_test_coroutine_task_status, 2);

    CASE_EXPECT_FALSE(co_task->is_completed());
  }

  ++g_test_coroutine_task_status;
  CASE_EXPECT_EQ(g_test_coroutine_task_status, 4);
}

struct test_context_task_next_action : public cotask::impl::task_action_impl {
  int set_;
  int check_;
  test_context_task_next_action(int s, int c) : cotask::impl::task_action_impl(), set_(s), check_(c) {}

  int operator()(void *) {
    CASE_EXPECT_EQ(g_test_coroutine_task_status, check_);
    g_test_coroutine_task_status = set_;

    CASE_EXPECT_EQ(copp::COPP_EC_IS_RUNNING, cotask::this_task::get_task()->start());

    ++g_test_coroutine_task_on_finished;
    return 0;
  }
};

CASE_TEST(coroutine_task, next) {
  typedef cotask::task<>::ptr_t task_ptr_type;

  {
    g_test_coroutine_task_status = 0;

    task_ptr_type co_task = cotask::task<>::create(test_context_task_next_action(15, 0));
    co_task->next(test_context_task_next_action(7, 15))
        ->next(test_context_task_next_action(99, 7))
        ->next(test_context_task_next_action(1023, 99))
        ->next(test_context_task_next_action(5, 1023));

    CASE_EXPECT_EQ(co_task->next(co_task), co_task);
    CASE_EXPECT_EQ(0, co_task->start());
    CASE_EXPECT_EQ(g_test_coroutine_task_status, 5);

    CASE_EXPECT_EQ(copp::COPP_EC_ALREADY_FINISHED, co_task->start());
  }

  {
    g_test_coroutine_task_status = 0;
    task_ptr_type co_task_a = cotask::task<>::create(test_context_task_next_action(2, 1));
    task_ptr_type co_task_b = cotask::task<>::create(test_context_task_function_1);

    CASE_EXPECT_EQ(0, co_task_b->start());
    co_task_a->next(co_task_b);
    CASE_EXPECT_EQ(0, co_task_a->start());
    CASE_EXPECT_TRUE(co_task_a->is_completed());
    CASE_EXPECT_TRUE(co_task_b->is_completed());
    CASE_EXPECT_TRUE(co_task_b->::cotask::impl::task_impl::is_completed());
  }
}

struct test_context_task_functor_drived : public cotask::impl::task_action_impl {
 public:
  int a_;
  int b_;
  test_context_task_functor_drived(int a, int b) : a_(a), b_(b) {}

  virtual int operator()(void *) {
    CASE_EXPECT_EQ(a_, 1);
    CASE_EXPECT_EQ(3, b_);

    return 0;
  }
};

CASE_TEST(coroutine_task, functor_drived_action) {
  typedef cotask::task<>::ptr_t task_ptr_type;
  cotask::task<>::coroutine_type::allocator_type alloc;
  task_ptr_type co_task = cotask::task<>::create_with<test_context_task_functor_drived>(alloc, 0, 0, 1, 3);
  CASE_EXPECT_EQ(0, co_task->start());
}

static int test_context_task_priavte_buffer(void *) {
  void *priv_data = cotask::task<>::this_task()->get_private_buffer();
  size_t priv_sz = cotask::task<>::this_task()->get_private_buffer_size();

  CASE_EXPECT_GE(priv_sz, 256);

  if (priv_sz >= 256) {
    char checked_data[256];
    memset(checked_data, 0x5e, 256);

    CASE_EXPECT_EQ(0, memcmp(priv_data, checked_data, 256));
  }

  return 0;
}

CASE_TEST(coroutine_task, priavte_buffer) {
  typedef cotask::task<>::ptr_t task_ptr_type;
  task_ptr_type co_task = cotask::task<>::create(test_context_task_priavte_buffer, 16384, 256);

  void *priv_data = co_task->get_private_buffer();
  memset(priv_data, 0x5e, 256);

  CASE_EXPECT_EQ(0, co_task->start());
}

static int test_context_task_timeout(void *) {
  cotask::task<>::this_task()->yield();

  CASE_EXPECT_TRUE(cotask::task<>::this_task()->is_timeout());
  CASE_EXPECT_TRUE(cotask::task<>::this_task()->is_faulted());
  CASE_EXPECT_FALSE(cotask::task<>::this_task()->is_completed());
  CASE_EXPECT_TRUE(cotask::task<>::this_task()->is_exiting());

  return 0;
}

CASE_TEST(coroutine_task, kill_and_timeout) {
  typedef cotask::task<>::ptr_t task_ptr_type;
  task_ptr_type co_task = cotask::task<>::create(test_context_task_timeout, 16384, 256);

  void *priv_data = co_task->get_private_buffer();
  memset(priv_data, 0x5e, 256);

  CASE_EXPECT_EQ(0, co_task->start());

  CASE_EXPECT_FALSE(co_task->is_timeout());
  CASE_EXPECT_FALSE(co_task->is_faulted());
  CASE_EXPECT_FALSE(co_task->is_completed());
  CASE_EXPECT_FALSE(co_task->is_exiting());

  CASE_EXPECT_EQ(0, co_task->kill(cotask::EN_TS_TIMEOUT, nullptr));

  CASE_EXPECT_TRUE(co_task->is_completed());

  CASE_EXPECT_NE(nullptr, co_task->get_raw_action());
}

static int test_context_task_await_1(void *) {
  typedef cotask::task<>::ptr_t task_ptr_type;

  task_ptr_type self = cotask::task<>::this_task();

  CASE_EXPECT_EQ(copp::COPP_EC_ARGS_ERROR, self->await_task(nullptr));
  CASE_EXPECT_EQ(copp::COPP_EC_TASK_CAN_NOT_WAIT_SELF, self->await_task(self));

  void *priv_data = self->get_private_buffer();
  cotask::task<>::self_t *other_task = *reinterpret_cast<cotask::task<>::self_t **>(priv_data);

  if (other_task->is_exiting()) {
    CASE_EXPECT_EQ(copp::COPP_EC_TASK_IS_EXITING, self->await_task(other_task));
  } else {
    if (self->is_exiting()) {
      CASE_EXPECT_EQ(copp::COPP_EC_TASK_IS_EXITING, self->await_task(other_task));
    } else {
      CASE_EXPECT_EQ(0, self->await_task(other_task));
    }
  }

  return 0;
}

static int test_context_task_await_2(void *) { return 0; }

CASE_TEST(coroutine_task, await_task) {
  typedef cotask::task<>::ptr_t task_ptr_type;

  // normal
  {
    task_ptr_type co_task_1 =
        cotask::task<>::create(test_context_task_await_1, 16384, sizeof(cotask::task<>::self_t *));
    task_ptr_type co_task_2 = cotask::task<>::create(test_context_task_await_2, 16384);

    void *priv_data = co_task_1->get_private_buffer();

    *reinterpret_cast<cotask::task<>::self_t **>(priv_data) = co_task_2.get();
    CASE_EXPECT_EQ(copp::COPP_EC_TASK_NOT_IN_ACTION, co_task_1->await_task(co_task_2));

    CASE_EXPECT_FALSE(co_task_1->is_exiting());
    CASE_EXPECT_FALSE(co_task_2->is_exiting());

    co_task_1->start();
    CASE_EXPECT_FALSE(co_task_1->is_exiting());
    CASE_EXPECT_FALSE(co_task_2->is_exiting());

    co_task_2->start();
    CASE_EXPECT_TRUE(co_task_1->is_exiting());
    CASE_EXPECT_TRUE(co_task_2->is_exiting());
  }

  // co_task_1 exiting
  {
    task_ptr_type co_task_1 =
        cotask::task<>::create(test_context_task_await_1, 16384, sizeof(cotask::task<>::self_t *));
    task_ptr_type co_task_2 = cotask::task<>::create(test_context_task_await_2, 16384);

    void *priv_data = co_task_1->get_private_buffer();

    CASE_EXPECT_EQ(copp::COPP_EC_ARGS_ERROR, co_task_1->await_task(nullptr));
    CASE_EXPECT_EQ(copp::COPP_EC_TASK_CAN_NOT_WAIT_SELF, co_task_1->await_task(co_task_1));

    *reinterpret_cast<cotask::task<>::self_t **>(priv_data) = co_task_2.get();
    CASE_EXPECT_EQ(copp::COPP_EC_TASK_NOT_IN_ACTION, co_task_1->await_task(co_task_2));

    CASE_EXPECT_FALSE(co_task_1->is_exiting());
    CASE_EXPECT_FALSE(co_task_2->is_exiting());

    co_task_1->kill(cotask::EN_TS_TIMEOUT);
    CASE_EXPECT_TRUE(co_task_1->is_exiting());
    co_task_1->start();
    CASE_EXPECT_TRUE(co_task_1->is_exiting());
    CASE_EXPECT_FALSE(co_task_2->is_exiting());

    co_task_2->start();
    CASE_EXPECT_TRUE(co_task_1->is_exiting());
    CASE_EXPECT_TRUE(co_task_2->is_exiting());
  }

  // co_task_2 exiting
  {
    task_ptr_type co_task_1 =
        cotask::task<>::create(test_context_task_await_1, 16384, sizeof(cotask::task<>::self_t *));
    task_ptr_type co_task_2 = cotask::task<>::create(test_context_task_await_2, 16384);

    void *priv_data = co_task_1->get_private_buffer();

    *reinterpret_cast<cotask::task<>::self_t **>(priv_data) = co_task_2.get();
    CASE_EXPECT_EQ(copp::COPP_EC_TASK_NOT_IN_ACTION, co_task_1->await_task(co_task_2));

    CASE_EXPECT_FALSE(co_task_1->is_exiting());
    CASE_EXPECT_FALSE(co_task_2->is_exiting());

    co_task_2->start();
    CASE_EXPECT_FALSE(co_task_1->is_exiting());
    CASE_EXPECT_TRUE(co_task_2->is_exiting());

    co_task_1->start();
    CASE_EXPECT_TRUE(co_task_1->is_exiting());
    CASE_EXPECT_TRUE(co_task_2->is_exiting());
  }
}

static int test_context_task_then_action_func(void *priv_data) {
  CASE_EXPECT_EQ(&g_test_coroutine_task_status, priv_data);
  ++g_test_coroutine_task_on_finished;
  return 0;
}

CASE_TEST(coroutine_task, then) {
  typedef cotask::task<>::ptr_t task_ptr_type;
  g_test_coroutine_task_status = 0;
  g_test_coroutine_task_on_finished = 0;

  task_ptr_type co_task = cotask::task<>::create(test_context_task_next_action(15, 0));
  co_task->then(test_context_task_next_action(7, 15))
      ->then(test_context_task_next_action(99, 7))
      ->then(test_context_task_next_action(1023, 99))
      ->then(test_context_task_next_action(5, 1023));

  CASE_EXPECT_EQ(0, co_task->start());
  CASE_EXPECT_EQ(g_test_coroutine_task_status, 5);

  CASE_EXPECT_EQ(copp::COPP_EC_ALREADY_FINISHED, co_task->start());

  CASE_EXPECT_TRUE(co_task->is_exiting());
  CASE_EXPECT_TRUE(co_task->is_completed());

  co_task->then(test_context_task_next_action(127, 5))
      ->then(test_context_task_then_action_func, &g_test_coroutine_task_status);
  CASE_EXPECT_EQ(g_test_coroutine_task_status, 127);
  CASE_EXPECT_EQ(g_test_coroutine_task_on_finished, 7);
}

typedef copp::stack_pool<copp::allocator::stack_allocator_malloc> test_context_task_stack_pool_t;
struct test_context_task_stack_pool_test_macro_coroutine {
  using stack_allocator_type = copp::allocator::stack_allocator_pool<test_context_task_stack_pool_t>;
  using coroutine_type = copp::coroutine_context_container<stack_allocator_type>;
  using value_type = int;
};

typedef cotask::task<test_context_task_stack_pool_test_macro_coroutine> test_context_task_stack_pool_test_task_t;

CASE_TEST(coroutine_task, then_with_stack_pool) {
  typedef test_context_task_stack_pool_test_task_t::ptr_t task_ptr_type;
  test_context_task_stack_pool_t::ptr_t stack_pool = test_context_task_stack_pool_t::create();

  g_test_coroutine_task_on_finished = 0;
  g_test_coroutine_task_status = 0;

  copp::allocator::stack_allocator_pool<test_context_task_stack_pool_t> base_alloc(stack_pool);
  task_ptr_type tp = test_context_task_stack_pool_test_task_t::create(test_context_task_next_action(15, 0), base_alloc);
  tp->then(test_context_task_next_action(127, 15))
      ->then(test_context_task_then_action_func, &g_test_coroutine_task_status);

  CASE_EXPECT_EQ(3, stack_pool->get_limit().used_stack_number);
  tp->start();
  CASE_EXPECT_EQ(1, stack_pool->get_limit().used_stack_number);

  tp->then(test_context_task_next_action(255, 127))
      ->then(test_context_task_then_action_func, &g_test_coroutine_task_status);

  CASE_EXPECT_EQ(1, stack_pool->get_limit().used_stack_number);
  CASE_EXPECT_EQ(g_test_coroutine_task_status, 255);
  CASE_EXPECT_EQ(g_test_coroutine_task_on_finished, 5);
}

// Issue #18: https://github.com/owent/libcopp/issues/18

class task_action_of_issue18 {
 public:
  task_action_of_issue18() {}
  int operator()(void *) { return 0; }
};

CASE_TEST(coroutine_task, github_issues_18) {
  using simple_task_t = cotask::task<>;

  {
    simple_task_t::ptr_t co_task = simple_task_t::create(task_action_of_issue18());
    if (co_task) {
      co_task->then(task_action_of_issue18());
    }
  }

  {
    task_action_of_issue18 copy_action;
    simple_task_t::ptr_t co_task = simple_task_t::create(copy_action);
    if (co_task) {
      co_task->then(copy_action);
    }
  }
}

static LIBCOPP_COPP_NAMESPACE_ID::util::lock::atomic_int_type<int> g_test_context_task_test_atomic;
static constexpr const int g_test_context_task_test_mt_run_times = 10000;
static size_t g_test_context_task_test_mt_max_run_thread_number = 0;
enum {
  test_context_task_test_mt_thread_num = 100,
  test_context_task_test_mt_task_num = 1000,
};

struct test_context_task_test_action_mt_thread : public cotask::impl::task_action_impl {
  int run_count;

 public:
  test_context_task_test_action_mt_thread() : run_count(0) {}

  int operator()(void *thread_func_address) {
    std::set<void *> thread_counter;

    while (run_count < g_test_context_task_test_mt_run_times) {
      ++run_count;
      ++g_test_context_task_test_atomic;

      thread_counter.insert(thread_func_address);

      cotask::this_task::get_task()->yield(&thread_func_address);
    }

    if (g_test_context_task_test_mt_max_run_thread_number < thread_counter.size()) {
      g_test_context_task_test_mt_max_run_thread_number = thread_counter.size();
    }
    return 0;
  }
};

struct test_context_task_test_mt_thread_runner {
  typedef cotask::task<>::ptr_t task_ptr_type;

  std::vector<task_ptr_type> *task_pool;
  test_context_task_test_mt_thread_runner(std::vector<task_ptr_type> *pool) : task_pool(pool) {}

  int operator()() {
    bool need_continue = true;
    while (need_continue) {
      need_continue = false;

      for (size_t i = 0; i < task_pool->size(); ++i) {
        if (false == (*task_pool)[i]->is_completed()) {
          need_continue = true;
          (*task_pool)[i]->resume(this);
        }
      }
    }
    return 0;
  }
};

CASE_TEST(coroutine_task, mt_run_competition) {
  typedef cotask::task<>::ptr_t task_ptr_type;
  std::vector<task_ptr_type> task_pool;
  task_pool.reserve(test_context_task_test_mt_task_num);
  for (int i = 0; i < test_context_task_test_mt_task_num; ++i) {
    task_pool.push_back(
        cotask::task<>::create(test_context_task_test_action_mt_thread(), 16 * 1024));  // use 16KB for stack
  }

  g_test_context_task_test_atomic.store(0);
  g_test_context_task_test_mt_max_run_thread_number = 0;

  std::unique_ptr<std::thread> thds[test_context_task_test_mt_thread_num];
  for (int i = 0; i < test_context_task_test_mt_thread_num; ++i) {
    thds[i].reset(new std::thread(test_context_task_test_mt_thread_runner(&task_pool)));
  }

  for (int i = 0; i < test_context_task_test_mt_thread_num; ++i) {
    thds[i]->join();
  }

  CASE_EXPECT_EQ(g_test_context_task_test_mt_run_times * test_context_task_test_mt_task_num,
                 g_test_context_task_test_atomic.load());
  CASE_MSG_INFO() << "Coroutine tasks are run on " << g_test_context_task_test_mt_max_run_thread_number
                  << " threads at most." << std::endl;
}

#  if defined(LIBCOPP_MACRO_ENABLE_STD_COROUTINE) && LIBCOPP_MACRO_ENABLE_STD_COROUTINE
static copp::callable_future<int> call_for_await_cotask(const cotask::task<>::ptr_t &t) {
  if (t) {
    co_return co_await t;
  }

  co_return 0;
}

static int cotask_action_callback(void *) {
  int ret = 234;
  void *ptr = nullptr;
  cotask::this_task::get_task()->yield(&ptr);
  if (ptr != nullptr) {
    ret = *reinterpret_cast<int *>(ptr);
  }
  return ret;
}

CASE_TEST(coroutine_task, callable_future_co_await) {
  cotask::task<>::ptr_t co_task = cotask::task<>::create(cotask_action_callback);

  auto t = call_for_await_cotask(co_task);
  co_task->start();
  CASE_EXPECT_FALSE(t.is_ready());

  int res = 345;
  co_task->resume(reinterpret_cast<void *>(&res));

  CASE_EXPECT_TRUE(t.is_ready());

  CASE_EXPECT_EQ(res, t.get_internal_promise().data());
}

CASE_TEST(coroutine_task, callable_future_co_await_do_not_start) {
  cotask::task<>::ptr_t co_task = cotask::task<>::create(cotask_action_callback);

  auto t = call_for_await_cotask(co_task);
  CASE_EXPECT_FALSE(t.is_ready());

  co_task.reset();

  CASE_EXPECT_TRUE(t.is_ready());

  CASE_EXPECT_EQ(LIBCOPP_COPP_NAMESPACE_ID::COPP_EC_TASK_IS_KILLED, t.get_internal_promise().data());
}

#  endif

#endif

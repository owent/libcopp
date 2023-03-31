// Copyright 2023 owent

#include <libcotask/task.h>
#include <libcotask/task_manager.h>

#include <cstdio>
#include <cstring>
#include <iostream>
#include <memory>

#include "frame/test_macros.h"

#ifdef LIBCOTASK_MACRO_ENABLED

static int g_test_coroutine_task_manager_status = 0;
class test_context_task_manager_action : public cotask::impl::task_action_impl {
 public:
  int operator()(void *) {
    ++g_test_coroutine_task_manager_status;

    // CASE_EXPECT_EQ(cotask::EN_TS_RUNNING, cotask::this_task::get_task()->get_status());
    // may be EN_TS_RUNNING or EN_TS_CANCELED or EN_TS_KILLED or EN_TS_TIMEOUT
    cotask::this_task::get_task()->yield();

    ++g_test_coroutine_task_manager_status;

    return 0;
  }
};

CASE_TEST(coroutine_task_manager, tickspec_t) {
  cotask::detail::tickspec_t l;
  cotask::detail::tickspec_t r;

  l.tv_sec = 123;
  l.tv_nsec = 456;

  r.tv_sec = 123;
  r.tv_nsec = 456;

  CASE_EXPECT_TRUE(l == r);
  CASE_EXPECT_FALSE(l != r);
  CASE_EXPECT_FALSE(l < r);
  CASE_EXPECT_TRUE(l <= r);

  r.tv_sec = 456;
  r.tv_nsec = 123;

  CASE_EXPECT_FALSE(l == r);
  CASE_EXPECT_TRUE(l != r);
  CASE_EXPECT_TRUE(l < r);
  CASE_EXPECT_TRUE(l <= r);

  r.tv_sec = 45;
  r.tv_nsec = 999;

  CASE_EXPECT_FALSE(l == r);
  CASE_EXPECT_TRUE(l != r);
  CASE_EXPECT_FALSE(l < r);
  CASE_EXPECT_FALSE(l <= r);
}

CASE_TEST(coroutine_task_manager, task_timer_node) {
  cotask::detail::task_timer_node<cotask::task<>::id_type> l;
  cotask::detail::task_timer_node<cotask::task<>::id_type> r;

  l.expired_time.tv_sec = 123;
  l.expired_time.tv_nsec = 456;
  l.task_id = 10;

  r.expired_time.tv_sec = 123;
  r.expired_time.tv_nsec = 456;
  r.task_id = 10;

  CASE_EXPECT_TRUE(l == r);
  CASE_EXPECT_FALSE(l != r);
  CASE_EXPECT_FALSE(l < r);
  CASE_EXPECT_TRUE(l <= r);

  r.task_id = 5;

  CASE_EXPECT_FALSE(l == r);
  CASE_EXPECT_TRUE(l != r);
  CASE_EXPECT_FALSE(l < r);
  CASE_EXPECT_FALSE(l <= r);

  r.expired_time.tv_nsec = 45;
  r.task_id = 10;

  CASE_EXPECT_FALSE(l == r);
  CASE_EXPECT_TRUE(l != r);
  CASE_EXPECT_FALSE(l < r);
  CASE_EXPECT_FALSE(l <= r);

  r.expired_time.tv_nsec = 456;
  r.task_id = 15;

  CASE_EXPECT_FALSE(l == r);
  CASE_EXPECT_TRUE(l != r);
  CASE_EXPECT_TRUE(l < r);
  CASE_EXPECT_TRUE(l <= r);

  r.expired_time.tv_nsec = 4567;
  r.task_id = 10;

  CASE_EXPECT_FALSE(l == r);
  CASE_EXPECT_TRUE(l != r);
  CASE_EXPECT_TRUE(l < r);
  CASE_EXPECT_TRUE(l <= r);
}

CASE_TEST(coroutine_task_manager, add_and_timeout) {
  typedef cotask::task<>::ptr_t task_ptr_type;
  task_ptr_type co_task = cotask::task<>::create(test_context_task_manager_action());
  task_ptr_type co_another_task = cotask::task<>::create(test_context_task_manager_action());  // share action

  typedef cotask::task_manager<cotask::task<> > mgr_t;
  mgr_t::ptr_t task_mgr = mgr_t::create();

  CASE_EXPECT_EQ(0, (int)task_mgr->get_task_size());
  g_test_coroutine_task_manager_status = 0;

  task_mgr->add_task(co_task, 5, 0);
  task_mgr->add_task(co_another_task);

  CASE_EXPECT_EQ(2, (int)task_mgr->get_task_size());
  CASE_EXPECT_EQ(1, (int)task_mgr->get_tick_checkpoint_size());

  CASE_EXPECT_EQ(co_task, task_mgr->find_task(co_task->get_id()));
  CASE_EXPECT_EQ(co_another_task, task_mgr->find_task(co_another_task->get_id()));

  CASE_EXPECT_EQ(0, (int)task_mgr->get_last_tick_time().tv_sec);
  task_mgr->tick(3);
  CASE_EXPECT_EQ(2, (int)task_mgr->get_task_size());
  CASE_EXPECT_EQ(1, (int)task_mgr->get_tick_checkpoint_size());
  task_mgr->tick(8);
  CASE_EXPECT_EQ(8, (int)task_mgr->get_last_tick_time().tv_sec);
  // tick reset timeout: 3 + 5 = 8
  CASE_EXPECT_EQ(8, (int)task_mgr->get_container().find(co_task->get_id())->second.timer_node->expired_time.tv_sec);
  CASE_EXPECT_EQ(1, (int)task_mgr->get_tick_checkpoint_size());
  CASE_EXPECT_EQ(1, (int)task_mgr->get_checkpoints().size());
  CASE_EXPECT_FALSE(cotask::EN_TS_TIMEOUT == co_task->get_status());

  task_mgr->tick(9);
  CASE_EXPECT_EQ(9, (int)task_mgr->get_last_tick_time().tv_sec);
  CASE_EXPECT_EQ(1, (int)task_mgr->get_task_size());
  CASE_EXPECT_EQ(1, (int)task_mgr->get_container().size());
  CASE_EXPECT_EQ(0, (int)task_mgr->get_tick_checkpoint_size());
  CASE_EXPECT_TRUE(cotask::EN_TS_TIMEOUT == co_task->get_status());

  CASE_EXPECT_NE(co_task, task_mgr->find_task(co_task->get_id()));
  CASE_EXPECT_EQ(co_another_task, task_mgr->find_task(co_another_task->get_id()));

  CASE_EXPECT_EQ(2, g_test_coroutine_task_manager_status);

  task_mgr->start(co_another_task->get_id());

  CASE_EXPECT_EQ(cotask::EN_TS_WAITING, co_another_task->get_status());

  CASE_EXPECT_EQ(3, g_test_coroutine_task_manager_status);
  task_mgr->resume(co_another_task->get_id());
  CASE_EXPECT_EQ(4, g_test_coroutine_task_manager_status);

  CASE_EXPECT_EQ(cotask::EN_TS_TIMEOUT, co_task->get_status());
  CASE_EXPECT_EQ(cotask::EN_TS_DONE, co_another_task->get_status());

  CASE_EXPECT_EQ(0, (int)task_mgr->get_task_size());
  CASE_EXPECT_EQ(0, (int)task_mgr->get_tick_checkpoint_size());

  CASE_EXPECT_EQ(copp::COPP_EC_TASK_IS_EXITING, task_mgr->add_task(co_another_task));

  task_mgr.reset();
}

CASE_TEST(coroutine_task_manager, kill) {
  typedef cotask::task<>::ptr_t task_ptr_type;
  task_ptr_type co_task = cotask::task<>::create(test_context_task_manager_action());
  task_ptr_type co_another_task = cotask::task<>::create(test_context_task_manager_action());  // share action

  typedef cotask::task_manager<cotask::task<> > mgr_t;
  mgr_t::ptr_t task_mgr = mgr_t::create();

  CASE_EXPECT_EQ(0, (int)task_mgr->get_task_size());
  g_test_coroutine_task_manager_status = 0;
  task_mgr->tick(10, 0);

  task_mgr->add_task(co_task, 10, 0);
  task_mgr->add_task(co_another_task, 10, 0);

  task_mgr->start(co_task->get_id());

  CASE_EXPECT_EQ(2, (int)task_mgr->get_task_size());
  CASE_EXPECT_EQ(2, (int)task_mgr->get_tick_checkpoint_size());

  task_mgr->kill(co_task->get_id());
  task_mgr->cancel(co_another_task->get_id());

  CASE_EXPECT_EQ(4, g_test_coroutine_task_manager_status);

  CASE_EXPECT_EQ(0, (int)task_mgr->get_task_size());
  CASE_EXPECT_EQ(0, (int)task_mgr->get_tick_checkpoint_size());

  CASE_EXPECT_EQ(cotask::EN_TS_KILLED, co_task->get_status());
  CASE_EXPECT_EQ(cotask::EN_TS_CANCELED, co_another_task->get_status());
}

CASE_TEST(coroutine_task_manager, duplicated_checkpoints) {
  typedef cotask::task<>::ptr_t task_ptr_type;
  task_ptr_type co_task = cotask::task<>::create(test_context_task_manager_action());

  typedef cotask::task_manager<cotask::task<> > mgr_t;
  mgr_t::ptr_t task_mgr = mgr_t::create();

  CASE_EXPECT_EQ(0, (int)task_mgr->get_task_size());
  g_test_coroutine_task_manager_status = 0;

  task_mgr->add_task(co_task, 10, 0);
  CASE_EXPECT_EQ(1, (int)task_mgr->get_task_size());
  CASE_EXPECT_EQ(1, (int)task_mgr->get_tick_checkpoint_size());

  task_mgr->tick(5);

  CASE_EXPECT_EQ(1, (int)task_mgr->get_task_size());
  CASE_EXPECT_EQ(1, (int)task_mgr->get_tick_checkpoint_size());

  task_mgr->tick(15, 1);

  CASE_EXPECT_EQ(0, (int)task_mgr->get_task_size());
  CASE_EXPECT_EQ(0, (int)task_mgr->get_tick_checkpoint_size());

  CASE_EXPECT_EQ(2, g_test_coroutine_task_manager_status);
}

CASE_TEST(coroutine_task_manager, update_timeout) {
  typedef cotask::task<>::ptr_t task_ptr_type;
  task_ptr_type co_task = cotask::task<>::create(test_context_task_manager_action());

  typedef cotask::task_manager<cotask::task<> > mgr_t;
  mgr_t::ptr_t task_mgr = mgr_t::create();

  CASE_EXPECT_EQ(0, (int)task_mgr->get_task_size());
  g_test_coroutine_task_manager_status = 0;

  task_mgr->add_task(co_task, 10, 0);
  CASE_EXPECT_EQ(1, (int)task_mgr->get_task_size());
  CASE_EXPECT_EQ(1, (int)task_mgr->get_tick_checkpoint_size());

  task_mgr->tick(5);
  CASE_EXPECT_EQ(copp::COPP_EC_SUCCESS, task_mgr->set_timeout(co_task->get_id(), 20, 0));

  CASE_EXPECT_EQ(1, (int)task_mgr->get_task_size());
  CASE_EXPECT_EQ(1, (int)task_mgr->get_tick_checkpoint_size());

  task_mgr->tick(24, 1);

  CASE_EXPECT_EQ(1, (int)task_mgr->get_task_size());
  CASE_EXPECT_EQ(1, (int)task_mgr->get_tick_checkpoint_size());

  task_mgr->tick(25, 1);

  CASE_EXPECT_EQ(0, (int)task_mgr->get_task_size());
  CASE_EXPECT_EQ(0, (int)task_mgr->get_tick_checkpoint_size());

  CASE_EXPECT_EQ(2, g_test_coroutine_task_manager_status);
}

class test_context_task_manager_action_protect_this_task : public cotask::impl::task_action_impl {
 public:
  int operator()(void *) {
    int use_count = static_cast<int>(cotask::this_task::get<cotask::task<> >()->use_count());
    CASE_EXPECT_EQ(2, use_count);
    cotask::this_task::get_task()->yield();
    use_count = static_cast<int>(cotask::this_task::get<cotask::task<> >()->use_count());
    // if we support rvalue-reference, reference may be smaller.
    // remove action will be 3, resume and destroy will be 1 or 2
    // CASE_EXPECT_TRUE(use_count >= 1 && use_count <= 3);
    CASE_EXPECT_TRUE(use_count >= 1 && use_count <= 2);

    ++g_test_coroutine_task_manager_status;
    return 0;
  }
};

CASE_TEST(coroutine_task_manager, protect_this_task) {
  typedef cotask::task<>::ptr_t task_ptr_type;

  {
    typedef cotask::task_manager<cotask::task<> > mgr_t;
    mgr_t::ptr_t task_mgr = mgr_t::create();

    g_test_coroutine_task_manager_status = 0;
    task_ptr_type co_task = cotask::task<>::create(test_context_task_manager_action_protect_this_task());
    cotask::task<>::id_t id_finished = co_task->get_id();
    task_mgr->add_task(co_task);

    co_task = cotask::task<>::create(test_context_task_manager_action_protect_this_task());
    cotask::task<>::id_t id_unfinished = co_task->get_id();
    task_mgr->add_task(co_task);

    co_task = cotask::task<>::create(test_context_task_manager_action_protect_this_task());
    cotask::task<>::id_t id_removed = co_task->get_id();
    task_mgr->add_task(co_task);

    co_task.reset();

    task_mgr->start(id_finished);
    task_mgr->start(id_unfinished);
    task_mgr->start(id_removed);

    CASE_EXPECT_EQ(3, (int)task_mgr->get_task_size());
    task_mgr->resume(id_finished);

    CASE_EXPECT_EQ(2, (int)task_mgr->get_task_size());

    task_mgr->remove_task(id_removed);

    CASE_EXPECT_EQ(1, (int)task_mgr->get_task_size());
  }

  // all task should be finished
  CASE_EXPECT_EQ(3, (int)g_test_coroutine_task_manager_status);
}

static LIBCOPP_COPP_NAMESPACE_ID::util::lock::atomic_int_type<int> g_test_coroutine_task_manager_atomic;

static constexpr const int test_context_task_manager_action_mt_run_times = 10000;
enum { test_context_task_manager_action_mt_thread_num = 1000 };

struct test_context_task_manager_action_mt_thread : public cotask::impl::task_action_impl {
 public:
  int operator()(void *run_count_p) {
    assert(run_count_p);
    int *run_count = reinterpret_cast<int *>(run_count_p);

    while (*run_count < test_context_task_manager_action_mt_run_times) {
      ++(*run_count);
      ++g_test_coroutine_task_manager_atomic;

      cotask::this_task::get_task()->yield(&run_count_p);

      run_count = reinterpret_cast<int *>(run_count_p);
    }
    return 0;
  }
};

struct test_context_task_manager_mt_thread_runner {
  typedef cotask::task_manager<cotask::task<> > mgr_t;
  int run_count;
  mgr_t::ptr_t task_mgr;
  test_context_task_manager_mt_thread_runner(mgr_t::ptr_t mgr) : run_count(0), task_mgr(mgr) {}

  int operator()() {
    typedef cotask::task<>::ptr_t task_ptr_type;

    task_ptr_type co_task =
        cotask::task<>::create(test_context_task_manager_action_mt_thread(), 16 * 1024);  // use 16KB for stack
    cotask::task<>::id_t task_id = co_task->get_id();
    task_mgr->add_task(co_task);

    task_mgr->start(task_id, &run_count);

    while (false == co_task->is_completed()) {
      task_mgr->resume(task_id, &run_count);
    }

    CASE_EXPECT_EQ(test_context_task_manager_action_mt_run_times, run_count);
    return 0;
  }
};

CASE_TEST(coroutine_task_manager, create_and_run_mt) {
  typedef cotask::task_manager<cotask::task<> > mgr_t;
  mgr_t::ptr_t task_mgr = mgr_t::create();

  g_test_coroutine_task_manager_atomic.store(0);

  std::unique_ptr<std::thread> thds[test_context_task_manager_action_mt_thread_num];
  for (int i = 0; i < test_context_task_manager_action_mt_thread_num; ++i) {
    thds[i].reset(new std::thread(test_context_task_manager_mt_thread_runner(task_mgr)));
  }

  for (int i = 0; i < test_context_task_manager_action_mt_thread_num; ++i) {
    thds[i]->join();
  }

  CASE_EXPECT_EQ(test_context_task_manager_action_mt_run_times * test_context_task_manager_action_mt_thread_num,
                 g_test_coroutine_task_manager_atomic.load());
}

#  if defined(LIBCOTASK_MACRO_AUTO_CLEANUP_MANAGER) && LIBCOTASK_MACRO_AUTO_CLEANUP_MANAGER
CASE_TEST(coroutine_task_manager, auto_cleanup_for_manager) {
  typedef cotask::task<>::ptr_t task_ptr_type;
  task_ptr_type co_task = cotask::task<>::create(test_context_task_manager_action());

  typedef cotask::task_manager<cotask::task<> > mgr_t;
  mgr_t::ptr_t task_mgr1 = mgr_t::create();
  mgr_t::ptr_t task_mgr2 = mgr_t::create();

  CASE_EXPECT_EQ(0, task_mgr1->add_task(co_task, 5, 0));
  CASE_EXPECT_EQ(copp::COPP_EC_TASK_ALREADY_IN_ANOTHER_MANAGER, task_mgr2->add_task(co_task, 5, 0));

  CASE_EXPECT_EQ(1, task_mgr1->get_task_size());
  CASE_EXPECT_EQ(1, task_mgr1->get_tick_checkpoint_size());

  co_task->start();
  co_task->resume();

  CASE_EXPECT_EQ(0, task_mgr1->get_task_size());
  CASE_EXPECT_EQ(0, task_mgr1->get_tick_checkpoint_size());
}
#  endif

#  if defined(LIBCOPP_MACRO_ENABLE_STD_EXCEPTION_PTR) && LIBCOPP_MACRO_ENABLE_STD_EXCEPTION_PTR
class test_context_task_manager_action_with_exception : public cotask::impl::task_action_impl {
 public:
  int operator()(void *) {
    ++g_test_coroutine_task_manager_status;

    cotask::this_task::get_task()->yield();

    ++g_test_coroutine_task_manager_status;

    CASE_MSG_INFO() << std::string().at(1) << std::endl;

    return 0;
  }
};

CASE_TEST(coroutine_task_manager, exception_safe) {
  typedef cotask::task<>::ptr_t task_ptr_type;
  task_ptr_type co_task = cotask::task<>::create(test_context_task_manager_action_with_exception());

  typedef cotask::task_manager<cotask::task<> > mgr_t;
  mgr_t::ptr_t task_mgr1 = mgr_t::create();
  mgr_t::ptr_t task_mgr2 = mgr_t::create();

  CASE_EXPECT_EQ(0, task_mgr1->add_task(co_task, 5, 0));
  CASE_EXPECT_EQ(copp::COPP_EC_TASK_ALREADY_IN_ANOTHER_MANAGER, task_mgr2->add_task(co_task, 5, 0));

  CASE_EXPECT_EQ(1, task_mgr1->get_task_size());
  CASE_EXPECT_EQ(1, task_mgr1->get_tick_checkpoint_size());

  int check_status = g_test_coroutine_task_manager_status;
  try {
    co_task->start();
    co_task->resume();
  } catch (const std::exception &e) {
    CASE_MSG_INFO() << "Catch a exception: " << e.what() << std::endl;
  }

  CASE_EXPECT_EQ(check_status + 2, g_test_coroutine_task_manager_status);
  CASE_EXPECT_EQ(0, task_mgr1->get_task_size());
  CASE_EXPECT_EQ(0, task_mgr1->get_tick_checkpoint_size());
  CASE_EXPECT_TRUE(co_task->is_completed());
}
#  endif
#else
CASE_TEST(coroutine_task_manager, disabled) {}
#endif

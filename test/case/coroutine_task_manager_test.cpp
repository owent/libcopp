#ifdef COTASK_MACRO_ENABLED

#include <cstdio>
#include <cstring>
#include <iostream>

#include <libcopp/utils/std/smart_ptr.h>

#include "frame/test_macros.h"
#include <libcotask/task.h>
#include <libcotask/task_manager.h>

static int g_test_coroutine_task_manager_status = 0;
class test_context_task_manager_action : public cotask::impl::task_action_impl {
public:
    int operator()() {
        ++g_test_coroutine_task_manager_status;

        // CASE_EXPECT_EQ(cotask::EN_TS_RUNNING, cotask::this_task::get_task()->get_status());
        // may be EN_TS_RUNNING or EN_TS_CANCELED or EN_TS_KILLED or EN_TS_TIMEOUT
        cotask::this_task::get_task()->yield();

        ++g_test_coroutine_task_manager_status;

        return 0;
    }
};


CASE_TEST(coroutine_task_manager, add_and_timeout) {
    typedef std::shared_ptr<cotask::task<> > task_ptr_type;
    cotask::task<>::action_ptr_t action = cotask::task<>::action_ptr_t(new test_context_task_manager_action());
    task_ptr_type co_task = cotask::task<>::create(action);
    task_ptr_type co_another_task = cotask::task<>::create(action); // share action

    typedef cotask::task_manager<cotask::task<>::id_t> mgr_t;
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
    CASE_EXPECT_EQ(2, (int)task_mgr->get_task_size());
    CASE_EXPECT_EQ(1, (int)task_mgr->get_tick_checkpoint_size());

    task_mgr->tick(9);
    CASE_EXPECT_EQ(9, (int)task_mgr->get_last_tick_time().tv_sec);
    CASE_EXPECT_EQ(1, (int)task_mgr->get_task_size());
    CASE_EXPECT_EQ(0, (int)task_mgr->get_tick_checkpoint_size());

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
}


CASE_TEST(coroutine_task_manager, kill) {
    typedef std::shared_ptr<cotask::task<> > task_ptr_type;
    cotask::task<>::action_ptr_t action = cotask::task<>::action_ptr_t(new test_context_task_manager_action());
    task_ptr_type co_task = cotask::task<>::create(action);
    task_ptr_type co_another_task = cotask::task<>::create(action); // share action

    typedef cotask::task_manager<cotask::task<>::id_t> mgr_t;
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
    CASE_EXPECT_EQ(2, (int)task_mgr->get_tick_checkpoint_size());

    CASE_EXPECT_EQ(cotask::EN_TS_KILLED, co_task->get_status());
    CASE_EXPECT_EQ(cotask::EN_TS_CANCELED, co_another_task->get_status());
}

CASE_TEST(coroutine_task_manager, multi_checkpoints) {
    typedef std::shared_ptr<cotask::task<> > task_ptr_type;
    cotask::task<>::action_ptr_t action = cotask::task<>::action_ptr_t(new test_context_task_manager_action());
    task_ptr_type co_task = cotask::task<>::create(action);

    typedef cotask::task_manager<cotask::task<>::id_t> mgr_t;
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


class test_context_task_manager_action_protect_this_task : public cotask::impl::task_action_impl {
public:
    int operator()() {
        int use_count = (int)cotask::this_task::get_task()->shared_from_this().use_count();
        CASE_EXPECT_EQ(4, use_count);
        cotask::this_task::get_task()->yield();
        use_count = (int)cotask::this_task::get_task()->shared_from_this().use_count();
        // remove action will be 4, resume and destroy will be 2
        CASE_EXPECT_TRUE(2 == use_count || 4 == use_count);
        if (2 != use_count && 4 != use_count) {
            CASE_MSG_INFO() << use_count << std::endl;
        }

        ++g_test_coroutine_task_manager_status;
        return 0;
    }
};

CASE_TEST(coroutine_task_manager, protect_this_task) {
    typedef std::shared_ptr<cotask::task<> > task_ptr_type;
    cotask::task<>::action_ptr_t action = cotask::task<>::action_ptr_t(new test_context_task_manager_action_protect_this_task());

    {
        typedef cotask::task_manager<cotask::task<>::id_t> mgr_t;
        mgr_t::ptr_t task_mgr = mgr_t::create();


        g_test_coroutine_task_manager_status = 0;
        task_ptr_type co_task = cotask::task<>::create(action);
        cotask::task<>::id_t id_finished = co_task->get_id();
        task_mgr->add_task(co_task);


        co_task = cotask::task<>::create(action);
        cotask::task<>::id_t id_unfinished = co_task->get_id();
        task_mgr->add_task(co_task);

        co_task = cotask::task<>::create(action);
        cotask::task<>::id_t id_removed = co_task->get_id();
        task_mgr->add_task(co_task);

        co_task.reset();

        task_mgr->start(id_finished);
        task_mgr->start(id_unfinished);
        task_mgr->start(id_removed);

        CASE_EXPECT_EQ(4, (int)action.use_count());
        CASE_EXPECT_EQ(3, (int)task_mgr->get_task_size());
        task_mgr->resume(id_finished);

        CASE_EXPECT_EQ(3, (int)action.use_count());
        CASE_EXPECT_EQ(2, (int)task_mgr->get_task_size());

        task_mgr->remove_task(id_removed);

        CASE_EXPECT_EQ(2, (int)action.use_count());
        CASE_EXPECT_EQ(1, (int)task_mgr->get_task_size());
    }

    // all task should be finished
    CASE_EXPECT_EQ(3, (int)g_test_coroutine_task_manager_status);
}


#if ((defined(__cplusplus) && __cplusplus >= 201103L) || (defined(_MSC_VER) && _MSC_VER >= 1800)) && \
    defined(UTIL_CONFIG_COMPILER_CXX_LAMBDAS) && UTIL_CONFIG_COMPILER_CXX_LAMBDAS

static util::lock::atomic_int_type<int> g_test_coroutine_task_manager_atomic;


struct test_context_task_manager_action_mt_thread : public cotask::impl::task_action_impl {
public:
    int operator()(void *run_count_p) {
        assert(run_count_p);
        int *run_count = reinterpret_cast<int *>(run_count_p);

        while (*run_count < 10000) {
            ++(*run_count);
            ++g_test_coroutine_task_manager_atomic;

            cotask::this_task::get_task()->yield(&run_count_p);

            run_count = reinterpret_cast<int *>(run_count_p);
        }
        return 0;
    }
};

struct test_context_task_manager_mt_thread_runner {
    typedef cotask::task_manager<cotask::task<>::id_t> mgr_t;
    int run_count;
    mgr_t::ptr_t task_mgr;
    cotask::task<>::action_ptr_t action;
    test_context_task_manager_mt_thread_runner(mgr_t::ptr_t mgr, const cotask::task<>::action_ptr_t &act)
        : run_count(0), task_mgr(mgr), action(act) {}

    int operator()() {
        typedef std::shared_ptr<cotask::task<> > task_ptr_type;

        task_ptr_type co_task = cotask::task<>::create(action, 16 * 1024); // use 16KB for stack
        cotask::task<>::id_t task_id = co_task->get_id();
        task_mgr->add_task(co_task);

        action.reset();

        task_mgr->start(task_id, &run_count);

        while (false == co_task->is_completed()) {
            task_mgr->resume(task_id, &run_count);
        }

        CASE_EXPECT_EQ(10000, run_count);
        return 0;
    }
};

CASE_TEST(coroutine_task_manager, create_and_run_mt) {
    typedef cotask::task_manager<cotask::task<>::id_t> mgr_t;
    mgr_t::ptr_t task_mgr = mgr_t::create();
    cotask::task<>::action_ptr_t action = cotask::task<>::action_ptr_t(new test_context_task_manager_action_mt_thread());

    g_test_coroutine_task_manager_atomic.store(0);

    std::unique_ptr<std::thread> thds[1000];
    for (int i = 0; i < 1000; ++i) {
        thds[i].reset(new std::thread(test_context_task_manager_mt_thread_runner(task_mgr, action)));
    }

    for (int i = 0; i < 1000; ++i) {
        thds[i]->join();
    }

    CASE_EXPECT_EQ(10000000, g_test_coroutine_task_manager_atomic.load());
}

#endif

#endif

#include <cstdio>
#include <cstring>
#include <iostream>
#include <list>
#include <string>

#include <libcopp/future/future.h>
#include <libcopp/future/poll.h>
#include <libcopp/future/std_coroutine_generator.h>
#include <libcopp/future/std_coroutine_task.h>

#include "frame/test_macros.h"

// ================= Unit Test - C++20 Coroutine Support =================
#if defined(LIBCOPP_MACRO_ENABLE_STD_COROUTINE) && LIBCOPP_MACRO_ENABLE_STD_COROUTINE

struct test_future_for_std_coroutine_trivial_result_message_t {
    int ret_code;
    int rsp_code;
};

struct test_future_for_std_coroutine_trivial_generator_waker_t;

typedef copp::future::result_t<test_future_for_std_coroutine_trivial_result_message_t, int32_t> test_trivial_result_t;
typedef copp::future::task_t<test_trivial_result_t>                                             test_trivial_task_t;
typedef copp::future::context_t<test_future_for_std_coroutine_trivial_generator_waker_t>        test_trivial_context_t;
typedef copp::future::generator_future_t<test_trivial_result_t>                                 test_trivial_generator_future_t;
typedef copp::future::poll_t<test_trivial_result_t>                                             test_trivial_poll_t;

std::list<test_trivial_context_t *> g_test_future_for_std_coroutine_waker_context_list;

struct test_future_for_std_coroutine_trivial_generator_waker_t {
    int32_t                                       code;
    int32_t                                       await_times;
    std::list<test_trivial_context_t *>::iterator refer_to;
    test_future_for_std_coroutine_trivial_generator_waker_t(int32_t c, int32_t at) : code(c), await_times(at) {
        refer_to = g_test_future_for_std_coroutine_waker_context_list.end();
    }

    ~test_future_for_std_coroutine_trivial_generator_waker_t() {
        if (refer_to != g_test_future_for_std_coroutine_waker_context_list.end()) {
            g_test_future_for_std_coroutine_waker_context_list.erase(refer_to);
        }
    }

    void operator()(test_trivial_generator_future_t &fut, test_trivial_context_t &ctx) {
        if (refer_to != g_test_future_for_std_coroutine_waker_context_list.end()) {
            g_test_future_for_std_coroutine_waker_context_list.erase(refer_to);
            refer_to = g_test_future_for_std_coroutine_waker_context_list.end();
        }
        if (await_times-- > 0) {
            refer_to =
                g_test_future_for_std_coroutine_waker_context_list.insert(g_test_future_for_std_coroutine_waker_context_list.end(), &ctx);
            return;
        }

        if (code > 0) {
            test_future_for_std_coroutine_trivial_result_message_t msg;
            msg.ret_code    = code;
            msg.rsp_code    = code;
            fut.poll_data() = test_trivial_result_t::make_success(msg);
        } else {
            fut.poll_data() = test_trivial_result_t::make_error(code);
        }
    }
};

typedef copp::future::generator_t<test_trivial_result_t, test_future_for_std_coroutine_trivial_generator_waker_t> test_generator_t;

static test_generator_t call_for_coroutine_fn_generator(int32_t code) { return test_generator_t{code, 1}; }

static test_trivial_task_t call_for_coroutine_fn_runtime_with_code(int32_t await_times, int32_t code) {
    test_trivial_poll_t ret;
    for (int32_t i = 0; i < await_times; ++i) {
        ret = co_await call_for_coroutine_fn_generator(code);
        if (nullptr != ret.data()) {
            if (ret.data()->is_success()) {
                CASE_MSG_INFO() << "co_await got success response: (" << ret.data()->get_success()->ret_code << ","
                                << ret.data()->get_success()->rsp_code << ")" << std::endl;
            } else if (ret.data()->is_error()) {
                CASE_MSG_INFO() << "co_await got error response: " << *ret.data()->get_error() << std::endl;
            }
        } else {
            CASE_MSG_INFO() << "co_await got pending poll data." << std::endl;
        }
    }

    if (nullptr != ret.data()) {
        co_return std::move(*ret.data());
    }

    co_return test_trivial_result_t::make_error(-1);
}

CASE_TEST(future_for_std_coroutine, poll_trival) {
    // auto test_var   = std::is_trivial<void>::value;
    // auto test_varsz = sizeof(void);

    // CASE_MSG_INFO() << test_var << std::endl;
    // CASE_MSG_INFO() << test_var << std::endl;
    test_trivial_task_t t1 = call_for_coroutine_fn_runtime_with_code(3, 200);
    test_trivial_task_t t2 = call_for_coroutine_fn_runtime_with_code(3, -200);

    CASE_EXPECT_FALSE(t1.is_finished());
    CASE_EXPECT_FALSE(t2.is_finished());

    for (int i = 0; i < 10 && (!t1.is_finished() || !t2.is_finished()); ++i) {
        for (std::list<test_generator_t::context_type *>::iterator iter = g_test_future_for_std_coroutine_waker_context_list.begin();
             iter != g_test_future_for_std_coroutine_waker_context_list.end();) {
            test_generator_t::context_type *ctx = (*iter);
            ++iter;
            ctx->wake();
        }
    }

    CASE_EXPECT_TRUE(t1.is_finished());
    CASE_EXPECT_TRUE(t2.is_finished());
    CASE_EXPECT_NE(nullptr, t1.data());
    CASE_EXPECT_NE(nullptr, t2.data());
    if (nullptr != t1.data()) {
        CASE_EXPECT_TRUE(t1.data()->is_success());
        if (t1.data()->is_success()) {
            CASE_EXPECT_EQ(200, t1.data()->get_success()->rsp_code);
        }
    }
    if (nullptr != t2.data()) {
        CASE_EXPECT_TRUE(t2.data()->is_error());
        CASE_EXPECT_EQ(-200, *t2.data()->get_error());
    }
}

static copp::future::task_t<int> call_for_coroutine_fn_runtime_trivial() {
    // test compile for trivial result type of task_t
    co_return 123;
}

static copp::future::task_t<void> call_for_coroutine_fn_runtime_void() {
    // test compile for void result type of task_t
    co_return;
}

CASE_TEST(future_for_std_coroutine, tast_with_trivial_result) {
    copp::future::task_t<int> t = call_for_coroutine_fn_runtime_trivial();

    CASE_EXPECT_TRUE(t.is_finished());
    CASE_EXPECT_TRUE(t.poll_data().is_ready());
    CASE_EXPECT_NE(nullptr, t.data());
    if (nullptr != t.data()) {
        CASE_EXPECT_EQ(123, *t.data());
    }
}

CASE_TEST(future_for_std_coroutine, tast_with_void_result) {
    copp::future::task_t<void> t = call_for_coroutine_fn_runtime_void();

    CASE_EXPECT_TRUE(t.is_finished());
    CASE_EXPECT_TRUE(t.poll_data().is_ready());
    CASE_EXPECT_NE(nullptr, t.data());
}

#endif

#include <cstdio>
#include <cstring>
#include <iostream>
#include <list>
#include <string>

#include <libcopp/future/future.h>
#include <libcopp/future/poll.h>

#include "frame/test_macros.h"

// ================= Unit Test - C++20 Coroutine Support =================
#if defined(LIBCOPP_MACRO_ENABLE_STD_COROUTINE) && LIBCOPP_MACRO_ENABLE_STD_COROUTINE
/*
struct test_no_trivial_parent_clazz {
    test_no_trivial_parent_clazz() : data(0) {}
    test_no_trivial_parent_clazz(int a) : data(a) {}
    virtual ~test_no_trivial_parent_clazz() {}
    virtual int get_type() { return 1; }

    int data;
};

struct test_no_trivial_child_clazz : public test_no_trivial_parent_clazz {
    test_no_trivial_child_clazz() {}
    test_no_trivial_child_clazz(int a) : test_no_trivial_parent_clazz(-a) {}
    virtual ~test_no_trivial_child_clazz() {}
    virtual int get_type() { return 2; }
};

struct test_future_for_std_coroutine_result_message_t {
    int ret_code;
    int rsp_code;
};

struct test_future_for_std_coroutine_waker_t;

typedef copp::future::result_t<test_future_for_std_coroutine_result_message_t, int32_t> test_result_t;
typedef copp::future::task_t<test_result_t>                                             test_task_t;
typedef copp::future::poll_t<test_result_t>                                             test_poll_t;
typedef copp::future::context_t<test_future_for_std_coroutine_waker_t>                  test_context_t;

std::list<test_context_t> g_test_future_for_std_coroutine_waker_context_list;

struct test_future_for_std_coroutine_waker_t {
    int code;
    int pend_poll;

    // for call_for_coroutine_fn_success
    test_future_for_std_coroutine_waker_t() : code(200), pend_poll(1) {}

    // for call_for_coroutine_fn_failed
    test_future_for_std_coroutine_waker_t(int32_t c) : code(c), pend_poll(1) {}

    void operator()(test_context_t &ctx) {}

    void operator()(test_context_t &ctx, test_poll_t &out) {
        //
        g_test_future_for_std_coroutine_waker_context_list.push_back(ctx);
    }
};

static test_task_t call_for_coroutine_fn_failed(int32_t code) {
    //
    co_return test_result_t::create_error(code);
}

static test_task_t call_for_coroutine_fn_success() {
    auto ret = copp::future::make_unique<test_future_for_std_coroutine_result_message_t>();

    ret->ret_code = 0;
    ret->rsp_code = 0;

    co_return test_result_t::create_success(std::move(ret));
}

static test_task_t call_for_coroutine_fn_runtime() {
    //
    auto failed_res = co_await call_for_coroutine_fn_failed(403);
}

CASE_TEST(future_for_std_coroutine, poll_trival) {
    // auto test_var   = std::is_trivial<void>::value;
    // auto test_varsz = sizeof(void);

    // CASE_MSG_INFO() << test_var << std::endl;
    // CASE_MSG_INFO() << test_var << std::endl;
}
*/
#endif

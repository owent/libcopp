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

typedef copp::future::result_t<test_future_for_std_coroutine_trivial_result_message_t, int32_t>     test_trivial_result_t;
typedef copp::future::task_t<test_trivial_result_t>                                                 test_trivial_task_t;
typedef copp::future::generator_context_t<test_future_for_std_coroutine_trivial_generator_waker_t>  test_trivial_generator_context_t;
typedef copp::future::generator_future_t<test_trivial_result_t>                                     test_trivial_generator_future_t;
typedef copp::future::poll_t<test_trivial_result_t>                                                 test_trivial_poll_t;

std::list<test_trivial_generator_context_t *> g_test_future_for_std_coroutine_trivial_context_waker_list;

struct test_future_for_std_coroutine_trivial_generator_waker_t {
    int32_t                                       code;
    int32_t                                       await_times;
    std::list<test_trivial_generator_context_t *>::iterator refer_to;
    test_future_for_std_coroutine_trivial_generator_waker_t(int32_t c, int32_t at) : code(c), await_times(at) {
        refer_to = g_test_future_for_std_coroutine_trivial_context_waker_list.end();
    }

    ~test_future_for_std_coroutine_trivial_generator_waker_t() {
        if (refer_to != g_test_future_for_std_coroutine_trivial_context_waker_list.end()) {
            g_test_future_for_std_coroutine_trivial_context_waker_list.erase(refer_to);
        }
    }

    void operator()(test_trivial_generator_future_t &fut, test_trivial_generator_context_t &ctx) {
        if (refer_to != g_test_future_for_std_coroutine_trivial_context_waker_list.end()) {
            g_test_future_for_std_coroutine_trivial_context_waker_list.erase(refer_to);
            refer_to = g_test_future_for_std_coroutine_trivial_context_waker_list.end();
        }
        if (await_times-- > 0) {
            refer_to = g_test_future_for_std_coroutine_trivial_context_waker_list.insert(
                g_test_future_for_std_coroutine_trivial_context_waker_list.end(), &ctx);
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

typedef copp::future::generator_t<test_trivial_result_t, test_future_for_std_coroutine_trivial_generator_waker_t> test_trivial_generator_t;

static test_trivial_task_t call_for_coroutine_fn_runtime_with_code(int32_t await_times, int32_t code) {
    test_trivial_poll_t ret;
    for (int32_t i = 0; i < await_times; ++i) {
        ret = co_await copp::future::make_generator<test_trivial_generator_t>(code, 1);
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

CASE_TEST(future_for_std_coroutine, poll_trival_generator) {
#if defined(UTIL_CONFIG_COMPILER_CXX_STATIC_ASSERT) && UTIL_CONFIG_COMPILER_CXX_STATIC_ASSERT
    static_assert(std::is_trivial<test_trivial_result_t>::value, "test_trivial_result_t must be trivial");
#endif
    test_trivial_task_t t1 = call_for_coroutine_fn_runtime_with_code(3, 200);
    test_trivial_task_t t2 = call_for_coroutine_fn_runtime_with_code(3, -200);

    CASE_EXPECT_FALSE(t1.done());
    CASE_EXPECT_FALSE(t2.done());

    for (int i = 0; i < 10 && (!t1.done() || !t2.done()); ++i) {
        for (std::list<test_trivial_generator_t::context_type *>::iterator iter =
                 g_test_future_for_std_coroutine_trivial_context_waker_list.begin();
             iter != g_test_future_for_std_coroutine_trivial_context_waker_list.end();) {
            test_trivial_generator_t::context_type *ctx = (*iter);
            ++iter;
            ctx->wake();
        }
    }

    CASE_EXPECT_TRUE(t1.done());
    CASE_EXPECT_TRUE(t2.done());
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

    CASE_EXPECT_TRUE(t.done());
    CASE_EXPECT_TRUE(t.poll_data()->is_ready());
    CASE_EXPECT_NE(nullptr, t.data());
    if (nullptr != t.data()) {
        CASE_EXPECT_EQ(123, *t.data());
    }
}

CASE_TEST(future_for_std_coroutine, tast_with_void_result) {
    copp::future::task_t<void> t = call_for_coroutine_fn_runtime_void();

    CASE_EXPECT_TRUE(t.done());
    CASE_EXPECT_TRUE(t.poll_data()->is_ready());
    CASE_EXPECT_NE(nullptr, t.data());
}


class test_future_for_std_coroutine_no_trivial_result_message_t {
private:
    test_future_for_std_coroutine_no_trivial_result_message_t(const test_future_for_std_coroutine_no_trivial_result_message_t &)
        UTIL_CONFIG_DELETED_FUNCTION;
    test_future_for_std_coroutine_no_trivial_result_message_t &
    operator=(const test_future_for_std_coroutine_no_trivial_result_message_t &) UTIL_CONFIG_DELETED_FUNCTION;
    test_future_for_std_coroutine_no_trivial_result_message_t(test_future_for_std_coroutine_no_trivial_result_message_t &&)
        UTIL_CONFIG_DELETED_FUNCTION;
    test_future_for_std_coroutine_no_trivial_result_message_t &
    operator=(test_future_for_std_coroutine_no_trivial_result_message_t &&) UTIL_CONFIG_DELETED_FUNCTION;

public:
    int ret_code;
    int rsp_code;

    test_future_for_std_coroutine_no_trivial_result_message_t() : ret_code(0), rsp_code(0) {}
    test_future_for_std_coroutine_no_trivial_result_message_t(int c) : ret_code(c), rsp_code(c) {}
    ~test_future_for_std_coroutine_no_trivial_result_message_t() {}
};

struct test_future_for_std_coroutine_no_trivial_generator_waker_t;
struct test_future_for_std_coroutine_no_trivial_task_waker_t;

typedef copp::future::result_t<test_future_for_std_coroutine_no_trivial_result_message_t, int32_t>            test_no_trivial_result_t;
typedef copp::future::task_t<test_no_trivial_result_t, test_future_for_std_coroutine_no_trivial_task_waker_t> test_no_trivial_task_t;
typedef copp::future::generator_context_t<test_future_for_std_coroutine_no_trivial_generator_waker_t>         test_no_trivial_generator_context_t;
typedef copp::future::generator_future_t<test_no_trivial_result_t>                                            test_no_trivial_generator_future_t;
typedef copp::future::poll_t<test_no_trivial_result_t>                                                        test_no_trivial_poll_t;
typedef copp::future::task_context_t<test_future_for_std_coroutine_no_trivial_task_waker_t>                   test_no_trivial_task_context_t;
typedef copp::future::task_future_t<test_no_trivial_result_t>                                                 test_no_trivial_task_future_t;

std::list<test_no_trivial_generator_context_t *> g_test_future_for_std_coroutine_no_trivial_generator_waker_list;
std::list<test_no_trivial_task_context_t *> g_test_future_for_std_coroutine_no_trivial_task_waker_list;

struct test_future_for_std_coroutine_no_trivial_generator_waker_t {
    int32_t                                          code;
    int32_t                                          await_times;
    std::list<test_no_trivial_generator_context_t *>::iterator refer_to;
    test_future_for_std_coroutine_no_trivial_generator_waker_t(int32_t c, int32_t at) : code(c), await_times(at) {
        refer_to = g_test_future_for_std_coroutine_no_trivial_generator_waker_list.end();
    }

    ~test_future_for_std_coroutine_no_trivial_generator_waker_t() {
        if (refer_to != g_test_future_for_std_coroutine_no_trivial_generator_waker_list.end()) {
            g_test_future_for_std_coroutine_no_trivial_generator_waker_list.erase(refer_to);
        }
    }

    void operator()(test_no_trivial_generator_future_t &fut, test_no_trivial_generator_context_t &ctx) {
        if (refer_to != g_test_future_for_std_coroutine_no_trivial_generator_waker_list.end()) {
            g_test_future_for_std_coroutine_no_trivial_generator_waker_list.erase(refer_to);
            refer_to = g_test_future_for_std_coroutine_no_trivial_generator_waker_list.end();
        }
        if (await_times-- > 0) {
            refer_to = g_test_future_for_std_coroutine_no_trivial_generator_waker_list.insert(
                g_test_future_for_std_coroutine_no_trivial_generator_waker_list.end(), &ctx);
            return;
        }

        if (code > 0) {
            fut.poll_data() = test_no_trivial_result_t::make_success(code);
        } else {
            fut.poll_data() = test_no_trivial_result_t::make_error(code);
        }
    }
};

struct test_future_for_std_coroutine_no_trivial_task_waker_t {
    int32_t                                               code;
    std::list<test_no_trivial_task_context_t *>::iterator refer_iter;

    template<class... TARGS>
    test_future_for_std_coroutine_no_trivial_task_waker_t(TARGS&&... args): code(0), 
        refer_iter(g_test_future_for_std_coroutine_no_trivial_task_waker_list.end()) {}
    ~test_future_for_std_coroutine_no_trivial_task_waker_t() {
        if (g_test_future_for_std_coroutine_no_trivial_task_waker_list.end() != refer_iter) {
            g_test_future_for_std_coroutine_no_trivial_task_waker_list.erase(refer_iter);
        }
    }

    void operator()(test_no_trivial_task_future_t &fut, test_no_trivial_task_context_t &ctx) {
        if (g_test_future_for_std_coroutine_no_trivial_task_waker_list.end() != refer_iter) {
            g_test_future_for_std_coroutine_no_trivial_task_waker_list.erase(refer_iter);
            refer_iter = g_test_future_for_std_coroutine_no_trivial_task_waker_list.end();
        }

        if (0 != code) {
            fut.poll_data() = test_no_trivial_result_t::make_error(code);
            return;
        }

        // repush into manager
        refer_iter = g_test_future_for_std_coroutine_no_trivial_task_waker_list.insert(g_test_future_for_std_coroutine_no_trivial_task_waker_list.end(), &ctx);
    }
};

typedef copp::future::generator_t<test_no_trivial_result_t, test_future_for_std_coroutine_no_trivial_generator_waker_t>
    test_no_trivial_generator_t;

static test_no_trivial_task_t call_for_no_trivial_coroutine_fn_runtime_with_code(int32_t await_times, int32_t code) {
    // refer to auto task_t::operator co_await() &&
    co_await call_for_coroutine_fn_runtime_void();
    // refer to auto task_t::operator co_await() &

    copp::future::task_t<int> t123 = call_for_coroutine_fn_runtime_trivial();

    // TODO lifetime of task_t::handle_
    // TODO timeout control for test_no_trivial_task_t
    decltype(auto) r123 = co_await t123;
    CASE_EXPECT_NE(nullptr, r123);
    if (nullptr != r123) {
        CASE_EXPECT_EQ(123, *r123);
    }

    CASE_MSG_INFO() << "ready to co_await generator." << std::endl;
    test_no_trivial_poll_t ret;
    for (int32_t i = 0; i < await_times; ++i) {
        ret = co_await copp::future::make_generator<test_no_trivial_generator_t>(code, 1);
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
        co_return std::move(ret.raw_ptr());
    }

    co_return test_no_trivial_result_t::make_error(-1);
}

CASE_TEST(future_for_std_coroutine, poll_no_trival_generator) {
#if defined(UTIL_CONFIG_COMPILER_CXX_STATIC_ASSERT) && UTIL_CONFIG_COMPILER_CXX_STATIC_ASSERT
    static_assert(!std::is_trivial<test_no_trivial_result_t>::value, "test_no_trivial_result_t must not be trivial");
#endif
    test_no_trivial_task_t t1 = call_for_no_trivial_coroutine_fn_runtime_with_code(3, 200);
    test_no_trivial_task_t t2 = call_for_no_trivial_coroutine_fn_runtime_with_code(3, -200);

    CASE_EXPECT_FALSE(t1.done());
    CASE_EXPECT_FALSE(t2.done());

    for (int i = 0; i < 10 && (!t1.done() || !t2.done()); ++i) {
        for (std::list<test_no_trivial_generator_t::context_type *>::iterator iter =
                 g_test_future_for_std_coroutine_no_trivial_generator_waker_list.begin();
             iter != g_test_future_for_std_coroutine_no_trivial_generator_waker_list.end();) {
            test_no_trivial_generator_t::context_type *ctx = (*iter);
            ++iter;
            ctx->wake();
        }
    }

    CASE_EXPECT_TRUE(t1.done());
    CASE_EXPECT_TRUE(t2.done());
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

#endif

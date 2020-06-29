#include <cstdio>
#include <cstring>
#include <iostream>
#include <string>

#include <libcopp/future/future.h>
#include <libcopp/future/poll.h>

#include "frame/test_macros.h"

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

CASE_TEST(future, poll_void) {
    copp::future::poll_t<void> p1;
    CASE_EXPECT_FALSE(p1.is_ready());

    copp::future::poll_t<void> p2(123);
    CASE_EXPECT_TRUE(p2.is_ready() && p2.data());
    CASE_EXPECT_EQ(p2.data(), &p2.raw_ptr());

    std::unique_ptr<bool>      param3 = std::unique_ptr<bool>(new bool(false));
    copp::future::poll_t<void> p3(std::move(param3));
    CASE_EXPECT_TRUE(p3.is_ready() && p3.data());
    CASE_EXPECT_EQ(p3.data(), &p3.raw_ptr());

    copp::future::poll_t<void> p4;
    // set ready
    p4 = true;
    CASE_EXPECT_TRUE(p4.is_ready() && p4.data());
    CASE_EXPECT_EQ(p4.data(), &p4.raw_ptr());
}

CASE_TEST(future, poll_trival) {
    copp::future::poll_t<int> p1;
    CASE_EXPECT_FALSE(p1.is_ready());

    copp::future::poll_t<int> p2(123);
    CASE_EXPECT_TRUE(p2.is_ready() && p2.data());
    CASE_EXPECT_EQ(p2.data() ? *p2.data() : 0, 123);

    std::unique_ptr<int>      param3 = std::unique_ptr<int>(new int(234));
    copp::future::poll_t<int> p3(std::move(param3));
    CASE_EXPECT_TRUE(p3.is_ready() && p3.data());
    CASE_EXPECT_EQ(p3.data() ? *p3.data() : 0, 234);
}

CASE_TEST(future, poll_no_trivial) {
    typedef copp::future::poll_t<test_no_trivial_parent_clazz> test_poll_type;

    test_poll_type p1;
    CASE_EXPECT_FALSE(p1.is_ready());

    test_poll_type p2(std::unique_ptr<test_no_trivial_parent_clazz>(new test_no_trivial_parent_clazz(123)));
    CASE_EXPECT_TRUE(p2.is_ready() && p2.data());
    CASE_EXPECT_EQ(p2.data() ? p2.data()->data : 0, 123);

    test_poll_type p3(std::unique_ptr<test_no_trivial_child_clazz>(new test_no_trivial_child_clazz(234)));
    CASE_EXPECT_EQ(p3.data() ? p3.data()->data : 0, -234);
}

CASE_TEST(future, poll_shared_ptr) {
    typedef copp::future::poll_t<test_no_trivial_parent_clazz, std::shared_ptr<test_no_trivial_parent_clazz> > test_poll_type;

    test_poll_type p1;
    CASE_EXPECT_FALSE(p1.is_ready());

    test_poll_type p2(std::unique_ptr<test_no_trivial_parent_clazz>(new test_no_trivial_parent_clazz(123)));
    CASE_EXPECT_TRUE(p2.is_ready() && p2.data());
    CASE_EXPECT_EQ(p2.data() ? p2.data()->data : 0, 123);
    CASE_EXPECT_EQ(p2.data() ? p2.data()->get_type() : 0, 1);

    test_poll_type p3(std::make_shared<test_no_trivial_parent_clazz>(234));
    CASE_EXPECT_TRUE(p3.is_ready() && p3.data());
    CASE_EXPECT_EQ(p3.data() ? p3.data()->data : 0, 234);
    CASE_EXPECT_EQ(p3.data() ? p3.data()->get_type() : 0, 1);

    test_poll_type p4(std::unique_ptr<test_no_trivial_child_clazz>(new test_no_trivial_child_clazz(345)));
    CASE_EXPECT_TRUE(p4.is_ready() && p4.data());
    CASE_EXPECT_EQ(p4.data() ? p4.data()->data : 0, -345);
    CASE_EXPECT_EQ(p4.data() ? p4.data()->get_type() : 0, 2);

    test_poll_type p5(std::make_shared<test_no_trivial_child_clazz>(456));
    CASE_EXPECT_TRUE(p5.is_ready() && p5.data());
    CASE_EXPECT_EQ(p5.data() ? p5.data()->data : 0, -456);
    CASE_EXPECT_EQ(p5.data() ? p5.data()->get_type() : 0, 2);
}

CASE_TEST(future, poll_void_reset_and_swap) {
    // using copp::future::swap;
    {
        copp::future::poll_t<void> p1;
        copp::future::poll_t<void> p2;

        p1 = true;
        CASE_EXPECT_TRUE(p1.is_ready());
        CASE_EXPECT_TRUE(p2.is_pending());

        swap(p1, p2);

        CASE_EXPECT_FALSE(p1.is_ready());
        CASE_EXPECT_FALSE(p2.is_pending());

        p2.reset();
        CASE_EXPECT_TRUE(p2.is_pending());
    }
}

CASE_TEST(future, poll_trivial_reset_and_swap) {
    // using copp::future::swap;
    {
        copp::future::poll_t<int> p1;
        copp::future::poll_t<int> p2;

        p1 = 123;
        CASE_EXPECT_TRUE(p1.is_ready());
        CASE_EXPECT_TRUE(p2.is_pending());

        swap(p1, p2);

        CASE_EXPECT_FALSE(p1.is_ready());
        CASE_EXPECT_FALSE(p2.is_pending());
        CASE_EXPECT_EQ(123, *p2.data());

        p2.reset();
        CASE_EXPECT_TRUE(p2.is_pending());
    }
}

CASE_TEST(future, poll_no_trivial_reset_and_swap) {
    // using copp::future::swap;
    {
        copp::future::poll_t<test_no_trivial_parent_clazz> p1;
        copp::future::poll_t<test_no_trivial_parent_clazz> p2;

        p1 = copp::future::make_unique<test_no_trivial_parent_clazz>(123);
        CASE_EXPECT_TRUE(p1.is_ready());
        CASE_EXPECT_TRUE(p2.is_pending());

        swap(p1, p2);

        CASE_EXPECT_FALSE(p1.is_ready());
        CASE_EXPECT_FALSE(p2.is_pending());
        CASE_EXPECT_EQ(123, p2.data()->data);

        p2.reset();
        CASE_EXPECT_TRUE(p2.is_pending());
    }
}

CASE_TEST(future, swap_trivial_result) {
    // using copp::future::swap;
    {
        copp::future::result_t<test_no_trivial_parent_clazz, int> p1 = copp::future::result_t<test_no_trivial_parent_clazz, int>::create_success(123);
        copp::future::result_t<test_no_trivial_parent_clazz, int> p2 = copp::future::result_t<test_no_trivial_parent_clazz, int>::create_error(456);

        CASE_EXPECT_TRUE(p1.is_success());
        CASE_EXPECT_TRUE(p2.is_error());

        swap(p1, p2);

        CASE_EXPECT_FALSE(p1.is_success());
        CASE_EXPECT_FALSE(p2.is_error());

        CASE_EXPECT_EQ(456, *p1.get_error());
        CASE_EXPECT_EQ(123, p2.get_success()->data);
    }
}

CASE_TEST(future, swap_no_trivial_result) {
    // using copp::future::swap;
    {
        copp::future::result_t<int, int> p1 = copp::future::result_t<int, int>::create_success(123);
        copp::future::result_t<int, int> p2 = copp::future::result_t<int, int>::create_error(456);

        CASE_EXPECT_TRUE(p1.is_success());
        CASE_EXPECT_TRUE(p2.is_error());

        swap(p1, p2);

        CASE_EXPECT_FALSE(p1.is_success());
        CASE_EXPECT_FALSE(p2.is_error());

        CASE_EXPECT_EQ(456, *p1.get_error());
        CASE_EXPECT_EQ(123, *p2.get_success());
    }
}

template <class T>
struct test_future_void_context_poll_functor;

template <>
struct test_future_void_context_poll_functor<void> {
    int32_t delay;

    test_future_void_context_poll_functor(int32_t d) : delay(d) {}

    static void on_destroy(copp::future::context_t<void> &ctx) {
        CASE_MSG_INFO() << "[Future] custom void poll context " << &ctx << "destroyed." << std::endl;
    }

    void operator()(copp::future::context_t<void> &ctx, copp::future::context_t<void>::poll_event_data_t evt) {
        CASE_MSG_INFO() << "[Future] custom void poll functor " << this << " polled by " << &ctx << ". future: " << evt.future_ptr
                        << std::endl;

        if (delay > 0) {
            --delay;
            return;
        }

        ctx.set_on_destroy(on_destroy);

        copp::future::future_t<void> *fut = reinterpret_cast<copp::future::future_t<void> *>(evt.future_ptr);

        fut->poll_data() = true;

        CASE_MSG_INFO() << "[Future] custom void poll functor " << this << " finished" << std::endl;
    }
};

template <class T>
struct test_future_void_context_poll_functor {
    T       data;
    int32_t delay;

    template <class U>
    test_future_void_context_poll_functor(int32_t d, U in) : data(in), delay(d) {}

    static void on_destroy(copp::future::context_t<void> &ctx) {
        CASE_MSG_INFO() << "[Future] custom void poll context " << &ctx << "destroyed." << std::endl;
    }

    void operator()(copp::future::context_t<void> &ctx, copp::future::context_t<void>::poll_event_data_t evt) {
        CASE_MSG_INFO() << "[Future] custom poll functor " << this << " polled by " << &ctx << ". future: " << evt.future_ptr << std::endl;

        copp::future::future_t<T> *fut = reinterpret_cast<copp::future::future_t<T> *>(evt.future_ptr);

        if (delay > 0) {
            --delay;
            return;
        }

        ctx.set_on_destroy(on_destroy);

        T *r = reinterpret_cast<T *>(evt.private_data);
        if (NULL == r) {
            fut->poll_data() = copp::future::poll_t<T>(data);
        } else {
            fut->poll_data() = copp::future::poll_t<T>(*r);
        }

        CASE_MSG_INFO() << "[Future] custom poll functor " << this << " finished" << std::endl;
    }
};

template <bool>
static const char *test_future_trivial_name();

template <>
const char *test_future_trivial_name<false>() {
    return "no trivial";
}

template <>
const char *test_future_trivial_name<true>() {
    return "trivial";
}

template <class T>
struct test_future_custom_poller_for_context {
    typedef test_future_custom_poller_for_context<T> self_type;
    T                                                data;
    int32_t                                          delay;
    copp::future::context_t<self_type> *             last_trigger;

    template <class U, class UPTR>
    void operator()(copp::future::future_t<U, UPTR> &fut, copp::future::context_t<self_type> &ctx) {
        last_trigger = &ctx;

        CASE_MSG_INFO() << "[Future] custom " << test_future_trivial_name<std::is_trivial<self_type>::value>() << " poller " << this
                        << " polled by context " << &ctx << ". future_t: " << &fut << std::endl;
        if (delay > 0) {
            --delay;
            return;
        }

        fut.poll_data() = copp::future::poll_t<U, UPTR>(std::move(data));
        CASE_MSG_INFO() << "[Future] custom poller " << this << " finished" << std::endl;
    }
};

CASE_TEST(future, future_with_void_result_and_void_context) {
    copp::future::future_t<void> fut;

    CASE_EXPECT_FALSE(fut.is_ready());
    CASE_EXPECT_TRUE(fut.is_pending());
    CASE_EXPECT_EQ(NULL, fut.data());
    CASE_EXPECT_EQ(NULL, fut.raw_ptr().get());

    copp::future::context_t<void> ctx(copp::future::context_t<void>::construct(test_future_void_context_poll_functor<void>(1), NULL));
    fut.poll(ctx);
    CASE_EXPECT_FALSE(fut.is_ready());
    CASE_EXPECT_TRUE(fut.is_pending());

    // After first poll, ctx is binded to the future
    CASE_EXPECT_TRUE(!!ctx.get_wake_fn());
    // When jobs finished, call wake to poll again
    ctx.wake();
    CASE_EXPECT_TRUE(fut.is_ready());
    CASE_EXPECT_FALSE(fut.is_pending());
}

CASE_TEST(future, future_with_trival_result_and_void_context) {
    copp::future::future_t<int32_t> fut;
    int32_t                         simulator_result = 12345678;

    CASE_EXPECT_FALSE(fut.is_ready());
    CASE_EXPECT_TRUE(fut.is_pending());
    CASE_EXPECT_EQ(NULL, fut.data());
    CASE_EXPECT_EQ(NULL, fut.raw_ptr().get());

    copp::future::context_t<void> ctx(
        copp::future::context_t<void>::construct(test_future_void_context_poll_functor<int32_t>(1, 0), &simulator_result));
    fut.poll(ctx);
    CASE_EXPECT_FALSE(fut.is_ready());
    CASE_EXPECT_TRUE(fut.is_pending());

    // After first poll, ctx is binded to the future
    CASE_EXPECT_TRUE(!!ctx.get_wake_fn());
    // When jobs finished, call wake to poll again
    ctx.wake();
    CASE_EXPECT_TRUE(fut.is_ready());
    CASE_EXPECT_FALSE(fut.is_pending());

    CASE_EXPECT_EQ(simulator_result, *fut.data());
}


CASE_TEST(future, future_and_custom_poller_context_trivial) {
    typedef test_future_custom_poller_for_context<int32_t> custom_poller_t;
    UTIL_CONFIG_STATIC_ASSERT(std::is_trivial<custom_poller_t>::value);

    copp::future::future_t<int32_t> fut;
    int32_t                         simulator_result = 12345679;

    CASE_EXPECT_FALSE(fut.is_ready());
    CASE_EXPECT_TRUE(fut.is_pending());
    CASE_EXPECT_EQ(NULL, fut.data());
    CASE_EXPECT_EQ(NULL, fut.raw_ptr().get());


    copp::future::context_t<custom_poller_t> ctx;
    ctx.get_private_data().delay = 1;
    ctx.get_private_data().data  = simulator_result;

    fut.poll(ctx);
    CASE_EXPECT_FALSE(fut.is_ready());
    CASE_EXPECT_TRUE(fut.is_pending());

    // After first poll, ctx is binded to future
    CASE_EXPECT_TRUE(!!ctx.get_wake_fn());
    CASE_EXPECT_NE(NULL, ctx.get_private_data().last_trigger);
    // When jobs finished, call wake to poll again

    ctx.wake();

    CASE_EXPECT_TRUE(fut.is_ready());
    CASE_EXPECT_FALSE(fut.is_pending());

    CASE_EXPECT_EQ(simulator_result, *fut.data());

    ctx.wake();
}

CASE_TEST(future, future_and_custom_poller_context_no_trivial) {
    typedef test_future_custom_poller_for_context<std::unique_ptr<test_no_trivial_child_clazz> > custom_poller_t;
    UTIL_CONFIG_STATIC_ASSERT(!std::is_trivial<custom_poller_t>::value);

    copp::future::future_t<test_no_trivial_child_clazz> fut;
    int32_t                                             simulator_result = 12345680;

    CASE_EXPECT_FALSE(fut.is_ready());
    CASE_EXPECT_TRUE(fut.is_pending());
    CASE_EXPECT_EQ(NULL, fut.data());
    CASE_EXPECT_EQ(NULL, fut.raw_ptr().get());


    copp::future::context_t<custom_poller_t> ctx;
    ctx.get_private_data().delay = 1;

    fut.poll(ctx);
    CASE_EXPECT_FALSE(fut.is_ready());
    CASE_EXPECT_TRUE(fut.is_pending());

    // After first poll, ctx is binded to future
    CASE_EXPECT_TRUE(!!ctx.get_wake_fn());
    CASE_EXPECT_NE(NULL, ctx.get_private_data().last_trigger);
    // When jobs finished, call wake to poll again

    ctx.get_private_data().data.reset(new test_no_trivial_child_clazz(simulator_result));
    ctx.wake();

    CASE_EXPECT_TRUE(fut.is_ready());
    CASE_EXPECT_FALSE(fut.is_pending());
    // already moved into fut
    CASE_EXPECT_EQ(NULL, ctx.get_private_data().data.get());

    CASE_EXPECT_EQ(-simulator_result, fut.data()->data);

    ctx.wake();
}

CASE_TEST(future, future_with_copp_result_and_custom_poller_context_trivial) {
    typedef copp::future::result_t<int, long>                  result_type;
    typedef test_future_custom_poller_for_context<result_type> custom_poller_t;
    UTIL_CONFIG_STATIC_ASSERT(std::is_trivial<custom_poller_t>::value);

    copp::future::future_t<result_type> fut;
    int                                 simulator_result = 12345681;

    CASE_EXPECT_FALSE(fut.is_ready());
    CASE_EXPECT_TRUE(fut.is_pending());
    CASE_EXPECT_EQ(NULL, fut.data());
    CASE_EXPECT_EQ(NULL, fut.raw_ptr().get());


    copp::future::context_t<custom_poller_t> ctx;
    ctx.get_private_data().delay = 1;
    ctx.get_private_data().data  = result_type::create_success(simulator_result);

    fut.poll(ctx);
    CASE_EXPECT_FALSE(fut.is_ready());
    CASE_EXPECT_TRUE(fut.is_pending());

    // After first poll, ctx is binded to future
    CASE_EXPECT_TRUE(!!ctx.get_wake_fn());
    CASE_EXPECT_NE(NULL, ctx.get_private_data().last_trigger);
    // When jobs finished, call wake to poll again

    ctx.wake();

    CASE_EXPECT_TRUE(fut.is_ready());
    CASE_EXPECT_FALSE(fut.is_pending());

    CASE_EXPECT_TRUE(fut.data()->is_success());
    CASE_EXPECT_FALSE(fut.data()->is_error());
    CASE_EXPECT_EQ(simulator_result, *fut.data()->get_success());

    ctx.wake();
}

CASE_TEST(future, future_with_copp_result_and_custom_poller_context_no_trivial) {
    typedef copp::future::result_t<int, test_no_trivial_child_clazz>             result_type;
    typedef test_future_custom_poller_for_context<std::unique_ptr<result_type> > custom_poller_t;
    UTIL_CONFIG_STATIC_ASSERT(!std::is_trivial<custom_poller_t>::value);

    copp::future::future_t<result_type> fut;
    int                                 simulator_result = 12345682;

    CASE_EXPECT_FALSE(fut.is_ready());
    CASE_EXPECT_TRUE(fut.is_pending());
    CASE_EXPECT_EQ(NULL, fut.data());
    CASE_EXPECT_EQ(NULL, fut.raw_ptr().get());


    copp::future::context_t<custom_poller_t> ctx;
    ctx.get_private_data().delay = 1;

    fut.poll(ctx);
    CASE_EXPECT_FALSE(fut.is_ready());
    CASE_EXPECT_TRUE(fut.is_pending());

    // After first poll, ctx is binded to future
    CASE_EXPECT_TRUE(!!ctx.get_wake_fn());
    CASE_EXPECT_NE(NULL, ctx.get_private_data().last_trigger);
    // When jobs finished, call wake to poll again

    ctx.get_private_data().data.reset(new result_type(result_type::create_error(test_no_trivial_child_clazz(simulator_result))));
    ctx.wake();

    CASE_EXPECT_TRUE(fut.is_ready());
    CASE_EXPECT_FALSE(fut.is_pending());
    // already moved into fut
    CASE_EXPECT_FALSE(fut.data()->is_success());
    CASE_EXPECT_TRUE(fut.data()->is_error());
    CASE_EXPECT_EQ(NULL, ctx.get_private_data().data.get());

    CASE_EXPECT_EQ(-simulator_result, fut.data()->get_error()->data);

    ctx.wake();
}

#include <libcotask/task.h>

#include <libcopp/future/std_coroutine_generator.h>
#include <libcopp/future/std_coroutine_task.h>
#if defined(LIBCOPP_MACRO_ENABLE_STD_EXCEPTION_PTR) && LIBCOPP_MACRO_ENABLE_STD_EXCEPTION_PTR
#include <stdexcept>
#endif
// ================= Unit Test - C++20 Coroutine Support =================
#if defined(LIBCOPP_MACRO_ENABLE_STD_COROUTINE) && LIBCOPP_MACRO_ENABLE_STD_COROUTINE

struct test_future_for_std_coroutine_trivial_result_message_t {
    int ret_code;
    int rsp_code;
};

struct test_future_for_std_coroutine_trivial_generator_waker_t;

typedef copp::future::result_t<test_future_for_std_coroutine_trivial_result_message_t, int32_t>    test_trivial_result_t;
typedef copp::future::task_t<test_trivial_result_t>                                                test_trivial_task_t;
typedef copp::future::generator_context_t<test_future_for_std_coroutine_trivial_generator_waker_t> test_trivial_generator_context_t;
typedef copp::future::generator_future_t<test_trivial_result_t>                                    test_trivial_generator_future_t;
typedef copp::future::poll_t<test_trivial_result_t>                                                test_trivial_poll_t;

std::list<test_trivial_generator_context_t *> g_test_future_for_std_coroutine_trivial_context_waker_list;

struct test_future_for_std_coroutine_trivial_generator_waker_t {
    int32_t                                                 code;
    int32_t                                                 await_times;
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
        auto     gen = copp::future::make_generator<test_trivial_generator_t>(code, 1);
        (void)co_await gen;
        ret = std::move(gen.poll_data());
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

#if defined(LIBCOPP_MACRO_ENABLE_STD_EXCEPTION_PTR) && LIBCOPP_MACRO_ENABLE_STD_EXCEPTION_PTR
static copp::future::task_t<int> call_for_coroutine_fn_and_throw_exception() {
    co_return (int)std::string().at(1);
}

CASE_TEST(future_for_std_coroutine, tast_with_exception) {
    try {
        copp::future::task_t<int> t = call_for_coroutine_fn_and_throw_exception();
        CASE_EXPECT_TRUE(t.done());
        CASE_EXPECT_FALSE(t.poll_data()->is_ready());
        CASE_EXPECT_NE(nullptr, t.data());

        // this should never be called
        CASE_EXPECT_TRUE(false);
    } catch (const std::exception& e) {
        CASE_MSG_INFO() << "Caught exception \"" << e.what() << "\""<<std::endl;
    }
}

#endif

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

#define STD_COROUTINE_TASK_TIMEOUT_ERROR_CODE (-500)

struct test_future_for_std_coroutine_no_trivial_generator_waker_t;
struct test_future_for_std_coroutine_no_trivial_task_waker_t;

typedef copp::future::result_t<test_future_for_std_coroutine_no_trivial_result_message_t, int32_t>            test_no_trivial_result_t;
typedef copp::future::task_t<test_no_trivial_result_t, test_future_for_std_coroutine_no_trivial_task_waker_t> test_no_trivial_task_t;
typedef copp::future::generator_context_t<test_future_for_std_coroutine_no_trivial_generator_waker_t> test_no_trivial_generator_context_t;
typedef copp::future::generator_future_t<test_no_trivial_result_t>                                    test_no_trivial_generator_future_t;
typedef copp::future::poll_t<test_no_trivial_result_t>                                                test_no_trivial_poll_t;
typedef copp::future::task_context_t<test_future_for_std_coroutine_no_trivial_task_waker_t>           test_no_trivial_task_context_t;
typedef copp::future::task_future_t<test_no_trivial_result_t>                                         test_no_trivial_task_future_t;

std::list<test_no_trivial_generator_context_t *> g_test_future_for_std_coroutine_no_trivial_generator_waker_list;
std::list<test_no_trivial_task_context_t *>      g_test_future_for_std_coroutine_no_trivial_task_waker_list;

struct test_future_for_std_coroutine_no_trivial_generator_waker_t {
    int32_t                                                    code;
    int32_t                                                    await_times;
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

    template <class... TARGS>
    test_future_for_std_coroutine_no_trivial_task_waker_t(TARGS &&...)
        : code(0), refer_iter(g_test_future_for_std_coroutine_no_trivial_task_waker_list.end()) {}
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
        refer_iter = g_test_future_for_std_coroutine_no_trivial_task_waker_list.insert(
            g_test_future_for_std_coroutine_no_trivial_task_waker_list.end(), &ctx);
    }
};

typedef copp::future::generator_t<test_no_trivial_result_t, test_future_for_std_coroutine_no_trivial_generator_waker_t>
    test_no_trivial_generator_t;

static test_no_trivial_task_t call_for_no_trivial_coroutine_fn_runtime_with_code(int32_t await_times, int32_t code) {
    // refer to auto task_t::operator co_await() &&
    auto     t_void = call_for_coroutine_fn_runtime_void();
    (void)co_await t_void;
    // refer to auto task_t::operator co_await() &

    copp::future::task_t<int> t123 = call_for_coroutine_fn_runtime_trivial();

    decltype(auto) r123 = co_await t123;
    CASE_EXPECT_NE(nullptr, r123);
    if (nullptr != r123) {
        CASE_EXPECT_EQ(123, *r123);
    }

    CASE_MSG_INFO() << "ready to co_await generator." << std::endl;
    test_no_trivial_poll_t ret;
    for (int32_t i = 0; i < await_times; ++i) {
        auto     gen = copp::future::make_generator<test_no_trivial_generator_t>(code, 1);
        (void)co_await gen;
        ret = std::move(gen.poll_data());
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
    CASE_EXPECT_TRUE(test_no_trivial_task_t::status_type::RUNNING == t1.get_status());
    CASE_EXPECT_TRUE(test_no_trivial_task_t::status_type::RUNNING == t2.get_status());

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
    CASE_EXPECT_TRUE(test_no_trivial_task_t::status_type::DONE == t1.get_status());
    CASE_EXPECT_TRUE(test_no_trivial_task_t::status_type::DONE == t2.get_status());
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

static test_no_trivial_task_t call_for_no_trivial_coroutine_await_generator_and_timeout(int32_t await_times) {
    CASE_MSG_INFO() << "ready to co_await generator." << std::endl;
    auto                   gen = copp::future::make_generator<test_no_trivial_generator_t>(200, await_times);
    test_no_trivial_poll_t ret;
    (void)co_await               gen;

    ret = std::move(gen.poll_data());

    // the return is still pending, because it's resumed by timeout waker
    CASE_EXPECT_TRUE(ret.is_pending());
    test_no_trivial_task_context_t *context = co_yield test_no_trivial_task_t::current_context();
    CASE_EXPECT_NE(nullptr, context);
    if (context) {
        CASE_EXPECT_EQ(STD_COROUTINE_TASK_TIMEOUT_ERROR_CODE, context->get_private_data().code);
    }

    test_no_trivial_task_future_t *fut = co_yield test_no_trivial_task_t::current_future();
    CASE_EXPECT_NE(nullptr, fut);
    if (fut) {
        CASE_EXPECT_TRUE(fut->is_ready());
        CASE_EXPECT_TRUE(fut->data() && fut->data()->is_error());
        if (fut->data() && fut->data()->is_error()) {
            CASE_EXPECT_EQ(STD_COROUTINE_TASK_TIMEOUT_ERROR_CODE, *fut->data()->get_error());
        }
    }

    co_return test_no_trivial_result_t::make_error(-1);
}

CASE_TEST(future_for_std_coroutine, poll_no_trival_generator_and_timeout) {
    test_no_trivial_task_t t1 = call_for_no_trivial_coroutine_await_generator_and_timeout(3);
    CASE_EXPECT_FALSE(t1.done());
    CASE_EXPECT_TRUE(test_no_trivial_task_t::status_type::RUNNING == t1.get_status());

    // set timeout result
    for (int retry_times = 10; retry_times >= 0 && !g_test_future_for_std_coroutine_no_trivial_task_waker_list.empty(); --retry_times) {
        (*g_test_future_for_std_coroutine_no_trivial_task_waker_list.begin())->get_private_data().code =
            STD_COROUTINE_TASK_TIMEOUT_ERROR_CODE;
        // wake and resume task
        (*g_test_future_for_std_coroutine_no_trivial_task_waker_list.begin())->wake();
    }

    CASE_EXPECT_TRUE(t1.done());
    CASE_EXPECT_TRUE(test_no_trivial_task_t::status_type::DONE == t1.get_status());
    CASE_EXPECT_TRUE(t1.data() && t1.data()->is_error());
    if (t1.data() && t1.data()->is_error()) {
        CASE_EXPECT_EQ(STD_COROUTINE_TASK_TIMEOUT_ERROR_CODE, *t1.data()->get_error());
    }

    // cleanup generator dispatcher
    for (int retry_times = 10; retry_times >= 0 && !g_test_future_for_std_coroutine_no_trivial_generator_waker_list.empty();
         --retry_times) {
        (*g_test_future_for_std_coroutine_no_trivial_generator_waker_list.begin())->wake();
    }

    // cleanup task dispatcher
    for (int retry_times = 10; retry_times >= 0 && !g_test_future_for_std_coroutine_no_trivial_task_waker_list.empty(); --retry_times) {
        (*g_test_future_for_std_coroutine_no_trivial_task_waker_list.begin())->wake();
    }
}

static test_no_trivial_task_t call_for_no_trivial_coroutine_await_task_and_timeout(int32_t await_times) {
    CASE_MSG_INFO() << "ready to co_await task." << std::endl;
    auto t_dep = call_for_no_trivial_coroutine_await_generator_and_timeout(await_times);
    auto ret   = co_await t_dep;

    // the return is still pending, because it's resumed by timeout waker
    CASE_EXPECT_EQ(nullptr, ret);
    CASE_EXPECT_FALSE(t_dep.done());
    test_no_trivial_task_context_t *context = co_yield test_no_trivial_task_t::current_context();
    CASE_EXPECT_NE(nullptr, context);
    if (context) {
        CASE_EXPECT_EQ(STD_COROUTINE_TASK_TIMEOUT_ERROR_CODE, context->get_private_data().code);
    }

    test_no_trivial_task_future_t *fut = co_yield test_no_trivial_task_t::current_future();
    CASE_EXPECT_NE(nullptr, fut);
    if (fut) {
        CASE_EXPECT_TRUE(fut->is_ready());
        CASE_EXPECT_TRUE(fut->data() && fut->data()->is_error());
        if (fut->data() && fut->data()->is_error()) {
            CASE_EXPECT_EQ(STD_COROUTINE_TASK_TIMEOUT_ERROR_CODE, *fut->data()->get_error());
        }
    }

    co_return test_no_trivial_result_t::make_error(-1);
}

CASE_TEST(future_for_std_coroutine, poll_no_trival_task_and_timeout) {
    test_no_trivial_task_t t1 = call_for_no_trivial_coroutine_await_task_and_timeout(3);
    CASE_EXPECT_FALSE(t1.done());
    CASE_EXPECT_TRUE(test_no_trivial_task_t::status_type::RUNNING == t1.get_status());

    // set timeout result
    for (int retry_times = 10; retry_times >= 0 && !t1.done() && t1.get_context(); --retry_times) {
        CASE_EXPECT_NE(0, t1.get_context()->get_task_id());
        CASE_EXPECT_EQ(t1.get_task_id(), t1.get_context()->get_task_id());
        
        t1.get_context()->get_private_data().code = STD_COROUTINE_TASK_TIMEOUT_ERROR_CODE;
        // wake and resume task
        t1.get_context()->wake();
    }

    CASE_EXPECT_TRUE(t1.done());
    CASE_EXPECT_TRUE(test_no_trivial_task_t::status_type::DONE == t1.get_status());
    CASE_EXPECT_TRUE(t1.data() && t1.data()->is_error());
    if (t1.data() && t1.data()->is_error()) {
        CASE_EXPECT_EQ(STD_COROUTINE_TASK_TIMEOUT_ERROR_CODE, *t1.data()->get_error());
    }

    // cleanup task dispatcher
    for (int retry_times = 10; retry_times >= 0 && !g_test_future_for_std_coroutine_no_trivial_task_waker_list.empty(); --retry_times) {
        (*g_test_future_for_std_coroutine_no_trivial_task_waker_list.begin())->get_private_data().code =
            STD_COROUTINE_TASK_TIMEOUT_ERROR_CODE;
        (*g_test_future_for_std_coroutine_no_trivial_task_waker_list.begin())->wake();
    }


    // cleanup generator dispatcher
    for (int retry_times = 10; retry_times >= 0 && !g_test_future_for_std_coroutine_no_trivial_generator_waker_list.empty();
         --retry_times) {
        (*g_test_future_for_std_coroutine_no_trivial_generator_waker_list.begin())->wake();
    }
}

static copp::future::task_t<int> call_for_await_cotask(cotask::task<>::ptr_t t) {
    if (t) {
        co_return co_await t;
    }

    co_return 0;
}

static int cotask_action_callback(void *) {
    int ret = 234;
    void* ptr = nullptr;
    cotask::this_task::get_task()->yield(&ptr);
    if (ptr != nullptr) {
        ret = *reinterpret_cast<int*>(ptr);
    }
    return ret;
}

CASE_TEST(future_for_std_coroutine, co_await_cotask) {
    cotask::task<>::ptr_t co_task = cotask::task<>::create(cotask_action_callback);

    auto t = call_for_await_cotask(co_task);
    co_task->start();
    CASE_EXPECT_FALSE(t.done());

    int res = 345;
    co_task->resume(reinterpret_cast<void*>(&res));

    CASE_EXPECT_TRUE(t.done());
    if (nullptr != t.data()) {
        CASE_EXPECT_EQ(res, *t.data());
    }
}

#endif

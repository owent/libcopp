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

template <class T>
struct test_future_void_context_poll_functor {
    T       data;
    int32_t delay;

    template <class U>
    test_future_void_context_poll_functor(int32_t d, U in) : delay(d), data(in) {}

    void operator()(copp::future::context_t<void> &ctx, copp::future::context_t<void>::poll_event_data_t evt) {
        CASE_MSG_INFO() << "[Future] custom poll functor " << this << " polled by " << &ctx << ". poll_t: " << evt.poll_output << std::endl;

        copp::future::poll_t<T> *poll_out = reinterpret_cast<copp::future::poll_t<int32_t> *>(evt.poll_output);

        if (delay > 0) {
            --delay;
            return;
        }

        T *r = reinterpret_cast<T *>(evt.private_data);
        if (NULL == r) {
            *poll_out = copp::future::poll_t<T>(data);
        } else {
            *poll_out = copp::future::poll_t<T>(*r);
        }

        CASE_MSG_INFO() << "[Future] custom poll functor " << this << " finished" << std::endl;
    }
};

template <class T>
struct test_future_custom_poller_for_context {
    typedef test_future_custom_poller_for_context<T> self_type;
    T                                                data;
    int32_t                                          delay;
    copp::future::context_t<self_type> *             last_trigger;

    template <bool>
    static const char *trivial_name();

    template <>
    static const char *trivial_name<false>() {
        return "no trivial";
    }

    template <>
    static const char *trivial_name<true>() {
        return "trivial";
    }

    void operator()(copp::future::context_t<self_type> &ctx) {
        last_trigger = &ctx;
        CASE_MSG_INFO() << "[Future] custom " << trivial_name<std::is_trivial<self_type>::value>() << " poller " << this
                        << " created/moved by context " << &ctx << std::endl;
    }

    template <class U, class UPTR>
    void operator()(copp::future::context_t<self_type> &ctx, copp::future::poll_t<U, UPTR> &out) {
        if (false == ctx.is_shared_storage()) {
            CASE_EXPECT_EQ(last_trigger, &ctx);
        }
        CASE_MSG_INFO() << "[Future] custom " << trivial_name<std::is_trivial<self_type>::value>() << " poller " << this
                        << " polled by context " << &ctx << ". poll_t: " << &out << std::endl;
        if (delay > 0) {
            --delay;
            return;
        }

        out = copp::future::poll_t<U, UPTR>(COPP_MACRO_STD_MOVE(data));
        CASE_MSG_INFO() << "[Future] custom poller " << this << " finished" << std::endl;
    }
};

CASE_TEST(future, future_with_trival_result_and_void_context) {
    copp::future::future_t<int32_t> fut;
    int32_t                         simulator_result = 12345678;

    CASE_EXPECT_FALSE(fut.is_ready());
    CASE_EXPECT_TRUE(fut.is_pending());
    CASE_EXPECT_EQ(NULL, fut.data());
    CASE_EXPECT_EQ(NULL, fut.raw_ptr().get());

    copp::future::context_t<void> ctx(test_future_void_context_poll_functor<int32_t>(1, 0), &simulator_result);
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
    ctx.get_private_data()->delay = 1;
    ctx.get_private_data()->data  = simulator_result;

    fut.poll(ctx);
    CASE_EXPECT_FALSE(fut.is_ready());
    CASE_EXPECT_TRUE(fut.is_pending());

    // After first poll, ctx is binded to future
    CASE_EXPECT_TRUE(!!ctx.get_wake_fn());
    CASE_EXPECT_NE(NULL, ctx.get_private_data()->last_trigger);
    // When jobs finished, call wake to poll again
    // Notice: Copying a context may trigger to copy the private data object (as is TPD) if it's a trivial type
    copp::future::context_t<custom_poller_t> ctx_cloned1 = *ctx.get_private_data()->last_trigger;
    copp::future::context_t<custom_poller_t> ctx_cloned2 =
        *const_cast<const copp::future::context_t<custom_poller_t> *>(ctx.get_private_data()->last_trigger);
    ctx_cloned1.wake();

    CASE_EXPECT_TRUE(fut.is_ready());
    CASE_EXPECT_FALSE(fut.is_pending());

    CASE_EXPECT_EQ(simulator_result, *fut.data());

    ctx_cloned2.wake();
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
    ctx.get_private_data()->delay = 1;

    fut.poll(ctx);
    CASE_EXPECT_FALSE(fut.is_ready());
    CASE_EXPECT_TRUE(fut.is_pending());

    // After first poll, ctx is binded to future
    CASE_EXPECT_TRUE(!!ctx.get_wake_fn());
    CASE_EXPECT_NE(NULL, ctx.get_private_data()->last_trigger);
    // When jobs finished, call wake to poll again
    // Notice: Copying a context may trigger to copy the private data object (as is TPD) if it's a trivial type
    copp::future::context_t<custom_poller_t> ctx_cloned1 = *ctx.get_private_data()->last_trigger;
    copp::future::context_t<custom_poller_t> ctx_cloned2 =
        *const_cast<const copp::future::context_t<custom_poller_t> *>(ctx.get_private_data()->last_trigger);

    ctx_cloned1.get_private_data()->data.reset(new test_no_trivial_child_clazz(simulator_result));
    ctx_cloned1.wake();

    CASE_EXPECT_TRUE(fut.is_ready());
    CASE_EXPECT_FALSE(fut.is_pending());
    // already moved into fut
    CASE_EXPECT_EQ(NULL, ctx_cloned1.get_private_data()->data.get());

    CASE_EXPECT_EQ(-simulator_result, fut.data()->data);

    ctx_cloned2.wake();
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
    ctx.get_private_data()->delay = 1;
    ctx.get_private_data()->data  = result_type::create_success(simulator_result);

    fut.poll(ctx);
    CASE_EXPECT_FALSE(fut.is_ready());
    CASE_EXPECT_TRUE(fut.is_pending());

    // After first poll, ctx is binded to future
    CASE_EXPECT_TRUE(!!ctx.get_wake_fn());
    CASE_EXPECT_NE(NULL, ctx.get_private_data()->last_trigger);
    // When jobs finished, call wake to poll again
    // Notice: Copying a context may trigger to copy the private data object (as is TPD) if it's a trivial type
    copp::future::context_t<custom_poller_t> ctx_cloned1 = *ctx.get_private_data()->last_trigger;
    copp::future::context_t<custom_poller_t> ctx_cloned2 =
        *const_cast<const copp::future::context_t<custom_poller_t> *>(ctx.get_private_data()->last_trigger);
    ctx_cloned1.wake();

    CASE_EXPECT_TRUE(fut.is_ready());
    CASE_EXPECT_FALSE(fut.is_pending());

    CASE_EXPECT_TRUE(fut.data()->is_success());
    CASE_EXPECT_FALSE(fut.data()->is_error());
    CASE_EXPECT_EQ(simulator_result, *fut.data()->get_success());

    ctx_cloned2.wake();
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
    ctx.get_private_data()->delay = 1;

    fut.poll(ctx);
    CASE_EXPECT_FALSE(fut.is_ready());
    CASE_EXPECT_TRUE(fut.is_pending());

    // After first poll, ctx is binded to future
    CASE_EXPECT_TRUE(!!ctx.get_wake_fn());
    CASE_EXPECT_NE(NULL, ctx.get_private_data()->last_trigger);
    // When jobs finished, call wake to poll again
    // Notice: Copying a context may trigger to copy the private data object (as is TPD) if it's a trivial type
    copp::future::context_t<custom_poller_t> ctx_cloned1 = *ctx.get_private_data()->last_trigger;
    copp::future::context_t<custom_poller_t> ctx_cloned2 =
        *const_cast<const copp::future::context_t<custom_poller_t> *>(ctx.get_private_data()->last_trigger);

    ctx_cloned1.get_private_data()->data.reset(new result_type(result_type::create_error(test_no_trivial_child_clazz(simulator_result))));
    ctx_cloned1.wake();

    CASE_EXPECT_TRUE(fut.is_ready());
    CASE_EXPECT_FALSE(fut.is_pending());
    // already moved into fut
    CASE_EXPECT_FALSE(fut.data()->is_success());
    CASE_EXPECT_TRUE(fut.data()->is_error());
    CASE_EXPECT_EQ(NULL, ctx_cloned1.get_private_data()->data.get());

    CASE_EXPECT_EQ(-simulator_result, fut.data()->get_error()->data);

    ctx_cloned2.wake();
}

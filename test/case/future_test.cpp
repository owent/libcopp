// Copyright 2022 owent

#include <libcopp/future/future.h>
#include <libcopp/future/poller.h>

#include <cstdio>
#include <cstring>
#include <iostream>
#include <string>

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
  copp::future::poller<void> p1;
  CASE_EXPECT_FALSE(p1.is_ready());

  copp::future::poller<void> p2(123);
  CASE_EXPECT_TRUE(p2.is_ready() && p2.data());
  CASE_EXPECT_EQ(p2.data(), &p2.raw_ptr());

  std::unique_ptr<bool> param3 = std::unique_ptr<bool>(new bool(false));
  copp::future::poller<void> p3(std::move(param3));
  CASE_EXPECT_TRUE(p3.is_ready() && p3.data());
  CASE_EXPECT_EQ(p3.data(), &p3.raw_ptr());

  copp::future::poller<void> p4;
  // set ready
  p4 = true;
  CASE_EXPECT_TRUE(p4.is_ready() && p4.data());
  CASE_EXPECT_EQ(p4.data(), &p4.raw_ptr());
}

CASE_TEST(future, poll_trival) {
  copp::future::poller<int> p1;
  CASE_EXPECT_FALSE(p1.is_ready());

  copp::future::poller<int> p2(123);
  CASE_EXPECT_TRUE(p2.is_ready() && p2.data());
  CASE_EXPECT_EQ(p2.data() ? *p2.data() : 0, 123);

  std::unique_ptr<int> param3 = std::unique_ptr<int>(new int(234));
  copp::future::poller<int> p3(std::move(param3));
  CASE_EXPECT_TRUE(p3.is_ready() && p3.data());
  CASE_EXPECT_EQ(p3.data() ? *p3.data() : 0, 234);
}

CASE_TEST(future, poll_no_trivial) {
  typedef copp::future::poller<test_no_trivial_parent_clazz> test_poll_type;

  test_poll_type p1;
  CASE_EXPECT_FALSE(p1.is_ready());

  test_poll_type p2(std::unique_ptr<test_no_trivial_parent_clazz>(new test_no_trivial_parent_clazz(123)));
  CASE_EXPECT_TRUE(p2.is_ready() && p2.data());
  CASE_EXPECT_EQ(p2.data() ? p2.data()->data : 0, 123);

  test_poll_type p3(std::unique_ptr<test_no_trivial_child_clazz>(new test_no_trivial_child_clazz(234)));
  CASE_EXPECT_EQ(p3.data() ? p3.data()->data : 0, -234);
}

CASE_TEST(future, poll_shared_ptr) {
  typedef copp::future::poller<test_no_trivial_parent_clazz, std::shared_ptr<test_no_trivial_parent_clazz> >
      test_poll_type;

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
    copp::future::poller<void> p1;
    copp::future::poller<void> p2;

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
    copp::future::poller<int> p1;
    copp::future::poller<int> p2;

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
    copp::future::poller<test_no_trivial_parent_clazz> p1;
    copp::future::poller<test_no_trivial_parent_clazz> p2;

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
    copp::future::result_type<test_no_trivial_parent_clazz, int> p1 =
        copp::future::result_type<test_no_trivial_parent_clazz, int>::create_success(123);
    copp::future::result_type<test_no_trivial_parent_clazz, int> p2 =
        copp::future::result_type<test_no_trivial_parent_clazz, int>::create_error(456);

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
    copp::future::result_type<int, int> p1 = copp::future::result_type<int, int>::create_success(123);
    copp::future::result_type<int, int> p2 = copp::future::result_type<int, int>::create_error(456);

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

  static void on_destroy(copp::future::context<void> &ctx) {
    CASE_MSG_INFO() << "[Future] custom void poll context " << &ctx << "destroyed." << std::endl;
  }

  void operator()(copp::future::context<void> &ctx, copp::future::context<void>::poll_event_data evt) {
    CASE_MSG_INFO() << "[Future] custom void poll functor " << this << " polled by " << &ctx
                    << ". future: " << evt.future_ptr << std::endl;

    if (delay > 0) {
      --delay;
      return;
    }

    ctx.set_on_destroy(on_destroy);

    copp::future::future_with_waker<void> *fut =
        reinterpret_cast<copp::future::future_with_waker<void> *>(evt.future_ptr);

    fut->poll_data() = true;

    CASE_MSG_INFO() << "[Future] custom void poll functor " << this << " finished" << std::endl;
  }
};

template <class T>
struct test_future_void_context_poll_functor {
  T data;
  int32_t delay;

  template <class U>
  test_future_void_context_poll_functor(int32_t d, U in) : data(in), delay(d) {}

  static void on_destroy(copp::future::context<void> &ctx) {
    CASE_MSG_INFO() << "[Future] custom void poll context " << &ctx << "destroyed." << std::endl;
  }

  void operator()(copp::future::context<void> &ctx, copp::future::context<void>::poll_event_data evt) {
    CASE_MSG_INFO() << "[Future] custom poll functor " << this << " polled by " << &ctx
                    << ". future: " << evt.future_ptr << std::endl;

    copp::future::future_with_waker<T> *fut = reinterpret_cast<copp::future::future_with_waker<T> *>(evt.future_ptr);

    if (delay > 0) {
      --delay;
      return;
    }

    ctx.set_on_destroy(on_destroy);

    T *r = reinterpret_cast<T *>(evt.private_data);
    if (nullptr == r) {
      fut->poll_data() = copp::future::poller<T>(data);
    } else {
      fut->poll_data() = copp::future::poller<T>(*r);
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
  T data;
  int32_t delay;
  copp::future::context<self_type> *last_trigger;

  template <class U, class UPTR>
  void operator()(copp::future::future_with_waker<U, UPTR> &fut, copp::future::context<self_type> &ctx) {
    last_trigger = &ctx;

    CASE_MSG_INFO() << "[Future] custom " << test_future_trivial_name<std::is_trivial<self_type>::value>() << " poller "
                    << this << " polled by context " << &ctx << ". future_with_waker: " << &fut << std::endl;
    if (delay > 0) {
      --delay;
      return;
    }

    fut.reset_data(copp::future::poller<U, UPTR>(std::move(data)));
    CASE_MSG_INFO() << "[Future] custom poller " << this << " finished" << std::endl;
  }
};

CASE_TEST(future, future_with_void_result_and_void_context) {
  copp::future::future_with_waker<void> fut;

  CASE_EXPECT_FALSE(fut.is_ready());
  CASE_EXPECT_TRUE(fut.is_pending());
  CASE_EXPECT_EQ(nullptr, fut.data());
  CASE_EXPECT_EQ(nullptr, fut.raw_ptr().get());

  copp::future::context<void> ctx(
      copp::future::context<void>::construct(test_future_void_context_poll_functor<void>(1), nullptr));
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
  copp::future::future_with_waker<int32_t> fut;
  int32_t simulator_result = 12345678;

  CASE_EXPECT_FALSE(fut.is_ready());
  CASE_EXPECT_TRUE(fut.is_pending());
  CASE_EXPECT_EQ(nullptr, fut.data());
  CASE_EXPECT_EQ(nullptr, fut.raw_ptr().get());

  copp::future::context<void> ctx(
      copp::future::context<void>::construct(test_future_void_context_poll_functor<int32_t>(1, 0), &simulator_result));
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
  static_assert(std::is_trivial<custom_poller_t>::value, "custom_poller_t must be trivial");

  copp::future::future_with_waker<int32_t> fut;
  int32_t simulator_result = 12345679;

  CASE_EXPECT_FALSE(fut.is_ready());
  CASE_EXPECT_TRUE(fut.is_pending());
  CASE_EXPECT_EQ(nullptr, fut.data());
  CASE_EXPECT_EQ(nullptr, fut.raw_ptr().get());

  copp::future::context<custom_poller_t> ctx;
  ctx.get_private_data().delay = 1;
  ctx.get_private_data().data = simulator_result;

  fut.poll(ctx);
  CASE_EXPECT_FALSE(fut.is_ready());
  CASE_EXPECT_TRUE(fut.is_pending());

  // After first poll, ctx is binded to future
  CASE_EXPECT_TRUE(!!ctx.get_wake_fn());
  CASE_EXPECT_NE(nullptr, ctx.get_private_data().last_trigger);
  // When jobs finished, call wake to poll again

  ctx.wake();

  CASE_EXPECT_TRUE(fut.is_ready());
  CASE_EXPECT_FALSE(fut.is_pending());

  CASE_EXPECT_EQ(simulator_result, *fut.data());

  ctx.wake();
}

CASE_TEST(future, future_and_custom_poller_context_no_trivial) {
  typedef test_future_custom_poller_for_context<std::unique_ptr<test_no_trivial_child_clazz> > custom_poller_t;
  static_assert(!std::is_trivial<custom_poller_t>::value, "custom_poller_t must not be trivial");

  copp::future::future_with_waker<test_no_trivial_child_clazz> fut;
  int32_t simulator_result = 12345680;

  CASE_EXPECT_FALSE(fut.is_ready());
  CASE_EXPECT_TRUE(fut.is_pending());
  CASE_EXPECT_EQ(nullptr, fut.data());
  CASE_EXPECT_EQ(nullptr, fut.raw_ptr().get());

  copp::future::context<custom_poller_t> ctx;
  ctx.get_private_data().delay = 1;

  fut.poll(ctx);
  CASE_EXPECT_FALSE(fut.is_ready());
  CASE_EXPECT_TRUE(fut.is_pending());

  // After first poll, ctx is binded to future
  CASE_EXPECT_TRUE(!!ctx.get_wake_fn());
  CASE_EXPECT_NE(nullptr, ctx.get_private_data().last_trigger);
  // When jobs finished, call wake to poll again

  ctx.get_private_data().data.reset(new test_no_trivial_child_clazz(simulator_result));
  ctx.wake();

  CASE_EXPECT_TRUE(fut.is_ready());
  CASE_EXPECT_FALSE(fut.is_pending());
  // already moved into fut
  CASE_EXPECT_EQ(nullptr, ctx.get_private_data().data.get());

  CASE_EXPECT_EQ(-simulator_result, fut.data()->data);

  ctx.wake();
}

CASE_TEST(future, future_with_copp_result_and_custom_poller_context_trivial) {
  typedef copp::future::result_type<int, long> result_type;
  typedef test_future_custom_poller_for_context<result_type> custom_poller_t;
  static_assert(std::is_trivial<custom_poller_t>::value, "custom_poller_t must be trivial");

  copp::future::future_with_waker<result_type> fut;
  int simulator_result = 12345681;

  CASE_EXPECT_FALSE(fut.is_ready());
  CASE_EXPECT_TRUE(fut.is_pending());
  CASE_EXPECT_EQ(nullptr, fut.data());
  CASE_EXPECT_EQ(nullptr, fut.raw_ptr().get());

  copp::future::context<custom_poller_t> ctx;
  ctx.get_private_data().delay = 1;
  ctx.get_private_data().data = result_type::create_success(simulator_result);

  fut.poll(ctx);
  CASE_EXPECT_FALSE(fut.is_ready());
  CASE_EXPECT_TRUE(fut.is_pending());

  // After first poll, ctx is binded to future
  CASE_EXPECT_TRUE(!!ctx.get_wake_fn());
  CASE_EXPECT_NE(nullptr, ctx.get_private_data().last_trigger);
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
  typedef copp::future::result_type<int, test_no_trivial_child_clazz> result_type;
  typedef test_future_custom_poller_for_context<std::unique_ptr<result_type> > custom_poller_t;
  static_assert(!std::is_trivial<custom_poller_t>::value, "custom_poller_t must not be trivial");

  copp::future::future_with_waker<result_type> fut;
  int simulator_result = 12345682;

  CASE_EXPECT_FALSE(fut.is_ready());
  CASE_EXPECT_TRUE(fut.is_pending());
  CASE_EXPECT_EQ(nullptr, fut.data());
  CASE_EXPECT_EQ(nullptr, fut.raw_ptr().get());

  copp::future::context<custom_poller_t> ctx;
  ctx.get_private_data().delay = 1;

  fut.poll(ctx);
  CASE_EXPECT_FALSE(fut.is_ready());
  CASE_EXPECT_TRUE(fut.is_pending());

  // After first poll, ctx is binded to future
  CASE_EXPECT_TRUE(!!ctx.get_wake_fn());
  CASE_EXPECT_NE(nullptr, ctx.get_private_data().last_trigger);
  // When jobs finished, call wake to poll again

  ctx.get_private_data().data.reset(
      new result_type(result_type::create_error(test_no_trivial_child_clazz(simulator_result))));
  ctx.wake();

  CASE_EXPECT_TRUE(fut.is_ready());
  CASE_EXPECT_FALSE(fut.is_pending());
  // already moved into fut
  CASE_EXPECT_FALSE(fut.data()->is_success());
  CASE_EXPECT_TRUE(fut.data()->is_error());
  CASE_EXPECT_EQ(nullptr, ctx.get_private_data().data.get());

  CASE_EXPECT_EQ(-simulator_result, fut.data()->get_error()->data);

  ctx.wake();
}

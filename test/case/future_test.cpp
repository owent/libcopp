// Copyright 2023 owent

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

struct test_trivial_clazz {
  test_trivial_clazz() = default;
  test_trivial_clazz(int a) : data(a) {}
  test_trivial_clazz(int a, int) : data(a) {}

  int data;
};

#if (defined(__cplusplus) && __cplusplus >= 201402L) || ((defined(_MSVC_LANG) && _MSVC_LANG >= 201402L))
static_assert(std::is_trivially_constructible<test_trivial_clazz>::value &&
                  std::is_trivially_copyable<test_trivial_clazz>::value,
              "trivial check");
#elif (defined(__cplusplus) && __cplusplus >= 201103L) || ((defined(_MSVC_LANG) && _MSVC_LANG >= 201103L))
static_assert(std::is_trivial<test_trivial_clazz>::value, "trivial check");
#endif

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
  copp::future::poller<test_trivial_clazz> p1;
  CASE_EXPECT_FALSE(p1.is_ready());

  copp::future::poller<test_trivial_clazz> p2(123);
  CASE_EXPECT_TRUE(p2.is_ready() && p2.data());
  CASE_EXPECT_EQ(p2.data() ? p2.data()->data : 0, 123);

  std::unique_ptr<test_trivial_clazz> param3 = std::unique_ptr<test_trivial_clazz>(new test_trivial_clazz(234));
  copp::future::poller<test_trivial_clazz> p3(std::move(param3));
  CASE_EXPECT_TRUE(p3.is_ready() && p3.data());
  CASE_EXPECT_EQ(p3.data() ? p3.data()->data : 0, 234);

  copp::future::poller<test_trivial_clazz> p4(test_trivial_clazz(345));
  CASE_EXPECT_TRUE(p4.is_ready() && p4.data());
  CASE_EXPECT_EQ(p4.data() ? p4.data()->data : 0, 345);

  copp::future::poller<test_trivial_clazz> p5(456, 456);
  CASE_EXPECT_TRUE(p5.is_ready() && p5.data());
  CASE_EXPECT_EQ(p5.data() ? p5.data()->data : 0, 456);
}

CASE_TEST(future, poll_trival_object) {
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

CASE_TEST(future, future_with_void_result) {
  copp::future::future<void> fut;

  CASE_EXPECT_FALSE(fut.is_ready());
  CASE_EXPECT_TRUE(fut.is_pending());
  CASE_EXPECT_EQ(nullptr, fut.data());
  CASE_EXPECT_EQ(nullptr, fut.raw_ptr().get());

  fut.reset_data(true);
  CASE_EXPECT_TRUE(fut.is_ready());
  CASE_EXPECT_FALSE(fut.is_pending());
}

CASE_TEST(future, future_with_trival_result) {
  copp::future::future<int32_t> fut;
  int32_t simulator_result = 12345678;

  CASE_EXPECT_FALSE(fut.is_ready());
  CASE_EXPECT_TRUE(fut.is_pending());
  CASE_EXPECT_EQ(nullptr, fut.data());
  CASE_EXPECT_EQ(nullptr, fut.raw_ptr().get());

  fut.reset_data(simulator_result);
  CASE_EXPECT_TRUE(fut.is_ready());
  CASE_EXPECT_FALSE(fut.is_pending());

  CASE_EXPECT_EQ(simulator_result, *fut.data());
}

CASE_TEST(future, future_with_no_trival_result) {
  copp::future::future<std::string> fut;

  CASE_EXPECT_FALSE(fut.is_ready());
  CASE_EXPECT_TRUE(fut.is_pending());
  CASE_EXPECT_EQ(nullptr, fut.data());
  CASE_EXPECT_EQ(nullptr, fut.raw_ptr().get());

  fut.reset_data("hello");
  CASE_EXPECT_TRUE(fut.is_ready());
  CASE_EXPECT_FALSE(fut.is_pending());

  CASE_EXPECT_EQ("hello", *fut.data());
}

CASE_TEST(future, future_with_copp_trivial_result) {
  using result_type = copp::future::result_type<int, long>;
  static_assert(COPP_IS_TIRVIALLY_COPYABLE_V(result_type), "result_type<int, long> must be trivial");

  copp::future::future<result_type> fut;

  CASE_EXPECT_FALSE(fut.is_ready());
  CASE_EXPECT_TRUE(fut.is_pending());
  CASE_EXPECT_EQ(nullptr, fut.data());
  CASE_EXPECT_EQ(nullptr, fut.raw_ptr().get());

  fut.reset_data(result_type::make_success(171));

  CASE_EXPECT_TRUE(fut.data()->is_success());
  CASE_EXPECT_FALSE(fut.data()->is_error());
  CASE_EXPECT_EQ(171, *fut.data()->get_success());
}

CASE_TEST(future, future_with_copp_no_trivial_result) {
  using result_type = copp::future::result_type<int, std::string>;
  static_assert(!COPP_IS_TIRVIALLY_COPYABLE_V(result_type), "result_type<int, std::string> must not be trivial");

  copp::future::future<result_type> fut;

  CASE_EXPECT_FALSE(fut.is_ready());
  CASE_EXPECT_TRUE(fut.is_pending());
  CASE_EXPECT_EQ(nullptr, fut.data());
  CASE_EXPECT_EQ(nullptr, fut.raw_ptr().get());

  fut.reset_data(result_type::make_error("hello"));

  CASE_EXPECT_TRUE(fut.is_ready());
  CASE_EXPECT_FALSE(fut.is_pending());
  // already moved into fut
  CASE_EXPECT_FALSE(fut.data()->is_success());
  CASE_EXPECT_TRUE(fut.data()->is_error());

  CASE_EXPECT_EQ("hello", *fut.data()->get_error());
}

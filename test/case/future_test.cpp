#include <cstdio>
#include <cstring>
#include <iostream>

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

CASE_TEST(future, pool_trival) {
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

CASE_TEST(future, pool_no_trivial) {
    typedef copp::future::poll_t<test_no_trivial_parent_clazz> test_poll_type;

    test_poll_type p1;
    CASE_EXPECT_FALSE(p1.is_ready());

    test_poll_type p2(std::unique_ptr<test_no_trivial_parent_clazz>(new test_no_trivial_parent_clazz(123)));
    CASE_EXPECT_TRUE(p2.is_ready() && p2.data());
    CASE_EXPECT_EQ(p2.data() ? p2.data()->data : 0, 123);

    test_poll_type p3(std::unique_ptr<test_no_trivial_child_clazz>(new test_no_trivial_child_clazz(234)));
    CASE_EXPECT_EQ(p3.data() ? p3.data()->data : 0, -234);
}

CASE_TEST(future, pool_shared_ptr) {
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

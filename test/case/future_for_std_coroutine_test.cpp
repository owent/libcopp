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

// ================= Unit Test - C++20 Coroutine Support =================
#if defined(LIBCOPP_MACRO_ENABLE_STD_COROUTINE) && LIBCOPP_MACRO_ENABLE_STD_COROUTINE
CASE_TEST(future_for_std_coroutine, poll_trival) {
    // auto test_var   = std::is_trivial<void>::value;
    // auto test_varsz = sizeof(void);

    // CASE_MSG_INFO() << test_var << std::endl;
    // CASE_MSG_INFO() << test_var << std::endl;
}
#endif

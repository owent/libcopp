// Copyright 2022 owent

// #include <libcotask/task_promise.h>

#include <cstdio>
#include <cstring>
#include <iostream>
#include <list>
#include <string>

#include "frame/test_macros.h"

#if defined(LIBCOPP_MACRO_ENABLE_STD_COROUTINE) && LIBCOPP_MACRO_ENABLE_STD_COROUTINE
CASE_TEST(task_promise, todo) {}
#else
CASE_TEST(task_promise, disabled) {}
#endif

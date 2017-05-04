/*
 * test_macros.h
 *
 *  Created on: 2014年3月11日
 *      Author: owent
 *
 *  Released under the MIT license
 */

#ifndef TEST_MACROS_H_
#define TEST_MACROS_H_

#include <iostream>
#include <sstream>
#include <cstdio>

#ifdef _MSC_VER
#include <Windows.h>
#endif

#ifdef UTILS_TEST_MACRO_TEST_ENABLE_GTEST
#include "gtest/gtest.h"

#define CASE_TEST(test_name, case_name) TEST(test_name, case_name)

#define CASE_EXPECT_TRUE(c) EXPECT_TRUE(c)
#define CASE_EXPECT_FALSE(c) EXPECT_FALSE(c)
#define CASE_EXPECT_EQ(l, r) EXPECT_EQ(l, r)
#define CASE_EXPECT_NE(l, r) EXPECT_NE(l, r)
#define CASE_EXPECT_LT(l, r) EXPECT_LT(l, r)
#define CASE_EXPECT_LE(l, r) EXPECT_LE(l, r)
#define CASE_EXPECT_GT(l, r) EXPECT_GT(l, r)
#define CASE_EXPECT_GE(l, r) EXPECT_GE(l, r)

#else
#include "test_manager.h"

#define test_case_func_name(test_name, case_name) test_func_test_##test_name##_case_##case_name
#define test_case_obj_name(test_name, case_name) test_obj_test_##test_name##_case_##case_name##_obj

#define CASE_TEST(test_name, case_name) \
static void test_case_func_name(test_name, case_name) (); \
static test_case_base test_case_obj_name(test_name, case_name) (#test_name, #case_name, test_case_func_name(test_name, case_name)); \
void test_case_func_name(test_name, case_name) ()


#ifdef UTILS_TEST_MACRO_TEST_ENABLE_BOOST_TEST
    #define CASE_EXPECT_TRUE(c)  BOOST_CHECK(c)
    #define CASE_EXPECT_FALSE(c) BOOST_CHECK(!(c))
    #define CASE_EXPECT_EQ(l, r) BOOST_CHECK_EQUAL(l, r)
    #define CASE_EXPECT_NE(l, r) BOOST_CHECK_NE(l, r)
    #define CASE_EXPECT_LT(l, r) BOOST_CHECK_LT(l, r)
    #define CASE_EXPECT_LE(l, r) BOOST_CHECK_LE(l, r)
    #define CASE_EXPECT_GT(l, r) BOOST_CHECK_GT(l, r)
    #define CASE_EXPECT_GE(l, r) BOOST_CHECK_GE(l, r)

#else
    #define CASE_EXPECT_TRUE(c) test_manager::me().expect_true((c), #c, __FILE__, __LINE__)
    #define CASE_EXPECT_FALSE(c) test_manager::me().expect_false((c), #c, __FILE__, __LINE__)
    #define CASE_EXPECT_EQ(l, r) test_manager::me().expect_eq((l), (r), #l, #r, __FILE__, __LINE__)
    #define CASE_EXPECT_NE(l, r) test_manager::me().expect_ne((l), (r), #l, #r, __FILE__, __LINE__)
    #define CASE_EXPECT_LT(l, r) test_manager::me().expect_lt((l), (r), #l, #r, __FILE__, __LINE__)
    #define CASE_EXPECT_LE(l, r) test_manager::me().expect_le((l), (r), #l, #r, __FILE__, __LINE__)
    #define CASE_EXPECT_GT(l, r) test_manager::me().expect_gt((l), (r), #l, #r, __FILE__, __LINE__)
    #define CASE_EXPECT_GE(l, r) test_manager::me().expect_ge((l), (r), #l, #r, __FILE__, __LINE__)

#endif

#endif

// 前景色: BLACK,RED,GREEN,YELLOW,BLUE,MAGENTA,CYAN,WHITE
#define CASE_MSG_FCOLOR(x) util::cli::shell_font_style::SHELL_FONT_COLOR_##x
// 背景色: BLACK,RED,GREEN,YELLOW,BLUE,MAGENTA,CYAN,WHITE
#define CASE_MSG_BCOLOR(x) util::cli::shell_font_style::SHELL_FONT_BACKGROUND_COLOR_##x
// 字体格式: BOLD,UNDERLINE,FLASH,DARK
#define CASE_MSG_STYLE(x) util::cli::shell_font_style::SHELL_FONT_SPEC_##x

#define CASE_MSG_INFO() util::cli::shell_stream(std::cout)()<< "[ RUNNING  ] "
#define CASE_MSG_ERROR() util::cli::shell_stream(std::cerr)()<< "[ RUNNING  ] "

// 测试中休眠
#if (defined(__cplusplus) && __cplusplus >= 201103L) || (defined(_MSC_VER) && _MSC_VER >= 1800)
    #include <thread>
    #define CASE_THREAD_SLEEP_MS(x) std::this_thread::sleep_for(std::chrono::milliseconds(x))
    #define CASE_THREAD_YIELD() std::this_thread::yield()

#elif  defined(_MSC_VER)
    #include <Windows.h>
    #define CASE_THREAD_SLEEP_MS(x) Sleep(x)
    #define CASE_THREAD_YIELD() YieldProcessor()

#else
    #include <unistd.h>

    #define CASE_THREAD_SLEEP_MS(x) ((x > 1000)? sleep(x / 1000): usleep(0)); usleep((x % 1000) * 1000)
    #if defined(__linux__) || defined(__unix__)
        #include <sched.h>
        #define CASE_THREAD_YIELD() sched_yield()
    #elif defined(__GNUC__) || defined(__clang__)
        #if defined(__i386__) || defined(__x86_64__)
            /**
            * See: Intel(R) 64 and IA-32 Architectures Software Developer's Manual V2
            * PAUSE-Spin Loop Hint, 4-57
            * http://www.intel.com/content/www/us/en/architecture-and-technology/64-ia-32-architectures-software-developer-instruction-set-reference-manual-325383.html?wapkw=instruction+set+reference
            */
            #define CASE_THREAD_YIELD() __asm__ __volatile__("pause")
        #elif defined(__ia64__) || defined(__ia64)
            /**
            * See: Intel(R) Itanium(R) Architecture Developer's Manual, Vol.3
            * hint - Performance Hint, 3:145
            * http://www.intel.com/content/www/us/en/processors/itanium/itanium-architecture-vol-3-manual.html
            */
            #define CASE_THREAD_YIELD() __asm__ __volatile__ ("hint @pause")
        #elif defined(__arm__) && !defined(__ANDROID__)
            /**
            * See: ARM Architecture Reference Manuals (YIELD)
            * http://infocenter.arm.com/help/index.jsp?topic=/com.arm.doc.subset.architecture.reference/index.html
            */
            #define CASE_THREAD_YIELD() __asm__ __volatile__ ("yield")
        #else
            #define CASE_THREAD_YIELD()
        #endif
    #else
        #define CASE_THREAD_YIELD()
    #endif

#endif

#endif /* TEST_MACROS_H_ */

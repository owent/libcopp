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
#include "shell_font.h"
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
    #define CASE_EXPECT_EXPR(expr) \
        if(expr){ \
            ++(*test_manager::me().success_counter_ptr); \
        } else { \
            ++(*test_manager::me().failed_counter_ptr);\
            shell_stream ss(std::cout); \
            ss() << ShekkFontStyle::SHELL_FONT_COLOR_RED<< "FAILED => " << __FILE__<< ":" << __LINE__<< std::endl << \
            "Expected: "<< #expr<< std::endl; \
        }

    #define CASE_EXPECT_TRUE(c) CASE_EXPECT_EXPR(c)
    #define CASE_EXPECT_FALSE(c) CASE_EXPECT_EXPR(!(c))
    #define CASE_EXPECT_EQ(l, r) CASE_EXPECT_EXPR((l) == (r))
    #define CASE_EXPECT_NE(l, r) CASE_EXPECT_EXPR((l) != (r))
    #define CASE_EXPECT_LT(l, r) CASE_EXPECT_EXPR((l) < (r))
    #define CASE_EXPECT_LE(l, r) CASE_EXPECT_EXPR((l) <= (r))
    #define CASE_EXPECT_GT(l, r) CASE_EXPECT_EXPR((l) > (r))
    #define CASE_EXPECT_GE(l, r) CASE_EXPECT_EXPR((l) >= (r))

#endif

#endif

#endif /* TEST_MACROS_H_ */

#include <iostream>
#include <cstdio>
#include <cstring>

#include "libcopp/fcontext/all.hpp"
#include "frame/test_macros.h"


copp::fcontext::fcontext_t test_core_fcontext_main_func, test_core_fcontext_a_func, test_core_fcontext_b_func;
char test_core_fcontext_stack_a[128 * 1024] = { 0 };
char test_core_fcontext_stack_b[128 * 1024] = { 0 };
int g_test_core_fcontext_status = 0;

void test_core_fcontext_func_a(::copp::fcontext::transfer_t src)
{
    test_core_fcontext_main_func = src.fctx;

    copp::fcontext::fcontext_t* fc = reinterpret_cast<copp::fcontext::fcontext_t*>(src.data);

    ++ g_test_core_fcontext_status;
    CASE_EXPECT_EQ(g_test_core_fcontext_status, 2);
    CASE_EXPECT_EQ(fc, &test_core_fcontext_a_func);
    
    src = copp::fcontext::copp_jump_fcontext(test_core_fcontext_b_func, &test_core_fcontext_b_func);
    fc = reinterpret_cast<copp::fcontext::fcontext_t*>(src.data);
    CASE_EXPECT_EQ(fc, &test_core_fcontext_b_func);
    test_core_fcontext_b_func = src.fctx;

    ++ g_test_core_fcontext_status;
    CASE_EXPECT_EQ(g_test_core_fcontext_status, 4);

    copp::fcontext::copp_jump_fcontext(test_core_fcontext_main_func, &test_core_fcontext_main_func);

}

void test_core_fcontext_func_b(::copp::fcontext::transfer_t src)
{
    ++ g_test_core_fcontext_status;
    CASE_EXPECT_EQ(g_test_core_fcontext_status, 3);

    copp::fcontext::fcontext_t* fc = reinterpret_cast<copp::fcontext::fcontext_t*>(src.data);
    CASE_EXPECT_EQ(fc, &test_core_fcontext_b_func);

    test_core_fcontext_a_func = src.fctx;
    copp::fcontext::copp_jump_fcontext(test_core_fcontext_a_func, fc);
}

CASE_TEST(core, fcontext)
{
    ++ g_test_core_fcontext_status;
    CASE_EXPECT_EQ(g_test_core_fcontext_status, 1);

    test_core_fcontext_a_func = copp::fcontext::copp_make_fcontext(test_core_fcontext_stack_a + sizeof(test_core_fcontext_stack_a), sizeof(test_core_fcontext_stack_a), test_core_fcontext_func_a);
    test_core_fcontext_b_func = copp::fcontext::copp_make_fcontext(test_core_fcontext_stack_b + sizeof(test_core_fcontext_stack_b), sizeof(test_core_fcontext_stack_b), test_core_fcontext_func_b);

    copp::fcontext::copp_jump_fcontext(test_core_fcontext_a_func, &test_core_fcontext_a_func);

    ++ g_test_core_fcontext_status;
    CASE_EXPECT_EQ(g_test_core_fcontext_status, 5);
}

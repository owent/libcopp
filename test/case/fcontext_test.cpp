#include <iostream>
#include <cstdio>
#include <cstring>

#include "libcopp/fcontext/all.hpp"
#include "frame/test_macros.h"


copp::fcontext::fcontext_t test_core_fcontext_main_func, test_core_fcontext_a_func, test_core_fcontext_b_func;
char test_core_fcontext_stack_a[64 * 1024] = { 0 };
char test_core_fcontext_stack_b[64 * 1024] = { 0 };
int g_test_core_fcontext_status = 0;

void test_core_fcontext_func_a(intptr_t p)
{
    copp::fcontext::fcontext_t* fc = (copp::fcontext::fcontext_t*)p;

    ++ g_test_core_fcontext_status;
    CASE_EXPECT_EQ(g_test_core_fcontext_status, 2);
    
    copp::fcontext::copp_jump_fcontext(fc, test_core_fcontext_b_func, (intptr_t) &test_core_fcontext_b_func, true);

    ++ g_test_core_fcontext_status;
    CASE_EXPECT_EQ(g_test_core_fcontext_status, 4);

    copp::fcontext::copp_jump_fcontext(fc, test_core_fcontext_main_func, (intptr_t) &test_core_fcontext_main_func, true);

}

void test_core_fcontext_func_b(intptr_t p)
{
    ++ g_test_core_fcontext_status;
    CASE_EXPECT_EQ(g_test_core_fcontext_status, 3);

    copp::fcontext::fcontext_t* fc = (copp::fcontext::fcontext_t*)p;

    copp::fcontext::copp_jump_fcontext(fc, test_core_fcontext_a_func, (intptr_t) fc, true);
}

CASE_TEST(core, fcontext)
{
    ++ g_test_core_fcontext_status;
    CASE_EXPECT_EQ(g_test_core_fcontext_status, 1);

    test_core_fcontext_a_func = copp::fcontext::copp_make_fcontext(test_core_fcontext_stack_a + sizeof(test_core_fcontext_stack_a), sizeof(test_core_fcontext_stack_a), test_core_fcontext_func_a);
    test_core_fcontext_b_func = copp::fcontext::copp_make_fcontext(test_core_fcontext_stack_b + sizeof(test_core_fcontext_stack_b), sizeof(test_core_fcontext_stack_b), test_core_fcontext_func_b);

    copp::fcontext::copp_jump_fcontext(&test_core_fcontext_main_func, test_core_fcontext_a_func, (intptr_t)&test_core_fcontext_a_func, true);

    ++ g_test_core_fcontext_status;
    CASE_EXPECT_EQ(g_test_core_fcontext_status, 5);
}

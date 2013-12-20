#include <iostream>
#include <cstdio>
#include <cstring>

#include "libcopp/fcontext/all.hpp"

copp::fcontext::fcontext_t main_func, *a_func, *b_func;
char stack_a[64 * 1024] = { 0 };
char stack_b[64 * 1024] = { 0 };

void func_a(intptr_t p)
{
    copp::fcontext::fcontext_t* fc = (copp::fcontext::fcontext_t*)p;

    puts("func_a");

    
    copp::fcontext::copp_jump_fcontext(fc, b_func, (intptr_t) b_func, true);
    printf("%d\n", __LINE__);
    copp::fcontext::copp_jump_fcontext(fc, &main_func, (intptr_t) &main_func, true);

    printf("%d\n", __LINE__);
}

void func_b(intptr_t p)
{
    puts("func_b");

    copp::fcontext::fcontext_t* fc = (copp::fcontext::fcontext_t*)p;

    copp::fcontext::copp_jump_fcontext(fc, a_func, (intptr_t) fc, true);
}

int main() {
    printf("%d\n", __LINE__);

    a_func = copp::fcontext::copp_make_fcontext(stack_a + sizeof(stack_a), sizeof(stack_a), func_a);
    b_func = copp::fcontext::copp_make_fcontext(stack_b + sizeof(stack_b), sizeof(stack_b), func_b);

    printf("%d\n", __LINE__);

    copp::fcontext::copp_jump_fcontext(&main_func, a_func, (intptr_t)a_func, true);

    printf("%d\n", __LINE__);
    return 0;
}

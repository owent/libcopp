#include <iostream>
#include <cstdio>
#include <cstring>

#include <libcopp/coroutine/coroutine_context_container.h>


#define CHECK_STATUS(x, y) \
    (x) == (y) ?  \
    printf("check %s == %s: PASS\n", #x, #y) : \
    printf("check %s == %s: FAILED!!!!!!!!!!\n", #x, #y)

typedef copp::detail::coroutine_context_container<
    copp::detail::coroutine_context_safe_base,
    copp::allocator::default_statck_allocator
    > test_context_safe_base_context_type;

int g_status = 0;

class test_context_safe_base_foo_runner : public copp::coroutine_runnable_base
{
public:
    int operator()()
    {
        puts("co run");
        CHECK_STATUS(++g_status, 2);

        get_coroutine_context<test_context_safe_base_context_type>()->resume();
        puts("co restart.");
        CHECK_STATUS(++g_status, 3);

        get_coroutine_context<test_context_safe_base_context_type>()->yield();
        puts("co resumed.");
        CHECK_STATUS(++g_status, 5);

        return 0;
    }
};

int main() {
    puts("co create.");
    CHECK_STATUS(++g_status, 1);

    test_context_safe_base_context_type co;
    test_context_safe_base_foo_runner runner;
    co.create(&runner);
    co.start();

    puts("co yield.");
    CHECK_STATUS(++g_status, 4);
    co.resume();

    puts("co done.");
    CHECK_STATUS(++g_status, 6);
    return 0;
}

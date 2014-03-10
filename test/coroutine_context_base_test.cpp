#include <iostream>
#include <cstdio>
#include <cstring>

#include <libcopp/coroutine/coroutine_context_container.h>


#define CHECK_STATUS(x, y) \
    (x) == (y) ? \
    printf("check %s == %s: PASS\n", #x, #y) : \
    printf("check %s == %s: FAILED!!!!!!!!!!\n", #x, #y)

typedef copp::detail::coroutine_context_container<
    copp::detail::coroutine_context_base,
    copp::allocator::stack_allocator_memory
> test_context_base_coroutine_context_test_type;

int g_status = 0;

class test_context_base_foo_runner : public copp::coroutine_runnable_base
{
public:
    int operator()() {
        puts("co start.");
        CHECK_STATUS(++g_status, 2);

        get_coroutine_context<test_context_base_coroutine_context_test_type>()->yield();

        puts("co resumed.");
        CHECK_STATUS(++g_status, 4);

        return 0;
    }
};

char stack_buff[64 * 1024] = {0};

int main() {
    puts("co create.");
    CHECK_STATUS(++g_status, 1);

    test_context_base_coroutine_context_test_type co;
    test_context_base_foo_runner runner;
    co.get_allocator().attach(stack_buff, sizeof(stack_buff));
    co.create(&runner, sizeof(stack_buff));
    co.start();

    puts("co yield.");
    CHECK_STATUS(++g_status, 3);
    co.resume();

    puts("co done.");
    CHECK_STATUS(++g_status, 5);
    return 0;
}

#include <iostream>
#include <cstdio>
#include <cstring>

#include <libcopp/coroutine_context/coroutine_context_container.h>


#define CHECK_STATUS(x, y) \
    (x) == (y) ? \
    printf("check %s == %s: PASS\n", #x, #y) : \
    printf("check %s == %s: FAILED!!!!!!!!!!\n", #x, #y)

typedef copp::detail::coroutine_context_container<
    copp::detail::coroutine_context_base,
    copp::allocator::stack_allocator_memory
> coroutine_context_test_base_type;

int g_status = 0;

class foo_coroutine_context : public coroutine_context_test_base_type
{
public:
    virtual void run()
    {
        puts("co start.");
        CHECK_STATUS(++g_status, 2);

        yield();

        puts("co resumed.");
        CHECK_STATUS(++g_status, 4);
    }
};

char stack_buff[64 * 1024] = {0};

int main() {
    puts("co create.");
    CHECK_STATUS(++g_status, 1);

    foo_coroutine_context co;
    co.get_allocator().attach(stack_buff, sizeof(stack_buff));
    co.create(sizeof(stack_buff));
    co.start();

    puts("co yield.");
    CHECK_STATUS(++g_status, 3);
    co.resume();

    puts("co done.");
    CHECK_STATUS(++g_status, 5);
    return 0;
}

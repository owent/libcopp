#include <iostream>
#include <cstdio>
#include <cstring>

#include <libcopp/coroutine_context/coroutine_context_container.h>


#define CHECK_STATUS(x, y) \
    (x) == (y) ?  \
    printf("check %s == %s: PASS\n", #x, #y) : \
    printf("check %s == %s: FAILED!!!!!!!!!!\n", #x, #y)

typedef copp::detail::coroutine_context_container<
    copp::detail::coroutine_context_safe_base,
    copp::allocator::default_statck_allocator
    > coroutine_context_test_base_type;

int g_status = 0;

class foo_coroutine_context : public coroutine_context_test_base_type
{
private:
    virtual void run() {
        puts("co run");
        CHECK_STATUS(++g_status, 2);

        resume();
        puts("co restart.");
        CHECK_STATUS(++g_status, 3);

        yield();
        puts("co resumed.");
        CHECK_STATUS(++g_status, 5);
    }

public:
};

int main() {
    puts("co create.");
    CHECK_STATUS(++g_status, 1);

    foo_coroutine_context co;
    co.create();
    co.start();

    puts("co yield.");
    CHECK_STATUS(++g_status, 4);
    co.resume();

    puts("co done.");
    CHECK_STATUS(++g_status, 6);
    return 0;
}

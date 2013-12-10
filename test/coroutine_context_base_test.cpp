#include <iostream>
#include <cstdio>
#include <cstring>

#include <libcopp/coroutine_context/coroutine_context_base.h>

class foo_coroutine_context : public copp::detail::coroutine_context_base
{
public:
    virtual void run()
    {
        printf("enter %s.\n", __FUNCTION__);
        yield();

        puts("co resumed.");
    }
};

char stack_buff[64 * 1024] = {0};

int main() {
    puts("co create.");

    foo_coroutine_context co;
    co.create(stack_buff + sizeof(stack_buff), sizeof(stack_buff));
    co.start();

    puts("co yield.");
    co.resume();

    puts("co done.");
    return 0;
}

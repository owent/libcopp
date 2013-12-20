#include <iostream>
#include <cstdio>
#include <cstring>

#include <libcopp/coroutine_context/coroutine_context_safe_base.h>
#include <libcopp/stack_context/stack_allocator.h>

class foo_coroutine_context : public copp::detail::coroutine_context_safe_base
{
private:
    copp::allocator::default_statck_allocator alloc_;

    virtual void run() {
        puts("co run");

        resume();
        puts("co restart.");

        yield();
        puts("co resumed.");
    }

public:
    int create() {
        alloc_.allocate(callee_stack_, alloc_.default_stacksize());
        copp::detail::coroutine_context_safe_base::create();
        return 0; 
    }
};

int main() {
    puts("co create.");

    foo_coroutine_context co;
    co.create();
    co.start();

    puts("co yield.");
    co.resume();

    puts("co done.");
    return 0;
}

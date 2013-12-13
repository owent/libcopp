#include <iostream>
#include <cstdio>
#include <cstring>

#include <libcopp/coroutine_context/coroutine_context_base.h>
#include <libcopp/stack_context/allocator/stack_allocator_split_segment.h>

class foo_coroutine_context : public copp::detail::coroutine_context_base
{
private:
    copp::allocator::stack_allocator_split_segment alloc_;

private:
    void stack_test(int loop) {
        char a;
        char b[1024 * 1024];
        char c;
        printf("addr rela: %lld, sizeof(b): %d\n", (long long)(&c) - (long long)(&a), (int)(sizeof(b)));
        if (loop > 0)
            stack_test(loop - 1);
    }

public:

    virtual void run() {
        printf("enter %s.\n", __FUNCTION__);

        stack_test(4);

        yield();

        stack_test(20);
        puts("co resumed.");
    }

    int create(std::size_t size) {
        alloc_.allocate(callee_stack_, size);
        copp::detail::coroutine_context_base::create();        
        return 0; 
    }
};

int main() {
    puts("co create.");

    foo_coroutine_context co;
    co.create(2 * 1024 * 1024);
    co.start();

    puts("co yield.");
    co.resume();

    puts("co done.");
    return 0;
}

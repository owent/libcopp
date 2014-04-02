/*
 * sample_default_manager.cpp
 *
 *  Created on: 2014年3月17日
 *      Author: owent
 *
 *  Released under the MIT license
 */


#include <cstdio>
#include <cstring>
#include <inttypes.h>
#include <stdint.h>

// include manager header file
#include <libcopp/coroutine/coroutine_context_container.h>

// define a coroutine runner
class my_runner : public copp::detail::coroutine_runnable_base
{
public:
    int operator()() {
        // ... your code here ...
        copp::coroutine_context_default* addr = get_coroutine_context<copp::coroutine_context_default>();
        printf("cortoutine %" PRIxPTR " is running.\n", (intptr_t)addr);

        addr->yield();
        printf("cortoutine %" PRIxPTR " is resumed.\n", (intptr_t)addr);

        return 1;
    }
};

int main() {
    // create a coroutine manager
    copp::coroutine_context_default co_obj;
    printf("cortoutine %" PRIxPTR " created.\n", (intptr_t)&co_obj);

    // create a runner
    my_runner runner;

    // bind runner to coroutine object
    co_obj.create(&runner);

    // start a coroutine
    co_obj.start();

    // yield from runner
    printf("cortoutine %" PRIxPTR " is yield.\n", (intptr_t)&co_obj);
    co_obj.resume();

    printf("cortoutine %" PRIxPTR " exit and return %d.\n", (intptr_t)&co_obj, co_obj.get_ret_code());
    return 0;
}


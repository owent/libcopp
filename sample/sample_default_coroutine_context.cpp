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
#include <iostream>
#include <stdint.h>

// include manager header file
#include <libcopp/coroutine/coroutine_context_container.h>

// define a coroutine runner
class my_runner : public copp::detail::coroutine_runnable_base {
public:
    int operator()() {
        // ... your code here ...printf("cortoutine %" PRIxPTR " exit and return %d.\n", (intptr_t)&co_obj, co_obj.get_ret_code());
        copp::coroutine_context_default *addr = copp::this_coroutine::get<copp::coroutine_context_default>();
        std::cout << "cortoutine " << addr << " is running." << std::endl;

        addr->yield();
        std::cout << "cortoutine " << addr << " is resumed." << std::endl;

        return 1;
    }
};

int main() {
    // create a coroutine
    copp::coroutine_context_default co_obj;
    std::cout << "cortoutine " << &co_obj << " is created." << std::endl;

    // create a runner
    my_runner runner;

    // bind runner to coroutine object
    co_obj.create(&runner);

    // start a coroutine
    co_obj.start();

    // yield from runner
    std::cout << "cortoutine " << &co_obj << " is yield." << std::endl;
    co_obj.resume();

    std::cout << "cortoutine " << &co_obj << " exit and return " << co_obj.get_ret_code() << "." << std::endl;
    return 0;
}

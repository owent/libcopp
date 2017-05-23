/*
 * sample_default_manager.cpp
 *
 *  Created on: 2014年3月17日
 *      Author: owent
 *
 *  Released under the MIT license
 */

#include <algorithm>
#include <cstdio>
#include <cstring>
#include <inttypes.h>
#include <iostream>
#include <limits>
#include <stdint.h>

// include manager header file
#include <libcopp/coroutine/coroutine_context_container.h>

#ifdef COPP_MACRO_USE_SEGMENTED_STACKS

typedef copp::coroutine_context_container<copp::allocator::stack_allocator_split_segment>
    co_context_t;

// define a coroutine runner
class my_runner : public copp::detail::coroutine_runnable_base {
public:
    bool is_running_;
    intptr_t min_;
    intptr_t max_;

private:
    void loop(int times) {
        char buffer[64 * 1024] = {0};
        co_context_t *pco = copp::this_coroutine::get<co_context_t>();
        std::cout << "context " << pco << " => buffer start addr " << buffer << ", end addr " << (buffer + sizeof(buffer)) << "."
                  << std::endl;

        min_ = std::min<intptr_t>(min_, (intptr_t)buffer);
        max_ = std::max<intptr_t>(max_, (intptr_t)(buffer + sizeof(buffer)));
        pco->yield();
        if (times > 0) loop(times - 1);

        is_running_ = false;
    }

public:
    int operator()(void*) {
        // test split stack(64MB of stack)
        is_running_ = true;
        min_ = std::numeric_limits<intptr_t>::max();
        max_ = std::numeric_limits<intptr_t>::min();
        loop(64 * 16);
        // action code here ...
        return 1;
    }
};

int main() {
    // create coroutines

    co_context_t co_obj1, co_obj2;
    std::cout << "cortoutine " << &co_obj1 << " created.\n" << std::endl;
    std::cout << "cortoutine " << &co_obj2 << " created.\n" << std::endl;

    // create a runner
    my_runner runner1, runner2;

    // bind runner to coroutine object
    co_obj1.create(&runner1, 32 * 1024);
    co_obj2.create(&runner2, 32 * 1024);

    // start a coroutine
    co_obj1.start();
    co_obj2.start();

    while (runner1.is_running_ || runner2.is_running_) {
        if (runner1.is_running_) co_obj1.resume();

        if (runner2.is_running_) co_obj2.resume();
    }

    // print stack distance
    std::cout << "co_obj1 stack min addr " << runner1.min_ << ", max addr " << runner1.max_ << ". dis "
              << ((runner1.max_ - runner1.min_) / 1024) << "KB." << std::endl;
    std::cout << "co_obj2 stack min addr " << runner2.min_ << ", max addr " << runner2.max_ << ". dis "
              << ((runner2.max_ - runner2.min_) / 1024) << "KB." << std::endl;

    puts("all jobs done.");
    return 0;
}

#else
int main() {
    puts("split stack disabled.");
    return 0;
}
#endif

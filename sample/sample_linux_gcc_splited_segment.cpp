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
#include <limits>
#include <inttypes.h>
#include <stdint.h>

// include manager header file
#include <libcopp/coroutine/coroutine_context_container.h>

#ifdef COPP_MACRO_USE_SEGMENTED_STACKS

typedef copp::detail::coroutine_context_container<
    copp::detail::coroutine_context_safe_base,
    copp::allocator::stack_allocator_split_segment
> co_context_t;

// define a coroutine runner
class my_runner : public copp::detail::coroutine_runnable_base
{
public:
    bool is_running_;
    intptr_t min_;
    intptr_t max_;

private:
    void loop(int times) {
        char buffer[64 * 1024] = {0};
        co_context_t* pco = get_coroutine_context<co_context_t>();
        printf("context %" PRIxPTR " => buffer start addr 0x%" PRIxPTR ", end addr 0x%" PRIxPTR ".\n",
            (intptr_t)pco,
            (intptr_t)buffer,
            (intptr_t)(buffer + sizeof(buffer))
        );

        min_ = std::min<intptr_t>(min_, (intptr_t)buffer);
        max_ = std::max<intptr_t>(max_, (intptr_t)(buffer + sizeof(buffer)));
        pco->yield();
        if (times > 0)
            loop(times - 1);

        is_running_ = false;
    }

public:
    int operator()() {
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
    // create a coroutine manager

    co_context_t co_obj1, co_obj2;
    printf("cortoutine %" PRIxPTR " created.\n", (intptr_t)&co_obj1);
    printf("cortoutine %" PRIxPTR " created.\n", (intptr_t)&co_obj2);

    // create a runner
    my_runner runner1, runner2;

    // bind runner to coroutine object
    co_obj1.create(&runner1, 32 * 1024);
    co_obj2.create(&runner2, 32 * 1024);

    // start a coroutine
    co_obj1.start();
    co_obj2.start();

    while(runner1.is_running_ || runner2.is_running_)
    {
        if (runner1.is_running_)
            co_obj1.resume();

        if (runner2.is_running_)
            co_obj2.resume();
    }

    // print stack distance
    printf("co_obj1 stack min addr 0x%" PRIxPTR ", max addr 0x%" PRIxPTR ". dis %" PRIu64 "KB.\n",
        runner1.min_, runner1.max_, (int64_t)(runner1.max_ - runner1.min_) / 1024
    );

    printf("co_obj2 stack min addr 0x%" PRIxPTR ", max addr 0x%" PRIxPTR ". dis %" PRIu64 "KB.\n",
        runner2.min_, runner2.max_, (int64_t)(runner2.max_ - runner2.min_) / 1024
    );

    puts("all jobs done.");
    return 0;
}

#else
int main() {
    puts("split stack disabled.");
    return 0;
}
#endif


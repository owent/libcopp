/*
 * sample_stress_test_coroutine.cpp
 *
 *  Created on: 2014年5月11日
 *      Author: owent
 *
 *  Released under the MIT license
 */


#include <cstdio>
#include <cstring>
#include <inttypes.h>
#include <stdint.h>
#include <ctime>
#include <cstdlib>

// include manager header file
#include <libcopp/coroutine/coroutine_context_container.h>

int switch_count = 100;

// define a coroutine runner
class my_runner : public copp::detail::coroutine_runnable_base
{
public:
    int operator()() {
        // ... your code here ...
        int count = switch_count; // 每个协程N次切换
        copp::coroutine_context_default* addr = get_coroutine_context<copp::coroutine_context_default>();

        while (count -- > 0)
            addr->yield();

        return 1;
    }
};

int MAX_COROUTINE_NUMBER = 100000; // 协程数量
copp::coroutine_context_default* co_arr = NULL;
my_runner* runner = NULL;
int main(int argc, char* argv[]) {
    if (argc > 1) {
        MAX_COROUTINE_NUMBER = atoi(argv[1]);
    }

    if (argc > 2) {
        switch_count = atoi(argv[2]);
    }

    time_t begin_time = time(NULL);
    clock_t begin_clock = clock();

    // create a coroutine manager
    co_arr = new copp::coroutine_context_default[MAX_COROUTINE_NUMBER];
    runner = new my_runner[MAX_COROUTINE_NUMBER];

    time_t end_time = time(NULL);
    clock_t end_clock = clock();
    printf("allocate %d coroutine, cost time: %d s, clock time: %d ms\n",
        MAX_COROUTINE_NUMBER,
        static_cast<int>(end_time - begin_time), 
        static_cast<int>(1000 * (end_clock - begin_clock) / CLOCKS_PER_SEC)
    );

    // create a runner
    // bind runner to coroutine object
    for (int i = 0; i < MAX_COROUTINE_NUMBER; ++ i) {
        co_arr[i].create(&runner[i]);
    }

    end_time = time(NULL);
    end_clock = clock();
    printf("create %d coroutine, cost time: %d s, clock time: %d ms\n",
        MAX_COROUTINE_NUMBER,
        static_cast<int>(end_time - begin_time),
        static_cast<int>(1000 * (end_clock - begin_clock) / CLOCKS_PER_SEC)
    );

    begin_time = end_time;
    begin_clock = end_clock;

    // start a coroutine
    for (int i = 0; i < MAX_COROUTINE_NUMBER; ++ i) {
        co_arr[i].start();
    }

    // yield & resume from runner
    bool continue_flag = true;
    long long real_switch_times = static_cast<long long>(MAX_COROUTINE_NUMBER);

    while (continue_flag) {
        continue_flag = false;
        for (int i = 0; i < MAX_COROUTINE_NUMBER; ++ i) {
            if (false == co_arr[i].is_finished()){
                continue_flag = true;
                ++ real_switch_times;
                co_arr[i].resume();
            }
        }
    }

    end_time = time(NULL);
    end_clock = clock();
    printf("switch %d coroutine contest %lld times, cost time: %d s, clock time: %d ms\n",
        MAX_COROUTINE_NUMBER,
        real_switch_times,
        static_cast<int>(end_time - begin_time),
        static_cast<int>(1000 * (end_clock - begin_clock) / CLOCKS_PER_SEC)
    );

    begin_time = end_time;
    begin_clock = end_clock;

    delete []co_arr;
    delete []runner;

    end_time = time(NULL);
    end_clock = clock();
    printf("remove %d coroutine, cost time: %d s, clock time: %d ms\n",
        MAX_COROUTINE_NUMBER,
        static_cast<int>(end_time - begin_time),
        static_cast<int>(1000 * (end_clock - begin_clock) / CLOCKS_PER_SEC)
    );

    return 0;
}


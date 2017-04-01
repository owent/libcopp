/*
 * sample_stress_test_coroutine_malloc.cpp
 *
 *  Created on: 2014年5月15日
 *      Author: owent
 *
 *  Released under the MIT license
 */


#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <ctime>
#include <inttypes.h>
#include <stdint.h>

// include manager header file
#include <libcopp/coroutine/coroutine_context_container.h>

#define CALC_MS_CLOCK(x) static_cast<int>((x) / (CLOCKS_PER_SEC / 1000))
#define CALC_NS_AVG_CLOCK(x, y) (1000000LL * static_cast<long long>((x) / (CLOCKS_PER_SEC / 1000)) / (y ? y : 1))

int switch_count = 100;

typedef copp::detail::coroutine_context_container<copp::detail::coroutine_context_base, copp::allocator::stack_allocator_memory>
    my_cotoutine_t;

// define a coroutine runner
class my_runner : public copp::detail::coroutine_runnable_base {
public:
    int operator()() {
        // ... your code here ...
        int count = switch_count; // 每个协程N次切换
        my_cotoutine_t *addr = copp::this_coroutine::get<my_cotoutine_t>();

        while (count-- > 0)
            addr->yield();

        return 1;
    }
};

int MAX_COROUTINE_NUMBER = 100000; // 协程数量
my_cotoutine_t *co_arr = NULL;
my_runner *runner = NULL;

// === 栈内存池 ===
char *stack_mem_pool = NULL;

int main(int argc, char *argv[]) {
    puts("###################### context coroutine (stack using memory pool) ###################");
    printf("########## Cmd:");
    for (int i = 0; i < argc; ++i) {
        printf(" %s", argv[i]);
    }
    puts("");

    if (argc > 1) {
        MAX_COROUTINE_NUMBER = atoi(argv[1]);
    }

    if (argc > 2) {
        switch_count = atoi(argv[2]);
    }

    size_t stack_size = 16 * 1024;
    if (argc > 3) {
        stack_size = atoi(argv[3]) * 1024;
    }

    stack_mem_pool = new char[MAX_COROUTINE_NUMBER * stack_size];
    memset(stack_mem_pool, 0, MAX_COROUTINE_NUMBER * stack_size);

    time_t begin_time = time(NULL);
    clock_t begin_clock = clock();

    // create coroutines
    co_arr = new my_cotoutine_t[MAX_COROUTINE_NUMBER];
    runner = new my_runner[MAX_COROUTINE_NUMBER];

    time_t end_time = time(NULL);
    clock_t end_clock = clock();
    printf("allocate %d coroutine, cost time: %d s, clock time: %d ms, avg: %lld ns\n", MAX_COROUTINE_NUMBER,
           static_cast<int>(end_time - begin_time), CALC_MS_CLOCK(end_clock - begin_clock),
           CALC_NS_AVG_CLOCK(end_clock - begin_clock, MAX_COROUTINE_NUMBER));

    begin_time = end_time;
    begin_clock = end_clock;

    // create a runner
    // bind runner to coroutine object
    for (int i = 0; i < MAX_COROUTINE_NUMBER; ++i) {
        co_arr[i].get_allocator().attach(stack_mem_pool + i * stack_size, stack_size);
        co_arr[i].create(&runner[i], stack_size);
    }

    end_time = time(NULL);
    end_clock = clock();
    printf("create %d coroutine, cost time: %d s, clock time: %d ms, avg: %lld ns\n", MAX_COROUTINE_NUMBER,
           static_cast<int>(end_time - begin_time), CALC_MS_CLOCK(end_clock - begin_clock),
           CALC_NS_AVG_CLOCK(end_clock - begin_clock, MAX_COROUTINE_NUMBER));

    begin_time = end_time;
    begin_clock = end_clock;

    // start a coroutine
    for (int i = 0; i < MAX_COROUTINE_NUMBER; ++i) {
        co_arr[i].start();
    }

    // yield & resume from runner
    bool continue_flag = true;
    long long real_switch_times = static_cast<long long>(0);

    while (continue_flag) {
        continue_flag = false;
        for (int i = 0; i < MAX_COROUTINE_NUMBER; ++i) {
            if (false == co_arr[i].is_finished()) {
                continue_flag = true;
                ++real_switch_times;
                co_arr[i].resume();
            }
        }
    }

    end_time = time(NULL);
    end_clock = clock();
    printf("switch %d coroutine contest %lld times, cost time: %d s, clock time: %d ms, avg: %lld ns\n", MAX_COROUTINE_NUMBER,
           real_switch_times, static_cast<int>(end_time - begin_time), CALC_MS_CLOCK(end_clock - begin_clock),
           CALC_NS_AVG_CLOCK(end_clock - begin_clock, real_switch_times));

    begin_time = end_time;
    begin_clock = end_clock;

    delete[] co_arr;
    delete[] runner;
    delete[] stack_mem_pool;

    end_time = time(NULL);
    end_clock = clock();
    printf("remove %d coroutine, cost time: %d s, clock time: %d ms, avg: %lld ns\n", MAX_COROUTINE_NUMBER,
           static_cast<int>(end_time - begin_time), CALC_MS_CLOCK(end_clock - begin_clock),
           CALC_NS_AVG_CLOCK(end_clock - begin_clock, MAX_COROUTINE_NUMBER));

    return 0;
}

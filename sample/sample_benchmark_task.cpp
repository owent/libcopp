/*
 * sample_stress_test_task.cpp
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
#include <vector>

// include manager header file
#include <libcotask/task.h>

#ifdef COTASK_MACRO_ENABLED

#define CALC_MS_CLOCK(x) static_cast<int>((x) / (CLOCKS_PER_SEC / 1000))
#define CALC_NS_AVG_CLOCK(x, y) (1000000LL * static_cast<long long>((x) / (CLOCKS_PER_SEC / 1000)) / (y?y:1))

typedef cotask::task<> my_task_t;

int switch_count = 100;
int max_task_number = 100000; // 协程Task数量
std::vector< my_task_t::ptr_t > task_arr;

// define a coroutine runner
int my_task_action() {
    // ... your code here ...
    int count = switch_count; // 每个task地切换次数

    while (count -- > 0)
        cotask::this_task::get_task()->yield();

    return 0;
}

int main(int argc, char* argv[]) {
    if (argc > 1) {
        max_task_number = atoi(argv[1]);
    }

    if (argc > 2) {
        switch_count = atoi(argv[2]);
    }

    size_t stack_size = 16 * 1024;
    if (argc > 3) {
        stack_size = atoi(argv[3]) * 1024;
    }

    time_t begin_time = time(NULL);
    clock_t begin_clock = clock();

    // create coroutines
    task_arr.reserve(static_cast<size_t>(max_task_number));
    while(task_arr.size() < static_cast<size_t>(max_task_number)) {
        my_task_t::ptr_t new_task = my_task_t::create(my_task_action, stack_size);
        if (!new_task) {
            fprintf(stderr, "create coroutine task failed, real size is %d.\n", static_cast<int>(task_arr.size()));
            fprintf(stderr, "maybe sysconf [vm.max_map_count] extended.\n");
            max_task_number = static_cast<int>(task_arr.size());
        }
        task_arr.push_back(new_task);
    }

    time_t end_time = time(NULL);
    clock_t end_clock = clock();
    printf("create %d task, cost time: %d s, clock time: %d ms, avg: %lld ns\n",
        max_task_number,
        static_cast<int>(end_time - begin_time), 
        CALC_MS_CLOCK(end_clock - begin_clock),
        CALC_NS_AVG_CLOCK(end_clock - begin_clock, max_task_number)
    );

    begin_time = end_time;
    begin_clock = end_clock;

    // start a task
    for (int i = 0; i < max_task_number; ++ i) {
        task_arr[i]->start();
    }

    // yield & resume from runner
    bool continue_flag = true;
    long long real_switch_times = static_cast<long long>(0);

    while (continue_flag) {
        continue_flag = false;
        for (int i = 0; i < max_task_number; ++ i) {
            if (false == task_arr[i]->is_completed()){
                continue_flag = true;
                ++ real_switch_times;
                task_arr[i]->resume();
            }
        }
    }

    end_time = time(NULL);
    end_clock = clock();
    printf("switch %d tasks %lld times, cost time: %d s, clock time: %d ms, avg: %lld ns\n",
        max_task_number,
        real_switch_times,
        static_cast<int>(end_time - begin_time),
        CALC_MS_CLOCK(end_clock - begin_clock),
        CALC_NS_AVG_CLOCK(end_clock - begin_clock, real_switch_times)
    );

    begin_time = end_time;
    begin_clock = end_clock;

    task_arr.clear();

    end_time = time(NULL);
    end_clock = clock();
    printf("remove %d tasks, cost time: %d s, clock time: %d ms, avg: %lld ns\n",
        max_task_number,
        static_cast<int>(end_time - begin_time),
        CALC_MS_CLOCK(end_clock - begin_clock),
        CALC_NS_AVG_CLOCK(end_clock - begin_clock, max_task_number)
    );

    return 0;
}
#else
int main() {
    puts("cotask disabled");
    return 0;
}
#endif

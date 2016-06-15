/*
 * sample_stress_test_task_mem_pool.cpp
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

// =============== 内存池对象 ===============
struct stack_mem_pool_t {
    int index;
    char* buff;
    stack_mem_pool_t(): index(0), buff(NULL){}
};
stack_mem_pool_t stack_mem_pool;
// --------------- 内存池对象 ---------------

int switch_count = 100;
int max_task_number = 100000; // 协程Task数量
size_t stack_size = 16 * 1024;

struct my_macro_coroutine
{
    typedef copp::detail::coroutine_context_base coroutine_t;
    typedef copp::allocator::stack_allocator_memory stack_allocator_t;

    typedef copp::detail::coroutine_context_container<coroutine_t, stack_allocator_t> coroutine_container_t;
};

struct my_task_allocator : public cotask::macro_task::task_allocator_t
{
public:

#if defined(COPP_MACRO_ENABLE_VARIADIC_TEMPLATE) && COPP_MACRO_ENABLE_VARIADIC_TEMPLATE
    /**
     * @brief allocate a object
     * @param args construct parameters
     * @return pointer of new object
     */
    template<typename Ty, typename... TARGS>
    static Ty* allocate(Ty* t, TARGS... args) {
        Ty* ret = new Ty(args...);
        ret->get_coroutine_context().get_allocator().attach(stack_mem_pool.buff + stack_mem_pool.index * stack_size, stack_size);
        ++ stack_mem_pool.index;
        return ret;
    }
#else
    /**
     * @brief allocate a object with 0 parameter(s).
     * @return pointer of new object
     */
    template< typename Ty >
    static Ty* allocate(Ty* t) {
        Ty* ret = new Ty();
        ret->get_coroutine_context().get_allocator().attach(stack_mem_pool.buff + stack_mem_pool.index * stack_size, stack_size);
        ++ stack_mem_pool.index;
        return ret;
    }
#endif
};

struct my_macro_task
{
    typedef cotask::macro_task::id_t id_t;
    typedef cotask::macro_task::id_allocator_t id_allocator_t;

    typedef cotask::macro_task::action_allocator_t action_allocator_t;
    typedef my_task_allocator task_allocator_t;
};

typedef cotask::task<my_macro_coroutine, my_macro_task> my_task_t;

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

    if (argc > 3) {
        stack_size = atoi(argv[3]) * 1024;
    }

    stack_mem_pool.index = 0;
    stack_mem_pool.buff = new char[stack_size * max_task_number];

    time_t begin_time = time(NULL);
    clock_t begin_clock = clock();

    // create coroutines
    task_arr.reserve(static_cast<size_t>(max_task_number));
    while(task_arr.size() < static_cast<size_t>(max_task_number)) {
        task_arr.push_back(my_task_t::create(my_task_action, stack_size));
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

    delete []stack_mem_pool.buff;

    return 0;
}
#else
int main() {
    puts("cotask disabled.");
    return 0;
}

#endif

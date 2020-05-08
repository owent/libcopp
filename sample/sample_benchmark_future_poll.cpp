/*
 * sample_benchmark_future_poll.cpp
 *
 *  Created on: 2020-05-08
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
#include <vector>

// include manager header file
#include <libcopp/future/future.h>

#if defined(PROJECT_LIBCOPP_SAMPLE_HAS_CHRONO) && PROJECT_LIBCOPP_SAMPLE_HAS_CHRONO
#include <chrono>
#define CALC_CLOCK_T std::chrono::system_clock::time_point
#define CALC_CLOCK_NOW() std::chrono::system_clock::now()
#define CALC_MS_CLOCK(x) static_cast<int>(std::chrono::duration_cast<std::chrono::milliseconds>(x).count())
#define CALC_NS_AVG_CLOCK(x, y) static_cast<long long>(std::chrono::duration_cast<std::chrono::nanoseconds>(x).count() / (y ? y : 1))
#else
#define CALC_CLOCK_T clock_t
#define CALC_CLOCK_NOW() clock()
#define CALC_MS_CLOCK(x) static_cast<int>((x) / (CLOCKS_PER_SEC / 1000))
#define CALC_NS_AVG_CLOCK(x, y) (1000000LL * static_cast<long long>((x) / (CLOCKS_PER_SEC / 1000)) / (y ? y : 1))
#endif

struct custom_waker_t;
typedef copp::future::context_t<custom_waker_t> custom_context_t;
typedef copp::future::result_t<int, int>        custom_result_t;
typedef copp::future::poll_t<custom_result_t>   custom_poll_t;
typedef copp::future::future_t<custom_result_t> custom_future_t;

struct custom_waker_t {
    int  left_count;
    void operator()(custom_context_t &) {}
    void operator()(custom_context_t &ctx, custom_poll_t &out) {
        if (ctx.get_private_data()->left_count-- > 0) {
            return;
        }

        out = custom_result_t::create_success(200);
    }
};

int                                            switch_count    = 100;
int                                            max_task_number = 100000; // 协程Task数量
std::vector<std::unique_ptr<custom_future_t> > task_arr;
std::vector<custom_context_t>                  context_arr;

int main(int argc, char *argv[]) {
    puts("###################### future poll ###################");
    printf("########## Cmd:");
    for (int i = 0; i < argc; ++i) {
        printf(" %s", argv[i]);
    }
    puts("");

    if (argc > 1) {
        max_task_number = atoi(argv[1]);
    }

    if (argc > 2) {
        switch_count = atoi(argv[2]);
    }

    time_t       begin_time  = time(NULL);
    CALC_CLOCK_T begin_clock = CALC_CLOCK_NOW();

    // create coroutines
    task_arr.reserve(static_cast<size_t>(max_task_number));
    context_arr.reserve(static_cast<size_t>(max_task_number));
    while (task_arr.size() < static_cast<size_t>(max_task_number)) {
        task_arr.emplace_back(copp::future::make_unique<custom_future_t>());
        context_arr.push_back(custom_context_t());
        context_arr.back().get_private_data()->left_count = switch_count;
    }

    time_t       end_time  = time(NULL);
    CALC_CLOCK_T end_clock = CALC_CLOCK_NOW();
    printf("create %d future and context, cost time: %d s, clock time: %d ms, avg: %lld ns\n", max_task_number,
           static_cast<int>(end_time - begin_time), CALC_MS_CLOCK(end_clock - begin_clock),
           CALC_NS_AVG_CLOCK(end_clock - begin_clock, max_task_number));

    begin_time  = end_time;
    begin_clock = end_clock;

    // yield & resume from runner
    bool      continue_flag     = true;
    long long real_switch_times = static_cast<long long>(0);

    while (continue_flag) {
        continue_flag = false;
        for (int i = 0; i < max_task_number; ++i) {
            if (false == task_arr[i]->is_ready()) {
                continue_flag = true;
                ++real_switch_times;
                task_arr[i]->poll(context_arr[i]);
            }
        }
    }

    end_time  = time(NULL);
    end_clock = CALC_CLOCK_NOW();
    printf("poll %d future and context for %lld times, cost time: %d s, clock time: %d ms, avg: %lld ns\n", max_task_number,
           real_switch_times, static_cast<int>(end_time - begin_time), CALC_MS_CLOCK(end_clock - begin_clock),
           CALC_NS_AVG_CLOCK(end_clock - begin_clock, real_switch_times));

    begin_time  = end_time;
    begin_clock = end_clock;

    task_arr.clear();
    context_arr.clear();

    end_time  = time(NULL);
    end_clock = CALC_CLOCK_NOW();
    printf("remove %d future and context, cost time: %d s, clock time: %d ms, avg: %lld ns\n", max_task_number,
           static_cast<int>(end_time - begin_time), CALC_MS_CLOCK(end_clock - begin_clock),
           CALC_NS_AVG_CLOCK(end_clock - begin_clock, max_task_number));

    return 0;
}

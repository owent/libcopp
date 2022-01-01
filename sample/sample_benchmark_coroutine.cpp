/*
 * sample_stress_test_coroutine.cpp
 *
 *  Created on: 2014年5月11日
 *      Author: owent
 *
 *  Released under the MIT license
 */

#include <inttypes.h>
#include <stdint.h>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <ctime>

// include manager header file
#include <libcopp/coroutine/coroutine_context_container.h>

#if defined(PROJECT_LIBCOPP_SAMPLE_HAS_CHRONO) && PROJECT_LIBCOPP_SAMPLE_HAS_CHRONO
#  include <chrono>
#  define CALC_CLOCK_T std::chrono::system_clock::time_point
#  define CALC_CLOCK_NOW() std::chrono::system_clock::now()
#  define CALC_MS_CLOCK(x) static_cast<int>(std::chrono::duration_cast<std::chrono::milliseconds>(x).count())
#  define CALC_NS_AVG_CLOCK(x, y) \
    static_cast<long long>(std::chrono::duration_cast<std::chrono::nanoseconds>(x).count() / (y ? y : 1))
#else
#  define CALC_CLOCK_T clock_t
#  define CALC_CLOCK_NOW() clock()
#  define CALC_MS_CLOCK(x) static_cast<int>((x) / (CLOCKS_PER_SEC / 1000))
#  define CALC_NS_AVG_CLOCK(x, y) (1000000LL * static_cast<long long>((x) / (CLOCKS_PER_SEC / 1000)) / (y ? y : 1))
#endif

int switch_count = 100;

// define a coroutine runner
static int my_runner(void *) {
  // ... your code here ...
  int count = switch_count;  // 每个协程N次切换
  copp::coroutine_context *self = copp::this_coroutine::get_coroutine();
  while (count-- > 0) {
    self->yield();
  }

  return 1;
}

int max_coroutine_number = 100000;  // 协程数量
copp::coroutine_context_default::ptr_t *co_arr = nullptr;
int main(int argc, char *argv[]) {
#ifdef LIBCOPP_MACRO_SYS_POSIX
  puts("###################### context coroutine (stack using default allocator[mmap]) ###################");
#elif defined(LIBCOPP_MACRO_SYS_WIN)
  puts("###################### context coroutine (stack using default allocator[VirtualAlloc]) ###################");
#else
  puts("###################### context coroutine (stack using default allocator ###################");
#endif
  printf("########## Cmd:");
  for (int i = 0; i < argc; ++i) {
    printf(" %s", argv[i]);
  }
  puts("");

  if (argc > 1) {
    max_coroutine_number = atoi(argv[1]);
  }

  if (argc > 2) {
    switch_count = atoi(argv[2]);
  }

  size_t stack_size = 16 * 1024;
  if (argc > 3) {
    stack_size = atoi(argv[3]) * 1024;
  }

  time_t begin_time = time(nullptr);
  CALC_CLOCK_T begin_clock = CALC_CLOCK_NOW();

  // create coroutines
  co_arr = new copp::coroutine_context_default::ptr_t[max_coroutine_number];

  time_t end_time = time(nullptr);
  CALC_CLOCK_T end_clock = CALC_CLOCK_NOW();
  printf("allocate %d coroutine, cost time: %d s, clock time: %d ms, avg: %lld ns\n", max_coroutine_number,
         static_cast<int>(end_time - begin_time), CALC_MS_CLOCK(end_clock - begin_clock),
         CALC_NS_AVG_CLOCK(end_clock - begin_clock, max_coroutine_number));

  // create a runner
  // bind runner to coroutine object
  for (int i = 0; i < max_coroutine_number; ++i) {
    co_arr[i] = copp::coroutine_context_default::create(my_runner, stack_size);
    if (!co_arr[i]) {
      fprintf(stderr, "coroutine create failed, the real number is %d\n", i);
      fprintf(stderr, "maybe sysconf [vm.max_map_count] extended?\n");
      max_coroutine_number = i;
      break;
    }
  }

  end_time = time(nullptr);
  end_clock = CALC_CLOCK_NOW();
  printf("create %d coroutine, cost time: %d s, clock time: %d ms, avg: %lld ns\n", max_coroutine_number,
         static_cast<int>(end_time - begin_time), CALC_MS_CLOCK(end_clock - begin_clock),
         CALC_NS_AVG_CLOCK(end_clock - begin_clock, max_coroutine_number));

  begin_time = end_time;
  begin_clock = end_clock;

  // start a coroutine
  for (int i = 0; i < max_coroutine_number; ++i) {
    co_arr[i]->start();
  }

  // yield & resume from runner
  bool continue_flag = true;
  long long real_switch_times = static_cast<long long>(0);

  while (continue_flag) {
    continue_flag = false;
    for (int i = 0; i < max_coroutine_number; ++i) {
      if (false == co_arr[i]->is_finished()) {
        continue_flag = true;
        ++real_switch_times;
        co_arr[i]->resume();
      }
    }
  }

  end_time = time(nullptr);
  end_clock = CALC_CLOCK_NOW();
  printf("switch %d coroutine contest %lld times, cost time: %d s, clock time: %d ms, avg: %lld ns\n",
         max_coroutine_number, real_switch_times, static_cast<int>(end_time - begin_time),
         CALC_MS_CLOCK(end_clock - begin_clock), CALC_NS_AVG_CLOCK(end_clock - begin_clock, real_switch_times));

  begin_time = end_time;
  begin_clock = end_clock;

  delete[] co_arr;

  end_time = time(nullptr);
  end_clock = CALC_CLOCK_NOW();
  printf("remove %d coroutine, cost time: %d s, clock time: %d ms, avg: %lld ns\n", max_coroutine_number,
         static_cast<int>(end_time - begin_time), CALC_MS_CLOCK(end_clock - begin_clock),
         CALC_NS_AVG_CLOCK(end_clock - begin_clock, max_coroutine_number));

  return 0;
}

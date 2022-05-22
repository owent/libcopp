/*
 * sample_benchmark_coroutine_stack_pool.cpp
 *
 *  Created on: 2017年5月09日
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
#include <libcopp/stack/stack_pool.h>

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

// === 栈内存池 ===
typedef copp::stack_pool<copp::allocator::default_statck_allocator> stack_pool_t;
stack_pool_t::ptr_t global_stack_pool;
int switch_count = 100;

typedef copp::coroutine_context_container<copp::allocator::stack_allocator_pool<stack_pool_t> > my_cotoutine_t;

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

int MAX_COROUTINE_NUMBER = 100000;  // 协程数量

static void benchmark_round(int index) {
  printf("### Round: %d ###\n", index);

  time_t begin_time = time(nullptr);
  CALC_CLOCK_T begin_clock = CALC_CLOCK_NOW();

  // create coroutines
  my_cotoutine_t::ptr_t *co_arr = new my_cotoutine_t::ptr_t[MAX_COROUTINE_NUMBER];

  time_t end_time = time(nullptr);
  CALC_CLOCK_T end_clock = CALC_CLOCK_NOW();
  printf("allocate %d coroutine, cost time: %d s, clock time: %d ms, avg: %lld ns\n", MAX_COROUTINE_NUMBER,
         static_cast<int>(end_time - begin_time), CALC_MS_CLOCK(end_clock - begin_clock),
         CALC_NS_AVG_CLOCK(end_clock - begin_clock, MAX_COROUTINE_NUMBER));

  for (int i = 0; i < MAX_COROUTINE_NUMBER; ++i) {
    copp::allocator::stack_allocator_pool<stack_pool_t> alloc(global_stack_pool);
    co_arr[i] = my_cotoutine_t::create(my_runner, alloc);
    if (!co_arr[i]) {
      fprintf(stderr, "coroutine create failed, the real number is %d\n", i);
      fprintf(stderr, "maybe sysconf [vm.max_map_count] extended?\n");
      MAX_COROUTINE_NUMBER = i;
      break;
    }
  }

  end_time = time(nullptr);
  end_clock = CALC_CLOCK_NOW();
  printf("create %d coroutine, cost time: %d s, clock time: %d ms, avg: %lld ns\n", MAX_COROUTINE_NUMBER,
         static_cast<int>(end_time - begin_time), CALC_MS_CLOCK(end_clock - begin_clock),
         CALC_NS_AVG_CLOCK(end_clock - begin_clock, MAX_COROUTINE_NUMBER));

  begin_time = end_time;
  begin_clock = end_clock;

  // start a coroutine
  for (int i = 0; i < MAX_COROUTINE_NUMBER; ++i) {
    co_arr[i]->start();
  }

  // yield & resume from runner
  bool continue_flag = true;
  long long real_switch_times = static_cast<long long>(0);

  while (continue_flag) {
    continue_flag = false;
    for (int i = 0; i < MAX_COROUTINE_NUMBER; ++i) {
      if (0 == co_arr[i]->resume()) {
        continue_flag = true;
        ++real_switch_times;
      }
    }
  }

  end_time = time(nullptr);
  end_clock = CALC_CLOCK_NOW();
  printf("switch %d coroutine contest %lld times, cost time: %d s, clock time: %d ms, avg: %lld ns\n",
         MAX_COROUTINE_NUMBER, real_switch_times, static_cast<int>(end_time - begin_time),
         CALC_MS_CLOCK(end_clock - begin_clock), CALC_NS_AVG_CLOCK(end_clock - begin_clock, real_switch_times));

  begin_time = end_time;
  begin_clock = end_clock;

  delete[] co_arr;

  end_time = time(nullptr);
  end_clock = CALC_CLOCK_NOW();
  printf("remove %d coroutine, cost time: %d s, clock time: %d ms, avg: %lld ns\n", MAX_COROUTINE_NUMBER,
         static_cast<int>(end_time - begin_time), CALC_MS_CLOCK(end_clock - begin_clock),
         CALC_NS_AVG_CLOCK(end_clock - begin_clock, MAX_COROUTINE_NUMBER));
}

int main(int argc, char *argv[]) {
  puts("###################### context coroutine (stack using stack pool) ###################");
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
    stack_size = static_cast<size_t>(atoi(argv[3]) * 1024);
  }

  global_stack_pool = stack_pool_t::create();
  global_stack_pool->set_min_stack_number(static_cast<size_t>(MAX_COROUTINE_NUMBER));
  global_stack_pool->set_stack_size(stack_size);

  for (int i = 1; i <= 5; ++i) {
    benchmark_round(i);
  }
  return 0;
}

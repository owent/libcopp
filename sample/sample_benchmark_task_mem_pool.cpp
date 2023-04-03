/*
 * sample_stress_test_task_mem_pool.cpp
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
#include <vector>

// include manager header file
#include <libcotask/task.h>

#ifdef LIBCOTASK_MACRO_ENABLED

#  if defined(PROJECT_LIBCOPP_SAMPLE_HAS_CHRONO) && PROJECT_LIBCOPP_SAMPLE_HAS_CHRONO
#    include <chrono>
#    define CALC_CLOCK_T std::chrono::system_clock::time_point
#    define CALC_CLOCK_NOW() std::chrono::system_clock::now()
#    define CALC_MS_CLOCK(x) static_cast<int>(std::chrono::duration_cast<std::chrono::milliseconds>(x).count())
#    define CALC_NS_AVG_CLOCK(x, y) \
      static_cast<long long>(std::chrono::duration_cast<std::chrono::nanoseconds>(x).count() / (y ? y : 1))
#  else
#    define CALC_CLOCK_T clock_t
#    define CALC_CLOCK_NOW() clock()
#    define CALC_MS_CLOCK(x) static_cast<int>((x) / (CLOCKS_PER_SEC / 1000))
#    define CALC_NS_AVG_CLOCK(x, y) (1000000LL * static_cast<long long>((x) / (CLOCKS_PER_SEC / 1000)) / (y ? y : 1))
#  endif

// =============== 内存池对象 ===============
struct stack_mem_pool_t {
  int index;
  char *buff;
  stack_mem_pool_t() : index(0), buff(nullptr) {}
};
stack_mem_pool_t stack_mem_pool;
// --------------- 内存池对象 ---------------

int switch_count = 100;
int max_task_number = 100000;  // 协程Task数量
size_t g_stack_size = 16 * 1024;

struct my_macro_coroutine {
  using stack_allocator_type = copp::allocator::stack_allocator_memory;
  using coroutine_type = copp::coroutine_context_container<stack_allocator_type>;
  using value_type = int;
};

typedef cotask::task<my_macro_coroutine> my_task_t;

std::vector<my_task_t::ptr_t> task_arr;

// define a coroutine runner
int my_task_action(void *) {
  // ... your code here ...
  int count = switch_count;  // 每个task地切换次数
  cotask::impl::task_impl *self = cotask::this_task::get_task();
  while (count-- > 0) {
    self->yield();
  }

  return 0;
}

int main(int argc, char *argv[]) {
  puts("###################### task (stack using memory pool) ###################");
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

  if (argc > 3) {
    g_stack_size = atoi(argv[3]) * 1024;
  }
  if (g_stack_size < copp::stack_traits::minimum_size()) {
    g_stack_size = copp::stack_traits::minimum_size();
  }

  stack_mem_pool.index = 0;
  stack_mem_pool.buff = new char[g_stack_size * max_task_number];
  memset(stack_mem_pool.buff, 0, g_stack_size * max_task_number);

  time_t begin_time = time(nullptr);
  CALC_CLOCK_T begin_clock = CALC_CLOCK_NOW();

  // create coroutines
  task_arr.reserve(static_cast<size_t>(max_task_number));
  while (task_arr.size() < static_cast<size_t>(max_task_number)) {
    copp::allocator::stack_allocator_memory alloc(stack_mem_pool.buff + stack_mem_pool.index * g_stack_size,
                                                  g_stack_size);
    ++stack_mem_pool.index;
    task_arr.push_back(my_task_t::create(my_task_action, alloc, g_stack_size));
  }

  time_t end_time = time(nullptr);
  CALC_CLOCK_T end_clock = CALC_CLOCK_NOW();
  printf("create %d task, cost time: %d s, clock time: %d ms, avg: %lld ns\n", max_task_number,
         static_cast<int>(end_time - begin_time), CALC_MS_CLOCK(end_clock - begin_clock),
         CALC_NS_AVG_CLOCK(end_clock - begin_clock, max_task_number));

  begin_time = end_time;
  begin_clock = end_clock;

  // start a task
  for (int i = 0; i < max_task_number; ++i) {
    task_arr[i]->start();
  }

  // yield & resume from runner
  bool continue_flag = true;
  long long real_switch_times = static_cast<long long>(0);

  while (continue_flag) {
    continue_flag = false;
    for (int i = 0; i < max_task_number; ++i) {
      if (false == task_arr[i]->is_completed()) {
        continue_flag = true;
        ++real_switch_times;
        task_arr[i]->resume();
      }
    }
  }

  end_time = time(nullptr);
  end_clock = CALC_CLOCK_NOW();
  printf("switch %d tasks %lld times, cost time: %d s, clock time: %d ms, avg: %lld ns\n", max_task_number,
         real_switch_times, static_cast<int>(end_time - begin_time), CALC_MS_CLOCK(end_clock - begin_clock),
         CALC_NS_AVG_CLOCK(end_clock - begin_clock, real_switch_times));

  begin_time = end_time;
  begin_clock = end_clock;

  task_arr.clear();

  end_time = time(nullptr);
  end_clock = CALC_CLOCK_NOW();
  printf("remove %d tasks, cost time: %d s, clock time: %d ms, avg: %lld ns\n", max_task_number,
         static_cast<int>(end_time - begin_time), CALC_MS_CLOCK(end_clock - begin_clock),
         CALC_NS_AVG_CLOCK(end_clock - begin_clock, max_task_number));

  delete[] stack_mem_pool.buff;

  return 0;
}
#else
int main() {
  puts("cotask disabled.");
  return 0;
}

#endif

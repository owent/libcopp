/*
 * sample_benchmark_future_poll.cpp
 *
 *  Created on: 2020-05-08
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
#include <libcopp/future/std_coroutine_generator.h>
#include <libcopp/future/std_coroutine_task.h>

#if defined(LIBCOPP_MACRO_ENABLE_STD_COROUTINE) && LIBCOPP_MACRO_ENABLE_STD_COROUTINE

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

struct benchmark_no_trivial_message_t {
  benchmark_no_trivial_message_t() {}
  ~benchmark_no_trivial_message_t() {}
  benchmark_no_trivial_message_t(const benchmark_no_trivial_message_t &) {}
  benchmark_no_trivial_message_t(benchmark_no_trivial_message_t &&) {}
  benchmark_no_trivial_message_t &operator=(const benchmark_no_trivial_message_t &) { return *this; }
  benchmark_no_trivial_message_t &operator=(const benchmark_no_trivial_message_t &&) { return *this; }
};

struct benchmark_generator_waker_t;
typedef copp::future::result_type<benchmark_no_trivial_message_t, int32_t> benchmark_result_t;
typedef copp::future::task_future<benchmark_result_t> benchmark_task_t;
typedef copp::future::generator_context<benchmark_generator_waker_t> benchmark_generator_context_t;
typedef copp::future::generator_future_data<benchmark_result_t> benchmark_generator_future_t;
typedef copp::future::poll_type<benchmark_result_t> benchmark_poll_t;

std::vector<benchmark_generator_context_t *> g_benchmark_waker_list;
std::vector<benchmark_task_t> g_benchmark_task_list;

struct benchmark_generator_waker_t {
  size_t index;
  int32_t code;
  benchmark_generator_waker_t(size_t idx) : index(idx), code(0) {}
  ~benchmark_generator_waker_t() { g_benchmark_waker_list[index] = nullptr; }

  void operator()(benchmark_generator_future_t &fut, benchmark_generator_context_t &ctx) {
    g_benchmark_waker_list[index] = &ctx;
    if (code) {
      fut.poll_data() = benchmark_result_t::make_error(code);
      code = 0;
    }
  }
};

typedef copp::future::generator<benchmark_result_t, benchmark_generator_waker_t> benchmark_generator_t;

static benchmark_task_t benchmark_start_sum_task(size_t index, int32_t await_times) {
  int32_t sum = 0;
  auto gen = copp::future::make_generator<benchmark_generator_t>(index);
  for (int32_t i = 0; i < await_times; ++i) {
    auto res = co_await gen;
    if (nullptr != res && res->is_error()) {
      sum += *res->get_error();
    }
    gen.reset_data();
  }

  co_return benchmark_result_t::make_error(sum);
}

int switch_count = 100;
int max_task_number = 100000;  // 协程Task数量

static void benchmark_round(int index) {
  g_benchmark_waker_list.resize(static_cast<size_t>(max_task_number), nullptr);
  g_benchmark_task_list.reserve(static_cast<size_t>(max_task_number));

  printf("### Round: %d ###\n", index);

  time_t begin_time = time(nullptr);
  CALC_CLOCK_T begin_clock = CALC_CLOCK_NOW();

  // create coroutines task
  while (g_benchmark_task_list.size() < static_cast<size_t>(max_task_number)) {
    g_benchmark_task_list.push_back(benchmark_start_sum_task(g_benchmark_task_list.size(), switch_count));
  }

  time_t end_time = time(nullptr);
  CALC_CLOCK_T end_clock = CALC_CLOCK_NOW();
  printf("create %d task(s) and generator(s), cost time: %d s, clock time: %d ms, avg: %lld ns\n", max_task_number,
         static_cast<int>(end_time - begin_time), CALC_MS_CLOCK(end_clock - begin_clock),
         CALC_NS_AVG_CLOCK(end_clock - begin_clock, max_task_number));

  begin_time = end_time;
  begin_clock = end_clock;

  // yield & resume from runner
  bool continue_flag = true;
  long long real_switch_times = static_cast<long long>(0);
  int32_t round = 0;

  while (continue_flag) {
    ++round;
    continue_flag = false;
    for (size_t i = 0; i < g_benchmark_waker_list.size(); ++i) {
      if (g_benchmark_waker_list[i]) {
        if (0 == g_benchmark_waker_list[i]->get_private_data().code) {
          g_benchmark_waker_list[i]->get_private_data().code = round;
          ++real_switch_times;
          continue_flag = true;
        }
        g_benchmark_waker_list[i]->wake();
      }
    }
  }

  end_time = time(nullptr);
  end_clock = CALC_CLOCK_NOW();
  printf("poll %d task(s) and generator(s) for %lld times, cost time: %d s, clock time: %d ms, avg: %lld ns\n",
         max_task_number, real_switch_times, static_cast<int>(end_time - begin_time),
         CALC_MS_CLOCK(end_clock - begin_clock), CALC_NS_AVG_CLOCK(end_clock - begin_clock, real_switch_times));

  begin_time = end_time;
  begin_clock = end_clock;

  g_benchmark_waker_list.clear();
  g_benchmark_task_list.clear();

  end_time = time(nullptr);
  end_clock = CALC_CLOCK_NOW();
  printf("remove %d task(s), cost time: %d s, clock time: %d ms, avg: %lld ns\n", max_task_number,
         static_cast<int>(end_time - begin_time), CALC_MS_CLOCK(end_clock - begin_clock),
         CALC_NS_AVG_CLOCK(end_clock - begin_clock, max_task_number));
}

int main(int argc, char *argv[]) {
  puts("###################### std task generator - no trivial ###################");
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

  for (int i = 1; i <= 5; ++i) {
    benchmark_round(i);
  }
  return 0;
}
#else
int main() {
  puts("std coroutine is not supported by current compiler.");
  return 0;
}
#endif
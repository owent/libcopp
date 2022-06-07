// Copyright 2022 owent
// std coroutine trivial callable benchmark

#include <libcopp/coroutine/callable_promise.h>
#include <libcopp/coroutine/generator_promise.h>

#include <inttypes.h>
#include <stdint.h>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <ctime>
#include <vector>

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

using benchmark_callable_future_type = copp::callable_future<int64_t>;
using benchmark_generator_future_type = copp::generator_future<int64_t>;

std::vector<std::unique_ptr<benchmark_callable_future_type>> g_benchmark_callable_list;
std::vector<benchmark_generator_future_type> g_benchmark_generator_list;

int recursive_count = 100;
int max_task_number = 100000;

benchmark_callable_future_type run_benchmark(size_t idx, int left_recursive_count,
                                             benchmark_generator_future_type* first_hang) {
  int64_t result = left_recursive_count;
  if (nullptr != first_hang) {
    auto gen_res = co_await *first_hang;
    result += gen_res;
  } else {
    result += idx;
  }

  if (left_recursive_count > 1) {
    auto gen_res = co_await run_benchmark(idx, left_recursive_count - 1, nullptr);
    result += gen_res;
  }

  co_return result;
}

static void benchmark_round(int index) {
  g_benchmark_callable_list.reserve(static_cast<size_t>(max_task_number));
  g_benchmark_generator_list.reserve(static_cast<size_t>(max_task_number));

  printf("### Round: %d ###\n", index);

  // create generators
  for (int i = 0; i < max_task_number; ++i) {
    g_benchmark_generator_list.emplace_back(
        benchmark_generator_future_type([](benchmark_generator_future_type::context_pointer_type) {}));
  }

  time_t begin_time = time(nullptr);
  CALC_CLOCK_T begin_clock = CALC_CLOCK_NOW();

  // create coroutines callable
  while (g_benchmark_callable_list.size() < static_cast<size_t>(max_task_number)) {
    size_t idx = g_benchmark_callable_list.size();
    g_benchmark_callable_list.push_back(std::unique_ptr<benchmark_callable_future_type>(
        new benchmark_callable_future_type(run_benchmark(idx, recursive_count, &g_benchmark_generator_list[idx]))));
  }

  time_t end_time = time(nullptr);
  CALC_CLOCK_T end_clock = CALC_CLOCK_NOW();
  printf("create %d callable(s) and generator(s), cost time: %d s, clock time: %d ms, avg: %lld ns\n", max_task_number,
         static_cast<int>(end_time - begin_time), CALC_MS_CLOCK(end_clock - begin_clock),
         CALC_NS_AVG_CLOCK(end_clock - begin_clock, max_task_number));

  begin_time = end_time;
  begin_clock = end_clock;

  // yield & resume from runner
  long long real_switch_times = static_cast<long long>(0);

  for (int i = 0; i < max_task_number; ++i) {
    g_benchmark_generator_list[i].get_context()->set_value(i);
  }

  end_time = time(nullptr);
  end_clock = CALC_CLOCK_NOW();
  printf("resume %d callable(s) and generator(s) for %lld times, cost time: %d s, clock time: %d ms, avg: %lld ns\n",
         max_task_number, real_switch_times, static_cast<int>(end_time - begin_time),
         CALC_MS_CLOCK(end_clock - begin_clock), CALC_NS_AVG_CLOCK(end_clock - begin_clock, real_switch_times));

  begin_time = end_time;
  begin_clock = end_clock;

  g_benchmark_callable_list.clear();

  end_time = time(nullptr);
  end_clock = CALC_CLOCK_NOW();
  printf("remove %d callable(s), cost time: %d s, clock time: %d ms, avg: %lld ns\n", max_task_number,
         static_cast<int>(end_time - begin_time), CALC_MS_CLOCK(end_clock - begin_clock),
         CALC_NS_AVG_CLOCK(end_clock - begin_clock, max_task_number));
}

int main(int argc, char* argv[]) {
  puts("###################### std callable - recursive ###################");
  printf("########## Cmd:");
  for (int i = 0; i < argc; ++i) {
    printf(" %s", argv[i]);
  }
  puts("");

  if (argc > 1) {
    max_task_number = atoi(argv[1]);
  }

  if (argc > 2) {
    recursive_count = atoi(argv[2]);
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

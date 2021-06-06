/*
 * sample_readme_9.cpp
 *
 *  Created on: 2020-05-20
 *      Author: owent
 *
 *  Released under the MIT license
 */

#include <iostream>

// include manager header file
#include <libcopp/future/std_coroutine_generator.h>
#include <libcopp/future/std_coroutine_task.h>

#if defined(LIBCOPP_MACRO_ENABLE_STD_COROUTINE) && LIBCOPP_MACRO_ENABLE_STD_COROUTINE

// ============================ types for task and generator ============================
class sample_message_t {
 private:
  sample_message_t(const sample_message_t &) UTIL_CONFIG_DELETED_FUNCTION;
  sample_message_t &operator=(const sample_message_t &) UTIL_CONFIG_DELETED_FUNCTION;
  sample_message_t(sample_message_t &&) UTIL_CONFIG_DELETED_FUNCTION;
  sample_message_t &operator=(sample_message_t &&) UTIL_CONFIG_DELETED_FUNCTION;

 public:
  int ret_code;
  int rsp_code;

  sample_message_t() : ret_code(0), rsp_code(0) {}
  sample_message_t(int c) : ret_code(c), rsp_code(c) {}
  ~sample_message_t() {}
};

#  define SAMPLE_TIMEOUT_ERROR_CODE (-500)

struct sample_generator_waker_t;
struct sample_task_waker_t;

typedef copp::future::result_t<sample_message_t, int32_t> sample_result_t;
typedef copp::future::task_t<sample_result_t, sample_task_waker_t> sample_task_t;
typedef copp::future::generator_context_t<sample_generator_waker_t> sample_generator_context_t;
typedef copp::future::generator_future_t<sample_result_t> sample_generator_future_t;
typedef copp::future::poll_t<sample_result_t> sample_poll_t;
typedef copp::future::task_context_t<sample_task_waker_t> sample_task_context_t;
typedef copp::future::task_future_t<sample_result_t> sample_task_future_t;

std::list<sample_generator_context_t *> g_sample_generator_waker_list;
std::list<sample_task_context_t *> g_sample_task_waker_list;

struct sample_generator_waker_t {
  int32_t code;
  int32_t await_times;
  std::list<sample_generator_context_t *>::iterator refer_to;
  sample_generator_waker_t(int32_t c, int32_t at) : code(c), await_times(at) {
    refer_to = g_sample_generator_waker_list.end();
  }

  ~sample_generator_waker_t() {
    // Remove from executor when destroyed
    if (refer_to != g_sample_generator_waker_list.end()) {
      g_sample_generator_waker_list.erase(refer_to);
    }
  }

  void operator()(sample_generator_future_t &fut, sample_generator_context_t &ctx) {
    // Remove from executor when polled
    if (refer_to != g_sample_generator_waker_list.end()) {
      g_sample_generator_waker_list.erase(refer_to);
      refer_to = g_sample_generator_waker_list.end();
    }
    if (await_times-- > 0) {
      // Add to executor when not ready
      refer_to = g_sample_generator_waker_list.insert(g_sample_generator_waker_list.end(), &ctx);
      return;
    }

    if (code > 0) {
      fut.poll_data() = sample_result_t::make_success(code);
    } else {
      fut.poll_data() = sample_result_t::make_error(code);
    }
  }
};

struct sample_task_waker_t {
  int32_t code;
  std::list<sample_task_context_t *>::iterator refer_iter;

  template <class... TARGS>
  sample_task_waker_t(TARGS &&...) : code(0), refer_iter(g_sample_task_waker_list.end()) {}
  ~sample_task_waker_t() {
    // Remove from executor when destroyed
    if (g_sample_task_waker_list.end() != refer_iter) {
      g_sample_task_waker_list.erase(refer_iter);
    }
  }

  void operator()(sample_task_future_t &fut, sample_task_context_t &ctx) {
    // Remove from executor when polled
    if (g_sample_task_waker_list.end() != refer_iter) {
      g_sample_task_waker_list.erase(refer_iter);
      refer_iter = g_sample_task_waker_list.end();
    }

    if (0 != code) {
      fut.poll_data() = sample_result_t::make_error(code);
      return;
    }

    // Add to executor when not ready
    refer_iter = g_sample_task_waker_list.insert(g_sample_task_waker_list.end(), &ctx);
  }
};

// ============================ timeout for generator ============================
typedef copp::future::generator_t<sample_result_t, sample_generator_waker_t> sample_generator_t;
static sample_task_t call_for_await_generator_and_timeout(int32_t await_times) {
  std::cout << "ready to co_await generator." << std::endl;

  auto gen = copp::future::make_generator<sample_generator_t>(200, await_times);
  sample_poll_t ret;
  auto await_fut = co_await gen;

  ret = std::move(gen.poll_data());

  // the return is still pending, because it's resumed by timeout waker
  std::cout << "\tret.is_pending() :" << ret.is_pending() << ", await_fut:" << await_fut << std::endl;
  sample_task_context_t *context = co_yield sample_task_t::current_context();
  if (context) {
    std::cout << "\texpected code: " << SAMPLE_TIMEOUT_ERROR_CODE << ", real is: " << context->get_private_data().code
              << std::endl;
    ;
  }

  sample_task_future_t *fut = co_yield sample_task_t::current_future();
  if (fut) {
    std::cout << "\tfut->is_ready() :" << fut->is_ready() << std::endl;
    if (fut->data() && fut->data()->is_error()) {
      std::cout << "\texpected code: " << SAMPLE_TIMEOUT_ERROR_CODE << ", real is: " << *fut->data()->get_error()
                << std::endl;
      ;
    }
  }

  co_return sample_result_t::make_error(-1);
}

static void poll_generator_and_timeout() {
  sample_task_t t1 = call_for_await_generator_and_timeout(3);

  // set timeout result
  for (int retry_times = 10; retry_times >= 0 && !g_sample_task_waker_list.empty(); --retry_times) {
    (*g_sample_task_waker_list.begin())->get_private_data().code = SAMPLE_TIMEOUT_ERROR_CODE;
    // wake and resume task
    (*g_sample_task_waker_list.begin())->wake();
  }

  std::cout << "task status should be DONE(" << static_cast<int>(sample_task_t::status_type::DONE)
            << "), real is: " << static_cast<int>(t1.get_status()) << std::endl;
  if (t1.data() && t1.data()->is_error()) {
    std::cout << "\texpected code: " << SAMPLE_TIMEOUT_ERROR_CODE << ", real is: " << *t1.data()->get_error()
              << std::endl;
  }

  // cleanup generator dispatcher
  for (int retry_times = 10; retry_times >= 0 && !g_sample_generator_waker_list.empty(); --retry_times) {
    (*g_sample_generator_waker_list.begin())->wake();
  }

  // cleanup task dispatcher
  for (int retry_times = 10; retry_times >= 0 && !g_sample_generator_waker_list.empty(); --retry_times) {
    (*g_sample_generator_waker_list.begin())->wake();
  }
}

// ============================ timeout for task ============================
static sample_task_t call_for_await_task_and_timeout(int32_t await_times) {
  std::cout << "ready to co_await task." << std::endl;

  auto t_dep = call_for_await_generator_and_timeout(await_times);
  auto t_fut = co_await t_dep;

  // the return is still pending, because it's resumed by timeout waker
  std::cout << "\tt_dep.done() :" << t_dep.done() << ", t_fut: " << t_fut << std::endl;
  sample_task_context_t *context = co_yield sample_task_t::current_context();
  if (context) {
    std::cout << "\texpected code: " << SAMPLE_TIMEOUT_ERROR_CODE << ", real is: " << context->get_private_data().code
              << std::endl;
    ;
  }

  sample_task_future_t *fut = co_yield sample_task_t::current_future();
  if (fut) {
    std::cout << "\tfut->is_ready() :" << fut->is_ready() << std::endl;
    if (fut->data() && fut->data()->is_error()) {
      std::cout << "\texpected code: " << SAMPLE_TIMEOUT_ERROR_CODE << ", real is: " << *fut->data()->get_error()
                << std::endl;
      ;
    }
  }

  co_return sample_result_t::make_error(-1);
}

static void poll_no_trival_task_and_timeout() {
  sample_task_t t1 = call_for_await_task_and_timeout(3);

  // set timeout result
  for (int retry_times = 10; retry_times >= 0 && !t1.done() && t1.get_context(); --retry_times) {
    t1.get_context()->get_private_data().code = SAMPLE_TIMEOUT_ERROR_CODE;
    // wake and resume task
    t1.get_context()->wake();
  }

  std::cout << "t1.done() :" << t1.done() << std::endl;
  std::cout << "\ttask " << t1.get_task_id() << " status should be DONE("
            << static_cast<int>(sample_task_t::status_type::DONE) << "), real is: " << static_cast<int>(t1.get_status())
            << std::endl;
  if (t1.data() && t1.data()->is_error()) {
    std::cout << "\texpected code: " << SAMPLE_TIMEOUT_ERROR_CODE << ", real is: " << *t1.data()->get_error()
              << std::endl;
  }

  // cleanup task dispatcher
  for (int retry_times = 10; retry_times >= 0 && !g_sample_task_waker_list.empty(); --retry_times) {
    (*g_sample_task_waker_list.begin())->get_private_data().code = SAMPLE_TIMEOUT_ERROR_CODE;
    (*g_sample_task_waker_list.begin())->wake();
  }

  // cleanup generator dispatcher
  for (int retry_times = 10; retry_times >= 0 && !g_sample_generator_waker_list.empty(); --retry_times) {
    (*g_sample_generator_waker_list.begin())->wake();
  }
}

int main() {
  // sample for await generator and timeout
  poll_generator_and_timeout();

  // sample for await task and timeout
  poll_no_trival_task_and_timeout();
  return 0;
}
#else
int main() {
  puts("this sample require cotask enabled and compiler support c++20 coroutine");
  return 0;
}
#endif

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
#include <libcopp/coroutine/callable_promise.h>
#include <libcopp/coroutine/generator_promise.h>

#if defined(LIBCOPP_MACRO_ENABLE_STD_COROUTINE) && LIBCOPP_MACRO_ENABLE_STD_COROUTINE

// ============================ types for task and generator ============================
class sample_message_t {
 public:
  int ret_code;

  sample_message_t() : ret_code(0) {}
  sample_message_t(int c) : ret_code(c) {}
  sample_message_t(const sample_message_t &) = default;
  sample_message_t &operator=(const sample_message_t &) = default;
  sample_message_t(sample_message_t &&) = default;
  sample_message_t &operator=(sample_message_t &&) = default;
  ~sample_message_t() {}
};

#  define SAMPLE_TIMEOUT_ERROR_CODE (-500)

LIBCOPP_COPP_NAMESPACE_BEGIN
template <>
struct std_coroutine_default_error_transform<sample_message_t> {
  using type = sample_message_t;
  type operator()(promise_status in) const {
    if (in == promise_status::kTimeout) {
      return sample_message_t{SAMPLE_TIMEOUT_ERROR_CODE};
    }
    return sample_message_t{static_cast<int>(in)};
  }
};
LIBCOPP_COPP_NAMESPACE_END

using int_generator = copp::generator_future<int>;
std::list<int_generator::context_pointer_type> g_int_executor;
using custom_generator = copp::generator_future<sample_message_t>;
std::list<custom_generator::context_pointer_type> g_sample_executor;

static void int_generator_callback(int_generator::context_pointer_type ctx) {
  g_int_executor.emplace_back(std::move(ctx));
}

static void custom_generator_callback(custom_generator::context_pointer_type ctx) {
  g_sample_executor.emplace_back(std::move(ctx));
}

static copp::callable_future<int> coroutine_simulator_rpc_integer_l2() {
  auto result = co_await int_generator{int_generator_callback};
  co_return result;
}

static copp::callable_future<void> coroutine_simulator_rpc_integer() {
  auto result = co_await coroutine_simulator_rpc_integer_l2();
  std::cout << "int generator is killed with code: " << result << std::endl;
  co_return;
}

static copp::callable_future<int> coroutine_simulator_rpc_custom_l2() {
  auto result = co_await custom_generator{custom_generator_callback};
  co_return result.ret_code;
}

static copp::callable_future<void> coroutine_simulator_rpc_custom() {
  auto result = co_await coroutine_simulator_rpc_custom_l2();
  std::cout << "custom generator is killed with code: " << result << std::endl;
  co_return;
}

int main() {
  // sample for await generator and timeout
  auto f1 = coroutine_simulator_rpc_integer();
  f1.kill(copp::promise_status::kCancle, true);
  std::cout << "int generator is killed" << std::endl;

  // sample for await task and timeout
  auto f2 = coroutine_simulator_rpc_custom();
  f2.kill(copp::promise_status::kTimeout, true);
  std::cout << "custom generator is killed" << std::endl;
  return 0;
}
#else
int main() {
  puts("this sample require cotask enabled and compiler support c++20 coroutine");
  return 0;
}
#endif

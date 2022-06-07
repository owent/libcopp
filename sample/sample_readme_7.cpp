/*
 * sample_readme_8.cpp
 *
 *  Created on: 2020-05-22
 *      Author: owent
 *
 *  Released under the MIT license
 */

#include <assert.h>
#include <iostream>
#include <string>

// include manager header file
#include <libcopp/coroutine/callable_promise.h>
#include <libcopp/coroutine/generator_promise.h>

#if defined(LIBCOPP_MACRO_ENABLE_STD_COROUTINE) && LIBCOPP_MACRO_ENABLE_STD_COROUTINE

using my_generator = copp::generator_future<int>;
std::list<my_generator::context_pointer_type> g_sample_executor;

static void generator_callback(my_generator::context_pointer_type ctx) {
  g_sample_executor.emplace_back(std::move(ctx));
}

static copp::callable_future<void> coroutine_simulator_rpc() {
  my_generator generator_object{generator_callback};
  auto value1 = co_await generator_object;
  std::cout << "co_await named generator: " << value1 << std::endl;
  auto value2 = co_await my_generator{generator_callback};
  std::cout << "co_await temporary generator: " << value2 << std::endl;

  generator_object.get_context()->reset_value();
  auto value3 = co_await generator_object;
  std::cout << "reset and co_await named generator again: " << value3 << std::endl;
  co_return;
}

int main() {
  int result = 191;
  auto f = coroutine_simulator_rpc();

  while (!g_sample_executor.empty()) {
    auto ctx = g_sample_executor.front();
    g_sample_executor.pop_front();
    ctx->set_value(++result);
  }
  return 0;
}
#else
int main() {
  puts("this sample require cotask enabled and compiler support c++20 coroutine");
  return 0;
}
#endif

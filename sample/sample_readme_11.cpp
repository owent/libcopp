/*
 * sample_readme_11.cpp
 *
 *  Created on: 2025-03-04
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

using my_receiver = copp::generator_channel_receiver<int>;
using my_sender = copp::generator_channel_sender<int>;
std::list<my_sender> g_sample_executor;

static my_receiver generator_pick_receiver(std::pair<my_receiver, my_sender>&& receiver_and_sender) {
  g_sample_executor.emplace_back(std::move(receiver_and_sender.second));
  return receiver_and_sender.first;
}

static copp::callable_future<void> coroutine_simulator_rpc() {
  my_receiver my_generator = generator_pick_receiver(copp::make_channel<int>());

  auto value1 = co_await my_generator;
  std::cout << "co_await named channel receiver: " << value1 << std::endl;
  auto value2 = co_await generator_pick_receiver(copp::make_channel<int>());
  std::cout << "co_await temporary channel receiver: " << value2 << std::endl;

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

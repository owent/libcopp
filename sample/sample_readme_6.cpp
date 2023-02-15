// Copyright 2023 owent
// Created by owent on 2022-05-27

#include <iostream>

// include manager header file
#include <libcopp/coroutine/callable_promise.h>

#if defined(LIBCOPP_MACRO_ENABLE_STD_COROUTINE) && LIBCOPP_MACRO_ENABLE_STD_COROUTINE

static copp::callable_future<int> coroutine_callable_with_int_result() {
  // ... any code
  co_return 123;
}

static copp::callable_future<void> coroutine_callable_with_void_result() {
  // ... any code
  co_return;
}

static copp::callable_future<void> coroutine_simulator_task() {
  // suspend and wait custom waker
  (void)co_await LIBCOPP_MACRO_STD_COROUTINE_NAMESPACE suspend_always();
  // ... any code
  // We can get current status by co_yield yield_status()
  auto current_status = co_yield copp::callable_future<int>::yield_status();
  // The return value will be ignored when the future is already set by custom waker
  std::cout << "Current coroutine callable status: " << static_cast<uint32_t>(current_status) << std::endl;

  co_await coroutine_callable_with_void_result();

  int result = co_await coroutine_callable_with_int_result();
  std::cout << "Coroutine int callable result: " << result << std::endl;
  co_return;
}

int main() {
  auto rpc_result = coroutine_simulator_task();

  // We should not explict call start and get_internal_handle().resume() in a real usage
  // It's only allowed to start and resume by co_wait the callable_future object
  rpc_result.get_internal_handle().resume();  // resume co_await LIBCOPP_MACRO_STD_COROUTINE_NAMESPACE suspend_always();

  std::cout << "Current coroutine callable status: " << static_cast<uint32_t>(rpc_result.get_status()) << std::endl;
  return 0;
}
#else
int main() {
  puts("this sample require cotask enabled and compiler support c++20 coroutine");
  return 0;
}
#endif

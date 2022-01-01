#include <inttypes.h>
#include <stdint.h>
#include <cstdio>
#include <cstring>
#include <iostream>

// include context header file
#include <libcopp/coroutine/coroutine_context_container.h>

// define a coroutine runner
int my_runner(void *) {
  copp::coroutine_context *addr = copp::this_coroutine::get_coroutine();

  std::cout << "cortoutine " << addr << " is running." << std::endl;

  addr->yield();

  std::cout << "cortoutine " << addr << " is resumed." << std::endl;

  return 1;
}

int main() {
  typedef copp::coroutine_context_default coroutine_type;

  // create a coroutine
  copp::coroutine_context_default::ptr_t co_obj = coroutine_type::create(my_runner);
  std::cout << "cortoutine " << co_obj << " is created." << std::endl;

  // start a coroutine
  co_obj->start();

  // yield from my_runner
  std::cout << "cortoutine " << co_obj << " is yield." << std::endl;
  co_obj->resume();

  std::cout << "cortoutine " << co_obj << " exit and return " << co_obj->get_ret_code() << "." << std::endl;
  return 0;
}
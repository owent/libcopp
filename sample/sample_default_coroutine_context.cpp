/*
 * sample_default_manager.cpp
 *
 *  Created on: 2014年3月17日
 *      Author: owent
 *
 *  Released under the MIT license
 */

#include <inttypes.h>
#include <stdint.h>
#include <cstdio>
#include <cstring>
#include <iostream>

// include manager header file
#include <libcopp/coroutine/coroutine_context_container.h>

// define a coroutine runner
class my_runner {
 public:
  int operator()(void *) {
    // ... your code here ...printf("cortoutine %" PRIxPTR " exit and return %d.\n", (intptr_t)&co_obj,
    // co_obj.get_ret_code());
    copp::coroutine_context_default *addr = copp::this_coroutine::get<copp::coroutine_context_default>();
    std::cout << "cortoutine " << addr << " is running." << std::endl;

    addr->yield();
    std::cout << "cortoutine " << addr << " is resumed." << std::endl;

    return 1;
  }
};

int main() {
  // create a coroutine
  copp::coroutine_context_default::ptr_t co_obj = copp::coroutine_context_default::create(my_runner());
  std::cout << "cortoutine " << co_obj.get() << " is created." << std::endl;

  // start a coroutine
  co_obj->start();

  // yield from runner
  std::cout << "cortoutine " << co_obj.get() << " is yield." << std::endl;
  co_obj->resume();

  std::cout << "cortoutine " << co_obj.get() << " exit and return " << co_obj->get_ret_code() << "." << std::endl;
  return 0;
}

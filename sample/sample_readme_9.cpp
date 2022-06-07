/*
 * sample_readme_10.cpp
 *
 *  Created on: 2020-05-20
 *      Author: owent
 *
 *  Released under the MIT license
 */

#include <iostream>

// include manager header file
#include <libcopp/coroutine/callable_promise.h>
#include <libcotask/task.h>

#if defined(LIBCOPP_MACRO_ENABLE_STD_COROUTINE) && LIBCOPP_MACRO_ENABLE_STD_COROUTINE

typedef cotask::task<> my_task_t;

static copp::callable_future<int> call_for_await_cotask(my_task_t::ptr_t t) {
  if (t) {
    auto ret = co_await t;
    co_return ret;
  }

  co_return 0;
}

static int cotask_action_callback(void*) {
  int ret = 234;
  void* ptr = nullptr;
  cotask::this_task::get_task()->yield(&ptr);
  if (ptr != nullptr) {
    ret = *reinterpret_cast<int*>(ptr);
  }
  return ret;
}

int main() {
  my_task_t::ptr_t co_task = my_task_t::create(cotask_action_callback);

  auto t = call_for_await_cotask(co_task);
  co_task->start();

  int res = 345;
  co_task->resume(reinterpret_cast<void*>(&res));

  std::cout << "co_await a cotask::task and get result: " << t.get_internal_promise().data() << std::endl;
  return 0;
}
#else
int main() {
  puts("this sample require cotask enabled and compiler support c++20 coroutine");
  return 0;
}
#endif

/*
 * sample_readme_12.cpp
 *
 *  Created on: 2025-03-05
 *      Author: owent
 *
 *  Released under the MIT license
 */

#include <iostream>

// include task header file
#include <libcopp/coroutine/stackful_channel.h>
#include <libcotask/task.h>

typedef cotask::task<> my_task_t;

int main() {
  // Returns <receiver, sender>
  auto channel = copp::make_stackful_channel<int>();
  // Create a task and use channel to receive data
  {
    my_task_t::ptr_t task = my_task_t::create([&channel]() {
      std::cout << "task " << cotask::this_task::get<my_task_t>()->get_id() << " started" << std::endl;
      auto receiver = channel.first;
      int value = cotask::this_task::get<my_task_t>()->await_value(receiver);
      std::cout << "task " << cotask::this_task::get<my_task_t>()->get_id() << " resumed, got value: " << value
                << std::endl;
      return 0;
    });

    task->start();
    channel.second.set_value(42);
  }

  // Use channel and custom transform function to receive error
  channel.first.reset_value();
  {
    my_task_t::ptr_t task = my_task_t::create([&channel]() {
      std::cout << "task " << cotask::this_task::get<my_task_t>()->get_id() << " started" << std::endl;
      auto receiver = channel.first;
      int value = cotask::this_task::get<my_task_t>()->await_value(receiver, [](copp::copp_error_code) { return -5; });
      std::cout << "task " << cotask::this_task::get<my_task_t>()->get_id() << " resumed, got value: " << value
                << std::endl;
      return 0;
    });

    task->start();
    task->kill();
  }

  return 0;
}

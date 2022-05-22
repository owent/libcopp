#include <inttypes.h>
#include <stdint.h>
#include <cstdio>
#include <cstring>
#include <ctime>
#include <iostream>

// include context header file
#include <libcotask/task.h>
#include <libcotask/task_manager.h>

// create a task manager
typedef cotask::task<> my_task_t;
typedef my_task_t::ptr_t task_ptr_type;
typedef cotask::task_manager<my_task_t> mgr_t;
mgr_t::ptr_t task_mgr = mgr_t::create();

// If you task manager to manage timeout, it's important to call tick interval

void tick() {
  // the first parameter is second, and the second is nanosecond
  task_mgr->tick(time(nullptr), 0);
}

int main() {
  // create two coroutine task
  task_ptr_type co_task = my_task_t::create([]() {
    std::cout << "task " << cotask::this_task::get<my_task_t>()->get_id() << " started" << std::endl;
    cotask::this_task::get_task()->yield();
    std::cout << "task " << cotask::this_task::get<my_task_t>()->get_id() << " resumed" << std::endl;
    return 0;
  });
  task_ptr_type co_another_task = my_task_t::create([]() {
    std::cout << "task " << cotask::this_task::get<my_task_t>()->get_id() << " started" << std::endl;
    cotask::this_task::get_task()->yield();
    std::cout << "task " << cotask::this_task::get<my_task_t>()->get_id() << " resumed" << std::endl;
    return 0;
  });

  int res = task_mgr->add_task(co_task, 5, 0);  // add task and setup 5s for timeout
  if (res < 0) {
    std::cerr << "some error: " << res << std::endl;
    return res;
  }

  res = task_mgr->add_task(co_another_task);  // add task without timeout
  if (res < 0) {
    std::cerr << "some error: " << res << std::endl;
    return res;
  }

  res = task_mgr->start(co_task->get_id());
  if (res < 0) {
    std::cerr << "start task " << co_task->get_id() << " failed, error code: " << res << std::endl;
  }

  res = task_mgr->start(co_another_task->get_id());
  if (res < 0) {
    std::cerr << "start task " << co_another_task->get_id() << " failed, error code: " << res << std::endl;
  }

  res = task_mgr->resume(co_task->get_id());
  if (res < 0) {
    std::cerr << "resume task " << co_task->get_id() << " failed, error code: " << res << std::endl;
  }

  res = task_mgr->kill(co_another_task->get_id());
  if (res < 0) {
    std::cerr << "kill task " << co_another_task->get_id() << " failed, error code: " << res << std::endl;
  } else {
    std::cout << "kill task " << co_another_task->get_id() << " finished." << std::endl;
  }

  return 0;
}
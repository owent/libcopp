#include <inttypes.h>
#include <stdint.h>
#include <cstdio>
#include <cstring>
#include <ctime>
#include <iostream>

// include context header file
#include <libcopp/stack/stack_pool.h>
#include <libcotask/task.h>

// define the stack pool type
typedef copp::stack_pool<copp::allocator::default_statck_allocator> stack_pool_t;

// define how to create coroutine context
struct sample_macro_coroutine {
  using stack_allocator_type = copp::allocator::stack_allocator_pool<stack_pool_t>;
  using coroutine_type = copp::coroutine_context_container<stack_allocator_type>;
  using value_type = int;
};

// create a stack pool
static stack_pool_t::ptr_t global_stack_pool = stack_pool_t::create();

typedef cotask::task<sample_macro_coroutine> sample_task_t;

int main() {
#if defined(LIBCOTASK_MACRO_ENABLED)

  global_stack_pool->set_min_stack_number(4);
  std::cout << "stack pool=> used stack number: " << global_stack_pool->get_limit().used_stack_number
            << ", used stack size: " << global_stack_pool->get_limit().used_stack_size
            << ", free stack number: " << global_stack_pool->get_limit().free_stack_number
            << ", free stack size: " << global_stack_pool->get_limit().free_stack_size << std::endl;
  // create two coroutine task
  {
    copp::allocator::stack_allocator_pool<stack_pool_t> alloc(global_stack_pool);
    sample_task_t::ptr_t co_task = sample_task_t::create(
        []() {
          std::cout << "task " << cotask::this_task::get<sample_task_t>()->get_id() << " started" << std::endl;
          cotask::this_task::get_task()->yield();
          std::cout << "task " << cotask::this_task::get<sample_task_t>()->get_id() << " resumed" << std::endl;
          return 0;
        },
        alloc);

    if (!co_task) {
      std::cerr << "create coroutine task with stack pool failed" << std::endl;
      return 0;
    }

    std::cout << "stack pool=> used stack number: " << global_stack_pool->get_limit().used_stack_number
              << ", used stack size: " << global_stack_pool->get_limit().used_stack_size
              << ", free stack number: " << global_stack_pool->get_limit().free_stack_number
              << ", free stack size: " << global_stack_pool->get_limit().free_stack_size << std::endl;

    // ..., then do anything you want to do with these tasks
  }

  std::cout << "stack pool=> used stack number: " << global_stack_pool->get_limit().used_stack_number
            << ", used stack size: " << global_stack_pool->get_limit().used_stack_size
            << ", free stack number: " << global_stack_pool->get_limit().free_stack_number
            << ", free stack size: " << global_stack_pool->get_limit().free_stack_size << std::endl;

  {
    copp::allocator::stack_allocator_pool<stack_pool_t> alloc(global_stack_pool);
    sample_task_t::ptr_t co_another_task = sample_task_t::create(
        []() {
          std::cout << "task " << cotask::this_task::get<sample_task_t>()->get_id() << " started" << std::endl;
          cotask::this_task::get_task()->yield();
          std::cout << "task " << cotask::this_task::get<sample_task_t>()->get_id() << " resumed" << std::endl;
          return 0;
        },
        alloc);

    if (!co_another_task) {
      std::cerr << "create coroutine task with stack pool failed" << std::endl;
      return 0;
    }

    // ..., then do anything you want to do with these tasks
  }

  std::cout << "stack pool=> used stack number: " << global_stack_pool->get_limit().used_stack_number
            << ", used stack size: " << global_stack_pool->get_limit().used_stack_size
            << ", free stack number: " << global_stack_pool->get_limit().free_stack_number
            << ", free stack size: " << global_stack_pool->get_limit().free_stack_size << std::endl;
#else
  std::cerr << "this sample require cotask enabled." << std::endl;
#endif
  return 0;
}
USAGE
=====

> Just include headers and linking library file of your platform to use libcopp

Get Start & Example
-------

### coroutine_context example
This is a simple example of using basic coroutine context below:

```cpp
#include <cstdio>
#include <cstring>
#include <inttypes.h>
#include <iostream>
#include <stdint.h>

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
    typedef copp::coroutine_context_default coroutine_t;

    // create a coroutine
    copp::coroutine_context_default::ptr_t co_obj = coroutine_t::create(my_runner);
    std::cout << "cortoutine " << co_obj << " is created." << std::endl;

    // start a coroutine
    co_obj->start();

    // yield from my_runner
    std::cout << "cortoutine " << co_obj << " is yield." << std::endl;
    co_obj->resume();

    std::cout << "cortoutine " << co_obj << " exit and return " << co_obj->get_ret_code() << "." << std::endl;
    return 0;
}
```

Also, you can use copp::coroutine_context_container<ALLOCATOR> instead of copp::coroutine_context_default to use a different stack allocator.

### coroutine task example
This is a simple example of using coroutine task with lambda expression:

```cpp
#include <iostream>

// include task header file
#include <libcotask/task.h>

typedef cotask::task<> my_task_t;

int main(int argc, char *argv[]) {
#if defined(UTIL_CONFIG_COMPILER_CXX_LAMBDAS) && UTIL_CONFIG_COMPILER_CXX_LAMBDAS
    // create a task using factory function [with lambda expression]
    my_task_t::ptr_t task = my_task_t::create([]() {
        std::cout << "task " << cotask::this_task::get<my_task_t>()->get_id() << " started" << std::endl;
        cotask::this_task::get_task()->yield();
        std::cout << "task " << cotask::this_task::get<my_task_t>()->get_id() << " resumed" << std::endl;
        return 0;
    });

    std::cout << "task " << task->get_id() << " created" << std::endl;
    // start a task
    task->start();

    std::cout << "task " << task->get_id() << " yield" << std::endl;
    task->resume();
    std::cout << "task " << task->get_id() << " stoped, ready to be destroyed." << std::endl;
#else
    std::cerr << "lambda not supported, this sample is not available." << std::endl;
#endif
    return 0;
}
```
Also, you can your stack allocator or id allocator by setting different parameters in template class **cotask::task<TCO_MACRO, TTASK_MACRO>**

### using coroutine task manager
This is a simple example of using task manager:

```cpp
#include <cstdio>
#include <cstring>
#include <ctime>
#include <inttypes.h>
#include <iostream>
#include <stdint.h>

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
    task_mgr->tick(time(NULL), 0);
}

int main() {
#if defined(UTIL_CONFIG_COMPILER_CXX_LAMBDAS) && UTIL_CONFIG_COMPILER_CXX_LAMBDAS
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


    int res = task_mgr->add_task(co_task, 5, 0); // add task and setup 5s for timeout
    if (res < 0) {
        std::cerr << "some error: " << res << std::endl;
        return res;
    }

    res = task_mgr->add_task(co_another_task); // add task without timeout
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

#else
    std::cerr << "lambda not supported, this sample is not available." << std::endl;
#endif
    return 0;
}
```

### using stack pool
This is a simple example of using stack pool for cotask:

```cpp
#include <cstdio>
#include <cstring>
#include <ctime>
#include <inttypes.h>
#include <iostream>
#include <stdint.h>

// include context header file
#include <libcopp/stack/stack_pool.h>
#include <libcotask/task.h>

// define the stack pool type
typedef copp::stack_pool<copp::allocator::default_statck_allocator> stack_pool_t;

// define how to create coroutine context
struct sample_macro_coroutine {
    typedef copp::allocator::stack_allocator_pool<stack_pool_t> stack_allocator_t;
    typedef copp::coroutine_context_container<stack_allocator_t> coroutine_t;
};

// create a stack pool
static stack_pool_t::ptr_t global_stack_pool = stack_pool_t::create();

typedef cotask::task<sample_macro_coroutine> sample_task_t;

int main() {
#if defined(UTIL_CONFIG_COMPILER_CXX_LAMBDAS) && UTIL_CONFIG_COMPILER_CXX_LAMBDAS

    std::cout << "stack pool=> used stack number: " << global_stack_pool->get_limit().used_stack_number
              << ", used stack size: " << global_stack_pool->get_limit().used_stack_size
              << "free stack number: " << global_stack_pool->get_limit().free_stack_number
              << ", free stack size: " << global_stack_pool->get_limit().free_stack_size << std::endl;
    // create two coroutine task
    {
        sample_task_t::ptr_t co_task = sample_task_t::create([]() {
            std::cout << "task " << cotask::this_task::get<sample_task_t>()->get_id() << " started" << std::endl;
            cotask::this_task::get_task()->yield();
            std::cout << "task " << cotask::this_task::get<sample_task_t>()->get_id() << " resumed" << std::endl;
            return 0;
        });

        if (!co_task) {
            std::cerr << "create coroutine task with stack pool failed" << std::endl;
            return 0;
        }

        std::cout << "stack pool=> used stack number: " << global_stack_pool->get_limit().used_stack_number
                  << ", used stack size: " << global_stack_pool->get_limit().used_stack_size
                  << "free stack number: " << global_stack_pool->get_limit().free_stack_number
                  << ", free stack size: " << global_stack_pool->get_limit().free_stack_size << std::endl;


        // ..., then do anything you want to do with these tasks
    }

    std::cout << "stack pool=> used stack number: " << global_stack_pool->get_limit().used_stack_number
              << ", used stack size: " << global_stack_pool->get_limit().used_stack_size
              << "free stack number: " << global_stack_pool->get_limit().free_stack_number
              << ", free stack size: " << global_stack_pool->get_limit().free_stack_size << std::endl;

    {
        sample_task_t::ptr_t co_another_task = sample_task_t::create([]() {
            std::cout << "task " << cotask::this_task::get<sample_task_t>()->get_id() << " started" << std::endl;
            cotask::this_task::get_task()->yield();
            std::cout << "task " << cotask::this_task::get<sample_task_t>()->get_id() << " resumed" << std::endl;
            return 0;
        });

        if (!co_another_task) {
            std::cerr << "create coroutine task with stack pool failed" << std::endl;
            return 0;
        }

        // ..., then do anything you want to do with these tasks
    }

    std::cout << "stack pool=> used stack number: " << global_stack_pool->get_limit().used_stack_number
              << ", used stack size: " << global_stack_pool->get_limit().used_stack_size
              << "free stack number: " << global_stack_pool->get_limit().free_stack_number
              << ", free stack size: " << global_stack_pool->get_limit().free_stack_size << std::endl;
#else
    std::cerr << "lambda not supported, this sample is not available." << std::endl;
#endif
    return 0;
}
```

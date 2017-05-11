USAGE
=====

> Just include headers and linking library file of your platform to use libcopp

Get Start & Example
-------

### coroutine_context example
There is a simple example of using coroutine context below:

```cpp
#include <cstdio>
#include <cstring>
#include <iostream>
#include <inttypes.h>
#include <stdint.h>

// include context header file
#include <libcopp/coroutine/coroutine_context_container.h>

// define a coroutine runner
class my_runner : public copp::detail::coroutine_runnable_base
{
public:
    int operator()() {
        // ... your code here ...printf("cortoutine %" PRIxPTR " exit and return %d.\n", (intptr_t)&co_obj, co_obj.get_ret_code());
        copp::coroutine_context_default* addr = get_coroutine_context<copp::coroutine_context_default>();
        std::cout<< "cortoutine "<< addr<< " is running."<< std::endl;

        addr->yield();
        std::cout<< "cortoutine "<< addr<< " is resumed."<< std::endl;

        return 1;
    }
};

int main() {
    // create a coroutine
    copp::coroutine_context_default co_obj;
    std::cout<< "cortoutine "<< &co_obj<< " is created."<< std::endl;

    // create a runner
    my_runner runner;

    // bind runner to coroutine object
    co_obj.create(&runner);

    // start a coroutine
    co_obj.start();

    // yield from runner
    std::cout<< "cortoutine "<< &co_obj<< " is yield."<< std::endl;
    co_obj.resume();

    std::cout<< "cortoutine "<< &co_obj<< " exit and return "<< co_obj.get_ret_code()<< "."<< std::endl;
    return 0;
}
```

And then, you can custom many function such as set your stack allocator, coroutine type and etc. by set your template parameters of coroutine context.
***Notice:*** *One coroutine runner can only below to one coroutine context*


### coroutine task example
There is a simple example of using coroutine task below:

```cpp
#include <iostream>

// include task header file
#include <libcotask/task.h>

typedef cotask::task<> my_task_t;

int main(int argc, char* argv[]) {
    // create a task using factory function [with lambda expression]
    my_task_t::prt_t task = my_task_t::create([](){
        std::cout<< "task "<< cotask::this_task::get_task()->get_id()<< " started"<< std::endl;
        cotask::this_task::get_task()->yield();
        std::cout<< "task "<< cotask::this_task::get_task()->get_id()<< " resumed"<< std::endl;
        return 0;
    });
    
    std::cout<< "task "<< task->get_id()<< " created"<< std::endl;
    // start a task
    task->start();

    std::cout<< "task "<< task->get_id()<< " yield"<< std::endl;
    task->resume();
    std::cout<< "task "<< task->get_id()<< " stoped, ready to be destroyed."<< std::endl;

    return 0;
}
```
And then, you can custom many functions by set your macro type of coroutine and task to do some other function.

### using coroutine task manager
There is a simple example of using coroutine context below:

```cpp
#include <cstdio>
#include <cstring>
#include <iostream>
#include <inttypes.h>
#include <stdint.h>
#include <ctime>

// include context header file
#include <libcotask/task.h>
#include <libcotask/task_manager.h>

// create a task manager
typedef cotask::task_manager<cotask::task<>::id_t> mgr_t;
mgr_t::ptr_t task_mgr = mgr_t::create();

// If you task manager to manage timeout, it's important to call tick interval

void tick() {
    // the first parameter is second, and the second is nanosecond
    task_mgr->tick(time(NULL), 0);
}

int main() {
    // type define 
    typedef std::shared_ptr<cotask::task<> > task_ptr_type;

    // create two coroutine task
    task_ptr_type co_task = cotask::task<>::create([](){
        std::cout<< "task "<< cotask::this_task::get_task()->get_id()<< " started"<< std::endl;
        cotask::this_task::get_task()->yield();
        std::cout<< "task "<< cotask::this_task::get_task()->get_id()<< " resumed"<< std::endl;
        return 0;
    });
    task_ptr_type co_another_task = cotask::task<>::create([](){
        std::cout<< "task "<< cotask::this_task::get_task()->get_id()<< " started"<< std::endl;
        cotask::this_task::get_task()->yield();
        std::cout<< "task "<< cotask::this_task::get_task()->get_id()<< " resumed"<< std::endl;
        return 0;
    });


    int res = task_mgr->add_task(co_task, 5, 0); // add task and setup 5s for timeout
    if (res < 0) {
        std::cerr<< "some error: "<< res<< std::endl;
        return res;
    }

    res = task_mgr->add_task(co_another_task); // add task without timeout
    if (res < 0) {
        std::cerr<< "some error: "<< res<< std::endl;
        return res;
    }

    res = task_mgr->start(co_task->get_id());
    if (res < 0) {
        std::cerr<< "start task "<< co_task->get_id()<< " failed, error code: "<< res<< std::endl;
    }

    res = task_mgr->start(co_another_task->get_id());
    if (res < 0) {
        std::cerr<< "start task "<< co_another_task->get_id()<< " failed, error code: "<< res<< std::endl;
    }

    res = task_mgr->resume(co_task->get_id());
    if (res < 0) {
        std::cerr<< "resume task "<< co_task->get_id()<< " failed, error code: "<< res<< std::endl;
    }

    res = task_mgr->kill(co_another_task->get_id());
    if (res < 0) {
        std::cerr<< "kill task "<< co_another_task->get_id()<< " failed, error code: "<< res<< std::endl;
    } else {
        std::cout<< "kill task "<< co_another_task->get_id()<< " finished."<< std::endl;
    }
    return 0;
}
```


### using stack pool
There is a simple example of using coroutine context below:

```cpp
#include <cstdio>
#include <cstring>
#include <iostream>
#include <inttypes.h>
#include <stdint.h>
#include <ctime>

// include context header file
#include <libcopp/stack/stack_pool.h>
#include <libcotask/task.h>

// define the stack pool type
typedef copp::stack_pool<copp::allocator::default_statck_allocator> stack_pool_t;

// define how to create coroutine context
struct sample_macro_coroutine {
    typedef copp::detail::coroutine_context_base coroutine_t;
    typedef copp::allocator::stack_allocator_pool<stack_pool_t> stack_allocator_t;

    typedef copp::detail::coroutine_context_container<coroutine_t, stack_allocator_t> coroutine_container_t;
};

// create a stack pool
static stack_pool_t::ptr_t global_stack_pool = stack_pool_t::create();

// define how to task allocator
struct sample_task_allocator : public cotask::macro_task::task_allocator_t {
public:
#if defined(COPP_MACRO_ENABLE_VARIADIC_TEMPLATE) && COPP_MACRO_ENABLE_VARIADIC_TEMPLATE
    template <typename Ty, typename... TARGS>
    static std::shared_ptr<Ty> allocate(Ty *t, TARGS COPP_MACRO_RV_REF... args) {
        std::shared_ptr<Ty> ret = cotask::macro_task::task_allocator_t::allocate(t, COPP_MACRO_STD_FORWARD(TARGS, args)...);
        ret->get_coroutine_context().get_allocator().attach(global_stack_pool);
        return COPP_MACRO_STD_MOVE(ret);
    }
#else
    template <typename Ty>
    static std::shared_ptr<Ty> allocate(Ty *t) {
        std::shared_ptr<Ty> ret = cotask::macro_task::task_allocator_t::allocate(t);
        ret->get_coroutine_context().get_allocator().attach(global_stack_pool);
        return COPP_MACRO_STD_MOVE(ret);
    }
#endif
};

// define how to create coroutine task
struct sample_macro_task {
    typedef cotask::macro_task::id_t id_t;
    typedef cotask::macro_task::id_allocator_t id_allocator_t;

    typedef cotask::macro_task::action_allocator_t action_allocator_t;
    typedef sample_task_allocator task_allocator_t;
};

typedef cotask::task<sample_macro_coroutine, sample_macro_task> sample_task_t;

int main() {
    // create two coroutine task
    sample_task_t::ptr_t co_task = cotask::task<>::create([](){
        std::cout<< "task "<< cotask::this_task::get_task()->get_id()<< " started"<< std::endl;
        cotask::this_task::get_task()->yield();
        std::cout<< "task "<< cotask::this_task::get_task()->get_id()<< " resumed"<< std::endl;
        return 0;
    });
    sample_task_t::ptr_t co_another_task = cotask::task<>::create([](){
        std::cout<< "task "<< cotask::this_task::get_task()->get_id()<< " started"<< std::endl;
        cotask::this_task::get_task()->yield();
        std::cout<< "task "<< cotask::this_task::get_task()->get_id()<< " resumed"<< std::endl;
        return 0;
    });

    if (!co_task) {
        std::cerr<< "create coroutine task with stack pool failed"<< std::endl;
        return 0;
    }

    if (!co_another_task) {
        std::cerr<< "create coroutine task with stack pool failed"<< std::endl;
        return 0;
    }

    // ..., then do anything you want to do with these tasks
    return 0;
}
```


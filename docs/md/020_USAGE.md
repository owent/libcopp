USAGE
=====

> Just include headers and linking library file of your platform to use libcopp

Example
-------

### coroutine_context example
There is a simple example of using coroutine context below:

``` {.cpp}
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

``` {.cpp}
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



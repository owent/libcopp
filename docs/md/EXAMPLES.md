
EXAMPLES
----------------

### coroutine_context example

This is a simple example of using basic coroutine context below:

~~~~~~~~~~cpp
// see https://github.com/owt5008137/libcopp/blob/v2/sample/sample_readme_1.cpp
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
~~~~~~~~~~

Also, you can use copp::coroutine_context_container<ALLOCATOR> instead of copp::coroutine_context_default to use a different stack allocator.

### coroutine task example

This is a simple example of using coroutine task with lambda expression:

~~~~~~~~~~cpp
// see https://github.com/owt5008137/libcopp/blob/v2/sample/sample_readme_2.cpp
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
~~~~~~~~~~
Also, you can your stack allocator or id allocator by setting different parameters in template class **cotask::task<TCO_MACRO>**

### using coroutine task manager

This is a simple example of using task manager:

~~~~~~~~~~cpp
// see https://github.com/owt5008137/libcopp/blob/v2/sample/sample_readme_3.cpp
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
~~~~~~~~~~

### using stack pool

This is a simple example of using stack pool for cotask:

~~~~~~~~~~cpp
// see https://github.com/owt5008137/libcopp/blob/v2/sample/sample_readme_4.cpp
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
    std::cerr << "lambda not supported, this sample is not available." << std::endl;
#endif
    return 0;
}
~~~~~~~~~~

### using then or await_task

This is a simple example of using ```then``` and ```await_task``` for cotask:

~~~~~~~~~~cpp
#include <assert.h>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <ctime>
#include <inttypes.h>
#include <stdint.h>
#include <vector>

// include manager header file
#include <libcotask/task.h>

#if defined(LIBCOTASK_MACRO_ENABLED) && defined(UTIL_CONFIG_COMPILER_CXX_LAMBDAS) && UTIL_CONFIG_COMPILER_CXX_LAMBDAS

typedef cotask::task<> my_task_t;

int main(int argc, char *argv[]) {
    int test_code = 128;

    // create a task using lambda expression
    my_task_t::ptr_t first_task = my_task_t::create([&]() {
        puts("|first task running and will be yield ...");
        cotask::this_task::get_task()->yield();
        puts("|first task resumed ...");
        printf("test code already reset => %d\n", ++test_code);
    });

    // add many then task using lambda expression
    first_task
        ->then([=]() {
            puts("|second task running...");
            printf("test code should be inited 128 => %d\n", test_code);
        })
        ->then([&]() {
            puts("|haha ... this is the third task.");
            printf("test code is the same => %d\n", ++test_code);
            return "return value will be ignored";
        })
        ->then(
            [&](void *priv_data) {
                puts("|it's boring");
                printf("test code is %d\n", ++test_code);
                assert(&test_code == priv_data);
                return 0;
            },
            &test_code);

    test_code = 0;
    // start a task
    first_task->start();
    first_task->resume();

    // these code below will failed.
    first_task->then([]() {
        puts("this will run immediately.");
        return 0;
    });

    my_task_t::ptr_t await_task = my_task_t::create([&]() {
        puts("await_task for first_task.");
        return 0;
    });
    await_task->await_task(first_task);

    printf("|task start twice will failed: %d\n", first_task->start());
    printf("|test_code end with %d\n", test_code);
    return 0;
}
#else
int main() {
    puts("this sample require cotask enabled and compiler support c++11");
    return 0;
}
#endif
~~~~~~~~~~

### using copp::future::future_t and prepare for c++20 coroutine

This is a simple example of using copp::future::future_t<RESULT> and using copp::future::context<RESULT>:

~~~~~~~~~~cpp
#include <assert.h>
#include <iostream>
#include <list>

// include manager header file
#include <libcopp/future/future.h>
#include <libcopp/future/context.h>

struct example_result_message_t {
    int return_code;
    int response_code;
};
struct example_poller;

typedef copp::future::result_t<
    example_result_message_t, // polled data if success
    int                       // error code if failed
> example_result_t;
typedef copp::future::future_t<example_result_t> example_future_t;
typedef copp::future::context_t<example_poller> example_context_t;

std::list<example_context_t*> g_executor;

struct example_poller {
    example_result_t::storage_type* result;
    std::list<example_context_t*>::iterator iter;
    example_poller(): result(NULL) {
        iter = g_executor.end();
    }
    ~example_poller() {
        // cleanup and remove from executor
        if (iter != g_executor.end()) {
            g_executor.erase(iter);
        }
    }
    void operator()(example_future_t& future, example_context_t &ctx) {
        if (NULL != result) {
            future.poll_data() = *result;
            result = NULL;
            // remove from executor
            if (iter != g_executor.end()) {
                g_executor.erase(iter);
                iter = g_executor.end();
            }
        } else {
            // add to executor
            if (iter == g_executor.end()) {
                iter = g_executor.insert(g_executor.end(), &ctx);
            }
        }
    }
};

void run_with_custom_context () {
    example_future_t future;
    example_context_t context;

    assert(future.is_ready() == false);

    // poll context for the first time and setup waker
    // future should only poll one context
    future.poll(context);

    while (!g_executor.empty()) {
        example_result_message_t msg;
        // set a result message
        msg.return_code = 0;
        msg.response_code = 200;
        // if both success type and error type is a small trivial type, storage_type will be result_t with union of success type and error type
        // else storage_type will be std::unique_ptr<result_t>
        // result_t::make_success(...) and result_t::make_error(...) will make sure to use the correct storage type
        example_result_t::storage_type result_storage = example_result_t::make_success(msg);
        (*g_executor.begin())->get_private_data().result = &result_storage;
        // just call context_t::wake to wakeup and poll again
        (*g_executor.begin())->wake();
    }

    // Then future is ready
    assert(future.is_ready() == true);
    example_result_t* result = future.data();
    assert(result != NULL);
    
    assert(result->is_success());
    assert(200 == result->get_success()->response_code);
    assert(false == result->is_error());
    assert(NULL == result->get_error());

    std::cout<< "Got future success response code: "<< result->get_success()->response_code<< std::endl;
}

static void custom_poller_function(copp::future::context_t<void> & ctx, copp::future::context_t<void>::poll_event_data_t evt_data) {
    if (NULL == evt_data.private_data) {
        return;
    }

    example_future_t* future = reinterpret_cast<example_future_t*>(evt_data.future_ptr);

    example_result_message_t* msg = reinterpret_cast<example_result_message_t*>(evt_data.private_data);

    // if both success type and error type is a small trivial type, storage_type will be result_t with union of success type and error type
    // else storage_type will be std::unique_ptr<result_t>
    // result_t::make_success(...) and result_t::make_error(...) will make sure to use the correct storage type
    future->poll_data() = example_result_t::make_success(*msg);
}

void run_with_void_context () {
    example_future_t future;
    copp::future::context_t<void> context(copp::future::context_t<void>::construct(custom_poller_function));
    // upper code equal to:
    // copp::future::context_t<void> context;
    // context.set_poll_fn(custom_poller_function);

    assert(future.is_ready() == false);

    // poll context for the first time and setup waker
    // future should only poll one context
    future.poll(context);

    while (!future.is_ready()) {
        example_result_message_t msg;
        // set a result message
        msg.return_code = 0;
        msg.response_code = 200;
        context.set_private_data(reinterpret_cast<void*>(&msg));
        
        context.wake();
    }

    // Then future is ready
    assert(future.is_ready() == true);
    example_result_t* result = future.data();
    assert(result != NULL);
    
    assert(result->is_success());
    assert(200 == result->get_success()->response_code);
    assert(false == result->is_error());
    assert(NULL == result->get_error());

    std::cout<< "Got future success response code(context_t<void>): "<< result->get_success()->response_code<< std::endl;
}

int main() {
    run_with_custom_context();
    run_with_void_context();
    return 0;
}
~~~~~~~~~~

### using c++20 coroutine

~~~~~~~~~~cpp
#include <iostream>

// include manager header file
#include <libcopp/future/std_coroutine_task.h>

#if defined(LIBCOPP_MACRO_ENABLE_STD_COROUTINE) && LIBCOPP_MACRO_ENABLE_STD_COROUTINE

static copp::future::task_t<int> call_for_coroutine_task_with_int_result() {
    // ... any code
    co_return 123;
}

static copp::future::task_t<void> call_for_coroutine_task_with_void_result() {
    // ... any code
    co_return;
}

struct sample_task_waker_t;
typedef copp::future::task_t<int, sample_task_waker_t>    sample_task_t;
typedef copp::future::task_future_t<int>                  sample_future_t;
typedef copp::future::task_context_t<sample_task_waker_t> sample_task_context_t;

std::list<std::pair<sample_task_context_t *,int> > g_sample_executor;

struct sample_task_waker_t {
    std::list<std::pair<sample_task_context_t *,int> >::iterator refer_to;

    sample_task_waker_t() {
        refer_to = g_sample_executor.end();
    }

    ~sample_task_waker_t() {
        if (refer_to != g_sample_executor.end()) {
            g_sample_executor.erase(refer_to);
        }
    }

    void operator()(sample_future_t &fut, sample_task_context_t &ctx) {
        if (refer_to == g_sample_executor.end()) {
            // Add to custom executor when first polled
            refer_to = g_sample_executor.insert(
                g_sample_executor.end(), 
                std::make_pair(&ctx, 0)
            );
            return;
        }

        if (0 != (*refer_to).second) {
            fut.poll_data() = (*refer_to).second;
            // Because return type is a trivial type, we can just assign to value
            // It the return type is a trivial type, we can use 
            //     fut.poll_data() = copp::future::make_unique<T>(...);
            //   or 
            //     fut.poll_data() = std::make_unique<T>(...);
            //   to set the result data.
            g_sample_executor.erase(refer_to);
            refer_to = g_sample_executor.end();
        }
    }
};

static sample_task_t call_for_coroutine_task_with_custom_waker() {
    // suspend and wait custom waker
    co_await LIBCOPP_MACRO_STD_COROUTINE_NAMESPACE suspend_always();
    // ... any code
    // We can get the pointer to the future and th context of current task by co_yield current_future and current_context
    sample_future_t* future = co_yield sample_task_t::current_future();
    sample_task_context_t* context = co_yield sample_task_t::current_context();
    if (future && context && future->is_ready()) {
        // The return value will be ignored when the future is already set by custom waker
        std::cout<< "Coroutine: "<< context->get_task_id()<< " already got future data "<< (*future->data())<< " and will ignore co_return."<< std::endl;
    }
    co_return 123;
}

int main() {
    copp::future::task_t<int> t1 = call_for_coroutine_task_with_int_result();
    copp::future::task_t<void> t2 = call_for_coroutine_task_with_void_result();
    sample_task_t t3 = call_for_coroutine_task_with_custom_waker();
    std::cout<< "Coroutine t1: "<< t1.get_task_id()<< " -> "<< *t1.data()<< std::endl;
    std::cout<< "Coroutine t2: "<< t2.get_task_id()<< " -> "<< (t2.done()? "done": "running")<< std::endl;

    while (!g_sample_executor.empty()) {
        (*g_sample_executor.begin()).second = 456;
        (*g_sample_executor.begin()).first->wake();
    }

    std::cout<< "Coroutine t3: "<< t3.get_task_id()<< " -> "<< *t3.data()<< std::endl;
    return 0;
}
#else
int main() {
    puts("this sample require cotask enabled and compiler support c++20 coroutine");
    return 0;
}
#endif
~~~~~~~~~~

### using c++20 coroutine with custom generator

~~~~~~~~~~cpp
/*
 * sample_readme_8.cpp
 *
 *  Created on: 2020-05-22
 *      Author: owent
 *
 *  Released under the MIT license
 */

#include <assert.h>
#include <iostream>
#include <string>

// include manager header file
#include <libcopp/future/std_coroutine_task.h>
#include <libcopp/future/std_coroutine_generator.h>

#if defined(LIBCOPP_MACRO_ENABLE_STD_COROUTINE) && LIBCOPP_MACRO_ENABLE_STD_COROUTINE

struct sample_message_t {
    int ret_code;
    std::string response;
};

struct sample_generator_waker_t;

typedef copp::future::result_t<sample_message_t, int32_t> sample_result_t;
typedef copp::future::task_t<sample_result_t>             sample_task_t;
typedef copp::future::generator_future_t<sample_result_t> sample_future_t;
typedef copp::future::generator_context_t<sample_generator_waker_t> sample_generator_context_t;


std::list<std::pair<sample_generator_context_t *, std::string> > g_sample_executor;

struct sample_generator_waker_t {
    int32_t code;
    std::list<std::pair<sample_generator_context_t *, std::string> >::iterator refer_to;

    // All parameter passed into generator will be forward here
    sample_generator_waker_t(int32_t c) : code(c) {
        refer_to = g_sample_executor.end();
    }

    ~sample_generator_waker_t() {
        if (refer_to != g_sample_executor.end()) {
            g_sample_executor.erase(refer_to);
        }
    }

    void operator()(sample_future_t &fut, sample_generator_context_t &ctx) {
        if (refer_to == g_sample_executor.end()) {
            // Add to custom executor when first polled
            refer_to = g_sample_executor.insert(
                g_sample_executor.end(), 
                std::make_pair(&ctx, std::string())
            );
            return;
        }

        if (!(*refer_to).second.empty()) {
            // generator finished and produce a result message
            sample_message_t msg;
            msg.ret_code    = code;
            msg.response.swap((*refer_to).second);
            fut.poll_data() = sample_result_t::make_success(msg);

            g_sample_executor.erase(refer_to);
            refer_to = g_sample_executor.end();
        }
    }
};
typedef copp::future::generator_t<sample_result_t, sample_generator_waker_t> sample_generator_t;

static copp::future::task_t<void> call_for_noop_task() {
    co_return;
}

static copp::future::task_t<int> call_for_coroutine_task() {
    // We can start a subtask and await it
    copp::future::task_t<void> t = call_for_noop_task();
    co_await t;
    
    sample_generator_t generator = copp::future::make_generator<sample_generator_t>(200);
    auto result = co_await generator;

    if (result) {
        if (result->is_success()) {
            std::cout<< "Got response message: "<< result->get_success()->response<< std::endl;
            co_return result->get_success()->ret_code;    
        } else {
            co_return *result->get_error();
        }
    }
    co_return 0;
}

int main() {
    copp::future::task_t<int> t = call_for_coroutine_task();
    assert(false == t.done());
    assert(NULL == t.data());  // Task isn't finished and has no data

    while (!g_sample_executor.empty()) {
        // async jobs finished and wake coroutine here
        g_sample_executor.begin()->second = "Hello World!";
        g_sample_executor.begin()->first->wake();
    }

    assert(t.done());
    assert(t.data()); // Task is finished and has data
    std::cout<< "Task "<< t.get_task_id()<< " finished and got result: "<< *t.data()<< std::endl;
    return 0;
}
#else
int main() {
    puts("this sample require cotask enabled and compiler support c++20 coroutine");
    return 0;
}
#endif
~~~~~~~~~~

### custom error (timeout for example) when polling c++20 coroutine task or generator

By add context into custom executor or manager and remove it when destroyed, we can add more flexible error handling or procedure.

~~~~~~~~~~cpp
#include <iostream>

// include manager header file
#include <libcopp/future/std_coroutine_task.h>
#include <libcopp/future/std_coroutine_generator.h>

#if defined(LIBCOPP_MACRO_ENABLE_STD_COROUTINE) && LIBCOPP_MACRO_ENABLE_STD_COROUTINE

// ============================ types for task and generator ============================
class sample_message_t {
private:
    sample_message_t(const sample_message_t &)
        UTIL_CONFIG_DELETED_FUNCTION;
    sample_message_t &
    operator=(const sample_message_t &) UTIL_CONFIG_DELETED_FUNCTION;
    sample_message_t(sample_message_t &&)
        UTIL_CONFIG_DELETED_FUNCTION;
    sample_message_t &
    operator=(sample_message_t &&) UTIL_CONFIG_DELETED_FUNCTION;

public:
    int ret_code;
    int rsp_code;

    sample_message_t() : ret_code(0), rsp_code(0) {}
    sample_message_t(int c) : ret_code(c), rsp_code(c) {}
    ~sample_message_t() {}
};

#define SAMPLE_TIMEOUT_ERROR_CODE (-500)

struct sample_generator_waker_t;
struct sample_task_waker_t;

typedef copp::future::result_t<sample_message_t, int32_t>           sample_result_t;
typedef copp::future::task_t<sample_result_t, sample_task_waker_t>  sample_task_t;
typedef copp::future::generator_context_t<sample_generator_waker_t> sample_generator_context_t;
typedef copp::future::generator_future_t<sample_result_t>           sample_generator_future_t;
typedef copp::future::poll_t<sample_result_t>                       sample_poll_t;
typedef copp::future::task_context_t<sample_task_waker_t>           sample_task_context_t;
typedef copp::future::task_future_t<sample_result_t>                sample_task_future_t;

std::list<sample_generator_context_t *> g_sample_generator_waker_list;
std::list<sample_task_context_t *>      g_sample_task_waker_list;

struct sample_generator_waker_t {
    int32_t                                                    code;
    int32_t                                                    await_times;
    std::list<sample_generator_context_t *>::iterator refer_to;
    sample_generator_waker_t(int32_t c, int32_t at) : code(c), await_times(at) {
        refer_to = g_sample_generator_waker_list.end();
    }

    ~sample_generator_waker_t() {
        // Remove from executor when destroyed
        if (refer_to != g_sample_generator_waker_list.end()) {
            g_sample_generator_waker_list.erase(refer_to);
        }
    }

    void operator()(sample_generator_future_t &fut, sample_generator_context_t &ctx) {
        // Remove from executor when polled
        if (refer_to != g_sample_generator_waker_list.end()) {
            g_sample_generator_waker_list.erase(refer_to);
            refer_to = g_sample_generator_waker_list.end();
        }
        if (await_times-- > 0) {
            // Add to executor when not ready
            refer_to = g_sample_generator_waker_list.insert(
                g_sample_generator_waker_list.end(), &ctx);
            return;
        }

        if (code > 0) {
            fut.poll_data() = sample_result_t::make_success(code);
        } else {
            fut.poll_data() = sample_result_t::make_error(code);
        }
    }
};

struct sample_task_waker_t {
    int32_t                                               code;
    std::list<sample_task_context_t *>::iterator refer_iter;

    template <class... TARGS>
    sample_task_waker_t(TARGS &&...)
        : code(0), refer_iter(g_sample_task_waker_list.end()) {}
    ~sample_task_waker_t() {
        // Remove from executor when destroyed
        if (g_sample_task_waker_list.end() != refer_iter) {
            g_sample_task_waker_list.erase(refer_iter);
        }
    }

    void operator()(sample_task_future_t &fut, sample_task_context_t &ctx) {
        // Remove from executor when polled
        if (g_sample_task_waker_list.end() != refer_iter) {
            g_sample_task_waker_list.erase(refer_iter);
            refer_iter = g_sample_task_waker_list.end();
        }

        if (0 != code) {
            fut.poll_data() = sample_result_t::make_error(code);
            return;
        }

        // Add to executor when not ready
        refer_iter = g_sample_task_waker_list.insert(
            g_sample_task_waker_list.end(), &ctx);
    }
};

// ============================ timeout for generator ============================
typedef copp::future::generator_t<sample_result_t, sample_generator_waker_t> sample_generator_t;
static sample_task_t call_for_await_generator_and_timeout(int32_t await_times) {
    std::cout<< "ready to co_await generator." << std::endl;

    auto          gen = copp::future::make_generator<sample_generator_t>(200, await_times);
    sample_poll_t ret;
    co_await      gen;

    ret = std::move(gen.poll_data());

    // the return is still pending, because it's resumed by timeout waker
    std::cout<< "\tret.is_pending() :"<< ret.is_pending() << std::endl;
    sample_task_context_t *context = co_yield sample_task_t::current_context();
    if (context) {
        std::cout<< "\texpected code: "<< SAMPLE_TIMEOUT_ERROR_CODE<< ", real is: "<< context->get_private_data().code<< std::endl;;
    }

    sample_task_future_t *fut = co_yield sample_task_t::current_future();
    if (fut) {
        std::cout<< "\tfut->is_ready() :"<< fut->is_ready() << std::endl;
        if (fut->data() && fut->data()->is_error()) {
            std::cout<< "\texpected code: "<< SAMPLE_TIMEOUT_ERROR_CODE<< ", real is: "<< *fut->data()->get_error()<< std::endl;;
        }
    }

    co_return sample_result_t::make_error(-1);
}

static void poll_generator_and_timeout() {
    sample_task_t t1 = call_for_await_generator_and_timeout(3);

    // set timeout result
    for (int retry_times = 10; retry_times >= 0 && !g_sample_task_waker_list.empty(); --retry_times) {
        (*g_sample_task_waker_list.begin())->get_private_data().code = SAMPLE_TIMEOUT_ERROR_CODE;
        // wake and resume task
        (*g_sample_task_waker_list.begin())->wake();
    }

    std::cout<< "task status should be DONE(" << ((int)sample_task_t::status_type::DONE)<< "), real is: " << ((int)t1.get_status())<< std::endl;
    if (t1.data() && t1.data()->is_error()) {
        std::cout<< "\texpected code: "<< SAMPLE_TIMEOUT_ERROR_CODE<< ", real is: "<< *t1.data()->get_error()<< std::endl;
    }

    // cleanup generator dispatcher
    for (int retry_times = 10; retry_times >= 0 && !g_sample_generator_waker_list.empty();
         --retry_times) {
        (*g_sample_generator_waker_list.begin())->wake();
    }

    // cleanup task dispatcher
    for (int retry_times = 10; retry_times >= 0 && !g_sample_generator_waker_list.empty(); --retry_times) {
        (*g_sample_generator_waker_list.begin())->wake();
    }
}

// ============================ timeout for task ============================
static sample_task_t call_for_await_task_and_timeout(int32_t await_times) {
    std::cout<< "ready to co_await task." << std::endl;

    auto t_dep = call_for_await_generator_and_timeout(await_times);
    co_await t_dep;

    // the return is still pending, because it's resumed by timeout waker
    std::cout<< "\tt_dep.done() :"<< t_dep.done() << std::endl;
    sample_task_context_t *context = co_yield sample_task_t::current_context();
    if (context) {
        std::cout<< "\texpected code: "<< SAMPLE_TIMEOUT_ERROR_CODE<< ", real is: "<< context->get_private_data().code<< std::endl;;
    }

    sample_task_future_t *fut = co_yield sample_task_t::current_future();
    if (fut) {
        std::cout<< "\tfut->is_ready() :"<< fut->is_ready() << std::endl;
        if (fut->data() && fut->data()->is_error()) {
            std::cout<< "\texpected code: "<< SAMPLE_TIMEOUT_ERROR_CODE<< ", real is: "<< *fut->data()->get_error()<< std::endl;;
        }
    }

    co_return sample_result_t::make_error(-1);
}

static void poll_no_trival_task_and_timeout() {
    sample_task_t t1 = call_for_await_task_and_timeout(3);

    // set timeout result
    for (int retry_times = 10; retry_times >= 0 && !t1.done() && t1.get_context(); --retry_times) {
        t1.get_context()->get_private_data().code = SAMPLE_TIMEOUT_ERROR_CODE;
        // wake and resume task
        t1.get_context()->wake();
    }

    std::cout<< "t1.done() :"<< t1.done() << std::endl;
    std::cout<< "\ttask "<< t1.get_task_id() <<" status should be DONE(" << ((int)sample_task_t::status_type::DONE)<< "), real is: " << ((int)t1.get_status())<< std::endl;
    if (t1.data() && t1.data()->is_error()) {
        std::cout<< "\texpected code: "<< SAMPLE_TIMEOUT_ERROR_CODE<< ", real is: "<< *t1.data()->get_error()<< std::endl;
    }

    // cleanup task dispatcher
    for (int retry_times = 10; retry_times >= 0 && !g_sample_task_waker_list.empty(); --retry_times) {
        (*g_sample_task_waker_list.begin())->get_private_data().code = SAMPLE_TIMEOUT_ERROR_CODE;
        (*g_sample_task_waker_list.begin())->wake();
    }


    // cleanup generator dispatcher
    for (int retry_times = 10; retry_times >= 0 && !g_sample_generator_waker_list.empty();
         --retry_times) {
        (*g_sample_generator_waker_list.begin())->wake();
    }
}

int main() {
    // sample for await generator and timeout
    poll_generator_and_timeout();

    // sample for await task and timeout
    poll_no_trival_task_and_timeout();
    return 0;
}
#else
int main() {
    puts("this sample require cotask enabled and compiler support c++20 coroutine");
    return 0;
}
#endif
~~~~~~~~~~

### let c++20 coroutine work with cotask::task<MACRO>

This is a simple example to let c++20 coroutine await cotask::task

~~~~~~~~~~cpp
#include <iostream>

// include manager header file
#include <libcopp/future/std_coroutine_task.h>
#include <libcotask/task.h>

#if defined(LIBCOPP_MACRO_ENABLE_STD_COROUTINE) && LIBCOPP_MACRO_ENABLE_STD_COROUTINE

typedef cotask::task<> my_task_t;

static copp::future::task_t<int> call_for_await_cotask(my_task_t::ptr_t t) {
    if (t) {
        co_return co_await t;
    }

    co_return 0;
}

static int cotask_action_callback(void *) {
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

    if (nullptr != t.data()) {
        std::cout<< "co_await a cotask::task and get result: "<< *t.data()<< std::endl;
    }
    return 0;
}
#else
int main() {
    puts("this sample require cotask enabled and compiler support c++20 coroutine");
    return 0;
}
#endif
~~~~~~~~~~

### using ```SetUnhandledExceptionFilter``` on Windows with cotask::task<MACRO>

Some applications will use ```SetUnhandledExceptionFilter``` to catch unhandled exception and analysis crash problem. But ```SetUnhandledExceptionFilter``` is only works with [coroutine context of windows fiber](include/libcopp/coroutine/coroutine_context_fiber_container.h) . This is a sample of using **windows fiber** as coroutine context in ```cotask::task<MACRO>``` .

~~~~~~~~~~cpp
#include <iostream>

#include <libcopp/utils/config/libcopp_build_features.h>

#if (defined(LIBCOTASK_MACRO_ENABLED) && LIBCOTASK_MACRO_ENABLED)&& defined(LIBCOPP_MACRO_ENABLE_WIN_FIBER) && LIBCOPP_MACRO_ENABLE_WIN_FIBER && defined(UTIL_CONFIG_COMPILER_CXX_LAMBDAS) && UTIL_CONFIG_COMPILER_CXX_LAMBDAS
// include task header file
#include <libcotask/task.h>

struct my_task_macro_t {
    typedef copp::coroutine_fiber_context_default::allocator_type stack_allocator_t;
    typedef copp::coroutine_fiber_context_default                 coroutine_t;
};

typedef cotask::task<my_task_macro_t> my_task_t;

#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable : 4091)

#include <atlconv.h>
#include <imagehlp.h>

#pragma comment(lib, "dbghelp.lib")

#ifdef UNICODE
#define SAMPLE_VC_TEXT(x) A2W(x)
#else
#define SAMPLE_VC_TEXT(x) x
#endif

LPTOP_LEVEL_EXCEPTION_FILTER g_msvc_debuger_old_handle = NULL;
std::string g_msvc_debuger_pattern;

inline void CreateMiniDump(EXCEPTION_POINTERS *pep, LPCTSTR strFileName) {
    HANDLE hFile = CreateFile(strFileName, GENERIC_READ | GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);

    if ((hFile != NULL) && (hFile != INVALID_HANDLE_VALUE)) {
        MINIDUMP_EXCEPTION_INFORMATION mdei;
        mdei.ThreadId = GetCurrentThreadId();
        mdei.ExceptionPointers = pep;
        mdei.ClientPointers = FALSE;
        // MINIDUMP_CALLBACK_INFORMATION mci;
        // mci.CallbackRoutine = (MINIDUMP_CALLBACK_ROUTINE)MiniDumpCallback;
        // mci.CallbackParam = 0;
        MINIDUMP_TYPE mdt = (MINIDUMP_TYPE)(MiniDumpWithPrivateReadWriteMemory | MiniDumpWithDataSegs | MiniDumpWithHandleData |
                                            MiniDumpWithFullMemoryInfo | MiniDumpWithThreadInfo | MiniDumpWithUnloadedModules);
        MiniDumpWriteDump(GetCurrentProcess(), GetCurrentProcessId(), hFile, mdt, (pep != 0) ? &mdei : 0, 0, NULL);
        CloseHandle(hFile);
    }
}

LONG WINAPI GPTUnhandledExceptionFilter(PEXCEPTION_POINTERS pExceptionInfo) {
    //得到当前时间
    SYSTEMTIME st;
    ::GetLocalTime(&st);
    //得到程序所在文件夹
    // TCHAR exeFullPath[256]; // MAX_PATH
    // GetModuleFileName(NULL, exeFullPath, 256);//得到程序模块名称，全路径

    TCHAR szFileName[_MAX_FNAME] = {0};

    USES_CONVERSION;

    wsprintf(szFileName, TEXT("%s-%04d-%02d-%02d.%02d%02d%02d.%03d.dmp"), SAMPLE_VC_TEXT(g_msvc_debuger_pattern.c_str()), st.wYear,
             st.wMonth, st.wDay, st.wHour, st.wMinute, st.wSecond, st.wMilliseconds);
    CreateMiniDump(pExceptionInfo, szFileName);

    if (NULL == g_msvc_debuger_old_handle) {
        return EXCEPTION_EXECUTE_HANDLER;  // 下一个Handle, 一般是程序停止运行
    }

    return g_msvc_debuger_old_handle(pExceptionInfo);
}

void __cdecl sample_setup_msvc_mini_dump(const char *prefix) {
    g_msvc_debuger_pattern = prefix;
    g_msvc_debuger_old_handle = SetUnhandledExceptionFilter(GPTUnhandledExceptionFilter);
    if (g_msvc_debuger_old_handle == GPTUnhandledExceptionFilter) {
        g_msvc_debuger_old_handle = NULL;
    }
}
#endif

int main() {
#ifdef _MSC_VER
    sample_setup_msvc_mini_dump("d:/libcopp-test-minidump");
#endif
    // create a task using factory function [with lambda expression]
    my_task_t::ptr_t task = my_task_t::create([]() {
        std::cout << "task " << cotask::this_task::get<my_task_t>()->get_id() << " started" << std::endl;
        cotask::this_task::get_task()->yield();
        std::cout << "task " << cotask::this_task::get<my_task_t>()->get_id() << " resumed" << std::endl;

        // ! Make crash and it's will generate a mini dump into d:/libcopp-test-minidump-*.dmp
        // copp::this_coroutine::get_coroutine()->yield();
        return 0;
    });

    std::cout << "task " << task->get_id() << " created" << std::endl;
    // start a task
    task->start();

    std::cout << "task " << task->get_id() << " yield" << std::endl;
    task->resume();
    std::cout << "task " << task->get_id() << " stoped, ready to be destroyed." << std::endl;

    return 0;
}

#ifdef _MSC_VER
#pragma warning(pop)
#endif

#else
int main() {
    std::cerr << "lambda not supported, or fiber is not supported, this sample is not available." << std::endl;
    return 0;
}
#endif
~~~~~~~~~~

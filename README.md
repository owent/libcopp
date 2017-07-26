libcopp
=======

> cross-platform coroutine library in c++
>
> Build & Run Unit Test in |  Linux+OSX(Clang+GCC) | Windows(VC+MinGW) |
> -------------------------|-----------------------|-------------|
> Status |  [![Build Status](https://travis-ci.org/owt5008137/libcopp.svg?branch=master)](https://travis-ci.org/owt5008137/libcopp) | [![Build status](https://ci.appveyor.com/api/projects/status/7w6dfnpeahfmgaqj?svg=true)](https://ci.appveyor.com/project/owt5008137/libcopp) |
> Compilers | linux-gcc-4.4 <br /> linux-gcc-4.6 <br /> linux-gcc-4.9 <br /> linux-gcc-7 <br /> linux-clang-3.5 <br /> osx-apple-clang-6.0 <br /> | MSVC 12(Visual Studio 2013) <br /> MSVC 14(Visual Studio 2015) <br /> MSVC 15(Visual Studio 2017) <br /> MinGW64-gcc
>


Gitter
------
[![Gitter](https://badges.gitter.im/owt5008137/libcopp.svg)](https://gitter.im/owt5008137/libcopp?utm_source=badge&utm_medium=badge&utm_campaign=pr-badge)

LICENSE
-------

License under the MIT license

Document
--------

Generate document with doxygen.

Doxygen file located at *docs/libcopp.doxyfile* .


INSTALL
=======

> libcopp use cmake to generate makefile and switch build tools.

Prerequisites
-------------

-   **[required]** GCC or Clang or VC support ISO C++ 03 and upper
-   **[required]** [cmake](www.cmake.org) 3.3.0 and upper
-   **[optional]** [gtest](https://code.google.com/p/googletest/) 1.6.0 and upper (better test supported)
-   **[optional]** [Boost.Test](http://www.boost.org/doc/libs/release/libs/test/) (Boost.Test supported)

### Unix

-   **[required]** ar, as, ld ([binutils](http://www.gnu.org/software/binutils/))
-   **[optional]** if using [gtest](https://code.google.com/p/googletest/), pthread is required.

### Windows

-   **[required]** masm (in vc)
-   **[optional]** if using [gtest](https://code.google.com/p/googletest/), pthread is required.

Build
-----

**1. make a build directory**

    mkdir build

**2. run cmake command**

    cmake <libcopp dir> [options]

> options can be cmake options. such as set compile toolchains, source directory or options of libcopp that control build actions. libcopp options are listed below:

> > -DBUILD\_SHARED\_LIBS=YES|NO [default=NO] enable build dynamic library.

> > -DLIBCOPP\_ENABLE\_SEGMENTED\_STACKS=YES|NO [default=NO] enable split stack supported context.(it's only availabe in linux and gcc 4.7.0 or upper)

> > -DLIBCOPP\_ENABLE\_VALGRIND=YES|NO [default=YES] enable valgrind supported context.

> > -DGTEST\_ROOT=[path] set gtest library install prefix path

**3. make libcopp**

    make [options]

**4. run test** *[optional]*

    make run_test

**5. run benchmark** *[optional]*

    make benchmark

**6. install** *[optional]*

    make install

> Or you can just copy include directory and libcopp.a in lib or lib64 into your project to use it.


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
NOTICE
======

Split stack support: if in Linux and user gcc 4.7.0 or upper, add -DLIBCOPP\_ENABLE\_SEGMENTED\_STACKS=YES to use split stack supported context.

BENCHMARK
======
Please see CI output for latest benchmark report. the [benchmark on Linux and macOS can be see here](https://travis-ci.org/owt5008137/libcopp) and the [benchmark on Windows can be see here](https://ci.appveyor.com/project/owt5008137/libcopp).

DEVELOPER
=========

[docs/libcopp.doxyfile](docs/libcopp.doxyfile)

HISTORY
========
2017-06-11
------
1. [OPTIMIZE] V2 framework and APIs completed, all reports in clang-analysis and cppcheck are fixed.
2. [CI] benchmark and samples enabled in v2 branch
3. [CI] add sample code in README.md into CI 

2017-05-10
------
1. [BOOST] merge boost.context 1.64.0
2. [OPTIMIZE] add stack pool manager and unit test
3. [OPTIMIZE] reduce memory fragment when allocate coroutine task and task action
4. [CI] benchmark and sample will always be run in [Travis CI](https://travis-ci.org/owt5008137/libcopp) and [Appveyor CI](https://ci.appveyor.com/project/owt5008137/libcopp)


2016-06-16
------
1. [BOOST] merge boost.context 1.61.0 and use the new jump progress(see https://owent.net/90QQw for detail)
2. [BOOST] enable valgrind support if valgrind/valgrind.h exists
3. [CXX] use cmake to detect the function of compiler
4. [OPTIMIZE] using pthread key when c++11 TLS not available
5. [OPTIMIZE] remove coroutine_context_safe_base.coroutine_context_base is also thread safe now
6. [OPTIMIZE] remove all global variables of cotask
7. [OPTIMIZE] remove std/thread.h， add noexpect if available
8. [CI] CI use build matrix to test more compiler
9. [BUILD] use RelWithDebInfo for default

2016-02-27
------
1. v0.2.0, this version is used in our server for about one year.

2015-12-29
------
1. add support for valgrind
2. add ci configure
3. merge boost.context 1.60.0
4. add -fPIC, fix spin lock
5. some environment do not support TLS, make these environment can compile success

2014-07-25
------
v0.1.0

CONSTRIBUTORS
======
+ [owent](https://github.com/owt5008137)

THANKS TO
======

+ [mutouyun](https://github.com/mutouyun)


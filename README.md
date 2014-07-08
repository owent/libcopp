libcopp
=======

> cross-platform coroutine library of c++
>
> in developing ...

LICENSE
-------

License under the MIT license

Document
--------

Generate document with doxygen.

Doxygen file located at *doc/libcopp.doxyfile* .

INSTALL
=======

> libcopp use cmake to generate makefile and switch build tools.

Prerequisites
-------------

-   **[required]** GCC or Clang or VC support ISO C++ 98 and upper
-   **[required]** [cmake](www.cmake.org) 2.8.9 and upper
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

> > -DGTEST\_ROOT=[path] set gtest library install prefix path

**3. make libcopp**

    make [options]

**4. run test** *[optional]*

    test/coroutine_test

**5. install** *[optional]*

    make install

> Or you can just copy include directory and libcopp.a in lib or lib64 into your project to use it.

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


NOTICE
======

split stack support: if in Linux and user gcc 4.7.0 or upper, add -DLIBCOPP\_ENABLE\_SEGMENTED\_STACKS=YES to use split stack supported context.

DEVELOPER
=========

[basic coroutine object summary](doc/basic_coroutine_class.txt)

[safe coroutine object summary](doc/safe_basic_coroutine_class.txt)

SCHEDUAL
========

[task schedual](doc/task_schedual.txt)


THANKS TO
========

[mutouyun](https://github.com/mutouyun)


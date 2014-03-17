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

    bin/coroutine_test

**5. install** *[optional]*

    make install

> Or you can just copy include directory and libcopp.a in lib or lib64 into your project to use it.

USAGE
=====

> Just include headers and linking library file of your platform to use libcopp

Example
-------

There is a simple example of using coroutine manager below:

``` {.cpp}
#include <cstdio>
#include <cstring>
#include <stdint.h>

// include manager header file
#include <libcopp/coroutine/coroutine_manager.h>

// define a coroutine runner
class my_runner : public copp::coroutine_manager_runner_base
{
public:
    typedef copp::default_coroutine_manager::value_type value_type;
public:
    int operator()() {
    // ... your code here ...
    uint64_t key = get_coroutine_context<copp::default_coroutine_manager::value_type>()->get_key();
    printf("cortoutine %" PRIu64 " is running.\n", key);
        return 0;
    }
};

int main() {
    // create a coroutine manager
    copp::default_coroutine_manager co_mgr;
    typedef copp::default_coroutine_manager::value_type co_type;
    
    // create a coroutine object
    co_type* pco = g_co_mgr.create();
    // create and bind runner to coroutine object
    pco->create_runner<my_runner>(64 * 1024); // allocate 64KB stack memory

    // start a coroutine
    co_mgr.start(pco->get_key());

    // destroy a coroutine [optional], all coroutine will be destroyed automatically when co_mgr is destroyed
    co_mgr.remove(pco->get_key()); // pco is invalid now.
    return 0;
}
```

And then, you can custom many function such as set your id type, stack allocator, coroutine type and etc. by set your template parameters of coroutine manager.

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

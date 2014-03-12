# INSTALL

> libcopp use cmake to generate makefile and switch build tools.

## Prerequisites
+ **[required]** GCC or Clang or VC support ISO C++ 98 and upper
+ **[required]** [cmake](www.cmake.org) 2.8.9 and upper
+ **[optional]** [gtest](https://code.google.com/p/googletest/) 1.6.0 and upper (better test supported)

### Unix 
* **[required]** ar, as, ld ([binutils](http://www.gnu.org/software/binutils/))
* **[optional]** if using [gtest](https://code.google.com/p/googletest/), pthread is required.

### Windows
* **[required]** masm (in vc)
* **[optional]** if using [gtest](https://code.google.com/p/googletest/), pthread is required.


## Build
**1. make a build directory**

    mkdir build
    
**2. run cmake command**

    cmake <libcopp dir> [options]
> options can be cmake options. such as set compile toolchains, source directory or options of libcopp that control build actions.
> libcopp options are listed below:

>> -DBUILD_SHARED_LIBS=YES|NO [default=NO] enable build dynamic library.

>> -DLIBCOPP_ENABLE_SEGMENTED_STACKS=YES|NO [default=NO] enable split stack supported context.(it's only availabe in linux and gcc 4.7.0 or upper)

>> -DGTEST_ROOT=[path] set gtest library install prefix path

**3. make libcopp**

    make [options]

**4. run test** *[optional]*

    bin/coroutine_test

**5. install** *[optional]*

    make install

> Or you can just copy include directory and libcopp.a in lib or lib64 into your project to use it.

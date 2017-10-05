INSTALL
=======

> libcopp use cmake to generate makefile and switch build tools.

Prerequisites
-------------

-   **[required]** GCC or Clang or VC support ISO C++ 03 and upper
-   **[required]** [cmake](www.cmake.org) 3.7.0 and upper
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
```bash
    mkdir build
```

**2. run cmake command**
```bash
    cmake <libcopp dir> [options]
```

Options can be cmake options. such as set compile toolchains, source directory or options of libcopp that control build actions. libcopp options are listed below:

Option  | Description
--------|------------
BUILD\_SHARED\_LIBS=YES\|NO | [default=NO] enable build dynamic library.
LIBCOPP\_ENABLE\_SEGMENTED\_STACKS=YES\|NO | [default=NO] enable split stack supported context.(it's only availabe in linux and gcc 4.7.0 or upper)
LIBCOPP\_ENABLE\_VALGRIND=YES\|NO | [default=YES] enable valgrind supported context.
PROJECT\_ENABLE\_UNITTEST=YES\|NO | [default=NO] Build unit test.
PROJECT\_ENABLE\_SAMPLE=YES\|NO | [default=NO] Build samples.
PROJECT\_DISABLE\_MT=YES\|NO | [default=NO] Disable multi-thread support.
LIBCOTASK\_ENABLE=YES\|NO | [default=YES] Enable build libcotask.
GTEST\_ROOT=[path] | set gtest library install prefix path

**3. make libcopp**
```bash
    make [options] # or cmake --build . --config RelWithDebInfo
```
**4. run test** *[optional]*
```bash
    make run_test # Required: PROJECT_ENABLE_UNITTEST=YES
```
**5. run benchmark** *[optional]*
```bash
    make benchmark # Required: PROJECT_ENABLE_SAMPLE=YES
```
**6. install** *[optional]*
```bash
    make install
```
**7. run sample** *[optional]*
```bash
    make run_sample # Required: PROJECT_ENABLE_SAMPLE=YES
```
> Or you can just copy include directory and libcopp.a in lib or lib64 into your project to use it.



INSTALL
=======

> libcopp use cmake to generate makefile and switch build tools.

Prerequisites
-------------

-   **[required]** GCC or Clang or VC support ISO C++ 03 and upper
-   **[required]** [cmake](www.cmake.org) 3.1.0 and upper
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



INSTALL
----------------

> libcopp use cmake to generate makefile and switch build tools.

### Prerequisites

-   **[required]** GCC or Clang or MSVC or clang-cl support ISO C++ 03 and upper
-   **[required]** [cmake](www.cmake.org) 3.7.0 and upper
-   **[optional]** [gtest](https://code.google.com/p/googletest/) 1.6.0 and upper (Better unit test supported)
-   **[optional]** [Boost.Test](http://www.boost.org/doc/libs/release/libs/test/) (Boost.Test supported)

### Unix

-   **[required]** ar, as, ld ([binutils](http://www.gnu.org/software/binutils/)) or [llvm](http://llvm.org/)
-   **[optional]** if using [gtest](https://code.google.com/p/googletest/), pthread is required.

### Windows

-   **[required]** masm (in vc)
-   **[optional]** if using [gtest](https://code.google.com/p/googletest/), pthread is required.

### Install with vcpkg

**1. clone and setup vcpkg**(See more detail on https://github.com/Microsoft/vcpkg)
~~~~~~~~~~bash
git clone https://github.com/Microsoft/vcpkg.git
cd vcpkg

PS> .\bootstrap-vcpkg.bat
Linux:~/$ ./bootstrap-vcpkg.sh
~~~~~~~~~~

**2. install libcopp**
~~~~~~~~~~bash
PS> .\vcpkg install libcopp
Linux:~/$ ./vcpkg install libcopp
~~~~~~~~~~


### Custom Build

**1. clone and make a build directory**
~~~~~~~~~~bash
git clone --single-branch --depth=1 -b master https://github.com/owt5008137/libcopp.git 
mkdir libcopp/build && cd libcopp/build
~~~~~~~~~~

**2. run cmake command**
~~~~~~~~~~bash
# cmake <libcopp dir> [options...]
cmake .. -DLIBCOPP_FCONTEXT_USE_TSX=YES -DPROJECT_ENABLE_UNITTEST=YES -DPROJECT_ENABLE_SAMPLE=YES
~~~~~~~~~~

**3. make libcopp**
~~~~~~~~~~bash
cmake --build . --config RelWithDebInfo # or make [options] when using Makefile
~~~~~~~~~~

**4. run test/sample/benchmark** *[optional]*
~~~~~~~~~~bash
# Run test => Required: PROJECT_ENABLE_UNITTEST=YES
cmake --build . --config RelWithDebInfo --target run_test # or make run_test when using Makefile
# Run sample => Required: PROJECT_ENABLE_SAMPLE=YES
cmake --build . --config RelWithDebInfo --target run_sample # or make run_sample when using Makefile
# Run benchmark => Required: PROJECT_ENABLE_SAMPLE=YES
cmake --build . --config RelWithDebInfo --target benchmark # or make benchmark when using Makefile
~~~~~~~~~~

**5. install** *[optional]*
~~~~~~~~~~bash
cmake --build . --config RelWithDebInfo --target install # or make install when using Makefile
~~~~~~~~~~

> Or you can just copy include directory and libcopp.a in lib or lib64 into your project to use it.

#### CMake Options
Options can be cmake options. such as set compile toolchains, source directory or options of libcopp that control build actions. libcopp options are listed below:

| Option  | Description |
|---------|-------------|
BUILD\_SHARED\_LIBS=YES\|NO | [default=NO] Build dynamic library.
LIBCOPP\_ENABLE\_SEGMENTED\_STACKS=YES\|NO | [default=NO] Enable split stack supported context.(it's only availabe in linux and gcc 4.7.0 or upper)
LIBCOPP\_ENABLE\_VALGRIND=YES\|NO | [default=YES] Enable valgrind supported context.
PROJECT\_ENABLE\_UNITTEST=YES\|NO | [default=NO] Build unit test.
PROJECT\_ENABLE\_SAMPLE=YES\|NO | [default=NO] Build samples.
PROJECT\_DISABLE\_MT=YES\|NO | [default=NO] Disable multi-thread support.
LIBCOTASK\_ENABLE=YES\|NO | [default=YES] Enable build libcotask.
LIBCOPP\_FCONTEXT\_USE\_TSX=YES\|NO | [default=NO] Enable [Intel Transactional Synchronisation Extensions (TSX)](https://software.intel.com/en-us/node/695149).
GTEST\_ROOT=[path] | set gtest library install prefix path
BOOST\_ROOT=[path] | set Boost.Test library install prefix path

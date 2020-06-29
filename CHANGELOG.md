HISTORY
----------------

### 1.3.0 (2020-05-27)

1. [OPTIMIZE] Using cmake export to generate module files
2. [OPTIMIZE] Change default visibility of symbols to hidden, we support dll on windows now.
3. [FIX] Rename ```task::await``` into ```task::await_task```, Some compile don't allow ```await_task``` as function name.
4. [BOOST] Merge boost.context 1.73.0
5. [FEATURE] Implement context of windows fiber and ```SetUnhandledExceptionFilter``` is available to catchunhandled exception in coroutine action
6. [OPTIMIZE] Catch and rethrow unhandle exception after coroutine resumed when using C++11 or upper, most APIs are exception safe now.
7. [OPTIMIZE] Remove TTASK_MACRO in cotask::task, task id now is always set ```uint64_t``` and use the id allocator with thread cache.
8. [OPTIMIZE] Fix ```Warning: client switching stacks?``` in unit test when enable valgrind support.

#### BREAK CHANGES & UPGRADE GUIDE FROM 1.2.X

+ Rename ```cotask::task::await``` into ```cotask::task::await_task```
+ Replace ```cotask::task<TCO_MACRO, TTASK_MACRO>``` with ```cotask::task<TCO_MACRO>``` , we don't allow to custom id allocator now.
+ Replace ```cotask::core::standard_int_id_allocator<uint64_t>``` with ```copp::util::uint64_id_allocator``` , we don't allow to custom id allocator now.
+ Require gcc 4.7+, MSVC 14+(Visual Studio 2015)>)
+ Require cmake 3.12.0 or upper

### 1.2.1 (2019-10-04)

1. [OPTIMIZE] Add options to disable TLS usage for *this_task* and *this_coroutine* to reduce cache miss on single thread situation.
2. [OPTIMIZE] Implement **jump_to(...)** as static inline to reduce code cache miss.
3. [OPTIMIZE] Implement **coroutine_context_callback(...)** as static local symbol to reduce code cache miss.

### 1.2.0 (2019-08-20)

1. [OPTIMIZE] Add more unit test.Increase coverage to 90%+.
2. [OPTIMIZE] Rewrite toolchain detection scripts, support clang-cl now.
3. [BOOST] merge boost.context 1.71.0.
4. [FIX] Fix a coredump problem when task_ptr is reset when await_task other tasks.
5. [OPTIMIZE] Optimize timer implement, task_manager support modify task timer now.
6. [OPTIMIZE] A task can only be added to one task_manager now, and when finish task with task's API, task_manager will also be cleanup.
7. [OPTIMIZE] Fix some warning when compiling with gcc 9+ and clang 8+.
8. [OPTIMIZE] Use pthread  for TLS when both c++11 thead_local and pthread are available. It will reduce the minimal stack size.
9. [OPTIMIZE] Fix and add some example codes and documents.

### 2018-12-07

1. [OPTIMIZE] add adaptor for [vcpkg](https://github.com/Microsoft/vcpkg)
2. [OPTIMIZE] using [GNUInstallDirs](https://cmake.org/cmake/help/v3.13/module/GNUInstallDirs.html) for all directories
3. [BOOST] merge boost.context 1.67.0
4. [OPTIMIZE] add libcopp-config.cmake and libcopp-config-version.cmake for cmake module
5. [OPTIMIZE] add ***then*** and ***await_task*** API for easier usage
6. [OPTIMIZE] add ***/Zc:__cplusplus*** for MSVC to make __cplusplus to the standard value.
7. [OPTIMIZE] fix a guess to x86_64 environment
8. [OPTIMIZE] add more API for statistics
9. [FIX] fix the problem that calling compare_exchange_weak failed will lead the cotask to exit
10. [OPTIMIZE] merge the latest unit test framework, now we will support more readable output when test failed.
11. [CI] We use xenial to build in linux now.

### 2018-01-01

1. [BOOST] merge boost.context 1.66.0
2. [OPTIMIZE] add support for [Intel Transactional Synchronisation Extensions (TSX)](https://software.intel.com/en-us/node/695149)
3. [OPTIMIZE] fix a warning reported by clang-analyzer.
4. [CI] update README.md and automatic documents.(published at https://libcopp.atframe.work)
5. [FIX] make sure all the allocated stacks are always greater than the configure value after reloaded when using stack pool.
6. [FIX] allow segmented stacks when using clang and llvm.
7. [OPTIMIZE] using the new script to run clang-analyzer.
8. [OPTIMIZE] optimize the doxygen output and fix some markdown synax not supported by doxygen.

### 2017-10-01

1. [OPTIMIZE] optimize cmake files for all target
2. [OPTIMIZE] update samples and readme(fix sample for stack pool in README.md)
3. [CI] add gcc 7
4. [OPTIMIZE] using -std=c++17 for gcc/clang and /std:c++17 for MSVC 15(2015) and upper

### 2017-06-11

1. [OPTIMIZE] V2 framework and APIs completed, all reports in clang-analysis and cppcheck are fixed.
2. [CI] benchmark and samples enabled in v2 branch
3. [CI] add sample code in README.md into CI 

### 2017-05-10

1. [BOOST] merge boost.context 1.64.0
2. [OPTIMIZE] add stack pool manager and unit test
3. [OPTIMIZE] reduce memory fragment when allocate coroutine task and task action
4. [CI] benchmark and sample will always be run in [Travis CI](https://travis-ci.org/owt5008137/libcopp) and [Appveyor CI](https://ci.appveyor.com/project/owt5008137/libcopp)

### 2016-06-16

1. [BOOST] merge boost.context 1.61.0 and use the new jump progress(see https://owent.net/2016/1270.html for detail)
2. [BOOST] enable valgrind support if valgrind/valgrind.h exists
3. [CXX] use cmake to detect the function of compiler
4. [OPTIMIZE] using pthread key when c++11 TLS not available
5. [OPTIMIZE] remove coroutine_context_safe_base.coroutine_context_base is also thread safe now
6. [OPTIMIZE] remove all global variables of cotask
7. [OPTIMIZE] remove std/thread.hï¼Œ add noexpect if available
8. [CI] CI use build matrix to test more compiler
9. [BUILD] use RelWithDebInfo for default

### 2016-02-27

1. v0.2.0, this version is used in our server for about one year.

### 2015-12-29

1. add support for valgrind
2. add ci configure
3. merge boost.context 1.60.0
4. add -fPIC, fix spin lock
5. some environment do not support TLS, make these environment can compile success

### 2014-07-25

v0.1.0

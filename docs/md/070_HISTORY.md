HISTORY
========
2017-10-01
------
1. [OPTIMIZE] optimize cmake files for all target
2. [OPTIMIZE] update samples and readme(fix sample for stack pool in README.md)
3. [CI] add gcc 7
4. [OPTIMIZE] using -std=c++17 for gcc/clang and /std:c++17 for MSVC 15(2015) and upper

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
1. [BOOST] merge boost.context 1.61.0 and use the new jump progress(see https://owent.net/2016/1270.html for detail)
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



NOTICE
----------------

Split stack support: if in Linux and user gcc 4.7.0 or upper, add -DLIBCOPP\_ENABLE\_SEGMENTED\_STACKS=YES to use split stack supported context.

It's recommanded to use stack pool instead of gcc splited stack.

BENCHMARK
----------------

Please see CI output for latest benchmark report. the [benchmark on Linux and macOS can be see here](https://travis-ci.org/owt5008137/libcopp) and the [benchmark on Windows can be see here](https://ci.appveyor.com/project/owt5008137/libcopp).

FAQ
----------------

Q: How to enable c++20 coroutine

> ANS: Add ```/std:c++latest /await``` for MSVC or add ```-std=c++20 -fcoroutines-ts -stdlib=libc++``` for clang or add ```-std=c++20 -fcoroutines``` for gcc.

Q: Will libcopp handle exception?

> ANS: When using c++11 or above, libcopp will catch all unhandled exception and rethrow it after coroutine resumed.

Q: Why ```SetUnhandledExceptionFilter``` can not catch the unhandled exception in a coroutine?

> ANS: Please use ```AddVectoredExceptionHandler``` for tempolary, we will implement WinFiber in the future, and then ```SetUnhandledExceptionFilter``` will work.

FEEDBACK
----------------

If you has any question, please create a issue and provide the information of your environments. For example:

+ **OS**: Windows 10 Pro 19041 *(This can be see after running ```msinfo32```)* / Manjaro(Arch) Linux Linux 5.4.39-1-MANJARO
+ **Compiler**: Visual Studio 2019 C++ 16.5.5 with VS 2019 C++ v14.25 or MSVC 1925/ gcc 9.3.0
+ **CMake Commands**: ```cmake .. -G "Visual Studio 16 2019" -A x64 -DLIBCOPP_FCONTEXT_USE_TSX=ON -DPROJECT_ENABLE_UNITTEST=ON -DPROJECT_ENABLE_SAMPLE=ON -DCMAKE_BUILD_TYPE=RelWithDebInfo -DCMAKE_INSTALL_PREFIX=%cd%/install-prefix``` / ```cmake .. -G Ninja -DLIBCOPP_FCONTEXT_USE_TSX=ON -DPROJECT_ENABLE_UNITTEST=ON -DPROJECT_ENABLE_SAMPLE=ON -DCMAKE_BUILD_TYPE=RelWithDebInfo -DCMAKE_INSTALL_PREFIX=/opt/libcopp```
+ **Compile Commands**: ```cmake --build . -j```
+ **Related Environment Variables**: Please provide all the environment variables which will change the cmake toolchain, ```CC``` 、 ```CXX``` 、 ```AR``` and etc.


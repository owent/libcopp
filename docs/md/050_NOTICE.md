NOTICE
======

Split stack support: if in Linux and user gcc 4.7.0 or upper, add -DLIBCOPP\_ENABLE\_SEGMENTED\_STACKS=YES to use split stack supported context.

BENCHMARK
======
Please see CI output for latest benchmark report. the [benchmark on Linux and macOS can be see here](https://travis-ci.org/owt5008137/libcopp) and the [benchmark on Windows can be see here](https://ci.appveyor.com/project/owt5008137/libcopp).

On MinGW64, We use ms_pe_gas ASM code in boost.context to configure and build libcopp. It can be built successfully but will crash when running right now, so we don't support MinGW now.
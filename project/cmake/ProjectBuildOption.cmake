# 默认配置选项
#####################################################################
option(BUILD_SHARED_LIBS "Build shared libraries (DLLs)." OFF)

# libcotask 编译环境选项
option(LIBCOPP_ENABLE_SEGMENTED_STACKS "Enable segmented stacks." OFF)
option(LIBCOPP_ENABLE_VALGRIND "Enable valgrind." OFF)

set(LIBCOPP_FCONTEXT_OS_PLATFORM "" CACHE STRING "set system platform. arm/arm64/i386/x86_64/combined/mips/ppc32/ppc64/sparc/sparc64 and etc.")
set(LIBCOPP_FCONTEXT_ABI "" CACHE STRING "set abi. sysv/aapcs/mips/o32/ms and etc.")
set(LIBCOPP_FCONTEXT_BIN_FORMAT "" CACHE STRING "set binary format. elf/pe/macho/xcoff and etc.")
set(LIBCOPP_FCONTEXT_AS_TOOL "" CACHE STRING "set as toolset. gas/armasm/masm and etc.")
set(LIBCOPP_FCONTEXT_AS_ACTION "" CACHE STRING "set as action. x32/32/64 and etc.")

# libcotask 配置选项
option(LIBCOTASK_ENABLE "Enable libcotask." ON)

# 测试配置选项
set(GTEST_ROOT "" CACHE STRING "GTest root directory")
set(BOOST_ROOT "" CACHE STRING "Boost root directory")
option(LIBCOPP_TEST_ENABLE_BOOST_UNIT_TEST "Enable boost unit test." OFF)

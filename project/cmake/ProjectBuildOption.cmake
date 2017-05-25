# default option
#####################################################################
option(BUILD_SHARED_LIBS "Build shared libraries (DLLs)." OFF)

# libcopp configure
option(LIBCOPP_ENABLE_SEGMENTED_STACKS "Enable segmented stacks." OFF)

# fast find valgrind header to decide wether to enable valgrind
unset(_VALGRIND_HEADER)
unset(_VALGRIND_EXECUTABLE)
find_path(_VALGRIND_HEADER    NAMES valgrind/valgrind.h)
find_program(_VALGRIND_EXECUTABLE NAMES valgrind)

if (_VALGRIND_HEADER)
    option(LIBCOPP_ENABLE_VALGRIND "Enable valgrind." ON)
else()
    option(LIBCOPP_ENABLE_VALGRIND "Enable valgrind." OFF)
endif()

option(PROJECT_ENABLE_UNITTEST "Build unit test." OFF)
option(PROJECT_ENABLE_SAMPLE "Build samples." OFF)
option(PROJECT_DISABLE_MT "Disable multi-thread support." OFF)

set(LIBCOPP_FCONTEXT_OS_PLATFORM "" CACHE STRING "set system platform. arm/arm64/i386/x86_64/combined/mips/ppc32/ppc64 and etc.")
set(LIBCOPP_FCONTEXT_ABI "" CACHE STRING "set abi. sysv/aapcs/mips/o32/ms and etc.")
set(LIBCOPP_FCONTEXT_BIN_FORMAT "" CACHE STRING "set binary format. elf/pe/macho/xcoff and etc.")
set(LIBCOPP_FCONTEXT_AS_TOOL "" CACHE STRING "set as toolset. gas/armasm/masm and etc.")
set(LIBCOPP_FCONTEXT_AS_ACTION "" CACHE STRING "set as action. x32/32/64 and etc.")

# libcotask configure
option(LIBCOTASK_ENABLE "Enable libcotask." ON)

# unit test framework
set(GTEST_ROOT "" CACHE STRING "GTest root directory")
set(BOOST_ROOT "" CACHE STRING "Boost root directory")
option(LIBCOPP_TEST_ENABLE_BOOST_UNIT_TEST "Enable boost unit test." OFF)

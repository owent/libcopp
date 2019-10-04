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

# If you do not call libcopp's API on multi-thread at the same time
# You can set LIBCOPP_DISABLE_ATOMIC_LOCK=ON to reduce cache miss slightly
option(LIBCOPP_DISABLE_ATOMIC_LOCK "Do not use atomic API and lock to keep thread-safe for libcopp." OFF)
if (LIBCOPP_DISABLE_ATOMIC_LOCK)
    option(LOCK_DISABLE_MT "Disable multi-thread support for lock and intrusive_ptr." ON)
else ()
    option(LOCK_DISABLE_MT "Disable multi-thread support for lock and intrusive_ptr." OFF)
endif()

# This option can be set to ON only if the user do not use multi-thread at all. it can reduce the cache miss slightly. 
option(LIBCOPP_DISABLE_THIS_MT "Do not use multi-thread for this_coroutine/this_task, this options can only be set to ON on single thread process." OFF)

set(LIBCOPP_FCONTEXT_OS_PLATFORM "" CACHE STRING "set system platform. arm/arm64/i386/x86_64/combined/mips/ppc32/ppc64 and etc.")
set(LIBCOPP_FCONTEXT_ABI "" CACHE STRING "set abi. sysv/aapcs/mips/o32/ms and etc.")
set(LIBCOPP_FCONTEXT_BIN_FORMAT "" CACHE STRING "set binary format. elf/pe/macho/xcoff and etc.")
set(LIBCOPP_FCONTEXT_AS_TOOL "" CACHE STRING "set as toolset. gas/armasm/masm and etc.")

# [heading Intel Transactional Synchronisation Extensions (TSX)]
# 
# TSX is Intel's implementation of hardware transactional memory in modern Intel
# processors[footnote intel.com: [@https://software.intel.com/en-us/node/695149
# Intel Transactional Synchronization Extensions]].[br]
# In TSX the hardware keeps track of which cachelines have been read from and
# which have been written to in a transaction. The cache-line size (64-byte) and
# the n-way set associative cache determine the maximum size of memory in a
# transaction. For instance if a transaction modifies 9 cache-lines at a
# processor with a 8-way set associative cache, the transaction will always be
# aborted.
option(LIBCOPP_FCONTEXT_USE_TSX "Enable Intel Transactional Synchronisation Extensions (TSX)." OFF)
# [note A TSX-transaction will be aborted if the floating point state is modified
# inside a critical region. As a consequence floating point operations, e.g.
# tore/load of floating point related registers during a fiber (context) switch
# are disabled.]

# libcotask configure
option(LIBCOTASK_ENABLE "Enable libcotask." ON)
# libcotask configure
option(LIBCOTASK_AUTO_CLEANUP_MANAGER "Auto cleanup task manager after cotask finished(No need to call task_manager.start/resume())." ON)

# unit test framework
set(GTEST_ROOT "" CACHE STRING "GTest root directory")
set(BOOST_ROOT "" CACHE STRING "Boost root directory")
option(LIBCOPP_TEST_ENABLE_BOOST_UNIT_TEST "Enable boost unit test." OFF)

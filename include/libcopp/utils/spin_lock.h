/**
 * @file spin_lock.h
 * @brief 自旋锁
 * Licensed under the MIT licenses.
 *
 * @version 1.0
 * @author OWenT
 * @date 2013-3-18
 *
 * @note VC 2012+, GCC 4.4 + 使用C++0x/11实现实现原子操作
 * @note 低版本 VC使用InterlockedExchange等实现原子操作
 * @note 低版本 GCC采用__sync_lock_test_and_set等实现原子操作
 *
 * @history
 *     2013-12-20
 *         1. add support for clang & intel compiler
 *         2. add try unlock function
 *         3. fix atom operator
 *         4. add gcc atomic support
 *    2014-07-08
 *         1. add yield operation
 *    2016-06-15
 *         1. using atomic_int_type
 */

#pragma once

#if defined(_MSC_VER)

#  ifndef WIN32_LEAN_AND_MEAN
#    define WIN32_LEAN_AND_MEAN
#  endif

#  include <Windows.h>  // YieldProcessor

#  include <Processthreadsapi.h>
#  include <Synchapi.h>  // Windows server
#  include <intrin.h>
#elif defined(__i386__) || defined(__x86_64__)
#  if defined(__clang__)
#    include <emmintrin.h>
#  elif defined(__INTEL_COMPILER)
#    include <immintrin.h>
#  endif
#endif

#include <libcopp/utils/config/libcopp_build_features.h>

#include <libcopp/utils/atomic_int_type.h>

/**
 * ==============================================
 * ======            asm pause             ======
 * ==============================================
 */
#if defined(_MSC_VER)

/*
 * See: http://msdn.microsoft.com/en-us/library/windows/desktop/ms687419(v=vs.85).aspx
 * Not for intel c++ compiler, so ignore http://software.intel.com/en-us/forums/topic/296168
 */

#  ifdef YieldProcessor
#    define __LIBCOPP_UTIL_LOCK_SPIN_LOCK_PAUSE() YieldProcessor()
#  endif

#elif defined(__GNUC__) || defined(__clang__)
#  if defined(__i386__) || defined(__x86_64__)
/**
 * See: Intel(R) 64 and IA-32 Architectures Software Developer's Manual V2
 * PAUSE-Spin Loop Hint, 4-57
 * http://www.intel.com/content/www/us/en/architecture-and-technology/64-ia-32-architectures-software-developer-instruction-set-reference-manual-325383.html?wapkw=instruction+set+reference
 */
// #    define __LIBCOPP_UTIL_LOCK_SPIN_LOCK_PAUSE() __asm__ __volatile__("pause")
#    if defined(__clang__) || defined(__INTEL_COMPILER)
#      define __LIBCOPP_UTIL_LOCK_SPIN_LOCK_PAUSE() _mm_pause()
#    else
#      define __LIBCOPP_UTIL_LOCK_SPIN_LOCK_PAUSE() __builtin_ia32_pause()
#    endif

#  elif defined(__ia64__) || defined(__ia64)
/**
 * See: Intel(R) Itanium(R) Architecture Developer's Manual, Vol.3
 * hint - Performance Hint, 3:145
 * http://www.intel.com/content/www/us/en/processors/itanium/itanium-architecture-vol-3-manual.html
 */
#    define __LIBCOPP_UTIL_LOCK_SPIN_LOCK_PAUSE() __asm__ __volatile__("hint @pause")
#  elif defined(__arm__) && !defined(__ANDROID__)
/**
 * See: ARM Architecture Reference Manuals (YIELD)
 * http://infocenter.arm.com/help/index.jsp?topic=/com.arm.doc.subset.architecture.reference/index.html
 */
#    define __LIBCOPP_UTIL_LOCK_SPIN_LOCK_PAUSE() __asm__ __volatile__("yield")
#  endif

#endif /*compilers*/

// set pause do nothing
#if !defined(__LIBCOPP_UTIL_LOCK_SPIN_LOCK_PAUSE)
#  define __LIBCOPP_UTIL_LOCK_SPIN_LOCK_PAUSE()
#endif /*!defined(CAPO_SPIN_LOCK_PAUSE)*/

/**
 * ==============================================
 * ======            cpu yield             ======
 * ==============================================
 */
#if 0 && defined(_MSC_VER)

// SwitchToThread only can be used in desktop system

#  define __LIBCOPP_UTIL_LOCK_SPIN_LOCK_CPU_YIELD() SwitchToThread()

#elif defined(__linux__) || defined(__unix__)
#  include <sched.h>
#  define __LIBCOPP_UTIL_LOCK_SPIN_LOCK_CPU_YIELD() sched_yield()
#endif

#ifndef __LIBCOPP_UTIL_LOCK_SPIN_LOCK_CPU_YIELD
#  define __LIBCOPP_UTIL_LOCK_SPIN_LOCK_CPU_YIELD() __LIBCOPP_UTIL_LOCK_SPIN_LOCK_PAUSE()
#endif

/**
 * ==============================================
 * ======           thread yield           ======
 * ==============================================
 */
#if defined(__LIBCOPP_UTIL_LOCK_ATOMIC_INT_TYPE_ATOMIC_STD)
// clang-format off
#include <libcopp/utils/config/stl_include_prefix.h>  // NOLINT(build/include_order)
// clang-format on
#  include <chrono>
#  include <thread>
// clang-format off
#include <libcopp/utils/config/stl_include_suffix.h>  // NOLINT(build/include_order)
// clang-format on

#  if defined(__GNUC__) && !defined(__clang__)
#    if (__GNUC__ * 100 + __GNUC_MINOR__) <= 407
#      define __LIBCOPP_UTIL_LOCK_SPIN_LOCK_THREAD_YIELD() __LIBCOPP_UTIL_LOCK_SPIN_LOCK_CPU_YIELD()
#      define __LIBCOPP_UTIL_LOCK_SPIN_LOCK_THREAD_SLEEP() __LIBCOPP_UTIL_LOCK_SPIN_LOCK_CPU_YIELD()
#    endif
#  endif

#  ifndef __LIBCOPP_UTIL_LOCK_SPIN_LOCK_THREAD_YIELD
#    define __LIBCOPP_UTIL_LOCK_SPIN_LOCK_THREAD_YIELD() ::std::this_thread::yield()
#  endif
#  ifndef __LIBCOPP_UTIL_LOCK_SPIN_LOCK_THREAD_SLEEP
#    define __LIBCOPP_UTIL_LOCK_SPIN_LOCK_THREAD_SLEEP() ::std::this_thread::sleep_for(::std::chrono::milliseconds(1))
#  endif

#elif defined(_MSC_VER)
#  define __LIBCOPP_UTIL_LOCK_SPIN_LOCK_THREAD_YIELD() Sleep(0)
#  define __LIBCOPP_UTIL_LOCK_SPIN_LOCK_THREAD_SLEEP() Sleep(1)
#endif

#ifndef __LIBCOPP_UTIL_LOCK_SPIN_LOCK_THREAD_YIELD
#  define __LIBCOPP_UTIL_LOCK_SPIN_LOCK_THREAD_YIELD()
#  define __LIBCOPP_UTIL_LOCK_SPIN_LOCK_THREAD_SLEEP() __LIBCOPP_UTIL_LOCK_SPIN_LOCK_CPU_YIELD()
#endif

/**
 * ==============================================
 * ======           spin lock wait         ======
 * ==============================================
 * @note
 *   1. busy-wait
 *   2. asm pause
 *   3. thread give up cpu time slice but will not switch to another process
 *   4. thread give up cpu time slice (may switch to another process)
 *   5. sleep (will switch to another process when necessary)
 */

#define __LIBCOPP_UTIL_LOCK_SPIN_LOCK_WAIT(x)                     \
  {                                                               \
    unsigned char try_lock_times = static_cast<unsigned char>(x); \
    if (try_lock_times < 4) {                                     \
    } else if (try_lock_times < 16) {                             \
      __LIBCOPP_UTIL_LOCK_SPIN_LOCK_PAUSE();                      \
    } else if (try_lock_times < 32) {                             \
      __LIBCOPP_UTIL_LOCK_SPIN_LOCK_THREAD_YIELD();               \
    } else if (try_lock_times < 64) {                             \
      __LIBCOPP_UTIL_LOCK_SPIN_LOCK_CPU_YIELD();                  \
    } else {                                                      \
      __LIBCOPP_UTIL_LOCK_SPIN_LOCK_THREAD_SLEEP();               \
    }                                                             \
  }

LIBCOPP_COPP_NAMESPACE_BEGIN
namespace util {
namespace lock {
/**
 * @brief 自旋锁
 * @see
 * http://www.boost.org/doc/libs/1_61_0/doc/html/atomic/usage_examples.html#boost_atomic.usage_examples.example_spinlock
 */
class LIBCOPP_COPP_API_HEAD_ONLY spin_lock {
 private:
  typedef enum { UNLOCKED = 0, LOCKED = 1 } lock_state_t;
  LIBCOPP_COPP_NAMESPACE_ID::util::lock::atomic_int_type<
#if defined(LIBCOPP_LOCK_DISABLE_MT) && LIBCOPP_LOCK_DISABLE_MT
      LIBCOPP_COPP_NAMESPACE_ID::util::lock::unsafe_int_type<unsigned int>
#else
      unsigned int
#endif
      >
      lock_status_;

 public:
  inline spin_lock() noexcept { lock_status_.store(UNLOCKED); }

  inline void lock() noexcept {
    unsigned char try_times = 0;
    while (lock_status_.exchange(static_cast<unsigned int>(LOCKED),
                                 LIBCOPP_COPP_NAMESPACE_ID::util::lock::memory_order_acq_rel) == LOCKED)
      __LIBCOPP_UTIL_LOCK_SPIN_LOCK_WAIT(try_times++); /* busy-wait */
  }

  inline void unlock() noexcept {
    lock_status_.store(static_cast<unsigned int>(UNLOCKED),
                       LIBCOPP_COPP_NAMESPACE_ID::util::lock::memory_order_release);
  }

  inline bool is_locked() noexcept {
    return lock_status_.load(LIBCOPP_COPP_NAMESPACE_ID::util::lock::memory_order_acquire) == LOCKED;
  }

  inline bool try_lock() noexcept {
    return lock_status_.exchange(static_cast<unsigned int>(LOCKED),
                                 LIBCOPP_COPP_NAMESPACE_ID::util::lock::memory_order_acq_rel) == UNLOCKED;
  }

  inline bool try_unlock() noexcept {
    return lock_status_.exchange(static_cast<unsigned int>(UNLOCKED),
                                 LIBCOPP_COPP_NAMESPACE_ID::util::lock::memory_order_acq_rel) == LOCKED;
  }
};
}  // namespace lock
}  // namespace util
LIBCOPP_COPP_NAMESPACE_END

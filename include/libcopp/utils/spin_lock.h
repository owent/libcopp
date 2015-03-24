#ifndef _COPP_UTILS_SPIN_LOCK_H_
#define _COPP_UTILS_SPIN_LOCK_H_


#if defined(_MSC_VER) && (_MSC_VER >= 1020)
# pragma once
#endif

#include <cstddef>

#include <libcopp/utils/features.h>

#if defined(__clang__) && (__clang_major__ > 3 || (__clang_major__ == 3 && __clang_minor__ >= 1 ) ) && __cplusplus >= 201103L
    #include <atomic>
    #define COPP_MACRO_UTILS_SPINLOCK_ATOMIC_STD
#elif defined(_MSC_VER) && (_MSC_VER >= 1700) && defined(_HAS_CPP0X) && _HAS_CPP0X
    #include <atomic>
    #define COPP_MACRO_UTILS_SPINLOCK_ATOMIC_STD
#elif defined(__GNUC__) && __GNUC__ >= 4 && __GNUC_MINOR__ >= 5 && (__cplusplus >= 201103L || defined(__GXX_EXPERIMENTAL_CXX0X__))
    #include <atomic>
    #define COPP_MACRO_UTILS_SPINLOCK_ATOMIC_STD
#endif

/**
 * ==============================================
 * ======            asm pause             ======
 * ==============================================
 */
#if defined(_MSC_VER)
    #include <windows.h> // YieldProcessor

    /*
     * See: http://msdn.microsoft.com/en-us/library/windows/desktop/ms687419(v=vs.85).aspx
     * Not for intel c++ compiler, so ignore http://software.intel.com/en-us/forums/topic/296168
     */
    #define COPP_MACRO_UTILS_SPIN_LOCK_PAUSE() YieldProcessor()

#elif defined(__GNUC__) || defined(__clang__)
    #if defined(__i386__) || defined(__x86_64__)
        /**
         * See: Intel(R) 64 and IA-32 Architectures Software Developer's Manual V2
         * PAUSE-Spin Loop Hint, 4-57
         * http://www.intel.com/content/www/us/en/architecture-and-technology/64-ia-32-architectures-software-developer-instruction-set-reference-manual-325383.html?wapkw=instruction+set+reference
         */
        #define COPP_MACRO_UTILS_SPIN_LOCK_PAUSE() __asm__ __volatile__("pause")
    #elif defined(__ia64__) || defined(__ia64)
        /**
         * See: Intel(R) Itanium(R) Architecture Developer's Manual, Vol.3
         * hint - Performance Hint, 3:145
         * http://www.intel.com/content/www/us/en/processors/itanium/itanium-architecture-vol-3-manual.html
         */
        #define COPP_MACRO_UTILS_SPIN_LOCK_PAUSE() __asm__ __volatile__ ("hint @pause")
    #elif defined(__arm__)
        /**
         * See: ARM Architecture Reference Manuals (YIELD)
         * http://infocenter.arm.com/help/index.jsp?topic=/com.arm.doc.subset.architecture.reference/index.html
         */
        #define COPP_MACRO_UTILS_SPIN_LOCK_PAUSE() __asm__ __volatile__ ("yield")
    #endif

#endif /*compilers*/

// set pause do nothing
#if !defined(COPP_MACRO_UTILS_SPIN_LOCK_PAUSE)
    #define COPP_MACRO_UTILS_SPIN_LOCK_PAUSE()
#endif/*!defined(CAPO_SPIN_LOCK_PAUSE)*/


/**
 * ==============================================
 * ======            cpu yield             ======
 * ==============================================
 */
#if defined(_MSC_VER)
    #define COPP_MACRO_UTILS_SPIN_LOCK_CPU_YIELD() SwitchToThread()

#elif defined(__linux__) || defined(__unix__)
    #include <sched.h>
    #define COPP_MACRO_UTILS_SPIN_LOCK_CPU_YIELD() sched_yield()
#endif

#ifndef COPP_MACRO_UTILS_SPIN_LOCK_CPU_YIELD
    #define COPP_MACRO_UTILS_SPIN_LOCK_CPU_YIELD() COPP_MACRO_UTILS_SPIN_LOCK_PAUSE()
#endif

/**
 * ==============================================
 * ======           thread yield           ======
 * ==============================================
 */
#if defined(COPP_MACRO_UTILS_SPINLOCK_ATOMIC_STD)
    #include <thread>
    #include <chrono>
    #define COPP_MACRO_UTILS_SPIN_LOCK_THREAD_YIELD() std::this_thread::yield()
    #define COPP_MACRO_UTILS_SPIN_LOCK_THREAD_SLEEP() std::this_thread::sleep_for(std::chrono::milliseconds(1))
#elif defined(_MSC_VER)
    #define COPP_MACRO_UTILS_SPIN_LOCK_THREAD_YIELD() Sleep(0)
    #define COPP_MACRO_UTILS_SPIN_LOCK_THREAD_SLEEP() Sleep(1)
#endif

#ifndef COPP_MACRO_UTILS_SPIN_LOCK_THREAD_YIELD
    #define COPP_MACRO_UTILS_SPIN_LOCK_THREAD_YIELD()
    #define COPP_MACRO_UTILS_SPIN_LOCK_THREAD_SLEEP() COPP_MACRO_UTILS_SPIN_LOCK_CPU_YIELD()
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
#define COPP_MACRO_UTILS_SPIN_LOCK_WAIT(x) \
    { \
        unsigned char try_lock_times = static_cast<unsigned char>(x); \
        if (try_lock_times < 4) {} \
        else if (try_lock_times < 16) { COPP_MACRO_UTILS_SPIN_LOCK_PAUSE(); } \
        else if (try_lock_times < 32) { COPP_MACRO_UTILS_SPIN_LOCK_THREAD_YIELD(); } \
        else if (try_lock_times < 64) { COPP_MACRO_UTILS_SPIN_LOCK_CPU_YIELD(); } \
        else { COPP_MACRO_UTILS_SPIN_LOCK_THREAD_SLEEP(); } \
    }

namespace copp { 
    namespace utils {
        // C++ 0x/11
        #ifdef COPP_MACRO_UTILS_SPINLOCK_ATOMIC_STD
        /**
         * spin lock
         */
        class spin_lock
        {
        private:
        	/**
        	 * @brief lock status type
        	 */
            typedef enum { EN_SL_UNLOCKED = 0, EN_SL_LOCKED = 1 } lock_state;
            std::atomic_uint status_; /** lock status **/

        public:
            spin_lock() : status_(static_cast<unsigned int>(EN_SL_UNLOCKED)) {}

            /**
             * @brief lock
             */
            void lock()
            {
                unsigned char try_times = 0;
                while (status_.exchange(static_cast<unsigned int>(EN_SL_LOCKED), std::memory_order_acq_rel) == EN_SL_LOCKED)
                    COPP_MACRO_UTILS_SPIN_LOCK_WAIT(try_times ++); /* busy-wait */
            }

            /**
             * @brief unlock
             */
            void unlock()
            {
                status_.store(static_cast<unsigned int>(EN_SL_UNLOCKED), std::memory_order_release);
            }

            /**
             * @return true if locked
             */
            bool is_locked()
            {
                return status_.load(std::memory_order_acquire) == EN_SL_LOCKED;
            }

            /**
             * try lock
             * @return true if lock success
             */
            bool try_lock()
            {
                return status_.exchange(static_cast<unsigned int>(EN_SL_LOCKED), std::memory_order_acq_rel) == EN_SL_UNLOCKED;
            }

            /**
             * try unlock
             * @return true if unlock success
             */
            bool try_unlock()
            {
                return status_.exchange(static_cast<unsigned int>(EN_SL_UNLOCKED), std::memory_order_acq_rel) == EN_SL_LOCKED;
            }

        };
        #else
            #if defined(__clang__)
                #if !defined(__GCC_ATOMIC_INT_LOCK_FREE) && (!defined(__GNUC__) || __GNUC__ < 4 || (__GNUC__ == 4 && __GNUC_MINOR__ < 1))
                    #error Clang version is too old
                #endif

                #if defined(__GCC_ATOMIC_INT_LOCK_FREE)
                    #define COPP_MACRO_UTILS_SPINLOCK_ATOMIC_GCC_ATOMIC 1
                #else
                    #define COPP_MACRO_UTILS_SPINLOCK_ATOMIC_GCC 1
                #endif

            #elif defined(_MSC_VER)
                #include <WinBase.h>
                #define COPP_MACRO_UTILS_SPINLOCK_ATOMIC_MSVC 1

            #elif defined(__GNUC__) || defined(__clang__) || defined(__clang__) || defined(__INTEL_COMPILER)
                #if defined(__GNUC__) && (__GNUC__ < 4 || (__GNUC__ == 4 && __GNUC_MINOR__ < 1))
                    #error GCC version must be greater or equal than 4.1
                #endif
                #if defined(__INTEL_COMPILER) && __INTEL_COMPILER < 1100
                    #error Intel Compiler version must be greater or equal than 11.0
                #endif

                #if defined(__GCC_ATOMIC_INT_LOCK_FREE)
                    #define COPP_MACRO_UTILS_SPINLOCK_ATOMIC_GCC_ATOMIC 1
                #else
                    #define COPP_MACRO_UTILS_SPINLOCK_ATOMIC_GCC 1
                #endif

            #else
                #error Currently only gcc, msvc, intel compiler & llvm-clang are supported
            #endif

        /**
         * spin lock
         */
        class spin_lock
        {
        private:
        	/**
        	 * @brief lock status type
        	 */
            typedef enum { EN_SL_UNLOCKED = 0, EN_SL_LOCKED = 1 } lock_state;
            volatile unsigned int status_; /** lock status **/

        public:
            spin_lock() : status_(EN_SL_UNLOCKED) {}

            /**
             * @brief lock
             */
            void lock()
            {
                unsigned char try_times = 0;

            #ifdef COPP_MACRO_UTILS_SPINLOCK_ATOMIC_MSVC
                while (InterlockedExchange(&status_, EN_SL_LOCKED) == EN_SL_LOCKED)
                    COPP_MACRO_UTILS_SPIN_LOCK_WAIT(try_times ++); /* busy-wait */
            #elif defined(COPP_MACRO_UTILS_SPINLOCK_ATOMIC_GCC_ATOMIC)
                while (__atomic_exchange_n(&status_, EN_SL_LOCKED, __ATOMIC_ACQ_REL) == EN_SL_LOCKED)
                    COPP_MACRO_UTILS_SPIN_LOCK_WAIT(try_times ++); /* busy-wait */
            #else
                while (__sync_lock_test_and_set(&status_, EN_SL_LOCKED) == EN_SL_LOCKED)
                    COPP_MACRO_UTILS_SPIN_LOCK_WAIT(try_times ++); /* busy-wait */
            #endif
            }

            /**
             * @brief unlock
             */
            void unlock()
            {
            #ifdef COPP_MACRO_UTILS_SPINLOCK_ATOMIC_MSVC
                InterlockedExchange(&status_, EN_SL_UNLOCKED);
            #elif defined(COPP_MACRO_UTILS_SPINLOCK_ATOMIC_GCC_ATOMIC)
                __atomic_store_n(&status_, EN_SL_UNLOCKED, __ATOMIC_RELEASE);
            #else
                __sync_lock_release(&status_, EN_SL_UNLOCKED);
            #endif
            }

            /**
             * @return true if locked
             */
            bool is_locked()
            {
            #ifdef COPP_MACRO_UTILS_SPINLOCK_ATOMIC_MSVC
                return InterlockedExchangeAdd(&status_, 0) == EN_SL_LOCKED;
            #elif defined(COPP_MACRO_UTILS_SPINLOCK_ATOMIC_GCC_ATOMIC)
                __atomic_load_n(&status_, __ATOMIC_ACQUIRE) == EN_SL_LOCKED;
            #else
                return __sync_add_and_fetch(&status_, 0) == EN_SL_LOCKED;
            #endif
            }

            /**
             * try lock
             * @return true if lock success
             */
            bool try_lock()
            {

            #ifdef COPP_MACRO_UTILS_SPINLOCK_ATOMIC_MSVC
                return InterlockedExchange(&status_, EN_SL_LOCKED) == EN_SL_UNLOCKED;
            #elif defined(COPP_MACRO_UTILS_SPINLOCK_ATOMIC_GCC_ATOMIC)
                return __atomic_exchange_n(&status_, EN_SL_LOCKED, __ATOMIC_ACQ_REL) == EN_SL_UNLOCKED;
            #else
                return __sync_lock_test_and_set(&status_, EN_SL_LOCKED) == EN_SL_UNLOCKED;
            #endif
            }

            /**
             * try unlock
             * @return true if unlock success
             */
            bool try_unlock()
            {
            #ifdef COPP_MACRO_UTILS_SPINLOCK_ATOMIC_MSVC
                return InterlockedExchange(&status_, EN_SL_UNLOCKED) == EN_SL_LOCKED;
            #elif defined(COPP_MACRO_UTILS_SPINLOCK_ATOMIC_GCC_ATOMIC)
                return __atomic_exchange_n(&status_, EN_SL_UNLOCKED, __ATOMIC_ACQ_REL) == EN_SL_LOCKED;
            #else
                return __sync_lock_test_and_set(&status_, EN_SL_UNLOCKED) == EN_SL_LOCKED;
            #endif
            }

        };

        #endif
    }
}

#endif

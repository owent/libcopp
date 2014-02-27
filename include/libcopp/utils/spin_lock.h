#ifndef _COPP_UTILS_SPIN_LOCK_H_
#define _COPP_UTILS_SPIN_LOCK_H_


#if defined(_MSC_VER) && (_MSC_VER >= 1020)
# pragma once
#endif

#include <cstddef>

#include <libcopp/config/features.h>

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
            std::atomic<lock_state> status_; /** lock status **/

        public:
            spin_lock() : status_(EN_SL_UNLOCKED) {}

            /**
             * @brief lock
             */
            void lock()
            {
                while (status_.exchange(EN_SL_LOCKED, std::memory_order_acq_rel) == EN_SL_LOCKED); /* busy-wait */
            }

            /**
             * @brief unlock
             */
            void unlock()
            {
                status_.store(EN_SL_UNLOCKED, std::memory_order_release);
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
                return status_.exchange(EN_SL_LOCKED, std::memory_order_acq_rel) == EN_SL_UNLOCKED;
            }

            /**
             * try unlock
             * @return true if unlock success
             */
            bool try_unlock()
            {
                return status_.exchange(EN_SL_UNLOCKED, std::memory_order_acq_rel) == EN_SL_LOCKED;
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
            #ifdef COPP_MACRO_UTILS_SPINLOCK_ATOMIC_MSVC
                while (InterlockedExchange(&status_, EN_SL_LOCKED) == EN_SL_LOCKED); /* busy-wait */
            #elif defined(COPP_MACRO_UTILS_SPINLOCK_ATOMIC_GCC_ATOMIC)
                while (__atomic_exchange_n(&status_, EN_SL_LOCKED, __ATOMIC_ACQ_REL) == EN_SL_LOCKED); /* busy-wait */
            #else
                while (__sync_lock_test_and_set(&status_, EN_SL_LOCKED) == EN_SL_LOCKED); /* busy-wait */
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

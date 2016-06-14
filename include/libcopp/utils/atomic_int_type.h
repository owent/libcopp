﻿/**
 * @file atomic_int_type.h
 * @brief 整数类型的原子操作跨平台适配
 * Licensed under the MIT licenses.
 *
 * @version 1.0
 * @author OWenT
 * @date 2016-06-14
 *
 * @note VC 2012+, GCC 4.4 + 使用C++0x/11实现实现原子操作
 * @note 低版本 VC使用InterlockedExchange等实现原子操作
 * @note 低版本 GCC采用__sync_lock_test_and_set等实现原子操作
 *
 * @history
 *     2016-06-14
 */

#ifndef _UTIL_LOCK_ATOMIC_INT_TYPE_H_
#define _UTIL_LOCK_ATOMIC_INT_TYPE_H_

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif

#include <cstddef>
#if defined(__cplusplus) && __cplusplus >= 201103L
#include <atomic>
#define __UTIL_LOCK_ATOMIC_INT_TYPE_ATOMIC_STD
#elif defined(__clang__) && (__clang_major__ > 3 || (__clang_major__ == 3 && __clang_minor__ >= 1)) && __cplusplus >= 201103L
#include <atomic>
#define __UTIL_LOCK_ATOMIC_INT_TYPE_ATOMIC_STD
#elif defined(_MSC_VER) && (_MSC_VER > 1700 || (defined(_HAS_CPP0X) && _HAS_CPP0X))
#include <atomic>
#define __UTIL_LOCK_ATOMIC_INT_TYPE_ATOMIC_STD
#elif defined(__GNUC__) && ((__GNUC__ == 4 && __GNUC_MINOR__ >= 5) || __GNUC__ > 4) && defined(__GXX_EXPERIMENTAL_CXX0X__)
#include <atomic>
#define __UTIL_LOCK_ATOMIC_INT_TYPE_ATOMIC_STD
#endif

// check noexpect

namespace util {
    namespace lock {
#ifdef __UTIL_LOCK_ATOMIC_INT_TYPE_ATOMIC_STD
        typedef ::std::memory_order memory_order;

        /**
         * @brief atomic - C++ 0x/11版实现
         * @see http://en.cppreference.com/w/cpp/atomic/atomic
         * @exception noexcept
         */
        template<typename Ty = int>
        class atomic_int_type {
        public:
            typedef Ty value_type;

        private:
            ::std::atomic<value_type> data_;

        public:
            atomic_int_type(): data_() {}
            atomic_int_type(value_type desired): data_(desired) {}
            atomic_int_type(const atomic_int_type&);

            inline void store(value_type desired, ::util::lock::memory_order order = ::util::lock::memory_order_seq_cst) { data_.store(desired, order); }
            inline void store(value_type desired, ::util::lock::memory_order order = ::util::lock::memory_order_seq_cst) volatile { data_.store(desired, order); }

            inline value_type load( ::util::lock::memory_order order = ::util::lock::memory_order_seq_cst ) const { data_.load(order); }
            inline value_type load( ::util::lock::memory_order order = ::util::lock::memory_order_seq_cst ) const volatile { data_.load(order); }
	
            inline operator value_type() const { return load(); }
            inline operator value_type() const volatile { return load(); }

            inline value_type exchange( value_type desired, ::util::lock::memory_order order = ::util::lock::memory_order_seq_cst ) { data_.exchange(desired, order); }
            inline value_type exchange( value_type desired, ::util::lock::memory_order order = ::util::lock::memory_order_seq_cst ) volatile { data_.exchange(desired, order); }
	
            inline bool compare_exchange_weak( value_type& expected, value_type desired, ::util::lock::memory_order success, ::util::lock::memory_order failure ) { data_.compare_exchange_weak(expected, desired, success, failure); }
            inline bool compare_exchange_weak( value_type& expected, value_type desired, ::util::lock::memory_order success, ::util::lock::memory_order failure ) volatile { data_.compare_exchange_weak(expected, desired, success, failure); }

            inline bool compare_exchange_weak( value_type& expected, value_type desired, ::util::lock::memory_order order = ::util::lock::memory_order_seq_cst ) { data_.compare_exchange_weak(expected, desired, order); }
            inline bool compare_exchange_weak( value_type& expected, value_type desired, ::util::lock::memory_order order = ::util::lock::memory_order_seq_cst ) volatile { data_.compare_exchange_weak(expected, desired, order); }

            inline bool compare_exchange_strong( value_type& expected, value_type desired, ::util::lock::memory_order success, ::util::lock::memory_order failure ) { data_.compare_exchange_strong(expected, desired, success, failure); }
            inline bool compare_exchange_strong( value_type& expected, value_type desired, ::util::lock::memory_order success, ::util::lock::memory_order failure ) volatile { data_.compare_exchange_strong(expected, desired, success, failure); }

            inline bool compare_exchange_strong( value_type& expected, value_type desired, ::util::lock::memory_order order = ::util::lock::memory_order_seq_cst ) { data_.compare_exchange_strong(expected, desired, order); }
            inline bool compare_exchange_strong( value_type& expected, value_type desired, ::util::lock::memory_order order = ::util::lock::memory_order_seq_cst ) volatile { data_.compare_exchange_strong(expected, desired, order); }

            inline value_type fetch_add( value_type arg, ::util::lock::memory_order order = ::util::lock::memory_order_seq_cst ) { data_.fetch_add(arg, order); }
            inline value_type fetch_add( value_type arg, ::util::lock::memory_order order = ::util::lock::memory_order_seq_cst ) volatile { data_.fetch_add(arg, order); }

            inline value_type fetch_sub( value_type arg, ::util::lock::memory_order order = ::util::lock::memory_order_seq_cst ) { data_.fetch_sub(arg, order); }
            inline value_type fetch_sub( value_type arg, ::util::lock::memory_order order = ::util::lock::memory_order_seq_cst ) volatile { data_.fetch_sub(arg, order); }

            inline value_type fetch_and( value_type arg, ::util::lock::memory_order order = ::util::lock::memory_order_seq_cst ) { data_.fetch_and(arg, order); }
            inline value_type fetch_and( value_type arg, ::util::lock::memory_order order = ::util::lock::memory_order_seq_cst ) volatile { data_.fetch_and(arg, order); }

            inline value_type fetch_or( value_type arg, ::util::lock::memory_order order = ::util::lock::memory_order_seq_cst ) { data_.fetch_or(arg, order); }
            inline value_type fetch_or( value_type arg, ::util::lock::memory_order order = ::util::lock::memory_order_seq_cst ) volatile { data_.fetch_or(arg, order); }

            inline value_type fetch_xor( value_type arg, ::util::lock::memory_order order = ::util::lock::memory_order_seq_cst ) { data_.fetch_xor(arg, order); }
            inline value_type fetch_xor( value_type arg, ::util::lock::memory_order order = ::util::lock::memory_order_seq_cst ) volatile { data_.fetch_xor(arg, order); }
        };
#else

#if defined(__clang__)

#if !defined(__GCC_ATOMIC_INT_LOCK_FREE) && (!defined(__GNUC__) || __GNUC__ < 4 || (__GNUC__ == 4 && __GNUC_MINOR__ < 1))
#error clang version is too old
#endif

#if defined(__GCC_ATOMIC_INT_LOCK_FREE)
// @see https://gcc.gnu.org/onlinedocs/gcc/_005f_005fatomic-Builtins.html
#define __UTIL_LOCK_ATOMIC_INT_ATOMIC_GCC_ATOMIC 1
#else
// @see https://gcc.gnu.org/onlinedocs/gcc-4.1.2/gcc/Atomic-Builtins.html
#define __UTIL_LOCK_ATOMIC_INT_ATOMIC_GCC 1
#endif

#elif defined(_MSC_VER)

#include <WinBase.h>
#define __UTIL_LOCK_ATOMIC_INT_ATOMIC_MSVC 1

#elif defined(__GNUC__) || defined(__INTEL_COMPILER)

#if defined(__GNUC__) && (__GNUC__ < 4 || (__GNUC__ == 4 && __GNUC_MINOR__ < 1))
#error gcc version must be greater or equal than 4.1
#endif

#if defined(__INTEL_COMPILER) && __INTEL_COMPILER < 1100
#error intel compiler version must be greater or equal than 11.0
#endif

#if defined(__GCC_ATOMIC_INT_LOCK_FREE)
// @see https://gcc.gnu.org/onlinedocs/gcc/_005f_005fatomic-Builtins.html
#define __UTIL_LOCK_ATOMIC_INT_ATOMIC_GCC_ATOMIC 1
#else
// @see https://gcc.gnu.org/onlinedocs/gcc-4.1.2/gcc/Atomic-Builtins.html
#define __UTIL_LOCK_ATOMIC_INT_ATOMIC_GCC 1
#endif

#else

#error currently only gcc, msvc, intel compiler & llvm-clang are supported

#endif

#ifdef defined(__UTIL_LOCK_ATOMIC_INT_ATOMIC_GCC_ATOMIC)
        enum memory_order {
            memory_order_relaxed = __ATOMIC_RELAXED,
            memory_order_consume = __ATOMIC_CONSUME,
            memory_order_acquire = __ATOMIC_ACQUIRE,
            memory_order_release = __ATOMIC_RELEASE,
            memory_order_acq_rel = __ATOMIC_ACQ_REL,
            memory_order_seq_cst = __ATOMIC_SEQ_CST
        };
#else // old gcc and old msvc use this
        enum memory_order {
            memory_order_relaxed = 0,
            memory_order_consume,
            memory_order_acquire,
            memory_order_release,
            memory_order_acq_rel,
            memory_order_seq_cst
        };
#endif

#ifdef __UTIL_LOCK_ATOMIC_INT_ATOMIC_MSVC
        #include "atomic_int_type_msvc_impl.h"
#endif

        template<typename Ty = int>
        class atomic_int_type {
        public:
            typedef Ty value_type;

        private:
            typedef Ty value_type;
            volatile value_type data_;

        public:
            atomic_int_type(): data_() {}
            atomic_int_type(value_type desired): data_(desired) {}
            atomic_int_type(const atomic_int_type&);

            void store(value_type desired, ::util::lock::memory_order order = ::util::lock::memory_order_seq_cst) {
#ifdef __UTIL_LOCK_ATOMIC_INT_ATOMIC_MSVC
                typedef detail::atomic_msvc_oprs<sizeof(value_type)> int_opr_t;
                typedef typename int_opr_t::opr_t opr_t;
                int_opr_t::exchange(static_cast<opr_t*>(&data_), static_cast<opr_t>(desired), order);

#elif defined(__UTIL_LOCK_ATOMIC_INT_ATOMIC_GCC_ATOMIC)
                __atomic_store_n(&data_, desired, order);
#else
                data_ = desired;
                __sync_synchronize(data_);
#endif
            }

            void store(value_type desired, ::util::lock::memory_order order = ::util::lock::memory_order_seq_cst) volatile {
#ifdef __UTIL_LOCK_ATOMIC_INT_ATOMIC_MSVC
                typedef detail::atomic_msvc_oprs<sizeof(value_type)> int_opr_t;
                typedef typename int_opr_t::opr_t opr_t;
                int_opr_t::exchange(static_cast<opr_t*>(&data_), static_cast<opr_t>(desired), order);

#elif defined(__UTIL_LOCK_ATOMIC_INT_ATOMIC_GCC_ATOMIC)
                __atomic_store_n(&data_, desired, order);
#else
                data_ = desired;
                __sync_synchronize(data_);
#endif
            }

            value_type load( ::util::lock::memory_order order = ::util::lock::memory_order_seq_cst ) const {
#ifdef __UTIL_LOCK_ATOMIC_INT_ATOMIC_MSVC
                typedef detail::atomic_msvc_oprs<sizeof(value_type)> int_opr_t;
                typedef typename int_opr_t::opr_t opr_t;
                int_opr_t::or(static_cast<opr_t*>(&data_), static_cast<opr_t>(0), order);

#elif defined(__UTIL_LOCK_ATOMIC_INT_ATOMIC_GCC_ATOMIC)
                return __atomic_load_n(&data_, order);
#else
                __sync_synchronize(data_);
                return data_;
#endif
            }

            value_type load( ::util::lock::memory_order order = ::util::lock::memory_order_seq_cst ) const volatile {
#ifdef __UTIL_LOCK_ATOMIC_INT_ATOMIC_MSVC
                typedef detail::atomic_msvc_oprs<sizeof(value_type)> int_opr_t;
                typedef typename int_opr_t::opr_t opr_t;
                return int_opr_t::or(static_cast<opr_t*>(&data_), static_cast<opr_t>(0), order);

#elif defined(__UTIL_LOCK_ATOMIC_INT_ATOMIC_GCC_ATOMIC)
                return __atomic_load_n(&data_, order);
#else
                __sync_synchronize(data_);
                return data_;
#endif
            }
	
            inline operator value_type() const { return load(); }
            inline operator value_type() const volatile { return load(); }

            value_type exchange( value_type desired, ::util::lock::memory_order order = ::util::lock::memory_order_seq_cst ) {
#ifdef __UTIL_LOCK_ATOMIC_INT_ATOMIC_MSVC
                typedef detail::atomic_msvc_oprs<sizeof(value_type)> int_opr_t;
                typedef typename int_opr_t::opr_t opr_t;
                return static_cast<value_type>(int_opr_t::exchange(static_cast<opr_t*>(&data_), static_cast<opr_t>(desired), order));

#elif defined(__UTIL_LOCK_ATOMIC_INT_ATOMIC_GCC_ATOMIC)
                return __atomic_exchange_n(&data_, desired, order);
#else
                value_type old_value = data_;
                while(!__sync_bool_compare_and_swap(&data_, old_value, desired)) {
                    old_value = data_;
                }
                return old_value;
#endif
            }

            value_type exchange( value_type desired, ::util::lock::memory_order order = ::util::lock::memory_order_seq_cst ) volatile {
#ifdef __UTIL_LOCK_ATOMIC_INT_ATOMIC_MSVC
                typedef detail::atomic_msvc_oprs<sizeof(value_type)> int_opr_t;
                typedef typename int_opr_t::opr_t opr_t;
                return static_cast<value_type>(int_opr_t::exchange(static_cast<opr_t*>(&data_), static_cast<opr_t>(desired), order));

#elif defined(__UTIL_LOCK_ATOMIC_INT_ATOMIC_GCC_ATOMIC)
                return __atomic_exchange_n(&data_, desired, order);
#else
                value_type old_value = data_;
                while(!__sync_bool_compare_and_swap(&data_, old_value, desired)) {
                    old_value = data_;
                }
                return old_value;
#endif
            }
	
            bool compare_exchange_weak( value_type& expected, value_type desired, ::util::lock::memory_order success, ::util::lock::memory_order failure ) {
#ifdef __UTIL_LOCK_ATOMIC_INT_ATOMIC_MSVC
                typedef detail::atomic_msvc_oprs<sizeof(value_type)> int_opr_t;
                typedef typename int_opr_t::opr_t opr_t;
                if(expected == static_cast<value_type>(int_opr_t::cas(static_cast<opr_t*>(&data_), static_cast<opr_t>(desired), static_cast<opr_t>(expected), success))) {
                    return true;
                } else {
                    expected = data_;
                    return false;
                }

#elif defined(__UTIL_LOCK_ATOMIC_INT_ATOMIC_GCC_ATOMIC)
                return __atomic_compare_exchange_n(&data_, &expected, desired, true, success, failure);
#else
                if(__sync_bool_compare_and_swap(&data_, expected, desired)) {
                    return true;
                } else {
                    expected = data_;
                    return false;
                }
#endif
            }

            bool compare_exchange_weak( value_type& expected, value_type desired, ::util::lock::memory_order success, ::util::lock::memory_order failure ) volatile {
#ifdef __UTIL_LOCK_ATOMIC_INT_ATOMIC_MSVC
                typedef detail::atomic_msvc_oprs<sizeof(value_type)> int_opr_t;
                typedef typename int_opr_t::opr_t opr_t;
                if(expected == static_cast<value_type>(int_opr_t::cas(static_cast<opr_t*>(&data_), static_cast<opr_t>(desired), static_cast<opr_t>(expected), success))) {
                    return true;
                } else {
                    expected = data_;
                    return false;
                }

#elif defined(__UTIL_LOCK_ATOMIC_INT_ATOMIC_GCC_ATOMIC)
                return __atomic_compare_exchange_n(&data_, &expected, desired, true, success, failure);
#else
                if(__sync_bool_compare_and_swap(&data_, expected, desired)) {
                    return true;
                } else {
                    expected = data_;
                    return false;
                }
#endif
            }

            bool compare_exchange_weak( value_type& expected, value_type desired, ::util::lock::memory_order order = ::util::lock::memory_order_seq_cst ) {
#ifdef __UTIL_LOCK_ATOMIC_INT_ATOMIC_MSVC
                typedef detail::atomic_msvc_oprs<sizeof(value_type)> int_opr_t;
                typedef typename int_opr_t::opr_t opr_t;
                if(expected == static_cast<value_type>(int_opr_t::cas(static_cast<opr_t*>(&data_), static_cast<opr_t>(desired), static_cast<opr_t>(expected), order))) {
                    return true;
                } else {
                    expected = data_;
                    return false;
                }

#elif defined(__UTIL_LOCK_ATOMIC_INT_ATOMIC_GCC_ATOMIC)
                return __atomic_compare_exchange_n(&data_, &expected, desired, true, order, order);
#else
                if(__sync_bool_compare_and_swap(&data_, expected, desired)) {
                    return true;
                } else {
                    expected = data_;
                    return false;
                }
#endif
            }

            bool compare_exchange_weak( value_type& expected, value_type desired, ::util::lock::memory_order order = ::util::lock::memory_order_seq_cst ) volatile {
#ifdef __UTIL_LOCK_ATOMIC_INT_ATOMIC_MSVC
                typedef detail::atomic_msvc_oprs<sizeof(value_type)> int_opr_t;
                typedef typename int_opr_t::opr_t opr_t;
                if(expected == static_cast<value_type>(int_opr_t::cas(static_cast<opr_t*>(&data_), static_cast<opr_t>(desired), static_cast<opr_t>(expected), order))) {
                    return true;
                } else {
                    expected = data_;
                    return false;
                }

#elif defined(__UTIL_LOCK_ATOMIC_INT_ATOMIC_GCC_ATOMIC)
                return __atomic_compare_exchange_n(&data_, &expected, desired, true, order, order);
#else
                if(__sync_bool_compare_and_swap(&data_, expected, desired)) {
                    return true;
                } else {
                    expected = data_;
                    return false;
                }
#endif
            }

            bool compare_exchange_strong( value_type& expected, value_type desired, ::util::lock::memory_order success, ::util::lock::memory_order failure ) {
#ifdef __UTIL_LOCK_ATOMIC_INT_ATOMIC_MSVC
                typedef detail::atomic_msvc_oprs<sizeof(value_type)> int_opr_t;
                typedef typename int_opr_t::opr_t opr_t;
                if(expected == static_cast<value_type>(int_opr_t::cas(static_cast<opr_t*>(&data_), static_cast<opr_t>(desired), static_cast<opr_t>(expected), success))) {
                    return true;
                } else {
                    expected = data_;
                    return false;
                }

#elif defined(__UTIL_LOCK_ATOMIC_INT_ATOMIC_GCC_ATOMIC)
                return __atomic_compare_exchange_n(&data_, &expected, desired, false, success, failure);
#else
                if(__sync_bool_compare_and_swap(&data_, expected, desired)) {
                    return true;
                } else {
                    expected = data_;
                    return false;
                }
#endif
            }

            bool compare_exchange_strong( value_type& expected, value_type desired, ::util::lock::memory_order success, ::util::lock::memory_order failure ) volatile {
#ifdef __UTIL_LOCK_ATOMIC_INT_ATOMIC_MSVC
                typedef detail::atomic_msvc_oprs<sizeof(value_type)> int_opr_t;
                typedef typename int_opr_t::opr_t opr_t;
                if(expected == static_cast<value_type>(int_opr_t::cas(static_cast<opr_t*>(&data_), static_cast<opr_t>(desired), static_cast<opr_t>(expected), success))) {
                    return true;
                } else {
                    expected = data_;
                    return false;
                }

#elif defined(__UTIL_LOCK_ATOMIC_INT_ATOMIC_GCC_ATOMIC)
                return __atomic_compare_exchange_n(&data_, &expected, desired, false, success, failure);
#else
                if(__sync_bool_compare_and_swap(&data_, expected, desired)) {
                    return true;
                } else {
                    expected = data_;
                    return false;
                }
#endif
            }

            bool compare_exchange_strong( value_type& expected, value_type desired, ::util::lock::memory_order order = ::util::lock::memory_order_seq_cst ) {
#ifdef __UTIL_LOCK_ATOMIC_INT_ATOMIC_MSVC
                typedef detail::atomic_msvc_oprs<sizeof(value_type)> int_opr_t;
                typedef typename int_opr_t::opr_t opr_t;
                if(expected == static_cast<value_type>(int_opr_t::cas(static_cast<opr_t*>(&data_), static_cast<opr_t>(desired), static_cast<opr_t>(expected), order))) {
                    return true;
                } else {
                    expected = data_;
                    return false;
                }

#elif defined(__UTIL_LOCK_ATOMIC_INT_ATOMIC_GCC_ATOMIC)
                return __atomic_compare_exchange_n(&data_, &expected, desired, false, order, order);
#else
                if(__sync_bool_compare_and_swap(&data_, expected, desired)) {
                    return true;
                } else {
                    expected = data_;
                    return false;
                }
#endif
            }

            bool compare_exchange_strong( value_type& expected, value_type desired, ::util::lock::memory_order order = ::util::lock::memory_order_seq_cst ) volatile {
#ifdef __UTIL_LOCK_ATOMIC_INT_ATOMIC_MSVC
                typedef detail::atomic_msvc_oprs<sizeof(value_type)> int_opr_t;
                typedef typename int_opr_t::opr_t opr_t;
                if(expected == static_cast<value_type>(int_opr_t::cas(static_cast<opr_t*>(&data_), static_cast<opr_t>(desired), static_cast<opr_t>(expected), order))) {
                    return true;
                } else {
                    expected = data_;
                    return false;
                }

#elif defined(__UTIL_LOCK_ATOMIC_INT_ATOMIC_GCC_ATOMIC)
                return __atomic_compare_exchange_n(&data_, &expected, desired, false, order, order);
#else
                if(__sync_bool_compare_and_swap(&data_, expected, desired)) {
                    return true;
                } else {
                    expected = data_;
                    return false;
                }
#endif
            }

            value_type fetch_add( value_type arg, ::util::lock::memory_order order = ::util::lock::memory_order_seq_cst );
            value_type fetch_add( value_type arg, ::util::lock::memory_order order = ::util::lock::memory_order_seq_cst ) volatile;

            value_type fetch_sub( value_type arg, ::util::lock::memory_order order = ::util::lock::memory_order_seq_cst );
            value_type fetch_sub( value_type arg, ::util::lock::memory_order order = ::util::lock::memory_order_seq_cst ) volatile;

            value_type fetch_and( value_type arg, ::util::lock::memory_order order = ::util::lock::memory_order_seq_cst );
            value_type fetch_and( value_type arg, ::util::lock::memory_order order = ::util::lock::memory_order_seq_cst ) volatile;

            value_type fetch_or( value_type arg, ::util::lock::memory_order order = ::util::lock::memory_order_seq_cst );
            value_type fetch_or( value_type arg, ::util::lock::memory_order order = ::util::lock::memory_order_seq_cst ) volatile;

            value_type fetch_xor( value_type arg, ::util::lock::memory_order order = ::util::lock::memory_order_seq_cst );
            value_type fetch_xor( value_type arg, ::util::lock::memory_order order = ::util::lock::memory_order_seq_cst ) volatile;
        };

#endif
    }
}

#endif /* _UTIL_LOCK_ATOMIC_INT_TYPE_H_ */

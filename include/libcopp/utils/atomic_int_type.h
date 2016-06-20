/**
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

#pragma once

#if defined(__cplusplus) && __cplusplus >= 201103L

#include <atomic>
#define __UTIL_LOCK_ATOMIC_INT_TYPE_ATOMIC_STD

#elif defined(__clang__) && (__clang_major__ > 3 || (__clang_major__ == 3 && __clang_minor__ >= 1)) && __cplusplus >= 201103L

#include <atomic>
#define __UTIL_LOCK_ATOMIC_INT_TYPE_ATOMIC_STD

#elif defined(_MSC_VER) && (_MSC_VER > 1700 || (defined(_HAS_CPP0X) && _HAS_CPP0X)) // VC 11,2012

#include <atomic>
#define __UTIL_LOCK_ATOMIC_INT_TYPE_ATOMIC_STD

// There is a BUG in gcc 4.6, which will cause 'undefined reference to `std::atomic_thread_fence(std::memory_order)'
// In gcc 4.7 and upper, we can use -std=c++11 or upper
// https://gcc.gnu.org/bugzilla/show_bug.cgi?id=51038
// #elif defined(__GNUC__) && ((__GNUC__ == 4 && __GNUC_MINOR__ >= 5) || __GNUC__ > 4) && defined(__GXX_EXPERIMENTAL_CXX0X__)
// 
// #include <atomic>
// #define __UTIL_LOCK_ATOMIC_INT_TYPE_ATOMIC_STD

#endif

#if !defined(__UTIL_LOCK_ATOMIC_INT_TYPE_ATOMIC_STD) && defined(_MSC_VER)
#include "atomic_int_type_msvc_impl.h"
#endif

#include <cstddef>

namespace util {
    namespace lock {
#ifdef __UTIL_LOCK_ATOMIC_INT_TYPE_ATOMIC_STD
        using ::std::memory_order;
        using ::std::memory_order_relaxed;
        using ::std::memory_order_consume;
        using ::std::memory_order_acquire;
        using ::std::memory_order_release;
        using ::std::memory_order_acq_rel;
        using ::std::memory_order_seq_cst;

        #define UTIL_LOCK_ATOMIC_THREAD_FENCE(order) ::std::atomic_thread_fence(order)
        #define UTIL_LOCK_ATOMIC_SIGNAL_FENCE(order) ::std::atomic_signal_fence(order)

        /**
         * @brief atomic - C++ 0x/11版实现
         * @see http://en.cppreference.com/w/cpp/atomic/atomic
         * @exception noexcept
         * @note Ty can only be a integer or enum, can not be bool or raw pointer
         */
        template<typename Ty = int>
        class atomic_int_type {
        public:
            typedef Ty value_type;

        private:
            ::std::atomic<value_type> data_;
            atomic_int_type(const atomic_int_type&);

        public:
            atomic_int_type() : data_() {}
            atomic_int_type(value_type desired) : data_(desired) {}

            inline void store(value_type desired, ::util::lock::memory_order order = ::util::lock::memory_order_seq_cst) { data_.store(desired, order); }
            inline void store(value_type desired, ::util::lock::memory_order order = ::util::lock::memory_order_seq_cst) volatile { data_.store(desired, order); }

            inline value_type load(::util::lock::memory_order order = ::util::lock::memory_order_seq_cst) const { return data_.load(order); }
            inline value_type load(::util::lock::memory_order order = ::util::lock::memory_order_seq_cst) const volatile { return data_.load(order); }

            inline operator value_type() const { return load(); }
            inline operator value_type() const volatile { return load(); }

            inline value_type operator++() { return ++data_; }
            inline value_type operator++() volatile { return ++data_; }
            inline value_type operator++(int) { return data_++; }
            inline value_type operator++(int) volatile { return data_++; }
            inline value_type operator--() { return --data_; }
            inline value_type operator--() volatile { return --data_; }
            inline value_type operator--(int) { return data_--; }
            inline value_type operator--(int) volatile { return data_--; }

            inline value_type exchange(value_type desired, ::util::lock::memory_order order = ::util::lock::memory_order_seq_cst) { return data_.exchange(desired, order); }
            inline value_type exchange(value_type desired, ::util::lock::memory_order order = ::util::lock::memory_order_seq_cst) volatile { return data_.exchange(desired, order); }

            inline bool compare_exchange_weak(value_type& expected, value_type desired, ::util::lock::memory_order success, ::util::lock::memory_order failure) { return data_.compare_exchange_weak(expected, desired, success, failure); }
            inline bool compare_exchange_weak(value_type& expected, value_type desired, ::util::lock::memory_order success, ::util::lock::memory_order failure) volatile { return data_.compare_exchange_weak(expected, desired, success, failure); }

            inline bool compare_exchange_weak(value_type& expected, value_type desired, ::util::lock::memory_order order = ::util::lock::memory_order_seq_cst) { return data_.compare_exchange_weak(expected, desired, order); }
            inline bool compare_exchange_weak(value_type& expected, value_type desired, ::util::lock::memory_order order = ::util::lock::memory_order_seq_cst) volatile { return data_.compare_exchange_weak(expected, desired, order); }

            inline bool compare_exchange_strong(value_type& expected, value_type desired, ::util::lock::memory_order success, ::util::lock::memory_order failure) { return data_.compare_exchange_strong(expected, desired, success, failure); }
            inline bool compare_exchange_strong(value_type& expected, value_type desired, ::util::lock::memory_order success, ::util::lock::memory_order failure) volatile { return data_.compare_exchange_strong(expected, desired, success, failure); }

            inline bool compare_exchange_strong(value_type& expected, value_type desired, ::util::lock::memory_order order = ::util::lock::memory_order_seq_cst) { return data_.compare_exchange_strong(expected, desired, order); }
            inline bool compare_exchange_strong(value_type& expected, value_type desired, ::util::lock::memory_order order = ::util::lock::memory_order_seq_cst) volatile { return data_.compare_exchange_strong(expected, desired, order); }

            inline value_type fetch_add(value_type arg, ::util::lock::memory_order order = ::util::lock::memory_order_seq_cst) { return data_.fetch_add(arg, order); }
            inline value_type fetch_add(value_type arg, ::util::lock::memory_order order = ::util::lock::memory_order_seq_cst) volatile { return data_.fetch_add(arg, order); }

            inline value_type fetch_sub(value_type arg, ::util::lock::memory_order order = ::util::lock::memory_order_seq_cst) { return data_.fetch_sub(arg, order); }
            inline value_type fetch_sub(value_type arg, ::util::lock::memory_order order = ::util::lock::memory_order_seq_cst) volatile { return data_.fetch_sub(arg, order); }

            inline value_type fetch_and(value_type arg, ::util::lock::memory_order order = ::util::lock::memory_order_seq_cst) { return data_.fetch_and(arg, order); }
            inline value_type fetch_and(value_type arg, ::util::lock::memory_order order = ::util::lock::memory_order_seq_cst) volatile { return data_.fetch_and(arg, order); }

            inline value_type fetch_or(value_type arg, ::util::lock::memory_order order = ::util::lock::memory_order_seq_cst) { return data_.fetch_or(arg, order); }
            inline value_type fetch_or(value_type arg, ::util::lock::memory_order order = ::util::lock::memory_order_seq_cst) volatile { return data_.fetch_or(arg, order); }

            inline value_type fetch_xor(value_type arg, ::util::lock::memory_order order = ::util::lock::memory_order_seq_cst) { return data_.fetch_xor(arg, order); }
            inline value_type fetch_xor(value_type arg, ::util::lock::memory_order order = ::util::lock::memory_order_seq_cst) volatile { return data_.fetch_xor(arg, order); }
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

#if defined(__UTIL_LOCK_ATOMIC_INT_ATOMIC_GCC_ATOMIC)
        enum memory_order {
            memory_order_relaxed = __ATOMIC_RELAXED,
            memory_order_consume = __ATOMIC_CONSUME,
            memory_order_acquire = __ATOMIC_ACQUIRE,
            memory_order_release = __ATOMIC_RELEASE,
            memory_order_acq_rel = __ATOMIC_ACQ_REL,
            memory_order_seq_cst = __ATOMIC_SEQ_CST
        };

        #define UTIL_LOCK_ATOMIC_THREAD_FENCE(order) __atomic_thread_fence(order)
        #define UTIL_LOCK_ATOMIC_SIGNAL_FENCE(order) __atomic_signal_fence(order)

#elif !defined(__UTIL_LOCK_ATOMIC_INT_ATOMIC_MSVC) // old gcc and old msvc use this
        enum memory_order {
            memory_order_relaxed = 0,
            memory_order_consume,
            memory_order_acquire,
            memory_order_release,
            memory_order_acq_rel,
            memory_order_seq_cst
        };
#endif

#ifndef UTIL_LOCK_ATOMIC_THREAD_FENCE
#define UTIL_LOCK_ATOMIC_THREAD_FENCE(x)
#endif

#ifndef UTIL_LOCK_ATOMIC_SIGNAL_FENCE
#define UTIL_LOCK_ATOMIC_SIGNAL_FENCE(x)
#endif

        template<typename Ty = int>
        class atomic_int_type {
        public:
            typedef Ty value_type;

        private:
#ifdef __UTIL_LOCK_ATOMIC_INT_ATOMIC_MSVC
            // char has no cas api in msvc
            union {
                volatile value_type data_;
                volatile short padding;
            };
#else
            volatile value_type data_;
#endif
            atomic_int_type(const atomic_int_type&);

        public:
            atomic_int_type(): data_() {}
            atomic_int_type(value_type desired): data_(desired) {}

            inline void store(value_type desired, ::util::lock::memory_order order = ::util::lock::memory_order_seq_cst) {
#ifdef __UTIL_LOCK_ATOMIC_INT_ATOMIC_MSVC
                typedef detail::atomic_msvc_oprs<sizeof(value_type)> int_opr_t;
                typedef typename int_opr_t::opr_t opr_t;
                int_opr_t::exchange(reinterpret_cast<volatile opr_t*>(&data_), static_cast<opr_t>(desired), order);

#elif defined(__UTIL_LOCK_ATOMIC_INT_ATOMIC_GCC_ATOMIC)
                __atomic_store_n(&data_, desired, order);
#else
                __sync_lock_test_and_set(&data_, desired);
#endif
            }

            inline void store(value_type desired, ::util::lock::memory_order order = ::util::lock::memory_order_seq_cst) volatile {
#ifdef __UTIL_LOCK_ATOMIC_INT_ATOMIC_MSVC
                typedef detail::atomic_msvc_oprs<sizeof(value_type)> int_opr_t;
                typedef typename int_opr_t::opr_t opr_t;
                int_opr_t::exchange(static_cast<opr_t*>(&data_), static_cast<opr_t>(desired), order);

#elif defined(__UTIL_LOCK_ATOMIC_INT_ATOMIC_GCC_ATOMIC)
                __atomic_store_n(&data_, desired, order);
#else
                __sync_lock_test_and_set(&data_, desired);
#endif
            }

            inline value_type load( ::util::lock::memory_order order = ::util::lock::memory_order_seq_cst ) const {
#ifdef __UTIL_LOCK_ATOMIC_INT_ATOMIC_MSVC
                typedef detail::atomic_msvc_oprs<sizeof(value_type)> int_opr_t;
                typedef typename int_opr_t::opr_t opr_t;
                return int_opr_t::or(const_cast<opr_t*>(reinterpret_cast<volatile const opr_t*>(&data_)), static_cast<opr_t>(0), order);

#elif defined(__UTIL_LOCK_ATOMIC_INT_ATOMIC_GCC_ATOMIC)
                return __atomic_load_n(&data_, order);
#else
                __sync_synchronize();
                return data_;
#endif
            }

            inline value_type load( ::util::lock::memory_order order = ::util::lock::memory_order_seq_cst ) const volatile {
#ifdef __UTIL_LOCK_ATOMIC_INT_ATOMIC_MSVC
                typedef detail::atomic_msvc_oprs<sizeof(value_type)> int_opr_t;
                typedef typename int_opr_t::opr_t opr_t;
                return int_opr_t::or(const_cast<opr_t*>(reinterpret_cast<volatile const opr_t*>(&data_)), static_cast<opr_t>(0), order);

#elif defined(__UTIL_LOCK_ATOMIC_INT_ATOMIC_GCC_ATOMIC)
                return __atomic_load_n(&data_, order);
#else
                __sync_synchronize();
                return data_;
#endif
            }
    
            inline operator value_type() const { return load(); }
            inline operator value_type() const volatile { return load(); }

            inline value_type operator++() { 
#ifdef __UTIL_LOCK_ATOMIC_INT_ATOMIC_MSVC
                typedef detail::atomic_msvc_oprs<sizeof(value_type)> int_opr_t;
                typedef typename int_opr_t::opr_t opr_t;
                return int_opr_t::inc(reinterpret_cast<volatile opr_t*>(&data_), ::util::lock::memory_order_seq_cst);
#else
                return fetch_add(1) + 1;
#endif
            }
            inline value_type operator++() volatile { 
#ifdef __UTIL_LOCK_ATOMIC_INT_ATOMIC_MSVC
                typedef detail::atomic_msvc_oprs<sizeof(value_type)> int_opr_t;
                typedef typename int_opr_t::opr_t opr_t;
                return int_opr_t::inc(reinterpret_cast<volatile opr_t*>(&data_), ::util::lock::memory_order_seq_cst);
#else
                return fetch_add(1) + 1;
#endif
            }
            inline value_type operator++( int ) { 
#ifdef __UTIL_LOCK_ATOMIC_INT_ATOMIC_MSVC
                typedef detail::atomic_msvc_oprs<sizeof(value_type)> int_opr_t;
                typedef typename int_opr_t::opr_t opr_t;
                return int_opr_t::inc(reinterpret_cast<volatile opr_t*>(&data_), ::util::lock::memory_order_seq_cst) - 1;
#else
                return fetch_add(1);
#endif
            }
            inline value_type operator++( int ) volatile  { 
#ifdef __UTIL_LOCK_ATOMIC_INT_ATOMIC_MSVC
                typedef detail::atomic_msvc_oprs<sizeof(value_type)> int_opr_t;
                typedef typename int_opr_t::opr_t opr_t;
                return int_opr_t::inc(reinterpret_cast<volatile opr_t*>(&data_), ::util::lock::memory_order_seq_cst) - 1;
#else
                return fetch_add(1);
#endif
            }
            inline value_type operator--() { 
#ifdef __UTIL_LOCK_ATOMIC_INT_ATOMIC_MSVC
                typedef detail::atomic_msvc_oprs<sizeof(value_type)> int_opr_t;
                typedef typename int_opr_t::opr_t opr_t;
                return int_opr_t::dec(reinterpret_cast<volatile opr_t*>(&data_), ::util::lock::memory_order_seq_cst);
#else
                return fetch_sub(1) - 1;
#endif
            }
            inline value_type operator--() volatile { 
#ifdef __UTIL_LOCK_ATOMIC_INT_ATOMIC_MSVC
                typedef detail::atomic_msvc_oprs<sizeof(value_type)> int_opr_t;
                typedef typename int_opr_t::opr_t opr_t;
                return int_opr_t::dec(reinterpret_cast<volatile opr_t*>(&data_), ::util::lock::memory_order_seq_cst);
#else
                return fetch_sub(1) - 1;
#endif
            }
            inline value_type operator--( int ) { 
#ifdef __UTIL_LOCK_ATOMIC_INT_ATOMIC_MSVC
                typedef detail::atomic_msvc_oprs<sizeof(value_type)> int_opr_t;
                typedef typename int_opr_t::opr_t opr_t;
                return int_opr_t::dec(reinterpret_cast<volatile opr_t*>(&data_), ::util::lock::memory_order_seq_cst) + 1;
#else
                return fetch_sub(1);
#endif
            }
            inline value_type operator--( int ) volatile { 
#ifdef __UTIL_LOCK_ATOMIC_INT_ATOMIC_MSVC
                typedef detail::atomic_msvc_oprs<sizeof(value_type)> int_opr_t;
                typedef typename int_opr_t::opr_t opr_t;
                return int_opr_t::dec(static_cast<opr_t*>(&data_), ::util::lock::memory_order_seq_cst) + 1;
#else
                return fetch_sub(1);
#endif
            }

            inline value_type exchange( value_type desired, ::util::lock::memory_order order = ::util::lock::memory_order_seq_cst ) {
#ifdef __UTIL_LOCK_ATOMIC_INT_ATOMIC_MSVC
                typedef detail::atomic_msvc_oprs<sizeof(value_type)> int_opr_t;
                typedef typename int_opr_t::opr_t opr_t;
                return static_cast<value_type>(int_opr_t::exchange(reinterpret_cast<volatile opr_t*>(&data_), static_cast<opr_t>(desired), order));

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

            inline value_type exchange( value_type desired, ::util::lock::memory_order order = ::util::lock::memory_order_seq_cst ) volatile {
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
    
            inline bool compare_exchange_weak( value_type& expected, value_type desired, ::util::lock::memory_order success, ::util::lock::memory_order failure ) {
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

            inline bool compare_exchange_weak( value_type& expected, value_type desired, ::util::lock::memory_order success, ::util::lock::memory_order failure ) volatile {
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

            inline bool compare_exchange_weak( value_type& expected, value_type desired, ::util::lock::memory_order order = ::util::lock::memory_order_seq_cst ) {
#ifdef __UTIL_LOCK_ATOMIC_INT_ATOMIC_MSVC
                typedef detail::atomic_msvc_oprs<sizeof(value_type)> int_opr_t;
                typedef typename int_opr_t::opr_t opr_t;
                if(expected == static_cast<value_type>(int_opr_t::cas(reinterpret_cast<volatile opr_t*>(&data_), static_cast<opr_t>(desired), static_cast<opr_t>(expected), order))) {
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

            inline bool compare_exchange_weak( value_type& expected, value_type desired, ::util::lock::memory_order order = ::util::lock::memory_order_seq_cst ) volatile {
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

            inline bool compare_exchange_strong( value_type& expected, value_type desired, ::util::lock::memory_order success, ::util::lock::memory_order failure ) {
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

            inline bool compare_exchange_strong( value_type& expected, value_type desired, ::util::lock::memory_order success, ::util::lock::memory_order failure ) volatile {
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

            inline bool compare_exchange_strong( value_type& expected, value_type desired, ::util::lock::memory_order order = ::util::lock::memory_order_seq_cst ) {
#ifdef __UTIL_LOCK_ATOMIC_INT_ATOMIC_MSVC
                typedef detail::atomic_msvc_oprs<sizeof(value_type)> int_opr_t;
                typedef typename int_opr_t::opr_t opr_t;
                if(expected == static_cast<value_type>(int_opr_t::cas(reinterpret_cast<volatile opr_t*>(&data_), static_cast<opr_t>(desired), static_cast<opr_t>(expected), order))) {
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

            inline bool compare_exchange_strong( value_type& expected, value_type desired, ::util::lock::memory_order order = ::util::lock::memory_order_seq_cst ) volatile {
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

            inline value_type fetch_add( value_type arg, ::util::lock::memory_order order = ::util::lock::memory_order_seq_cst ) {
#ifdef __UTIL_LOCK_ATOMIC_INT_ATOMIC_MSVC
                typedef detail::atomic_msvc_oprs<sizeof(value_type)> int_opr_t;
                typedef typename int_opr_t::opr_t opr_t;
                return static_cast<value_type>(int_opr_t::add(reinterpret_cast<volatile opr_t*>(&data_), static_cast<opr_t>(arg), order)) - arg;

#elif defined(__UTIL_LOCK_ATOMIC_INT_ATOMIC_GCC_ATOMIC)
                return __atomic_fetch_add(&data_, arg, order);
#else
                return __sync_fetch_and_add(&data_, arg);
#endif
            }
            inline value_type fetch_add( value_type arg, ::util::lock::memory_order order = ::util::lock::memory_order_seq_cst ) volatile {
#ifdef __UTIL_LOCK_ATOMIC_INT_ATOMIC_MSVC
                typedef detail::atomic_msvc_oprs<sizeof(value_type)> int_opr_t;
                typedef typename int_opr_t::opr_t opr_t;
                return static_cast<value_type>(int_opr_t::add(static_cast<opr_t*>(&data_), static_cast<opr_t>(arg), order)) - arg;

#elif defined(__UTIL_LOCK_ATOMIC_INT_ATOMIC_GCC_ATOMIC)
                return __atomic_fetch_add(&data_, arg, order);
#else
                return __sync_fetch_and_add(&data_, arg);
#endif
            }

            inline value_type fetch_sub( value_type arg, ::util::lock::memory_order order = ::util::lock::memory_order_seq_cst ) {
#ifdef __UTIL_LOCK_ATOMIC_INT_ATOMIC_MSVC
                typedef detail::atomic_msvc_oprs<sizeof(value_type)> int_opr_t;
                typedef typename int_opr_t::opr_t opr_t;
                return static_cast<value_type>(int_opr_t::sub(reinterpret_cast<volatile opr_t*>(&data_), static_cast<opr_t>(arg), order)) + arg;

#elif defined(__UTIL_LOCK_ATOMIC_INT_ATOMIC_GCC_ATOMIC)
                return __atomic_fetch_sub(&data_, arg, order);
#else
                return __sync_fetch_and_sub(&data_, arg);
#endif
            }
            inline value_type fetch_sub( value_type arg, ::util::lock::memory_order order = ::util::lock::memory_order_seq_cst ) volatile {
#ifdef __UTIL_LOCK_ATOMIC_INT_ATOMIC_MSVC
                typedef detail::atomic_msvc_oprs<sizeof(value_type)> int_opr_t;
                typedef typename int_opr_t::opr_t opr_t;
                return static_cast<value_type>(int_opr_t::sub(static_cast<opr_t*>(&data_), static_cast<opr_t>(arg), order)) + arg;

#elif defined(__UTIL_LOCK_ATOMIC_INT_ATOMIC_GCC_ATOMIC)
                return __atomic_fetch_sub(&data_, arg, order);
#else
                return __sync_fetch_and_sub(&data_, arg);
#endif
            }

            inline value_type fetch_and( value_type arg, ::util::lock::memory_order order = ::util::lock::memory_order_seq_cst ) {
#ifdef __UTIL_LOCK_ATOMIC_INT_ATOMIC_MSVC
                typedef detail::atomic_msvc_oprs<sizeof(value_type)> int_opr_t;
                typedef typename int_opr_t::opr_t opr_t;
                return static_cast<value_type>(int_opr_t::and(reinterpret_cast<volatile opr_t*>(&data_), static_cast<opr_t>(arg), order));

#elif defined(__UTIL_LOCK_ATOMIC_INT_ATOMIC_GCC_ATOMIC)
                return __atomic_fetch_and(&data_, arg, order);
#else
                return __sync_fetch_and_and(&data_, arg);
#endif
            }
            inline value_type fetch_and( value_type arg, ::util::lock::memory_order order = ::util::lock::memory_order_seq_cst ) volatile {
#ifdef __UTIL_LOCK_ATOMIC_INT_ATOMIC_MSVC
                typedef detail::atomic_msvc_oprs<sizeof(value_type)> int_opr_t;
                typedef typename int_opr_t::opr_t opr_t;
                return static_cast<value_type>(int_opr_t::and(static_cast<opr_t*>(&data_), static_cast<opr_t>(arg), order));

#elif defined(__UTIL_LOCK_ATOMIC_INT_ATOMIC_GCC_ATOMIC)
                return __atomic_fetch_and(&data_, arg, order);
#else
                return __sync_fetch_and_and(&data_, arg);
#endif
            }

            inline value_type fetch_or( value_type arg, ::util::lock::memory_order order = ::util::lock::memory_order_seq_cst ) {
#ifdef __UTIL_LOCK_ATOMIC_INT_ATOMIC_MSVC
                typedef detail::atomic_msvc_oprs<sizeof(value_type)> int_opr_t;
                typedef typename int_opr_t::opr_t opr_t;
                return static_cast<value_type>(int_opr_t::or(reinterpret_cast<volatile opr_t*>(&data_), static_cast<opr_t>(arg), order));

#elif defined(__UTIL_LOCK_ATOMIC_INT_ATOMIC_GCC_ATOMIC)
                return __atomic_fetch_or(&data_, arg, order);
#else
                return __sync_fetch_and_or(&data_, arg);
#endif
            }
            inline value_type fetch_or( value_type arg, ::util::lock::memory_order order = ::util::lock::memory_order_seq_cst ) volatile {
#ifdef __UTIL_LOCK_ATOMIC_INT_ATOMIC_MSVC
                typedef detail::atomic_msvc_oprs<sizeof(value_type)> int_opr_t;
                typedef typename int_opr_t::opr_t opr_t;
                return static_cast<value_type>(int_opr_t::or(static_cast<opr_t*>(&data_), static_cast<opr_t>(arg), order));

#elif defined(__UTIL_LOCK_ATOMIC_INT_ATOMIC_GCC_ATOMIC)
                return __atomic_fetch_or(&data_, arg, order);
#else
                return __sync_fetch_and_or(&data_, arg);
#endif
            }

            inline value_type fetch_xor( value_type arg, ::util::lock::memory_order order = ::util::lock::memory_order_seq_cst ) {
#ifdef __UTIL_LOCK_ATOMIC_INT_ATOMIC_MSVC
                typedef detail::atomic_msvc_oprs<sizeof(value_type)> int_opr_t;
                typedef typename int_opr_t::opr_t opr_t;
                return static_cast<value_type>(int_opr_t::xor(reinterpret_cast<volatile opr_t*>(&data_), static_cast<opr_t>(arg), order));

#elif defined(__UTIL_LOCK_ATOMIC_INT_ATOMIC_GCC_ATOMIC)
                return __atomic_fetch_xor(&data_, arg, order);
#else
                return __sync_fetch_and_xor(&data_, arg);
#endif
            }
            inline value_type fetch_xor( value_type arg, ::util::lock::memory_order order = ::util::lock::memory_order_seq_cst ) volatile {
#ifdef __UTIL_LOCK_ATOMIC_INT_ATOMIC_MSVC
                typedef detail::atomic_msvc_oprs<sizeof(value_type)> int_opr_t;
                typedef typename int_opr_t::opr_t opr_t;
                return static_cast<value_type>(int_opr_t::xor(static_cast<opr_t*>(&data_), static_cast<opr_t>(arg), order));

#elif defined(__UTIL_LOCK_ATOMIC_INT_ATOMIC_GCC_ATOMIC)
                return __atomic_fetch_xor(&data_, arg, order);
#else
                return __sync_fetch_and_xor(&data_, arg);
#endif
            }
        };

#endif
    }
}

#endif /* _UTIL_LOCK_ATOMIC_INT_TYPE_H_ */

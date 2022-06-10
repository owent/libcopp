/**
 * @file atomic_int_type.h
 * @brief atomic wrapper fo integers
 * Licensed under the MIT licenses.
 *
 * @version 1.0
 * @author OWenT
 * @date 2016-06-14
 *
 *
 * @history
 *     2016-06-14 Created
 *     2021-07-19 Remove supporting of old compilers.
 */

#pragma once

#include <libcopp/utils/config/libcopp_build_features.h>
#include <libcopp/utils/features.h>
#include <libcopp/utils/std/explicit_declare.h>

// clang-format off
#include <libcopp/utils/config/stl_include_prefix.h>  // NOLINT(build/include_order)
// clang-format on
// patch for old gcc
#ifndef __STDC_LIMIT_MACROS
#  define _UNDEF__STDC_LIMIT_MACROS
#  define __STDC_LIMIT_MACROS
#endif
#ifndef __STDC_CONSTANT_MACROS
#  define _UNDEF__STDC_CONSTANT_MACROS
#  define __STDC_CONSTANT_MACROS
#endif
#include <limits.h>
#include <stdint.h>
#ifdef _UNDEF__STDC_LIMIT_MACROS
#  undef __STDC_LIMIT_MACROS
#  undef _UNDEF__STDC_LIMIT_MACROS
#endif
#ifdef _UNDEF__STDC_CONSTANT_MACROS
#  undef __STDC_CONSTANT_MACROS
#  undef _UNDEF__STDC_CONSTANT_MACROS
#endif

#if (defined(__cplusplus) && __cplusplus >= 201103L)
#  include <cstdint>
#elif defined(_MSVC_LANG) && _MSVC_LANG >= 201402L
#  include <cstdint>
#endif

#if (defined(__cplusplus) && __cplusplus >= 201103L) || (defined(_MSVC_LANG) && _MSVC_LANG >= 201103L)

#  include <atomic>
#  define __LIBCOPP_UTIL_LOCK_ATOMIC_INT_TYPE_ATOMIC_STD

#elif defined(__clang__) && (__clang_major__ > 3 || (__clang_major__ == 3 && __clang_minor__ >= 1)) && \
    __cplusplus >= 201103L

#  include <atomic>
#  define __LIBCOPP_UTIL_LOCK_ATOMIC_INT_TYPE_ATOMIC_STD

#elif defined(_MSC_VER) && \
    (_MSC_VER >= 1900)  // 1900 means VC 14.0,2015, there some problem with std::atomic implement in old MSVC

#  include <atomic>
#  define __LIBCOPP_UTIL_LOCK_ATOMIC_INT_TYPE_ATOMIC_STD

// There is a BUG in gcc 4.6, which will cause 'undefined reference to `std::atomic_thread_fence(std::memory_order)'
// In gcc 4.7 and upper, we can use -std=c++11 or upper
// https://gcc.gnu.org/bugzilla/show_bug.cgi?id=51038
// #elif defined(__GNUC__) && ((__GNUC__ == 4 && __GNUC_MINOR__ >= 5) || __GNUC__ > 4) &&
// defined(__GXX_EXPERIMENTAL_CXX0X__)
//
// #include <atomic>
// #define __LIBCOPP_UTIL_LOCK_ATOMIC_INT_TYPE_ATOMIC_STD

#endif

#include <cstddef>

// clang-format off
#include <libcopp/utils/config/stl_include_suffix.h>  // NOLINT(build/include_order)
// clang-format on

LIBCOPP_COPP_NAMESPACE_BEGIN
namespace util {
namespace lock {
#ifdef __LIBCOPP_UTIL_LOCK_ATOMIC_INT_TYPE_ATOMIC_STD
using ::std::memory_order;
using ::std::memory_order_acq_rel;
using ::std::memory_order_acquire;
using ::std::memory_order_consume;
using ::std::memory_order_relaxed;
using ::std::memory_order_release;
using ::std::memory_order_seq_cst;

#  define LIBCOPP_UTIL_LOCK_ATOMIC_THREAD_FENCE(order) ::std::atomic_thread_fence(order)
#  define LIBCOPP_UTIL_LOCK_ATOMIC_SIGNAL_FENCE(order) ::std::atomic_signal_fence(order)

/**
 * @brief atomic - C++ 0x/11版实现
 * @see http://en.cppreference.com/w/cpp/atomic/atomic
 * @exception noexcept
 * @note Ty can only be a integer or enum, can not be bool or raw pointer
 */
template <typename Ty = int>
class LIBCOPP_COPP_API_HEAD_ONLY atomic_int_type {
 public:
  using value_type = Ty;

 private:
  ::std::atomic<value_type> data_;
  atomic_int_type(const atomic_int_type &) = delete;
#  ifndef _MSC_VER
  atomic_int_type &operator=(const atomic_int_type &) = delete;
  atomic_int_type &operator=(const atomic_int_type &) volatile = delete;
#  endif

 public:
  atomic_int_type() LIBCOPP_MACRO_NOEXCEPT : data_() {}
  atomic_int_type(value_type desired) LIBCOPP_MACRO_NOEXCEPT : data_(desired) {}

  inline void store(value_type desired,
                    LIBCOPP_COPP_NAMESPACE_ID::util::lock::memory_order order =
                        LIBCOPP_COPP_NAMESPACE_ID::util::lock::memory_order_seq_cst) LIBCOPP_MACRO_NOEXCEPT {
    data_.store(desired, order);
  }
  inline void store(value_type desired,
                    LIBCOPP_COPP_NAMESPACE_ID::util::lock::memory_order order =
                        LIBCOPP_COPP_NAMESPACE_ID::util::lock::memory_order_seq_cst) volatile LIBCOPP_MACRO_NOEXCEPT {
    data_.store(desired, order);
  }

  inline value_type load(LIBCOPP_COPP_NAMESPACE_ID::util::lock::memory_order order =
                             LIBCOPP_COPP_NAMESPACE_ID::util::lock::memory_order_seq_cst) const LIBCOPP_MACRO_NOEXCEPT {
    return data_.load(order);
  }
  inline value_type load(LIBCOPP_COPP_NAMESPACE_ID::util::lock::memory_order order =
                             LIBCOPP_COPP_NAMESPACE_ID::util::lock::memory_order_seq_cst) const
      volatile LIBCOPP_MACRO_NOEXCEPT {
    return data_.load(order);
  }

  inline operator value_type() const LIBCOPP_MACRO_NOEXCEPT { return load(); }
  inline operator value_type() const volatile LIBCOPP_MACRO_NOEXCEPT { return load(); }

  inline value_type operator=(value_type desired) LIBCOPP_MACRO_NOEXCEPT {
    store(desired);
    return desired;
  }
  inline value_type operator=(value_type desired) volatile LIBCOPP_MACRO_NOEXCEPT {
    store(desired);
    return desired;
  }

  inline value_type operator++() LIBCOPP_MACRO_NOEXCEPT { return ++data_; }
  inline value_type operator++() volatile LIBCOPP_MACRO_NOEXCEPT { return ++data_; }
  inline value_type operator++(int) LIBCOPP_MACRO_NOEXCEPT { return data_++; }
  inline value_type operator++(int) volatile LIBCOPP_MACRO_NOEXCEPT { return data_++; }
  inline value_type operator--() LIBCOPP_MACRO_NOEXCEPT { return --data_; }
  inline value_type operator--() volatile LIBCOPP_MACRO_NOEXCEPT { return --data_; }
  inline value_type operator--(int) LIBCOPP_MACRO_NOEXCEPT { return data_--; }
  inline value_type operator--(int) volatile LIBCOPP_MACRO_NOEXCEPT { return data_--; }

  inline value_type exchange(value_type desired,
                             LIBCOPP_COPP_NAMESPACE_ID::util::lock::memory_order order =
                                 LIBCOPP_COPP_NAMESPACE_ID::util::lock::memory_order_seq_cst) LIBCOPP_MACRO_NOEXCEPT {
    return data_.exchange(desired, order);
  }
  inline value_type exchange(
      value_type desired,
      LIBCOPP_COPP_NAMESPACE_ID::util::lock::memory_order order =
          LIBCOPP_COPP_NAMESPACE_ID::util::lock::memory_order_seq_cst) volatile LIBCOPP_MACRO_NOEXCEPT {
    return data_.exchange(desired, order);
  }

  inline bool compare_exchange_weak(
      value_type &expected, value_type desired, LIBCOPP_COPP_NAMESPACE_ID::util::lock::memory_order success,
      LIBCOPP_COPP_NAMESPACE_ID::util::lock::memory_order failure) LIBCOPP_MACRO_NOEXCEPT {
    return data_.compare_exchange_weak(expected, desired, success, failure);
  }
  inline bool compare_exchange_weak(
      value_type &expected, value_type desired, LIBCOPP_COPP_NAMESPACE_ID::util::lock::memory_order success,
      LIBCOPP_COPP_NAMESPACE_ID::util::lock::memory_order failure) volatile LIBCOPP_MACRO_NOEXCEPT {
    return data_.compare_exchange_weak(expected, desired, success, failure);
  }

  inline bool compare_exchange_weak(value_type &expected, value_type desired,
                                    LIBCOPP_COPP_NAMESPACE_ID::util::lock::memory_order order =
                                        LIBCOPP_COPP_NAMESPACE_ID::util::lock::memory_order_seq_cst)
      LIBCOPP_MACRO_NOEXCEPT {
    return data_.compare_exchange_weak(expected, desired, order);
  }
  inline bool compare_exchange_weak(
      value_type &expected, value_type desired,
      LIBCOPP_COPP_NAMESPACE_ID::util::lock::memory_order order =
          LIBCOPP_COPP_NAMESPACE_ID::util::lock::memory_order_seq_cst) volatile LIBCOPP_MACRO_NOEXCEPT {
    return data_.compare_exchange_weak(expected, desired, order);
  }

  inline bool compare_exchange_strong(
      value_type &expected, value_type desired, LIBCOPP_COPP_NAMESPACE_ID::util::lock::memory_order success,
      LIBCOPP_COPP_NAMESPACE_ID::util::lock::memory_order failure) LIBCOPP_MACRO_NOEXCEPT {
    return data_.compare_exchange_strong(expected, desired, success, failure);
  }
  inline bool compare_exchange_strong(
      value_type &expected, value_type desired, LIBCOPP_COPP_NAMESPACE_ID::util::lock::memory_order success,
      LIBCOPP_COPP_NAMESPACE_ID::util::lock::memory_order failure) volatile LIBCOPP_MACRO_NOEXCEPT {
    return data_.compare_exchange_strong(expected, desired, success, failure);
  }

  inline bool compare_exchange_strong(value_type &expected, value_type desired,
                                      LIBCOPP_COPP_NAMESPACE_ID::util::lock::memory_order order =
                                          LIBCOPP_COPP_NAMESPACE_ID::util::lock::memory_order_seq_cst)
      LIBCOPP_MACRO_NOEXCEPT {
    return data_.compare_exchange_strong(expected, desired, order);
  }
  inline bool compare_exchange_strong(
      value_type &expected, value_type desired,
      LIBCOPP_COPP_NAMESPACE_ID::util::lock::memory_order order =
          LIBCOPP_COPP_NAMESPACE_ID::util::lock::memory_order_seq_cst) volatile LIBCOPP_MACRO_NOEXCEPT {
    return data_.compare_exchange_strong(expected, desired, order);
  }

  inline value_type fetch_add(value_type arg,
                              LIBCOPP_COPP_NAMESPACE_ID::util::lock::memory_order order =
                                  LIBCOPP_COPP_NAMESPACE_ID::util::lock::memory_order_seq_cst) LIBCOPP_MACRO_NOEXCEPT {
    return data_.fetch_add(arg, order);
  }
  inline value_type fetch_add(
      value_type arg, LIBCOPP_COPP_NAMESPACE_ID::util::lock::memory_order order =
                          LIBCOPP_COPP_NAMESPACE_ID::util::lock::memory_order_seq_cst) volatile LIBCOPP_MACRO_NOEXCEPT {
    return data_.fetch_add(arg, order);
  }

  inline value_type fetch_sub(value_type arg,
                              LIBCOPP_COPP_NAMESPACE_ID::util::lock::memory_order order =
                                  LIBCOPP_COPP_NAMESPACE_ID::util::lock::memory_order_seq_cst) LIBCOPP_MACRO_NOEXCEPT {
    return data_.fetch_sub(arg, order);
  }
  inline value_type fetch_sub(
      value_type arg, LIBCOPP_COPP_NAMESPACE_ID::util::lock::memory_order order =
                          LIBCOPP_COPP_NAMESPACE_ID::util::lock::memory_order_seq_cst) volatile LIBCOPP_MACRO_NOEXCEPT {
    return data_.fetch_sub(arg, order);
  }

  inline value_type fetch_and(value_type arg,
                              LIBCOPP_COPP_NAMESPACE_ID::util::lock::memory_order order =
                                  LIBCOPP_COPP_NAMESPACE_ID::util::lock::memory_order_seq_cst) LIBCOPP_MACRO_NOEXCEPT {
    return data_.fetch_and(arg, order);
  }
  inline value_type fetch_and(
      value_type arg, LIBCOPP_COPP_NAMESPACE_ID::util::lock::memory_order order =
                          LIBCOPP_COPP_NAMESPACE_ID::util::lock::memory_order_seq_cst) volatile LIBCOPP_MACRO_NOEXCEPT {
    return data_.fetch_and(arg, order);
  }

  inline value_type fetch_or(value_type arg,
                             LIBCOPP_COPP_NAMESPACE_ID::util::lock::memory_order order =
                                 LIBCOPP_COPP_NAMESPACE_ID::util::lock::memory_order_seq_cst) LIBCOPP_MACRO_NOEXCEPT {
    return data_.fetch_or(arg, order);
  }
  inline value_type fetch_or(
      value_type arg, LIBCOPP_COPP_NAMESPACE_ID::util::lock::memory_order order =
                          LIBCOPP_COPP_NAMESPACE_ID::util::lock::memory_order_seq_cst) volatile LIBCOPP_MACRO_NOEXCEPT {
    return data_.fetch_or(arg, order);
  }

  inline value_type fetch_xor(value_type arg,
                              LIBCOPP_COPP_NAMESPACE_ID::util::lock::memory_order order =
                                  LIBCOPP_COPP_NAMESPACE_ID::util::lock::memory_order_seq_cst) LIBCOPP_MACRO_NOEXCEPT {
    return data_.fetch_xor(arg, order);
  }
  inline value_type fetch_xor(
      value_type arg, LIBCOPP_COPP_NAMESPACE_ID::util::lock::memory_order order =
                          LIBCOPP_COPP_NAMESPACE_ID::util::lock::memory_order_seq_cst) volatile LIBCOPP_MACRO_NOEXCEPT {
    return data_.fetch_xor(arg, order);
  }
};
#else

#  if defined(__clang__)
#    if !defined(__GCC_ATOMIC_INT_LOCK_FREE) && \
        (!defined(__GNUC__) || __GNUC__ < 4 || (__GNUC__ == 4 && __GNUC_MINOR__ < 1))
#      error clang version is too old
#    endif
#    if defined(__GCC_ATOMIC_INT_LOCK_FREE)
// @see https://gcc.gnu.org/onlinedocs/gcc/_005f_005fatomic-Builtins.html
#      define __LIBCOPP_UTIL_LOCK_ATOMIC_INT_ATOMIC_GCC_ATOMIC 1
#    else
// @see https://gcc.gnu.org/onlinedocs/gcc-4.1.2/gcc/Atomic-Builtins.html
#      define __LIBCOPP_UTIL_LOCK_ATOMIC_INT_ATOMIC_GCC 1
#    endif
#  elif defined(__GNUC__) || defined(__INTEL_COMPILER)
#    if defined(__GNUC__) && (__GNUC__ < 4 || (__GNUC__ == 4 && __GNUC_MINOR__ < 1))
#      error gcc version must be greater or equal than 4.1
#    endif
#    if defined(__INTEL_COMPILER) && __INTEL_COMPILER < 1100
#      error intel compiler version must be greater or equal than 11.0
#    endif
#    if defined(__GCC_ATOMIC_INT_LOCK_FREE)
// @see https://gcc.gnu.org/onlinedocs/gcc/_005f_005fatomic-Builtins.html
#      define __LIBCOPP_UTIL_LOCK_ATOMIC_INT_ATOMIC_GCC_ATOMIC 1
#    else
// @see https://gcc.gnu.org/onlinedocs/gcc-4.1.2/gcc/Atomic-Builtins.html
#      define __LIBCOPP_UTIL_LOCK_ATOMIC_INT_ATOMIC_GCC 1
#    endif
#  else
#    error currently only gcc, msvc, intel compiler & llvm-clang are supported
#  endif

#  if defined(__LIBCOPP_UTIL_LOCK_ATOMIC_INT_ATOMIC_GCC_ATOMIC)
enum memory_order {
  memory_order_relaxed = __ATOMIC_RELAXED,
  memory_order_consume = __ATOMIC_CONSUME,
  memory_order_acquire = __ATOMIC_ACQUIRE,
  memory_order_release = __ATOMIC_RELEASE,
  memory_order_acq_rel = __ATOMIC_ACQ_REL,
  memory_order_seq_cst = __ATOMIC_SEQ_CST
};

#    define LIBCOPP_UTIL_LOCK_ATOMIC_THREAD_FENCE(order) __atomic_thread_fence(order)
#    define LIBCOPP_UTIL_LOCK_ATOMIC_SIGNAL_FENCE(order) __atomic_signal_fence(order)

#  else  // old gcc and old msvc use this
enum memory_order {
  memory_order_relaxed = 0,
  memory_order_consume,
  memory_order_acquire,
  memory_order_release,
  memory_order_acq_rel,
  memory_order_seq_cst
};
#  endif

#  ifndef LIBCOPP_UTIL_LOCK_ATOMIC_THREAD_FENCE
#    define LIBCOPP_UTIL_LOCK_ATOMIC_THREAD_FENCE(x)
#  endif

#  ifndef LIBCOPP_UTIL_LOCK_ATOMIC_SIGNAL_FENCE
#    define LIBCOPP_UTIL_LOCK_ATOMIC_SIGNAL_FENCE(x)
#  endif

template <typename Ty = int>
class LIBCOPP_COPP_API_HEAD_ONLY atomic_int_type {
 public:
  using value_type = Ty;

 private:
  volatile value_type data_;
  atomic_int_type(const atomic_int_type &) = delete;
#  ifndef _MSC_VER
  atomic_int_type &operator=(const atomic_int_type &) = delete;
  atomic_int_type &operator=(const atomic_int_type &) volatile = delete;
#  endif

 public:
  atomic_int_type() LIBCOPP_MACRO_NOEXCEPT : data_() {}

  atomic_int_type(value_type desired) LIBCOPP_MACRO_NOEXCEPT : data_(desired) {}

  inline void store(value_type desired,
                    EXPLICIT_UNUSED_ATTR LIBCOPP_COPP_NAMESPACE_ID::util::lock::memory_order order =
                        LIBCOPP_COPP_NAMESPACE_ID::util::lock::memory_order_seq_cst) LIBCOPP_MACRO_NOEXCEPT {
#  if defined(__LIBCOPP_UTIL_LOCK_ATOMIC_INT_ATOMIC_GCC_ATOMIC)
    __atomic_store_n(&data_, desired, order);
#  else
    __sync_lock_test_and_set(&data_, desired);
#  endif
  }

  inline void store(value_type desired,
                    EXPLICIT_UNUSED_ATTR LIBCOPP_COPP_NAMESPACE_ID::util::lock::memory_order order =
                        LIBCOPP_COPP_NAMESPACE_ID::util::lock::memory_order_seq_cst) volatile LIBCOPP_MACRO_NOEXCEPT {
#  if defined(__LIBCOPP_UTIL_LOCK_ATOMIC_INT_ATOMIC_GCC_ATOMIC)
    __atomic_store_n(&data_, desired, order);
#  else
    __sync_lock_test_and_set(&data_, desired);
#  endif
  }

  inline value_type load(EXPLICIT_UNUSED_ATTR LIBCOPP_COPP_NAMESPACE_ID::util::lock::memory_order order =
                             LIBCOPP_COPP_NAMESPACE_ID::util::lock::memory_order_seq_cst) const LIBCOPP_MACRO_NOEXCEPT {
#  if defined(__LIBCOPP_UTIL_LOCK_ATOMIC_INT_ATOMIC_GCC_ATOMIC)
    return __atomic_load_n(&data_, order);
#  else
    __sync_synchronize();
    return data_;
#  endif
  }

  inline value_type load(EXPLICIT_UNUSED_ATTR LIBCOPP_COPP_NAMESPACE_ID::util::lock::memory_order order =
                             LIBCOPP_COPP_NAMESPACE_ID::util::lock::memory_order_seq_cst) const
      volatile LIBCOPP_MACRO_NOEXCEPT {
#  if defined(__LIBCOPP_UTIL_LOCK_ATOMIC_INT_ATOMIC_GCC_ATOMIC)
    return __atomic_load_n(&data_, order);
#  else
    __sync_synchronize();
    return data_;
#  endif
  }

  inline operator value_type() const LIBCOPP_MACRO_NOEXCEPT { return load(); }
  inline operator value_type() const volatile LIBCOPP_MACRO_NOEXCEPT { return load(); }

  inline value_type operator=(value_type desired) LIBCOPP_MACRO_NOEXCEPT {
    store(desired);
    return desired;
  }
  inline value_type operator=(value_type desired) volatile LIBCOPP_MACRO_NOEXCEPT {
    store(desired);
    return desired;
  }

  inline value_type operator++() LIBCOPP_MACRO_NOEXCEPT { return fetch_add(1) + 1; }
  inline value_type operator++() volatile LIBCOPP_MACRO_NOEXCEPT { return fetch_add(1) + 1; }
  inline value_type operator++(int) LIBCOPP_MACRO_NOEXCEPT { return fetch_add(1); }
  inline value_type operator++(int) volatile LIBCOPP_MACRO_NOEXCEPT { return fetch_add(1); }
  inline value_type operator--() LIBCOPP_MACRO_NOEXCEPT { return fetch_sub(1) - 1; }
  inline value_type operator--() volatile LIBCOPP_MACRO_NOEXCEPT { return fetch_sub(1) - 1; }
  inline value_type operator--(int) LIBCOPP_MACRO_NOEXCEPT { return fetch_sub(1); }
  inline value_type operator--(int) volatile LIBCOPP_MACRO_NOEXCEPT { return fetch_sub(1); }

  inline value_type exchange(value_type desired,
                             EXPLICIT_UNUSED_ATTR LIBCOPP_COPP_NAMESPACE_ID::util::lock::memory_order order =
                                 LIBCOPP_COPP_NAMESPACE_ID::util::lock::memory_order_seq_cst) LIBCOPP_MACRO_NOEXCEPT {
#  if defined(__LIBCOPP_UTIL_LOCK_ATOMIC_INT_ATOMIC_GCC_ATOMIC)
    return __atomic_exchange_n(&data_, desired, order);
#  else
    value_type old_value = data_;
    while (!__sync_bool_compare_and_swap(&data_, old_value, desired)) {
      old_value = data_;
    }
    return old_value;
#  endif
  }

  inline value_type exchange(
      value_type desired,
      EXPLICIT_UNUSED_ATTR LIBCOPP_COPP_NAMESPACE_ID::util::lock::memory_order order =
          LIBCOPP_COPP_NAMESPACE_ID::util::lock::memory_order_seq_cst) volatile LIBCOPP_MACRO_NOEXCEPT {
#  if defined(__LIBCOPP_UTIL_LOCK_ATOMIC_INT_ATOMIC_GCC_ATOMIC)
    return __atomic_exchange_n(&data_, desired, order);
#  else
    value_type old_value = data_;
    while (!__sync_bool_compare_and_swap(&data_, old_value, desired)) {
      old_value = data_;
    }
    return old_value;
#  endif
  }

  inline bool compare_exchange_weak(value_type &expected, value_type desired,
                                    EXPLICIT_UNUSED_ATTR LIBCOPP_COPP_NAMESPACE_ID::util::lock::memory_order success,
                                    EXPLICIT_UNUSED_ATTR LIBCOPP_COPP_NAMESPACE_ID::util::lock::memory_order failure)
      LIBCOPP_MACRO_NOEXCEPT {
#  if defined(__LIBCOPP_UTIL_LOCK_ATOMIC_INT_ATOMIC_GCC_ATOMIC)
    return __atomic_compare_exchange_n(&data_, &expected, desired, true, success, failure);
#  else
    if (__sync_bool_compare_and_swap(&data_, expected, desired)) {
      return true;
    } else {
      expected = data_;
      return false;
    }
#  endif
  }

  inline bool compare_exchange_weak(value_type &expected, value_type desired,
                                    EXPLICIT_UNUSED_ATTR LIBCOPP_COPP_NAMESPACE_ID::util::lock::memory_order success,
                                    EXPLICIT_UNUSED_ATTR LIBCOPP_COPP_NAMESPACE_ID::util::lock::memory_order
                                        failure) volatile LIBCOPP_MACRO_NOEXCEPT {
#  if defined(__LIBCOPP_UTIL_LOCK_ATOMIC_INT_ATOMIC_GCC_ATOMIC)
    return __atomic_compare_exchange_n(&data_, &expected, desired, true, success, failure);
#  else
    if (__sync_bool_compare_and_swap(&data_, expected, desired)) {
      return true;
    } else {
      expected = data_;
      return false;
    }
#  endif
  }

  inline bool compare_exchange_weak(value_type &expected, value_type desired,
                                    EXPLICIT_UNUSED_ATTR LIBCOPP_COPP_NAMESPACE_ID::util::lock::memory_order order =
                                        LIBCOPP_COPP_NAMESPACE_ID::util::lock::memory_order_seq_cst)
      LIBCOPP_MACRO_NOEXCEPT {
#  if defined(__LIBCOPP_UTIL_LOCK_ATOMIC_INT_ATOMIC_GCC_ATOMIC)
    return __atomic_compare_exchange_n(&data_, &expected, desired, true, order, order);
#  else
    if (__sync_bool_compare_and_swap(&data_, expected, desired)) {
      return true;
    } else {
      expected = data_;
      return false;
    }
#  endif
  }

  inline bool compare_exchange_weak(
      value_type &expected, value_type desired,
      EXPLICIT_UNUSED_ATTR LIBCOPP_COPP_NAMESPACE_ID::util::lock::memory_order order =
          LIBCOPP_COPP_NAMESPACE_ID::util::lock::memory_order_seq_cst) volatile LIBCOPP_MACRO_NOEXCEPT {
#  if defined(__LIBCOPP_UTIL_LOCK_ATOMIC_INT_ATOMIC_GCC_ATOMIC)
    return __atomic_compare_exchange_n(&data_, &expected, desired, true, order, order);
#  else
    if (__sync_bool_compare_and_swap(&data_, expected, desired)) {
      return true;
    } else {
      expected = data_;
      return false;
    }
#  endif
  }

  inline bool compare_exchange_strong(value_type &expected, value_type desired,
                                      EXPLICIT_UNUSED_ATTR LIBCOPP_COPP_NAMESPACE_ID::util::lock::memory_order success,
                                      EXPLICIT_UNUSED_ATTR LIBCOPP_COPP_NAMESPACE_ID::util::lock::memory_order failure)
      LIBCOPP_MACRO_NOEXCEPT {
#  if defined(__LIBCOPP_UTIL_LOCK_ATOMIC_INT_ATOMIC_GCC_ATOMIC)
    return __atomic_compare_exchange_n(&data_, &expected, desired, false, success, failure);
#  else
    if (__sync_bool_compare_and_swap(&data_, expected, desired)) {
      return true;
    } else {
      expected = data_;
      return false;
    }
#  endif
  }

  inline bool compare_exchange_strong(value_type &expected, value_type desired,
                                      EXPLICIT_UNUSED_ATTR LIBCOPP_COPP_NAMESPACE_ID::util::lock::memory_order success,
                                      EXPLICIT_UNUSED_ATTR LIBCOPP_COPP_NAMESPACE_ID::util::lock::memory_order
                                          failure) volatile LIBCOPP_MACRO_NOEXCEPT {
#  if defined(__LIBCOPP_UTIL_LOCK_ATOMIC_INT_ATOMIC_GCC_ATOMIC)
    return __atomic_compare_exchange_n(&data_, &expected, desired, false, success, failure);
#  else
    if (__sync_bool_compare_and_swap(&data_, expected, desired)) {
      return true;
    } else {
      expected = data_;
      return false;
    }
#  endif
  }

  inline bool compare_exchange_strong(value_type &expected, value_type desired,
                                      EXPLICIT_UNUSED_ATTR LIBCOPP_COPP_NAMESPACE_ID::util::lock::memory_order order =
                                          LIBCOPP_COPP_NAMESPACE_ID::util::lock::memory_order_seq_cst)
      LIBCOPP_MACRO_NOEXCEPT {
#  if defined(__LIBCOPP_UTIL_LOCK_ATOMIC_INT_ATOMIC_GCC_ATOMIC)
    return __atomic_compare_exchange_n(&data_, &expected, desired, false, order, order);
#  else
    if (__sync_bool_compare_and_swap(&data_, expected, desired)) {
      return true;
    } else {
      expected = data_;
      return false;
    }
#  endif
  }

  inline bool compare_exchange_strong(
      value_type &expected, value_type desired,
      EXPLICIT_UNUSED_ATTR LIBCOPP_COPP_NAMESPACE_ID::util::lock::memory_order order =
          LIBCOPP_COPP_NAMESPACE_ID::util::lock::memory_order_seq_cst) volatile LIBCOPP_MACRO_NOEXCEPT {
#  if defined(__LIBCOPP_UTIL_LOCK_ATOMIC_INT_ATOMIC_GCC_ATOMIC)
    return __atomic_compare_exchange_n(&data_, &expected, desired, false, order, order);
#  else
    if (__sync_bool_compare_and_swap(&data_, expected, desired)) {
      return true;
    } else {
      expected = data_;
      return false;
    }
#  endif
  }

  inline value_type fetch_add(value_type arg,
                              EXPLICIT_UNUSED_ATTR LIBCOPP_COPP_NAMESPACE_ID::util::lock::memory_order order =
                                  LIBCOPP_COPP_NAMESPACE_ID::util::lock::memory_order_seq_cst) LIBCOPP_MACRO_NOEXCEPT {
#  if defined(__LIBCOPP_UTIL_LOCK_ATOMIC_INT_ATOMIC_GCC_ATOMIC)
    return __atomic_fetch_add(&data_, arg, order);
#  else
    return __sync_fetch_and_add(&data_, arg);
#  endif
  }
  inline value_type fetch_add(
      value_type arg, EXPLICIT_UNUSED_ATTR LIBCOPP_COPP_NAMESPACE_ID::util::lock::memory_order order =
                          LIBCOPP_COPP_NAMESPACE_ID::util::lock::memory_order_seq_cst) volatile LIBCOPP_MACRO_NOEXCEPT {
#  if defined(__LIBCOPP_UTIL_LOCK_ATOMIC_INT_ATOMIC_GCC_ATOMIC)
    return __atomic_fetch_add(&data_, arg, order);
#  else
    return __sync_fetch_and_add(&data_, arg);
#  endif
  }

  inline value_type fetch_sub(value_type arg,
                              EXPLICIT_UNUSED_ATTR LIBCOPP_COPP_NAMESPACE_ID::util::lock::memory_order order =
                                  LIBCOPP_COPP_NAMESPACE_ID::util::lock::memory_order_seq_cst) LIBCOPP_MACRO_NOEXCEPT {
#  if defined(__LIBCOPP_UTIL_LOCK_ATOMIC_INT_ATOMIC_GCC_ATOMIC)
    return __atomic_fetch_sub(&data_, arg, order);
#  else
    return __sync_fetch_and_sub(&data_, arg);
#  endif
  }
  inline value_type fetch_sub(
      value_type arg, EXPLICIT_UNUSED_ATTR LIBCOPP_COPP_NAMESPACE_ID::util::lock::memory_order order =
                          LIBCOPP_COPP_NAMESPACE_ID::util::lock::memory_order_seq_cst) volatile LIBCOPP_MACRO_NOEXCEPT {
#  if defined(__LIBCOPP_UTIL_LOCK_ATOMIC_INT_ATOMIC_GCC_ATOMIC)
    return __atomic_fetch_sub(&data_, arg, order);
#  else
    return __sync_fetch_and_sub(&data_, arg);
#  endif
  }

  inline value_type fetch_and(value_type arg,
                              EXPLICIT_UNUSED_ATTR LIBCOPP_COPP_NAMESPACE_ID::util::lock::memory_order order =
                                  LIBCOPP_COPP_NAMESPACE_ID::util::lock::memory_order_seq_cst) LIBCOPP_MACRO_NOEXCEPT {
#  if defined(__LIBCOPP_UTIL_LOCK_ATOMIC_INT_ATOMIC_GCC_ATOMIC)
    return __atomic_fetch_and(&data_, arg, order);
#  else
    return __sync_fetch_and_and(&data_, arg);
#  endif
  }
  inline value_type fetch_and(
      value_type arg, EXPLICIT_UNUSED_ATTR LIBCOPP_COPP_NAMESPACE_ID::util::lock::memory_order order =
                          LIBCOPP_COPP_NAMESPACE_ID::util::lock::memory_order_seq_cst) volatile LIBCOPP_MACRO_NOEXCEPT {
#  if defined(__LIBCOPP_UTIL_LOCK_ATOMIC_INT_ATOMIC_GCC_ATOMIC)
    return __atomic_fetch_and(&data_, arg, order);
#  else
    return __sync_fetch_and_and(&data_, arg);
#  endif
  }

  inline value_type fetch_or(value_type arg,
                             EXPLICIT_UNUSED_ATTR LIBCOPP_COPP_NAMESPACE_ID::util::lock::memory_order order =
                                 LIBCOPP_COPP_NAMESPACE_ID::util::lock::memory_order_seq_cst) LIBCOPP_MACRO_NOEXCEPT {
#  if defined(__LIBCOPP_UTIL_LOCK_ATOMIC_INT_ATOMIC_GCC_ATOMIC)
    return __atomic_fetch_or(&data_, arg, order);
#  else
    return __sync_fetch_and_or(&data_, arg);
#  endif
  }
  inline value_type fetch_or(
      value_type arg, EXPLICIT_UNUSED_ATTR LIBCOPP_COPP_NAMESPACE_ID::util::lock::memory_order order =
                          LIBCOPP_COPP_NAMESPACE_ID::util::lock::memory_order_seq_cst) volatile LIBCOPP_MACRO_NOEXCEPT {
#  if defined(__LIBCOPP_UTIL_LOCK_ATOMIC_INT_ATOMIC_GCC_ATOMIC)
    return __atomic_fetch_or(&data_, arg, order);
#  else
    return __sync_fetch_and_or(&data_, arg);
#  endif
  }

  inline value_type fetch_xor(value_type arg,
                              EXPLICIT_UNUSED_ATTR LIBCOPP_COPP_NAMESPACE_ID::util::lock::memory_order order =
                                  LIBCOPP_COPP_NAMESPACE_ID::util::lock::memory_order_seq_cst) LIBCOPP_MACRO_NOEXCEPT {
#  if defined(__LIBCOPP_UTIL_LOCK_ATOMIC_INT_ATOMIC_GCC_ATOMIC)
    return __atomic_fetch_xor(&data_, arg, order);
#  else
    return __sync_fetch_and_xor(&data_, arg);
#  endif
  }
  inline value_type fetch_xor(
      value_type arg, EXPLICIT_UNUSED_ATTR LIBCOPP_COPP_NAMESPACE_ID::util::lock::memory_order order =
                          LIBCOPP_COPP_NAMESPACE_ID::util::lock::memory_order_seq_cst) volatile LIBCOPP_MACRO_NOEXCEPT {
#  if defined(__LIBCOPP_UTIL_LOCK_ATOMIC_INT_ATOMIC_GCC_ATOMIC)
    return __atomic_fetch_xor(&data_, arg, order);
#  else
    return __sync_fetch_and_xor(&data_, arg);
#  endif
  }
};

#endif

// used for unsafe (not multi-thread safe)
template <typename Ty = int>
struct LIBCOPP_COPP_API_HEAD_ONLY unsafe_int_type {
  using value_type = Ty;
};

template <typename Ty>
class LIBCOPP_COPP_API_HEAD_ONLY atomic_int_type<unsafe_int_type<Ty> > {
 public:
  using value_type = typename unsafe_int_type<Ty>::value_type;

 private:
  value_type data_;
  atomic_int_type(const atomic_int_type &) = delete;
#ifndef _MSC_VER
  atomic_int_type &operator=(const atomic_int_type &) = delete;
  atomic_int_type &operator=(const atomic_int_type &) volatile = delete;
#endif

 public:
  atomic_int_type() : data_() {}
  atomic_int_type(value_type desired) : data_(desired) {}

  inline void store(value_type desired,
                    EXPLICIT_UNUSED_ATTR LIBCOPP_COPP_NAMESPACE_ID::util::lock::memory_order order =
                        LIBCOPP_COPP_NAMESPACE_ID::util::lock::memory_order_seq_cst) LIBCOPP_MACRO_NOEXCEPT {
    data_ = desired;
  }
  inline void store(value_type desired,
                    EXPLICIT_UNUSED_ATTR LIBCOPP_COPP_NAMESPACE_ID::util::lock::memory_order order =
                        LIBCOPP_COPP_NAMESPACE_ID::util::lock::memory_order_seq_cst) volatile LIBCOPP_MACRO_NOEXCEPT {
    data_ = desired;
  }

  inline value_type load(EXPLICIT_UNUSED_ATTR LIBCOPP_COPP_NAMESPACE_ID::util::lock::memory_order order =
                             LIBCOPP_COPP_NAMESPACE_ID::util::lock::memory_order_seq_cst) const LIBCOPP_MACRO_NOEXCEPT {
    return data_;
  }
  inline value_type load(EXPLICIT_UNUSED_ATTR LIBCOPP_COPP_NAMESPACE_ID::util::lock::memory_order order =
                             LIBCOPP_COPP_NAMESPACE_ID::util::lock::memory_order_seq_cst) const
      volatile LIBCOPP_MACRO_NOEXCEPT {
    return data_;
  }

  inline operator value_type() const LIBCOPP_MACRO_NOEXCEPT { return load(); }
  inline operator value_type() const volatile LIBCOPP_MACRO_NOEXCEPT { return load(); }

  inline value_type operator=(value_type desired) LIBCOPP_MACRO_NOEXCEPT {
    store(desired);
    return desired;
  }
  inline value_type operator=(value_type desired) volatile LIBCOPP_MACRO_NOEXCEPT {
    store(desired);
    return desired;
  }

  inline value_type operator++() LIBCOPP_MACRO_NOEXCEPT { return ++data_; }
  inline value_type operator++() volatile LIBCOPP_MACRO_NOEXCEPT { return ++data_; }
  inline value_type operator++(int) LIBCOPP_MACRO_NOEXCEPT { return data_++; }
  inline value_type operator++(int) volatile LIBCOPP_MACRO_NOEXCEPT { return data_++; }
  inline value_type operator--() LIBCOPP_MACRO_NOEXCEPT { return --data_; }
  inline value_type operator--() volatile LIBCOPP_MACRO_NOEXCEPT { return --data_; }
  inline value_type operator--(int) LIBCOPP_MACRO_NOEXCEPT { return data_--; }
  inline value_type operator--(int) volatile LIBCOPP_MACRO_NOEXCEPT { return data_--; }

  inline value_type exchange(value_type desired,
                             EXPLICIT_UNUSED_ATTR LIBCOPP_COPP_NAMESPACE_ID::util::lock::memory_order order =
                                 LIBCOPP_COPP_NAMESPACE_ID::util::lock::memory_order_seq_cst) LIBCOPP_MACRO_NOEXCEPT {
    value_type ret = data_;
    data_ = desired;
    return ret;
  }
  inline value_type exchange(
      value_type desired,
      EXPLICIT_UNUSED_ATTR LIBCOPP_COPP_NAMESPACE_ID::util::lock::memory_order order =
          LIBCOPP_COPP_NAMESPACE_ID::util::lock::memory_order_seq_cst) volatile LIBCOPP_MACRO_NOEXCEPT {
    value_type ret = data_;
    data_ = desired;
    return ret;
  }

 private:
  inline bool cas(value_type &expected, value_type desired) LIBCOPP_MACRO_NOEXCEPT {
    COPP_LIKELY_IF (data_ == expected) {
      data_ = desired;
      return true;
    } else {
      expected = data_;
      return false;
    }
  }

 public:
  inline bool compare_exchange_weak(value_type &expected, value_type desired,
                                    EXPLICIT_UNUSED_ATTR LIBCOPP_COPP_NAMESPACE_ID::util::lock::memory_order success,
                                    EXPLICIT_UNUSED_ATTR LIBCOPP_COPP_NAMESPACE_ID::util::lock::memory_order failure)
      LIBCOPP_MACRO_NOEXCEPT {
    return cas(expected, desired);
  }
  inline bool compare_exchange_weak(value_type &expected, value_type desired,
                                    EXPLICIT_UNUSED_ATTR LIBCOPP_COPP_NAMESPACE_ID::util::lock::memory_order success,
                                    EXPLICIT_UNUSED_ATTR LIBCOPP_COPP_NAMESPACE_ID::util::lock::memory_order
                                        failure) volatile LIBCOPP_MACRO_NOEXCEPT {
    return cas(expected, desired);
  }

  inline bool compare_exchange_weak(value_type &expected, value_type desired,
                                    EXPLICIT_UNUSED_ATTR LIBCOPP_COPP_NAMESPACE_ID::util::lock::memory_order order =
                                        LIBCOPP_COPP_NAMESPACE_ID::util::lock::memory_order_seq_cst)
      LIBCOPP_MACRO_NOEXCEPT {
    return cas(expected, desired);
  }
  inline bool compare_exchange_weak(
      value_type &expected, value_type desired,
      EXPLICIT_UNUSED_ATTR LIBCOPP_COPP_NAMESPACE_ID::util::lock::memory_order order =
          LIBCOPP_COPP_NAMESPACE_ID::util::lock::memory_order_seq_cst) volatile LIBCOPP_MACRO_NOEXCEPT {
    return cas(expected, desired);
  }

  inline bool compare_exchange_strong(value_type &expected, value_type desired,
                                      EXPLICIT_UNUSED_ATTR LIBCOPP_COPP_NAMESPACE_ID::util::lock::memory_order success,
                                      EXPLICIT_UNUSED_ATTR LIBCOPP_COPP_NAMESPACE_ID::util::lock::memory_order failure)
      LIBCOPP_MACRO_NOEXCEPT {
    return cas(expected, desired);
  }
  inline bool compare_exchange_strong(value_type &expected, value_type desired,
                                      EXPLICIT_UNUSED_ATTR LIBCOPP_COPP_NAMESPACE_ID::util::lock::memory_order success,
                                      EXPLICIT_UNUSED_ATTR LIBCOPP_COPP_NAMESPACE_ID::util::lock::memory_order
                                          failure) volatile LIBCOPP_MACRO_NOEXCEPT {
    return cas(expected, desired);
  }

  inline bool compare_exchange_strong(value_type &expected, value_type desired,
                                      EXPLICIT_UNUSED_ATTR LIBCOPP_COPP_NAMESPACE_ID::util::lock::memory_order order =
                                          LIBCOPP_COPP_NAMESPACE_ID::util::lock::memory_order_seq_cst)
      LIBCOPP_MACRO_NOEXCEPT {
    return cas(expected, desired);
  }
  inline bool compare_exchange_strong(
      value_type &expected, value_type desired,
      EXPLICIT_UNUSED_ATTR LIBCOPP_COPP_NAMESPACE_ID::util::lock::memory_order order =
          LIBCOPP_COPP_NAMESPACE_ID::util::lock::memory_order_seq_cst) volatile LIBCOPP_MACRO_NOEXCEPT {
    return cas(expected, desired);
  }

  inline value_type fetch_add(value_type arg,
                              EXPLICIT_UNUSED_ATTR LIBCOPP_COPP_NAMESPACE_ID::util::lock::memory_order order =
                                  LIBCOPP_COPP_NAMESPACE_ID::util::lock::memory_order_seq_cst) LIBCOPP_MACRO_NOEXCEPT {
    value_type ret = data_;
    data_ += arg;
    return ret;
  }
  inline value_type fetch_add(
      value_type arg, EXPLICIT_UNUSED_ATTR LIBCOPP_COPP_NAMESPACE_ID::util::lock::memory_order order =
                          LIBCOPP_COPP_NAMESPACE_ID::util::lock::memory_order_seq_cst) volatile LIBCOPP_MACRO_NOEXCEPT {
    value_type ret = data_;
    data_ += arg;
    return ret;
  }

  inline value_type fetch_sub(value_type arg,
                              EXPLICIT_UNUSED_ATTR LIBCOPP_COPP_NAMESPACE_ID::util::lock::memory_order order =
                                  LIBCOPP_COPP_NAMESPACE_ID::util::lock::memory_order_seq_cst) LIBCOPP_MACRO_NOEXCEPT {
    value_type ret = data_;
    data_ -= arg;
    return ret;
  }
  inline value_type fetch_sub(
      value_type arg, EXPLICIT_UNUSED_ATTR LIBCOPP_COPP_NAMESPACE_ID::util::lock::memory_order order =
                          LIBCOPP_COPP_NAMESPACE_ID::util::lock::memory_order_seq_cst) volatile LIBCOPP_MACRO_NOEXCEPT {
    value_type ret = data_;
    data_ -= arg;
    return ret;
  }

  inline value_type fetch_and(value_type arg,
                              EXPLICIT_UNUSED_ATTR LIBCOPP_COPP_NAMESPACE_ID::util::lock::memory_order order =
                                  LIBCOPP_COPP_NAMESPACE_ID::util::lock::memory_order_seq_cst) LIBCOPP_MACRO_NOEXCEPT {
    value_type ret = data_;
    data_ &= arg;
    return ret;
  }
  inline value_type fetch_and(
      value_type arg, EXPLICIT_UNUSED_ATTR LIBCOPP_COPP_NAMESPACE_ID::util::lock::memory_order order =
                          LIBCOPP_COPP_NAMESPACE_ID::util::lock::memory_order_seq_cst) volatile LIBCOPP_MACRO_NOEXCEPT {
    value_type ret = data_;
    data_ &= arg;
    return ret;
  }

  inline value_type fetch_or(value_type arg,
                             EXPLICIT_UNUSED_ATTR LIBCOPP_COPP_NAMESPACE_ID::util::lock::memory_order order =
                                 LIBCOPP_COPP_NAMESPACE_ID::util::lock::memory_order_seq_cst) LIBCOPP_MACRO_NOEXCEPT {
    value_type ret = data_;
    data_ |= arg;
    return ret;
  }
  inline value_type fetch_or(
      value_type arg, EXPLICIT_UNUSED_ATTR LIBCOPP_COPP_NAMESPACE_ID::util::lock::memory_order order =
                          LIBCOPP_COPP_NAMESPACE_ID::util::lock::memory_order_seq_cst) volatile LIBCOPP_MACRO_NOEXCEPT {
    value_type ret = data_;
    data_ |= arg;
    return ret;
  }

  inline value_type fetch_xor(value_type arg,
                              EXPLICIT_UNUSED_ATTR LIBCOPP_COPP_NAMESPACE_ID::util::lock::memory_order order =
                                  LIBCOPP_COPP_NAMESPACE_ID::util::lock::memory_order_seq_cst) LIBCOPP_MACRO_NOEXCEPT {
    value_type ret = data_;
    data_ ^= arg;
    return ret;
  }
  inline value_type fetch_xor(
      value_type arg, EXPLICIT_UNUSED_ATTR LIBCOPP_COPP_NAMESPACE_ID::util::lock::memory_order order =
                          LIBCOPP_COPP_NAMESPACE_ID::util::lock::memory_order_seq_cst) volatile LIBCOPP_MACRO_NOEXCEPT {
    value_type ret = data_;
    data_ ^= arg;
    return ret;
  }
};
}  // namespace lock
}  // namespace util
LIBCOPP_COPP_NAMESPACE_END

// cmake template file

#ifndef COPP_UTILS_FEATURES_H
#define COPP_UTILS_FEATURES_H

#pragma once

// ================ build options ================
#include <libcopp/utils/config/libcopp_build_features.h>
// ---------------- build options ----------------

// ================ select compiler ================
#if defined(__GCCXML__)
#  define COPP_MACRO_COMPILER_GCCXML 1
#elif defined(__clang__)
#  define COPP_MACRO_COMPILER_CLANG 1
#elif defined(_MSC_VER)
#  define COPP_MACRO_COMPILER_MSVC 1
#elif defined(__INTEL_COMPILER) || defined(__ICL) || defined(__ICC) || defined(__ECC)
#  define COPP_MACRO_COMPILER_INTELC 1
#elif defined(__GNUC__)
#  define COPP_MACRO_COMPILER_GCC 1
#elif defined(__BORLANDC__)
#  define COPP_MACRO_COMPILER_BORLANDC 1
#endif
// ---------------- select compiler ----------------

// ================ select platform ================
#if (defined(linux) || defined(__linux) || defined(__linux__) || defined(__GNU__) || defined(__GLIBC__)) && \
    !defined(_CRAYC)
#  define COPP_MACRO_PLATFORM_LINUX
#elif defined(__FreeBSD__) || defined(__NetBSD__) || defined(__OpenBSD__) || defined(__DragonFly__)
#  define COPP_MACRO_PLATFORM_BSD
#elif defined(sun) || defined(__sun)
#  define COPP_MACRO_PLATFORM_SOLARIS
#elif defined(__sgi)
#  define COPP_MACRO_PLATFORM_SGI
#elif defined(__hpux)
#  define COPP_MACRO_PLATFORM_HPUNIX
#elif defined(__CYGWIN__)
#  define COPP_MACRO_PLATFORM_CYGWIN
#elif defined(_WIN32) || defined(__WIN32__) || defined(WIN32)
#  define COPP_MACRO_PLATFORM_WIN32
#else
#  if defined(unix) || defined(__unix) || defined(_XOPEN_SOURCE) || defined(_POSIX_SOURCE)
#    define COPP_MACRO_PLATFORM_UNIX
#  endif
#endif
// ---------------- select platform ----------------

// ---------------- c extern ----------------

// ================ compiler abi headers ================
#if defined(COPP_MACRO_COMPILER_MSVC)
#  define COPP_HAS_ABI_HEADERS 1
#  define COPP_ABI_PREFIX "libcopp/utils/abi/msvc_prefix.hpp"
#  define COPP_ABI_SUFFIX "libcopp/utils/abi/msvc_suffix.hpp"
#elif defined COPP_MACRO_COMPILER_BORLANDC
#  define COPP_HAS_ABI_HEADERS 1
#  define COPP_ABI_PREFIX "libcopp/utils/abi/borland_prefix.hpp"
#  define COPP_ABI_SUFFIX "libcopp/utils/abi/borland_suffix.hpp"
#endif
// ---------------- compiler abi headers ----------------

// ================ function flags ================

#if defined(LIBCOPP_MACRO_USE_SEGMENTED_STACKS)
#  if !((defined(__GNUC__) && (__GNUC__ > 4 || (__GNUC__ > 3 && __GNUC_MINOR__ > 6))) || \
        (defined(__clang__) && __clang_major__ > 2 && __clang_minor__ > 3))
#    error "compiler does not support segmented_stack stacks"
#  endif
#  define COPP_MACRO_SEGMENTED_STACK_NUMBER 10
#endif

#ifndef COPP_MACRO_CPP_STD
#  if defined(__cplusplus) && __cplusplus >= 201103L
#    define COPP_MACRO_CPP_STD 201103L
#    define COPP_MACRO_ENABLE_SMART_PTR 1
#  else
#    define COPP_MACRO_CPP_STD 199711L
#  endif
#endif

#if defined(_POSIX_MT_) || defined(_MSC_VER)
#  define COPP_MACRO_ENABLE_MULTI_THREAD
#endif
// ---------------- function flags ----------------

// ================ branch prediction information ================
#ifndef likely
#  ifdef __GNUC__
#    define likely(x) __builtin_expect(!!(x), 1)
#  else
#    define likely(x) !!(x)
#  endif
#endif

#ifndef unlikely
#  ifdef __GNUC__
#    define unlikely(x) __builtin_expect(!!(x), 0)
#  else
#    define unlikely(x) !!(x)
#  endif
#endif

#ifndef unreachable
#  ifdef __GNUC__
#    ifdef __clang__
#      if __has_builtin(__builtin_unreachable)
#        define unreachable() __builtin_unreachable()
#      else
#        define unreachable() abort()
#      endif
#    else
#      if (__GNUC__ > 4 || (__GNUC__ == 4 && __GNUC_MINOR__ >= 6))
#        define unreachable() __builtin_unreachable()
#      else
#        define unreachable() abort()
#      endif
#    endif
#  else
#    define unreachable() abort()
#  endif
#endif
// ---------------- branch prediction information ----------------

#include "errno.h"

#if !defined(COPP_MACRO_RV_REF)
#  define COPP_MACRO_RV_REF &&
#endif

#if !defined(COPP_MACRO_STD_MOVE)
#  define COPP_MACRO_STD_MOVE(x) std::move(x)
#endif

#if !defined(COPP_MACRO_STD_FORWARD)
#  define COPP_MACRO_STD_FORWARD(t, x) std::forward<t>(x)
#endif

// ================ branch prediction information ================
#if defined(COPP_MACRO_COMPILER_GCC)
#  if (__GNUC__ * 100 + __GNUC_MINOR__) >= 408 && __cplusplus >= 201103L
#    define COPP_MACRO_COMPILER_CXX_THREAD_LOCAL 1
#  else
#    define COPP_MACRO_COMPILER_CXX_THREAD_LOCAL 0
#  endif
#elif defined(COPP_MACRO_COMPILER_CLANG)
#  if __has_feature(cxx_thread_local)
#    define COPP_MACRO_COMPILER_CXX_THREAD_LOCAL 1
#  else
#    define COPP_MACRO_COMPILER_CXX_THREAD_LOCAL 0
#  endif
#elif defined(_MSC_VER)
#  if _MSC_VER >= 1900
#    define COPP_MACRO_COMPILER_CXX_THREAD_LOCAL 1
#  else
#    define COPP_MACRO_COMPILER_CXX_THREAD_LOCAL 0
#  endif
#endif

// iOS may not link STL with thread_local
#if defined(__APPLE__)
#  include <TargetConditionals.h>

#  if TARGET_OS_IPHONE || TARGET_OS_EMBEDDED || TARGET_IPHONE_SIMULATOR
#    define COPP_MACRO_DISABLE_THREAD_LOCAL_KEYWORK
#  endif
#endif

// android may not link STL with thread_local
#if defined(__ANDROID__)
#  define COPP_MACRO_DISABLE_THREAD_LOCAL_KEYWORK
#endif

#if !defined(COPP_MACRO_DISABLE_THREAD_LOCAL_KEYWORK)
#  if defined(COPP_MACRO_COMPILER_CXX_THREAD_LOCAL) && COPP_MACRO_COMPILER_CXX_THREAD_LOCAL
#    define COPP_MACRO_THREAD_LOCAL thread_local
#  elif defined(COPP_MACRO_COMPILER_GCC) || defined(COPP_MACRO_COMPILER_CLANG)
#    define COPP_MACRO_THREAD_LOCAL __thread
#  elif defined(_MSC_VER)
#    define COPP_MACRO_THREAD_LOCAL __declspec(thread)
#  else
// COPP_MACRO_THREAD_LOCAL not defined for this configuration.
#  endif
#endif
// ---------------- branch prediction information ----------------

#endif

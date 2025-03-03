// cmake template file

#ifndef LIBCOPP_UTILS_FEATURES_H
#define LIBCOPP_UTILS_FEATURES_H

#pragma once

// ================ build options ================
#include <libcopp/utils/config/libcopp_build_features.h>
// ---------------- build options ----------------

// ================ select compiler ================
#if defined(__GCCXML__)
#  define LIBCOPP_MACRO_COMPILER_GCCXML 1
#elif defined(__clang__)
#  define LIBCOPP_MACRO_COMPILER_CLANG 1
#elif defined(_MSC_VER)
#  define LIBCOPP_MACRO_COMPILER_MSVC 1
#elif defined(__INTEL_COMPILER) || defined(__ICL) || defined(__ICC) || defined(__ECC)
#  define LIBCOPP_MACRO_COMPILER_INTELC 1
#elif defined(__GNUC__)
#  define LIBCOPP_MACRO_COMPILER_GCC 1
#elif defined(__BORLANDC__)
#  define LIBCOPP_MACRO_COMPILER_BORLANDC 1
#endif
// ---------------- select compiler ----------------

// ================ select platform ================
#if (defined(linux) || defined(__linux) || defined(__linux__) || defined(__GNU__) || defined(__GLIBC__)) && \
    !defined(_CRAYC)
#  define LIBCOPP_MACRO_PLATFORM_LINUX
#elif defined(__FreeBSD__) || defined(__NetBSD__) || defined(__OpenBSD__) || defined(__DragonFly__)
#  define LIBCOPP_MACRO_PLATFORM_BSD
#elif defined(sun) || defined(__sun)
#  define LIBCOPP_MACRO_PLATFORM_SOLARIS
#elif defined(__sgi)
#  define LIBCOPP_MACRO_PLATFORM_SGI
#elif defined(__hpux)
#  define LIBCOPP_MACRO_PLATFORM_HPUNIX
#elif defined(__CYGWIN__)
#  define LIBCOPP_MACRO_PLATFORM_CYGWIN
#elif defined(_WIN32) || defined(__WIN32__) || defined(WIN32)
#  define LIBCOPP_MACRO_PLATFORM_WIN32
#else
#  if defined(unix) || defined(__unix) || defined(_XOPEN_SOURCE) || defined(_POSIX_SOURCE)
#    define LIBCOPP_MACRO_PLATFORM_UNIX
#  endif
#endif
// ---------------- select platform ----------------

// ---------------- c extern ----------------

// ================ compiler abi headers ================
#if defined(LIBCOPP_MACRO_COMPILER_MSVC)
#  define LIBCOPP_HAS_ABI_HEADERS 1
#  define LIBCOPP_ABI_PREFIX "libcopp/utils/abi/msvc_prefix.hpp"
#  define LIBCOPP_ABI_SUFFIX "libcopp/utils/abi/msvc_suffix.hpp"
#elif defined LIBCOPP_MACRO_COMPILER_BORLANDC
#  define LIBCOPP_HAS_ABI_HEADERS 1
#  define LIBCOPP_ABI_PREFIX "libcopp/utils/abi/borland_prefix.hpp"
#  define LIBCOPP_ABI_SUFFIX "libcopp/utils/abi/borland_suffix.hpp"
#endif
// ---------------- compiler abi headers ----------------

// ================ function flags ================

#if defined(LIBCOPP_MACRO_USE_SEGMENTED_STACKS)
#  if !((defined(__GNUC__) && (__GNUC__ > 4 || (__GNUC__ > 3 && __GNUC_MINOR__ > 6))) || \
        (defined(__clang__) && __clang_major__ > 2 && __clang_minor__ > 3))
#    error "compiler does not support segmented_stack stacks"
#  endif
#  define LIBCOPP_MACRO_SEGMENTED_STACK_NUMBER 10
#endif

#ifndef LIBCOPP_MACRO_CPP_STD
#  if defined(__cplusplus) && __cplusplus >= 201103L
#    define LIBCOPP_MACRO_CPP_STD 201103L
#    define LIBCOPP_MACRO_ENABLE_SMART_PTR 1
#  else
#    define LIBCOPP_MACRO_CPP_STD 199711L
#  endif
#endif

// ---------------- function flags ----------------

// ---------------- branch prediction information ----------------

#include "errno.h"

// Deprecated macro for old toolchain
#if !defined(COPP_MACRO_RV_REF)
#  define COPP_MACRO_RV_REF &&
#endif

// Deprecated macro for old toolchain
#if !defined(COPP_MACRO_STD_MOVE)
#  define COPP_MACRO_STD_MOVE(x) std::move(x)
#endif

// Deprecated macro for old toolchain
#if !defined(COPP_MACRO_STD_FORWARD)
#  define COPP_MACRO_STD_FORWARD(t, x) std::forward<t>(x)
#endif

// ================ branch prediction information ================
#if defined(LIBCOPP_MACRO_COMPILER_GCC)
#  if (__GNUC__ * 100 + __GNUC_MINOR__) >= 408 && __cplusplus >= 201103L
#    define LIBCOPP_MACRO_COMPILER_CXX_THREAD_LOCAL 1
#  else
#    define LIBCOPP_MACRO_COMPILER_CXX_THREAD_LOCAL 0
#  endif
#elif defined(LIBCOPP_MACRO_COMPILER_CLANG)
#  if __has_feature(cxx_thread_local)
#    define LIBCOPP_MACRO_COMPILER_CXX_THREAD_LOCAL 1
#  else
#    define LIBCOPP_MACRO_COMPILER_CXX_THREAD_LOCAL 0
#  endif
#elif defined(_MSC_VER)
#  if _MSC_VER >= 1900
#    define LIBCOPP_MACRO_COMPILER_CXX_THREAD_LOCAL 1
#  else
#    define LIBCOPP_MACRO_COMPILER_CXX_THREAD_LOCAL 0
#  endif
#endif

// iOS may not link STL with thread_local
#if defined(__APPLE__)
#  include <TargetConditionals.h>

#  if TARGET_OS_IPHONE || TARGET_OS_EMBEDDED || TARGET_IPHONE_SIMULATOR
#    define LIBCOPP_MACRO_DISABLE_THREAD_LOCAL_KEYWORK
#  endif
#endif

// android may not link STL with thread_local
#if defined(__ANDROID__)
#  define LIBCOPP_MACRO_DISABLE_THREAD_LOCAL_KEYWORK
#endif

#if !defined(LIBCOPP_MACRO_DISABLE_THREAD_LOCAL_KEYWORK)
#  if defined(LIBCOPP_MACRO_COMPILER_CXX_THREAD_LOCAL) && LIBCOPP_MACRO_COMPILER_CXX_THREAD_LOCAL
#    define LIBCOPP_MACRO_THREAD_LOCAL thread_local
#  elif defined(LIBCOPP_MACRO_COMPILER_GCC) || defined(LIBCOPP_MACRO_COMPILER_CLANG)
#    define LIBCOPP_MACRO_THREAD_LOCAL __thread
#  elif defined(_MSC_VER)
#    define LIBCOPP_MACRO_THREAD_LOCAL __declspec(thread)
#  else
// LIBCOPP_MACRO_THREAD_LOCAL not defined for this configuration.
#  endif
#endif
// ---------------- branch prediction information ----------------

#endif

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
#if !defined(COPP_LIKELY_IF) && defined(__cplusplus)
// GCC 9 has likely attribute but do not support declare it at the beginning of statement
#  if defined(__has_cpp_attribute) && (defined(__clang__) || !defined(__GNUC__) || __GNUC__ > 9)
#    if __has_cpp_attribute(likely)
#      define COPP_LIKELY_IF(...) if (__VA_ARGS__) [[likely]]
#    endif
#  endif
#endif
#if !defined(COPP_LIKELY_IF) && (defined(__clang__) || defined(__GNUC__))
#  define COPP_LIKELY_IF(...) if (__builtin_expect(!!(__VA_ARGS__), true))
#endif
#ifndef COPP_LIKELY_IF
#  define COPP_LIKELY_IF(...) if (__VA_ARGS__)
#endif

#if !defined(COPP_UNLIKELY_IF) && defined(__cplusplus)
// GCC 9 has likely attribute but do not support declare it at the beginning of statement
#  if defined(__has_cpp_attribute) && (defined(__clang__) || !defined(__GNUC__) || __GNUC__ > 9)
#    if __has_cpp_attribute(likely)
#      define COPP_UNLIKELY_IF(...) if (__VA_ARGS__) [[unlikely]]
#    endif
#  endif
#endif
#if !defined(COPP_UNLIKELY_IF) && (defined(__clang__) || defined(__GNUC__))
#  define COPP_UNLIKELY_IF(...) if (__builtin_expect(!!(__VA_ARGS__), false))
#endif
#ifndef COPP_UNLIKELY_IF
#  define COPP_UNLIKELY_IF(...) if (__VA_ARGS__)
#endif

// ---------------- branch prediction information ----------------

#if !defined(COPP_NORETURN_ATTR) && defined(__has_cpp_attribute)
#  if __has_cpp_attribute(noreturn)
#    define COPP_NORETURN_ATTR [[noreturn]]
#  endif
#endif
#ifndef COPP_NORETURN_ATTR
#  define COPP_NORETURN_ATTR
#endif

#ifndef COPP_UNREACHABLE
#  if defined(__cpp_lib_unreachable)
#    if __cpp_lib_unreachable
#      define COPP_UNREACHABLE() std::unreachable()
#    endif
#  endif
#  if !defined(COPP_UNREACHABLE) && defined(unreachable)
#    define COPP_UNREACHABLE() unreachable()
#  endif
#  if !defined(COPP_UNREACHABLE)
#    ifdef __GNUC__
#      ifdef __clang__
#        if __has_builtin(__builtin_unreachable)
#          define COPP_UNREACHABLE() __builtin_unreachable()
#        endif
#      else
#        if (__GNUC__ > 4 || (__GNUC__ == 4 && __GNUC_MINOR__ >= 6))
#          define COPP_UNREACHABLE() __builtin_unreachable()
#        endif
#      endif
#    endif
#  endif
#endif
#if !defined(COPP_UNREACHABLE)
#  if defined(_DEBUG) || !defined(NDEBUG)
#    define COPP_UNREACHABLE() std::abort()
#  else
#    define COPP_UNREACHABLE()
#  endif
#endif

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

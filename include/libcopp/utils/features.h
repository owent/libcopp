#ifndef _COPP_UTILS_FEATURES_H_
#define _COPP_UTILS_FEATURES_H_


#if defined(_MSC_VER) && (_MSC_VER >= 1020)
# pragma once
#endif

#include <libcopp/utils/config/compiler_features.h>

// ================ build options ================
// ---------------- build options ----------------

// ================ select compiler ================
#if defined(__GCCXML__)
    #define COPP_MACRO_COMPILER_GCCXML 1
#elif defined (__clang__)
    #define COPP_MACRO_COMPILER_CLANG 1
#elif defined(_MSC_VER)
    #define COPP_MACRO_COMPILER_MSVC 1
#elif defined(__INTEL_COMPILER) || defined(__ICL) || defined(__ICC) || defined(__ECC)
    #define COPP_MACRO_COMPILER_INTELC 1
#elif defined(__GNUC__)
    #define COPP_MACRO_COMPILER_GCC 1
#elif defined(__BORLANDC__)
    #define COPP_MACRO_COMPILER_BORLANDC 1
#endif
// ---------------- select compiler ----------------

// ================ select platform ================
#if (defined(linux) || defined(__linux) || defined(__linux__) || defined(__GNU__) || defined(__GLIBC__)) && !defined(_CRAYC)
    #define COPP_MACRO_PLATFORM_LINUX
#elif defined(__FreeBSD__) || defined(__NetBSD__) || defined(__OpenBSD__) || defined(__DragonFly__)
    #define COPP_MACRO_PLATFORM_BSD
#elif defined(sun) || defined(__sun)
    #define COPP_MACRO_PLATFORM_SOLARIS
#elif defined(__sgi)
    #define COPP_MACRO_PLATFORM_SGI
#elif defined(__hpux)
    #define COPP_MACRO_PLATFORM_HPUNIX
#elif defined(__CYGWIN__)
    #define COPP_MACRO_PLATFORM_CYGWIN
#elif defined(_WIN32) || defined(__WIN32__) || defined(WIN32)
    #define COPP_MACRO_PLATFORM_WIN32
#else
    #if defined(unix) || defined(__unix) || defined(_XOPEN_SOURCE) || defined(_POSIX_SOURCE)
        #define COPP_MACRO_PLATFORM_UNIX
    #endif
#  endif
// ---------------- select platform ----------------

// ================ c extern ================
#if !defined(__BEGIN_DECLS) && !defined(__END_DECLS)
    #if defined(__cplusplus)
        #define	__BEGIN_DECLS	extern "C" {
        #define	__END_DECLS	}
    #else
        #define	__BEGIN_DECLS
        #define	__END_DECLS
    #endif
#endif

#if !defined(__THROW)
#define __THROW
#endif
// ---------------- c extern ----------------
#if defined(UTIL_CONFIG_NOEXCEPT)
    #define COPP_MACRO_NOEXCEPT UTIL_CONFIG_NOEXCEPT
#else
    #define COPP_MACRO_NOEXCEPT
#endif

// ================ compiler abi headers ================
#if defined(COPP_MACRO_COMPILER_MSVC)
    #define COPP_HAS_ABI_HEADERS 1
    #define COPP_ABI_PREFIX "libcopp/utils/abi/msvc_prefix.hpp"
    #define COPP_ABI_SUFFIX "libcopp/utils/abi/msvc_suffix.hpp"
#elif defined COPP_MACRO_COMPILER_BORLANDC
    #define COPP_HAS_ABI_HEADERS 1
    #define COPP_ABI_PREFIX "libcopp/utils/abi/borland_prefix.hpp"
    #define COPP_ABI_SUFFIX "libcopp/utils/abi/borland_suffix.hpp"
#endif
// ---------------- compiler abi headers ----------------


// ================ function flags ================


#if defined(COPP_MACRO_USE_SEGMENTED_STACKS)
# if ! ( (defined(__GNUC__) && (__GNUC__ > 4 || (__GNUC__ > 3 && __GNUC_MINOR__ > 6))) || \
         (defined(__clang__) && __clang_major__ > 2 && __clang_minor__ > 3) )
#  error "compiler does not support segmented_stack stacks"
# endif
# define COPP_MACRO_SEGMENTED_STACK_NUMBER 10
#endif

#ifndef COPP_MACRO_CPP_STD
    #if defined(__cplusplus) && __cplusplus >= 201103L
        #define COPP_MACRO_CPP_STD 201103L
        #define COPP_MACRO_ENABLE_SMART_PTR 1
    #else
        #define COPP_MACRO_CPP_STD 199711L
    #endif
#endif


#if defined(UTIL_CONFIG_COMPILER_CXX_VARIADIC_TEMPLATES) && UTIL_CONFIG_COMPILER_CXX_VARIADIC_TEMPLATES
#define COPP_MACRO_ENABLE_VARIADIC_TEMPLATE 1
#endif

#include <libcopp/utils/std/decltype.h>
#if defined(STD_DECLTYPE)
    #define COPP_MACRO_TYPEOF(...) STD_DECLTYPE(__VA_ARGS__)
#endif

#if defined(_POSIX_MT_) || defined(_MSC_VER)
    #define COPP_MACRO_ENABLE_MULTI_THREAD
#endif
// ---------------- function flags ----------------

#include "errno.h"

#endif

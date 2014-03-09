#ifndef _COPP_CONFIG_FEATURES_H_
#define _COPP_CONFIG_FEATURES_H_


#if defined(_MSC_VER) && (_MSC_VER >= 1020)
# pragma once
#endif

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


// ================ compiler abi headers ================
#if defined(COPP_MACRO_COMPILER_MSVC)
    #define COPP_HAS_ABI_HEADERS 1
    #define COPP_ABI_PREFIX "libcopp/config/abi/msvc_prefix.hpp"
    #define COPP_ABI_SUFFIX "libcopp/config/abi/msvc_suffix.hpp"
#elif defined COPP_MACRO_COMPILER_BORLANDC
    #define COPP_HAS_ABI_HEADERS 1
    #define COPP_ABI_PREFIX "libcopp/config/abi/borland_prefix.hpp"
    #define COPP_ABI_SUFFIX "libcopp/config/abi/borland_suffix.hpp"
#endif
// ---------------- compiler abi headers ----------------


// ================ function flags ================
#ifdef COPP_MACRO_USE_SEGMENTED_STACKS
#define COPP_MACRO_SEGMENTED_STACK_NUMBER 10
#endif

#if defined(__cplusplus) && __cplusplus >= 201103L
    #define COPP_MACRO_ENABLE_VARIADIC_TEMPLATE 1
#elif defined(_MSC_VER) && (_MSC_VER >= 1800 && defined(_HAS_CPP0X) && _HAS_CPP0X)
    // VC 12 and upper
    #define COPP_MACRO_ENABLE_VARIADIC_TEMPLATE 1
#elif defined(__clang__) && __clang_major__ >= 3 && (__cplusplus >= 201103L || !defined(_LIBCPP_HAS_NO_VARIADICS))
    #define COPP_MACRO_ENABLE_VARIADIC_TEMPLATE 1
#elif defined(__GNUC__) && __GNUC__ >= 4 && __GNUC_MINOR__ >= 4
    // use G++ std::tr1
    #if __cplusplus >= 201103L || defined(__GXX_EXPERIMENTAL_CXX0X__)
        #define COPP_MACRO_ENABLE_VARIADIC_TEMPLATE 1
    #endif
#endif

// ---------------- function flags ----------------

#include "errno.h"

#endif

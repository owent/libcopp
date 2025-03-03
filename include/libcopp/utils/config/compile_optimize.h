// Copyright 2025 owent
// Copy from https://github.com/owent/atframe_utils/blob/main/include/config/compile_optimize.h
//   and remove all macros start with UTIL_* and rename all macros start with ATFW_UTIL_ to LIBCOPP_UTIL_.

#pragma once

// Import the C++20 feature-test macros
#ifdef __has_include
#  if __has_include(<version>)
#    include <version>
#  endif
#elif defined(_MSC_VER) && \
    ((defined(__cplusplus) && __cplusplus >= 202002L) || (defined(_MSVC_LANG) && _MSVC_LANG >= 202002L))
#  if _MSC_VER >= 1922
#    include <version>
#  endif
#endif

// LIBCOPP_UTIL_HAVE_BUILTIN&LIBCOPP_UTIL_HAVE_FEATURE
//
// Checks whether the compiler supports a Clang Feature Checking Macro, and if
// so, checks whether it supports the provided builtin function "x" where x
// is one of the functions noted in
// https://clang.llvm.org/docs/LanguageExtensions.html
//
// Note: Use this macro to avoid an extra level of #ifdef __has_builtin check.
// http://releases.llvm.org/3.3/tools/clang/docs/LanguageExtensions.html
#if !defined(LIBCOPP_UTIL_HAVE_BUILTIN)
#  ifdef __has_builtin
#    define LIBCOPP_UTIL_HAVE_BUILTIN(x) __has_builtin(x)
#  else
#    define LIBCOPP_UTIL_HAVE_BUILTIN(x) 0
#  endif
#endif

#if !defined(LIBCOPP_UTIL_HAVE_FEATURE)
#  ifdef __has_feature
#    define LIBCOPP_UTIL_HAVE_FEATURE(f) __has_feature(f)
#  else
#    define LIBCOPP_UTIL_HAVE_FEATURE(f) 0
#  endif
#endif

// ================ has feature ================
// LIBCOPP_UTIL_HAVE_ATTRIBUTE
//
// A function-like feature checking macro that is a wrapper around
// `__has_attribute`, which is defined by GCC 5+ and Clang and evaluates to a
// nonzero constant integer if the attribute is supported or 0 if not.
//
// It evaluates to zero if `__has_attribute` is not defined by the compiler.
//
// GCC: https://gcc.gnu.org/gcc-5/changes.html
// Clang: https://clang.llvm.org/docs/LanguageExtensions.html
#if !defined(LIBCOPP_UTIL_HAVE_ATTRIBUTE)
#  ifdef __has_attribute
#    define LIBCOPP_UTIL_HAVE_ATTRIBUTE(x) __has_attribute(x)
#  else
#    define LIBCOPP_UTIL_HAVE_ATTRIBUTE(x) 0
#  endif
#endif

// LIBCOPP_UTIL_HAVE_CPP_ATTRIBUTE
//
// A function-like feature checking macro that accepts C++11 style attributes.
// It's a wrapper around `__has_cpp_attribute`, defined by ISO C++ SD-6
// (https://en.cppreference.com/w/cpp/experimental/feature_test). If we don't
// find `__has_cpp_attribute`, will evaluate to 0.
#if !defined(LIBCOPP_UTIL_HAVE_CPP_ATTRIBUTE)
#  if defined(__cplusplus) && defined(__has_cpp_attribute)
// NOTE: requiring __cplusplus above should not be necessary, but
// works around https://bugs.llvm.org/show_bug.cgi?id=23435.
#    define LIBCOPP_UTIL_HAVE_CPP_ATTRIBUTE(x) __has_cpp_attribute(x)
#  else
#    define LIBCOPP_UTIL_HAVE_CPP_ATTRIBUTE(x) 0
#  endif
#endif

// ================ branch prediction information ================
#if !defined(LIBCOPP_UTIL_LIKELY_IF) && defined(__cplusplus)
// GCC 9 has likely attribute but do not support declare it at the beginning of statement
#  if defined(__has_cpp_attribute) && (defined(__clang__) || !defined(__GNUC__) || __GNUC__ > 9)
#    if __has_cpp_attribute(likely)
#      define LIBCOPP_UTIL_LIKELY_IF(...) if (__VA_ARGS__) [[likely]]
#    endif
#  endif
#endif
#if !defined(LIBCOPP_UTIL_LIKELY_IF) && (defined(__clang__) || defined(__GNUC__))
#  define LIBCOPP_UTIL_LIKELY_IF(...) if (__builtin_expect(!!(__VA_ARGS__), true))
#endif
#ifndef LIBCOPP_UTIL_LIKELY_IF
#  define LIBCOPP_UTIL_LIKELY_IF(...) if (__VA_ARGS__)
#endif
#if !defined(LIBCOPP_UTIL_LIKELY_CONDITION) && defined(__cplusplus)
// GCC 9 has likely attribute but do not support declare it at the beginning of statement
#  if defined(__has_cpp_attribute) && (defined(__clang__) || !defined(__GNUC__) || __GNUC__ > 9)
#    if __has_cpp_attribute(likely)
#      define LIBCOPP_UTIL_LIKELY_CONDITION(__C) (__C) [[likely]]
#    endif
#  endif
#endif
#if !defined(LIBCOPP_UTIL_LIKELY_CONDITION) && (defined(__clang__) || defined(__GNUC__))
#  define LIBCOPP_UTIL_LIKELY_CONDITION(__C) (__builtin_expect(!!(__C), true))
#endif
#ifndef LIBCOPP_UTIL_LIKELY_CONDITION
#  define LIBCOPP_UTIL_LIKELY_CONDITION(__C) (__C)
#endif

#if !defined(LIBCOPP_UTIL_UNLIKELY_IF) && defined(__cplusplus)
// GCC 9 has likely attribute but do not support declare it at the beginning of statement
#  if defined(__has_cpp_attribute) && (defined(__clang__) || !defined(__GNUC__) || __GNUC__ > 9)
#    if __has_cpp_attribute(likely)
#      define LIBCOPP_UTIL_UNLIKELY_IF(...) if (__VA_ARGS__) [[unlikely]]
#    endif
#  endif
#endif
#if !defined(LIBCOPP_UTIL_UNLIKELY_IF) && (defined(__clang__) || defined(__GNUC__))
#  define LIBCOPP_UTIL_UNLIKELY_IF(...) if (__builtin_expect(!!(__VA_ARGS__), false))
#endif
#ifndef LIBCOPP_UTIL_UNLIKELY_IF
#  define LIBCOPP_UTIL_UNLIKELY_IF(...) if (__VA_ARGS__)
#endif
#if !defined(LIBCOPP_UTIL_UNLIKELY_CONDITION) && defined(__cplusplus)
// GCC 9 has likely attribute but do not support declare it at the beginning of statement
#  if defined(__has_cpp_attribute) && (defined(__clang__) || !defined(__GNUC__) || __GNUC__ > 9)
#    if __has_cpp_attribute(likely)
#      define LIBCOPP_UTIL_UNLIKELY_CONDITION(__C) (__C) [[unlikely]]
#    endif
#  endif
#endif
#if !defined(LIBCOPP_UTIL_UNLIKELY_CONDITION) && (defined(__clang__) || defined(__GNUC__))
#  define LIBCOPP_UTIL_UNLIKELY_CONDITION(__C) (__builtin_expect(!!(__C), false))
#endif
#ifndef LIBCOPP_UTIL_UNLIKELY_CONDITION
#  define LIBCOPP_UTIL_UNLIKELY_CONDITION(__C) (__C)
#endif

// ---------------- branch prediction information ----------------

// ================ import/export ================
// @see https://gcc.gnu.org/wiki/Visibility
// @see http://releases.llvm.org/9.0.0/tools/clang/docs/AttributeReference.html
// 不支持 borland/sunpro_cc/xlcpp

// ================ import/export: for compilers ================
#if defined(__GNUC__) && !defined(__ibmxl__)
//  GNU C++/Clang
//
// Dynamic shared object (DSO) and dynamic-link library (DLL) support
//
#  if __GNUC__ >= 4
#    if defined(_WIN32) || defined(__WIN32__) || defined(WIN32) || defined(__CYGWIN__)
// All Win32 development environments, including 64-bit Windows and MinGW, define
// _WIN32 or one of its variant spellings. Note that Cygwin is a POSIX environment,
// so does not define _WIN32 or its variants.
#      ifndef LIBCOPP_UTIL_SYMBOL_EXPORT
#        define LIBCOPP_UTIL_SYMBOL_EXPORT __attribute__((__dllexport__))
#      endif
#      ifndef LIBCOPP_UTIL_SYMBOL_IMPORT
#        define LIBCOPP_UTIL_SYMBOL_IMPORT __attribute__((__dllimport__))
#      endif

#    else

#      ifndef LIBCOPP_UTIL_SYMBOL_EXPORT
#        define LIBCOPP_UTIL_SYMBOL_EXPORT __attribute__((visibility("default")))
#      endif
#      ifndef LIBCOPP_UTIL_SYMBOL_IMPORT
#        define LIBCOPP_UTIL_SYMBOL_IMPORT __attribute__((visibility("default")))
#      endif
#      ifndef LIBCOPP_UTIL_SYMBOL_VISIBLE
#        define LIBCOPP_UTIL_SYMBOL_VISIBLE __attribute__((visibility("default")))
#      endif
#      ifndef LIBCOPP_UTIL_SYMBOL_LOCAL
#        define LIBCOPP_UTIL_SYMBOL_LOCAL __attribute__((visibility("hidden")))
#      endif

#    endif

#  else
// config/platform/win32.hpp will define LIBCOPP_UTIL_SYMBOL_EXPORT, etc., unless already defined
#    ifndef LIBCOPP_UTIL_SYMBOL_EXPORT
#      define LIBCOPP_UTIL_SYMBOL_EXPORT
#    endif

#    ifndef LIBCOPP_UTIL_SYMBOL_IMPORT
#      define LIBCOPP_UTIL_SYMBOL_IMPORT
#    endif
#    ifndef LIBCOPP_UTIL_SYMBOL_VISIBLE
#      define LIBCOPP_UTIL_SYMBOL_VISIBLE
#    endif
#    ifndef LIBCOPP_UTIL_SYMBOL_LOCAL
#      define LIBCOPP_UTIL_SYMBOL_LOCAL
#    endif

#  endif

#elif defined(_MSC_VER)
//  Microsoft Visual C++
//
//  Must remain the last #elif since some other vendors (Metrowerks, for
//  example) also #define _MSC_VER
#else
#endif
// ---------------- import/export: for compilers ----------------

// ================ import/export: for platform ================
//  Default defines for LIBCOPP_UTIL_SYMBOL_EXPORT and LIBCOPP_UTIL_SYMBOL_IMPORT
//  If a compiler doesn't support __declspec(dllexport)/__declspec(dllimport),
//  its boost/config/compiler/ file must define LIBCOPP_UTIL_SYMBOL_EXPORT and
//  LIBCOPP_UTIL_SYMBOL_IMPORT
#if !defined(LIBCOPP_UTIL_SYMBOL_EXPORT) && \
    (defined(_WIN32) || defined(__WIN32__) || defined(WIN32) || defined(__CYGWIN__))

#  ifndef LIBCOPP_UTIL_SYMBOL_EXPORT
#    define LIBCOPP_UTIL_SYMBOL_EXPORT __declspec(dllexport)
#  endif
#  ifndef LIBCOPP_UTIL_SYMBOL_IMPORT
#    define LIBCOPP_UTIL_SYMBOL_IMPORT __declspec(dllimport)
#  endif
#endif
// ---------------- import/export: for platform ----------------

#ifndef LIBCOPP_UTIL_SYMBOL_EXPORT
#  define LIBCOPP_UTIL_SYMBOL_EXPORT
#endif
#ifndef LIBCOPP_UTIL_SYMBOL_IMPORT
#  define LIBCOPP_UTIL_SYMBOL_IMPORT
#endif
#ifndef LIBCOPP_UTIL_SYMBOL_VISIBLE
#  define LIBCOPP_UTIL_SYMBOL_VISIBLE
#endif
#ifndef LIBCOPP_UTIL_SYMBOL_LOCAL
#  define LIBCOPP_UTIL_SYMBOL_LOCAL
#endif
#ifndef LIBCOPP_UTIL_SYMBOL_NONE
#  define LIBCOPP_UTIL_SYMBOL_NONE
#endif

// ---------------- import/export ----------------

// ================ __cdecl ================
#if defined(__GNUC__) || defined(__GNUG__)
#  ifndef __cdecl
// see https://gcc.gnu.org/onlinedocs/gcc-4.0.0/gcc/Function-Attributes.html
// Intel x86 architecture specific calling conventions
#    ifdef _M_IX86
#      define __cdecl __attribute__((__cdecl__))
#    else
#      define __cdecl
#    endif
#  endif
#endif
// ---------------- __cdecl ----------------

// ================ always inline ================

#ifndef LIBCOPP_UTIL_FORCEINLINE
#  if defined(__clang__)
#    if __cplusplus >= 201103L
#      define LIBCOPP_UTIL_FORCEINLINE [[gnu::always_inline]] inline
#    else
#      define LIBCOPP_UTIL_FORCEINLINE __attribute__((always_inline)) inline
#    endif
#  elif defined(__GNUC__) && __GNUC__ > 3
#    if __cplusplus >= 201103L && (__GNUC__ * 100 + __GNUC_MINOR__) >= 408
#      define LIBCOPP_UTIL_FORCEINLINE [[gnu::always_inline]] inline
#    else
#      define LIBCOPP_UTIL_FORCEINLINE __attribute__((always_inline)) inline
#    endif
#  elif defined(_MSC_VER)
#    define LIBCOPP_UTIL_FORCEINLINE __forceinline
#  else
#    define LIBCOPP_UTIL_FORCEINLINE inline
#  endif
#endif

#ifndef LIBCOPP_UTIL_NOINLINE_NOCLONE
#  if defined(__clang__)
#    if __cplusplus >= 201103L
#      define LIBCOPP_UTIL_NOINLINE_NOCLONE [[gnu::noinline]]
#    else
#      define LIBCOPP_UTIL_NOINLINE_NOCLONE __attribute__((noinline))
#    endif
#  elif defined(__GNUC__) && __GNUC__ > 3
#    if __cplusplus >= 201103L && (__GNUC__ * 100 + __GNUC_MINOR__) >= 408
#      define LIBCOPP_UTIL_NOINLINE_NOCLONE [[gnu::noinline, gnu::noclone]]
#    else
#      define LIBCOPP_UTIL_NOINLINE_NOCLONE __attribute__((noinline, noclone))
#    endif
#  elif defined(_MSC_VER)
#    define LIBCOPP_UTIL_NOINLINE_NOCLONE __declspec(noinline)
#  else
#    define LIBCOPP_UTIL_NOINLINE_NOCLONE
#  endif
#endif

#ifndef LIBCOPP_UTIL_CONST_INIT
#  if defined(__cpp_constinit) && __cpp_constinit >= 201907L
#    if defined(_MSC_VER)
#      define LIBCOPP_UTIL_CONST_INIT
#    else
#      define LIBCOPP_UTIL_CONST_INIT constinit
#    endif
#  elif LIBCOPP_UTIL_HAVE_CPP_ATTRIBUTE(clang::require_constant_initialization)
#    define LIBCOPP_UTIL_CONST_INIT [[clang::require_constant_initialization]]
#  else
#    define LIBCOPP_UTIL_CONST_INIT
#  endif
#endif

// LIBCOPP_UTIL_ATTRIBUTE_LIFETIME_BOUND indicates that a resource owned by a function
// parameter or implicit object parameter is retained by the return value of the
// annotated function (or, for a parameter of a constructor, in the value of the
// constructed object). This attribute causes warnings to be produced if a
// temporary object does not live long enough.
//
// When applied to a reference parameter, the referenced object is assumed to be
// retained by the return value of the function. When applied to a non-reference
// parameter (for example, a pointer or a class type), all temporaries
// referenced by the parameter are assumed to be retained by the return value of
// the function.
//
// See also the upstream documentation:
// https://clang.llvm.org/docs/AttributeReference.html#lifetimebound
#ifndef LIBCOPP_UTIL_ATTRIBUTE_LIFETIME_BOUND
#  if LIBCOPP_UTIL_HAVE_CPP_ATTRIBUTE(clang::lifetimebound)
#    define LIBCOPP_UTIL_ATTRIBUTE_LIFETIME_BOUND [[clang::lifetimebound]]
#  elif LIBCOPP_UTIL_HAVE_ATTRIBUTE(lifetimebound)
#    define LIBCOPP_UTIL_ATTRIBUTE_LIFETIME_BOUND __attribute__((lifetimebound))
#  else
#    define LIBCOPP_UTIL_ATTRIBUTE_LIFETIME_BOUND
#  endif
#endif

// Internal attribute; name and documentation TBD.
//
// See the upstream documentation:
// https://clang.llvm.org/docs/AttributeReference.html#lifetime_capture_by
#ifndef LIBCOPP_UTIL_INTERNAL_ATTRIBUTE_CAPTURED_BY
#  if LIBCOPP_UTIL_HAVE_CPP_ATTRIBUTE(clang::lifetime_capture_by)
#    define LIBCOPP_UTIL_INTERNAL_ATTRIBUTE_CAPTURED_BY(Owner) [[clang::lifetime_capture_by(Owner)]]
#  else
#    define LIBCOPP_UTIL_INTERNAL_ATTRIBUTE_CAPTURED_BY(Owner)
#  endif
#endif

// LIBCOPP_UTIL_ATTRIBUTE_VIEW indicates that a type is solely a "view" of data that it
// points to, similarly to a span, string_view, or other non-owning reference
// type.
// This enables diagnosing certain lifetime issues similar to those enabled by
// LIBCOPP_UTIL_ATTRIBUTE_LIFETIME_BOUND, such as:
//
//   struct LIBCOPP_UTIL_ATTRIBUTE_VIEW StringView {
//     template<class R>
//     StringView(const R&);
//   };
//
//   StringView f(std::string s) {
//     return s;  // warning: address of stack memory returned
//   }
//
//
// See the following links for details:
// https://reviews.llvm.org/D64448
// https://lists.llvm.org/pipermail/cfe-dev/2018-November/060355.html
#ifndef LIBCOPP_UTIL_ATTRIBUTE_VIEW
#  if LIBCOPP_UTIL_HAVE_CPP_ATTRIBUTE(gsl::Pointer) && (!defined(__clang_major__) || __clang_major__ >= 13)
#    define LIBCOPP_UTIL_ATTRIBUTE_VIEW [[gsl::Pointer]]
#  else
#    define LIBCOPP_UTIL_ATTRIBUTE_VIEW
#  endif
#endif

// LIBCOPP_UTIL_ATTRIBUTE_OWNER indicates that a type is a container, smart pointer, or
// similar class that owns all the data that it points to.
// This enables diagnosing certain lifetime issues similar to those enabled by
// LIBCOPP_UTIL_ATTRIBUTE_LIFETIME_BOUND, such as:
//
//   struct LIBCOPP_UTIL_ATTRIBUTE_VIEW StringView {
//     template<class R>
//     StringView(const R&);
//   };
//
//   struct LIBCOPP_UTIL_ATTRIBUTE_OWNER String {};
//
//   StringView f(String s) {
//     return s;  // warning: address of stack memory returned
//   }
//
//
// See the following links for details:
// https://reviews.llvm.org/D64448
// https://lists.llvm.org/pipermail/cfe-dev/2018-November/060355.html
#ifndef LIBCOPP_UTIL_ATTRIBUTE_OWNER
#  if LIBCOPP_UTIL_HAVE_CPP_ATTRIBUTE(gsl::Owner) && (!defined(__clang_major__) || __clang_major__ >= 13)
#    define LIBCOPP_UTIL_ATTRIBUTE_OWNER [[gsl::Owner]]
#  else
#    define LIBCOPP_UTIL_ATTRIBUTE_OWNER
#  endif
#endif

// LIBCOPP_UTIL_ATTRIBUTE_NO_UNIQUE_ADDRESS
//
// Indicates a data member can be optimized to occupy no space (if it is empty)
// and/or its tail padding can be used for other members.
//
// For code that is assured to only build with C++20 or later, prefer using
// the standard attribute `[[no_unique_address]]` directly instead of this
// macro.
//
// https://devblogs.microsoft.com/cppblog/msvc-cpp20-and-the-std-cpp20-switch/#c20-no_unique_address
// Current versions of MSVC have disabled `[[no_unique_address]]` since it
// breaks ABI compatibility, but offers `[[msvc::no_unique_address]]` for
// situations when it can be assured that it is desired. Since Abseil does not
// claim ABI compatibility in mixed builds, we can offer it unconditionally.
#ifndef LIBCOPP_UTIL_ATTRIBUTE_NO_UNIQUE_ADDRESS
#  if defined(_MSC_VER) && _MSC_VER >= 1929
#    define LIBCOPP_UTIL_ATTRIBUTE_NO_UNIQUE_ADDRESS [[msvc::no_unique_address]]
#  elif LIBCOPP_UTIL_HAVE_CPP_ATTRIBUTE(no_unique_address)
#    define LIBCOPP_UTIL_ATTRIBUTE_NO_UNIQUE_ADDRESS [[no_unique_address]]
#  else
#    define LIBCOPP_UTIL_ATTRIBUTE_NO_UNIQUE_ADDRESS
#  endif
#endif

// LIBCOPP_UTIL_ATTRIBUTE_UNINITIALIZED
//
// GCC and Clang support a flag `-ftrivial-auto-var-init=<option>` (<option>
// can be "zero" or "pattern") that can be used to initialize automatic stack
// variables. Variables with this attribute will be left uninitialized,
// overriding the compiler flag.
//
// See https://clang.llvm.org/docs/AttributeReference.html#uninitialized
// and https://gcc.gnu.org/onlinedocs/gcc/Common-Variable-Attributes.html#index-uninitialized-variable-attribute
#ifndef LIBCOPP_UTIL_ATTRIBUTE_UNINITIALIZED
#  if LIBCOPP_UTIL_HAVE_CPP_ATTRIBUTE(clang::uninitialized)
#    define LIBCOPP_UTIL_ATTRIBUTE_UNINITIALIZED [[clang::uninitialized]]
#  elif LIBCOPP_UTIL_HAVE_CPP_ATTRIBUTE(gnu::uninitialized)
#    define LIBCOPP_UTIL_ATTRIBUTE_UNINITIALIZED [[gnu::uninitialized]]
#  elif LIBCOPP_UTIL_HAVE_ATTRIBUTE(uninitialized)
#    define LIBCOPP_UTIL_ATTRIBUTE_UNINITIALIZED __attribute__((uninitialized))
#  else
#    define LIBCOPP_UTIL_ATTRIBUTE_UNINITIALIZED
#  endif
#endif

// LIBCOPP_UTIL_ATTRIBUTE_WARN_UNUSED
//
// Compilers routinely warn about trivial variables that are unused.  For
// non-trivial types, this warning is suppressed since the
// constructor/destructor may be intentional and load-bearing, for example, with
// a RAII scoped lock.
//
// For example:
//
// class LIBCOPP_UTIL_ATTRIBUTE_WARN_UNUSED MyType {
//  public:
//   MyType();
//   ~MyType();
// };
//
// void foo() {
//   // Warns with LIBCOPP_UTIL_ATTRIBUTE_WARN_UNUSED attribute present.
//   MyType unused;
// }
//
// See https://clang.llvm.org/docs/AttributeReference.html#warn-unused and
// https://gcc.gnu.org/onlinedocs/gcc/C_002b_002b-Attributes.html#index-warn_005funused-type-attribute
#ifndef LIBCOPP_UTIL_HAVE_CPP_ATTRIBUTE
#  if LIBCOPP_UTIL_HAVE_CPP_ATTRIBUTE(gnu::warn_unused)
#    define LIBCOPP_UTIL_ATTRIBUTE_WARN_UNUSED [[gnu::warn_unused]]
#  else
#    define LIBCOPP_UTIL_ATTRIBUTE_WARN_UNUSED
#  endif
#endif

// LIBCOPP_UTIL_HAVE_MEMORY_SANITIZER
//
// MemorySanitizer (MSan) is a detector of uninitialized reads. It consists of
// a compiler instrumentation module and a run-time library.
#ifndef LIBCOPP_UTIL_HAVE_MEMORY_SANITIZER
#  if !defined(__native_client__) && LIBCOPP_UTIL_HAVE_FEATURE(memory_sanitizer)
#    define LIBCOPP_UTIL_HAVE_MEMORY_SANITIZER 1
#  else
#    define LIBCOPP_UTIL_HAVE_MEMORY_SANITIZER 0
#  endif
#endif

#if LIBCOPP_UTIL_HAVE_MEMORY_SANITIZER && LIBCOPP_UTIL_HAVE_ATTRIBUTE(no_sanitize_memory)
#  define LIBCOPP_UTIL_SANITIZER_NO_MEMORY \
    __attribute__((no_sanitize_memory))  // __attribute__((no_sanitize("memory")))
#else
#  define LIBCOPP_UTIL_SANITIZER_NO_MEMORY
#endif

// LIBCOPP_UTIL_HAVE_THREAD_SANITIZER
//
// ThreadSanitizer (TSan) is a fast data race detector.
#ifndef LIBCOPP_UTIL_HAVE_THREAD_SANITIZER
#  if defined(__SANITIZE_THREAD__)
#    define LIBCOPP_UTIL_HAVE_THREAD_SANITIZER 1
#  elif LIBCOPP_UTIL_HAVE_FEATURE(thread_sanitizer)
#    define LIBCOPP_UTIL_HAVE_THREAD_SANITIZER 1
#  else
#    define LIBCOPP_UTIL_HAVE_THREAD_SANITIZER 0
#  endif
#endif

#if LIBCOPP_UTIL_HAVE_THREAD_SANITIZER && LIBCOPP_UTIL_HAVE_ATTRIBUTE(no_sanitize_thread)
#  define LIBCOPP_UTIL_SANITIZER_NO_THREAD \
    __attribute__((no_sanitize_thread))  // __attribute__((no_sanitize("thread")))
#else
#  define LIBCOPP_UTIL_SANITIZER_NO_THREAD
#endif

// LIBCOPP_UTIL_HAVE_ADDRESS_SANITIZER
//
// AddressSanitizer (ASan) is a fast memory error detector.
#ifndef LIBCOPP_UTIL_HAVE_ADDRESS_SANITIZER
#  if defined(__SANITIZE_ADDRESS__)
#    define LIBCOPP_UTIL_HAVE_ADDRESS_SANITIZER 1
#  elif LIBCOPP_UTIL_HAVE_FEATURE(address_sanitizer)
#    define LIBCOPP_UTIL_HAVE_ADDRESS_SANITIZER 1
#  else
#    define LIBCOPP_UTIL_HAVE_ADDRESS_SANITIZER 0
#  endif
#endif

// LIBCOPP_UTIL_HAVE_HWADDRESS_SANITIZER
//
// Hardware-Assisted AddressSanitizer (or HWASAN) is even faster than asan
// memory error detector which can use CPU features like ARM TBI, Intel LAM or
// AMD UAI.
#ifndef LIBCOPP_UTIL_HAVE_HWADDRESS_SANITIZER
#  if defined(__SANITIZE_HWADDRESS__)
#    define LIBCOPP_UTIL_HAVE_HWADDRESS_SANITIZER 1
#  elif LIBCOPP_UTIL_HAVE_FEATURE(hwaddress_sanitizer)
#    define LIBCOPP_UTIL_HAVE_HWADDRESS_SANITIZER 1
#  else
#    define LIBCOPP_UTIL_HAVE_HWADDRESS_SANITIZER 0
#  endif
#endif

#if LIBCOPP_UTIL_HAVE_ADDRESS_SANITIZER && LIBCOPP_UTIL_HAVE_ATTRIBUTE(no_sanitize_address)
#  define LIBCOPP_UTIL_SANITIZER_NO_ADDRESS \
    __attribute__((no_sanitize_address))  // __attribute__((no_sanitize("address")))
#elif LIBCOPP_UTIL_HAVE_ADDRESS_SANITIZER && defined(_MSC_VER) && _MSC_VER >= 1928
#  define LIBCOPP_UTIL_SANITIZER_NO_ADDRESS __declspec(no_sanitize_address)
#elif LIBCOPP_UTIL_HAVE_HWADDRESS_SANITIZER && LIBCOPP_UTIL_HAVE_ATTRIBUTE(no_sanitize)
#  define LIBCOPP_UTIL_SANITIZER_NO_ADDRESS __attribute__((no_sanitize("hwaddress")))
#else
#  define LIBCOPP_UTIL_SANITIZER_NO_ADDRESS
#endif

// LIBCOPP_UTIL_HAVE_DATAFLOW_SANITIZER
//
// Dataflow Sanitizer (or DFSAN) is a generalised dynamic data flow analysis.
#ifndef LIBCOPP_UTIL_HAVE_DATAFLOW_SANITIZER
#  if defined(DATAFLOW_SANITIZER)
// GCC provides no method for detecting the presence of the standalone
// DataFlowSanitizer (-fsanitize=dataflow), so GCC users of -fsanitize=dataflow
// should also use -DDATAFLOW_SANITIZER.
#    define LIBCOPP_UTIL_HAVE_DATAFLOW_SANITIZER 1
#  elif LIBCOPP_UTIL_HAVE_FEATURE(dataflow_sanitizer)
#    define LIBCOPP_UTIL_HAVE_DATAFLOW_SANITIZER 1
#  else
#    define LIBCOPP_UTIL_HAVE_DATAFLOW_SANITIZER 0
#  endif
#endif

// LIBCOPP_UTIL_HAVE_LEAK_SANITIZER
//
// LeakSanitizer (or lsan) is a detector of memory leaks.
// https://clang.llvm.org/docs/LeakSanitizer.html
// https://github.com/google/sanitizers/wiki/AddressSanitizerLeakSanitizer
//
// The macro LIBCOPP_UTIL_HAVE_LEAK_SANITIZER can be used to detect at compile-time
// whether the LeakSanitizer is potentially available. However, just because the
// LeakSanitizer is available does not mean it is active.
#ifndef LIBCOPP_UTIL_HAVE_LEAK_SANITIZER
#  if defined(LEAK_SANITIZER)
// GCC provides no method for detecting the presence of the standalone
// LeakSanitizer (-fsanitize=leak), so GCC users of -fsanitize=leak should also
// use -DLEAK_SANITIZER.
#    define LIBCOPP_UTIL_HAVE_LEAK_SANITIZER 1
// Clang standalone LeakSanitizer (-fsanitize=leak)
#  elif LIBCOPP_UTIL_HAVE_FEATURE(leak_sanitizer)
#    define LIBCOPP_UTIL_HAVE_LEAK_SANITIZER 1
#  elif defined(LIBCOPP_UTIL_HAVE_ADDRESS_SANITIZER)
// GCC or Clang using the LeakSanitizer integrated into AddressSanitizer.
#    define LIBCOPP_UTIL_HAVE_LEAK_SANITIZER 1
#  else
#    define LIBCOPP_UTIL_HAVE_LEAK_SANITIZER 0
#  endif
#endif

#ifndef LIBCOPP_UTIL_SANITIZER_NO_UNDEFINED
#  if LIBCOPP_UTIL_HAVE_ATTRIBUTE(no_sanitize_undefined)
#    define LIBCOPP_UTIL_SANITIZER_NO_UNDEFINED __attribute__((no_sanitize_undefined))
#  elif LIBCOPP_UTIL_HAVE_ATTRIBUTE(no_sanitize)
#    define LIBCOPP_UTIL_SANITIZER_NO_UNDEFINED __attribute__((no_sanitize("undefined")))
#  else
#    define LIBCOPP_UTIL_SANITIZER_NO_UNDEFINED
#  endif
#endif

#ifndef LIBCOPP_UTIL_MACRO_INLINE_VARIABLE
#  if (defined(__cplusplus) && __cplusplus >= 201703L) || (defined(_MSVC_LANG) && _MSVC_LANG >= 201703L)
#    define LIBCOPP_UTIL_MACRO_INLINE_VARIABLE inline
#  else
#    define LIBCOPP_UTIL_MACRO_INLINE_VARIABLE
#  endif
#endif

// LIBCOPP_UTIL_ATTRIBUTE_REINITIALIZES
//
// Indicates that a member function reinitializes the entire object to a known
// state, independent of the previous state of the object.
//
// The clang-tidy check bugprone-use-after-move allows member functions marked
// with this attribute to be called on objects that have been moved from;
// without the attribute, this would result in a use-after-move warning.
#ifndef LIBCOPP_UTIL_ATTRIBUTE_REINITIALIZES
#  if LIBCOPP_UTIL_HAVE_CPP_ATTRIBUTE(clang::reinitializes)
#    define LIBCOPP_UTIL_ATTRIBUTE_REINITIALIZES [[clang::reinitializes]]
#  else
#    define LIBCOPP_UTIL_ATTRIBUTE_REINITIALIZES
#  endif
#endif

// LIBCOPP_UTIL_ATTRIBUTE_RETURNS_NONNULL
//
// Tells the compiler that a particular function never returns a null pointer.
#if LIBCOPP_UTIL_HAVE_ATTRIBUTE(returns_nonnull) || \
    (defined(__GNUC__) && (__GNUC__ > 5 || (__GNUC__ == 4 && __GNUC_MINOR__ >= 9)) && !defined(__clang__))
#  define LIBCOPP_UTIL_ATTRIBUTE_RETURNS_NONNULL __attribute__((returns_nonnull))
#else
#  define LIBCOPP_UTIL_ATTRIBUTE_RETURNS_NONNULL
#endif

// Legacy macros
#ifndef LIBCOPP_UTIL_ATTRIBUTE_RETURNS_NONNULL
#  define LIBCOPP_UTIL_ATTRIBUTE_RETURNS_NONNULL LIBCOPP_UTIL_ATTRIBUTE_RETURNS_NONNULL
#endif

// Copyright 2025 owent

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

/**
 * @brief deprecated attribute
 * usage:
 *   LIBCOPP_EXPLICIT_DEPRECATED_ATTR int a;
 *   class LIBCOPP_EXPLICIT_DEPRECATED_ATTR a;
 *   LIBCOPP_EXPLICIT_DEPRECATED_ATTR int a();
 * usage:
 *   LIBCOPP_EXPLICIT_DEPRECATED_MSG("there is better choose") int a;
 *   class DEPRECATED_MSG("there is better choose") a;
 *   LIBCOPP_EXPLICIT_DEPRECATED_MSG("there is better choose") int a();
 */
#if defined(__cplusplus) && __cplusplus >= 201402L
#  define LIBCOPP_EXPLICIT_DEPRECATED_ATTR [[deprecated]]
#elif defined(__clang__)
#  define LIBCOPP_EXPLICIT_DEPRECATED_ATTR __attribute__((deprecated))
#elif defined(__GNUC__) && ((__GNUC__ >= 4) || ((__GNUC__ == 3) && (__GNUC_MINOR__ >= 1)))
#  define LIBCOPP_EXPLICIT_DEPRECATED_ATTR __attribute__((deprecated))
#elif defined(_MSC_VER) && _MSC_VER >= 1400  // vs 2005 or higher
#  if _MSC_VER >= 1910 && defined(_MSVC_LANG) && _MSVC_LANG >= 201703L
#    define LIBCOPP_EXPLICIT_DEPRECATED_ATTR [[deprecated]]
#  else
#    define LIBCOPP_EXPLICIT_DEPRECATED_ATTR __declspec(deprecated)
#  endif
#else
#  define LIBCOPP_EXPLICIT_DEPRECATED_ATTR
#endif

#if defined(__cplusplus) && __cplusplus >= 201402L
#  define LIBCOPP_EXPLICIT_DEPRECATED_MSG(msg) [[deprecated(msg)]]
#elif defined(__clang__)
#  define LIBCOPP_EXPLICIT_DEPRECATED_MSG(msg) __attribute__((deprecated(msg)))
#elif defined(__GNUC__) && ((__GNUC__ >= 4) || ((__GNUC__ == 3) && (__GNUC_MINOR__ >= 1)))
#  define LIBCOPP_EXPLICIT_DEPRECATED_MSG(msg) __attribute__((deprecated(msg)))
#elif defined(_MSC_VER) && _MSC_VER >= 1400  // vs 2005 or higher
#  if _MSC_VER >= 1910 && defined(_MSVC_LANG) && _MSVC_LANG >= 201703L
#    define LIBCOPP_EXPLICIT_DEPRECATED_MSG(msg) [[deprecated(msg)]]
#  else
#    define LIBCOPP_EXPLICIT_DEPRECATED_MSG(msg) __declspec(deprecated(msg))
#  endif
#else
#  define LIBCOPP_EXPLICIT_DEPRECATED_MSG(msg)
#endif

/**
 * @brief nodiscard attribute
 * usage:
 *   LIBCOPP_EXPLICIT_NODISCARD_ATTR int a;
 *   class LIBCOPP_EXPLICIT_NODISCARD_ATTR a;
 *   LIBCOPP_EXPLICIT_NODISCARD_ATTR int a();
 */
#if defined(__cplusplus) && __cplusplus >= 201703L
#  define LIBCOPP_EXPLICIT_NODISCARD_ATTR [[nodiscard]]
#elif defined(__clang__)
#  define LIBCOPP_EXPLICIT_NODISCARD_ATTR __attribute__((warn_unused_result))
#elif defined(__GNUC__) && ((__GNUC__ >= 4) || ((__GNUC__ == 3) && (__GNUC_MINOR__ >= 1)))
#  define LIBCOPP_EXPLICIT_NODISCARD_ATTR __attribute__((warn_unused_result))
#elif defined(_MSC_VER) && _MSC_VER >= 1700  // vs 2012 or higher
#  if _MSC_VER >= 1910 && defined(_MSVC_LANG) && _MSVC_LANG >= 201703L
#    define LIBCOPP_EXPLICIT_NODISCARD_ATTR [[nodiscard]]
#  else
#    define LIBCOPP_EXPLICIT_NODISCARD_ATTR _Check_return_
#  endif
#else
#  define LIBCOPP_EXPLICIT_NODISCARD_ATTR
#endif

/**
 * @brief maybe_unused attribute
 * usage:
 *   LIBCOPP_EXPLICIT_UNUSED_ATTR int a;
 *   class LIBCOPP_EXPLICIT_UNUSED_ATTR a;
 *   LIBCOPP_EXPLICIT_UNUSED_ATTR int a();
 */
#if defined(__cplusplus) && __cplusplus >= 201703L
#  define LIBCOPP_EXPLICIT_UNUSED_ATTR [[maybe_unused]]
#elif defined(__clang__)
#  define LIBCOPP_EXPLICIT_UNUSED_ATTR __attribute__((unused))
#elif defined(__GNUC__) && ((__GNUC__ >= 4) || ((__GNUC__ == 3) && (__GNUC_MINOR__ >= 1)))
#  define LIBCOPP_EXPLICIT_UNUSED_ATTR __attribute__((unused))
#elif defined(_MSC_VER) && _MSC_VER >= 1700  // vs 2012 or higher
#  if _MSC_VER >= 1910 && defined(_MSVC_LANG) && _MSVC_LANG >= 201703L
#    define LIBCOPP_EXPLICIT_UNUSED_ATTR [[maybe_unused]]
#  else
#    define LIBCOPP_EXPLICIT_UNUSED_ATTR
#  endif
#else
#  define LIBCOPP_EXPLICIT_UNUSED_ATTR
#endif

/**
 * @brief fallthrough, ignore fallthrough warning
 * usage:
 *   LIBCOPP_EXPLICIT_FALLTHROUGH int a;
 *   switch (xxx) {
 *      case XXX:
 *      LIBCOPP_EXPLICIT_FALLTHROUGH
 */
#if defined(__cplusplus) && __cplusplus >= 201703L
#  define LIBCOPP_EXPLICIT_FALLTHROUGH [[fallthrough]];
#elif defined(__clang__) && ((__clang_major__ * 100) + __clang_minor__) >= 309
#  if defined(__apple_build_version__)
#    define LIBCOPP_EXPLICIT_FALLTHROUGH
#  elif defined(__has_warning) && __has_feature(cxx_attributes) && __has_warning("-Wimplicit-fallthrough")
#    define LIBCOPP_EXPLICIT_FALLTHROUGH [[clang::fallthrough]];
#  else
#    define LIBCOPP_EXPLICIT_FALLTHROUGH
#  endif
#elif defined(__GNUC__) && (__GNUC__ >= 7)
#  define LIBCOPP_EXPLICIT_FALLTHROUGH [[gnu::fallthrough]];
#elif defined(_MSC_VER) && _MSC_VER >= 1700  // vs 2012 or higher
#  if _MSC_VER >= 1910 && defined(_MSVC_LANG) && _MSVC_LANG >= 201703L
#    define LIBCOPP_EXPLICIT_FALLTHROUGH [[fallthrough]];
#  else
#    define LIBCOPP_EXPLICIT_FALLTHROUGH
#  endif
#else
#  define LIBCOPP_EXPLICIT_FALLTHROUGH
#endif

/**
 * @brief may_alias attribute, allow to break strict-aliasing
 * usage:
 *   using target_type = LIBCOPP_EXPLICIT_MAY_ALIAS unsigned char[N];
 *   target_type a;
 */
#if defined(__clang__)
#  define LIBCOPP_EXPLICIT_MAY_ALIAS __attribute__((__may_alias__))
#elif defined(__GNUC__) && (__GNUC__ >= 4)
#  define LIBCOPP_EXPLICIT_MAY_ALIAS __attribute__((__may_alias__))
#else
#  define LIBCOPP_EXPLICIT_MAY_ALIAS
#endif

#if !defined(LIBCOPP_EXPLICIT_NORETURN_ATTR) && defined(__has_cpp_attribute)
#  if __has_cpp_attribute(noreturn)
#    define LIBCOPP_EXPLICIT_NORETURN_ATTR [[noreturn]]
#  endif
#endif
#ifndef LIBCOPP_EXPLICIT_NORETURN_ATTR
#  define LIBCOPP_EXPLICIT_NORETURN_ATTR
#endif

#ifndef LIBCOPP_EXPLICIT_UNREACHABLE
#  if defined(__cpp_lib_unreachable)
#    if __cpp_lib_unreachable
#      define LIBCOPP_EXPLICIT_UNREACHABLE() std::unreachable()
#    endif
#  endif
#  if !defined(LIBCOPP_EXPLICIT_UNREACHABLE) && defined(unreachable)
#    define LIBCOPP_EXPLICIT_UNREACHABLE() unreachable()
#  endif
#  if !defined(LIBCOPP_EXPLICIT_UNREACHABLE)
#    ifdef __GNUC__
#      ifdef __clang__
#        if __has_builtin(__builtin_unreachable)
#          define LIBCOPP_EXPLICIT_UNREACHABLE() __builtin_unreachable()
#        endif
#      else
#        if (__GNUC__ > 4 || (__GNUC__ == 4 && __GNUC_MINOR__ >= 6))
#          define LIBCOPP_EXPLICIT_UNREACHABLE() __builtin_unreachable()
#        endif
#      endif
#    endif
#  endif
#endif
#if !defined(LIBCOPP_EXPLICIT_UNREACHABLE)
#  if defined(_DEBUG) || !defined(NDEBUG)
#    define LIBCOPP_EXPLICIT_UNREACHABLE() std::abort()
#  else
#    define LIBCOPP_EXPLICIT_UNREACHABLE()
#  endif
#endif

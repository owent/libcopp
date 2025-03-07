// Copyright 2023 atframwork.
// Created by owent on 2022-05-23
// Usage:
//   // clang-format off
//   #include <libcopp/utils/config/stl_include_prefix.h>  // NOLINT(build/include_order)
//   // clang-format on
//   #include <sstream> //...
//   // clang-format off
//   #include <libcopp/utils/config/stl_include_suffix.h>  // NOLINT(build/include_order)
//   // clang-format on

// This file may be include multiple times, do not add #pragma once here
// NOLINT(build/header_guard)

#ifndef LIBCOPP_UTILS_CONFIG_STL_INCLUDE_PREFIX_H
#define LIBCOPP_UTILS_CONFIG_STL_INCLUDE_PREFIX_H

// ============= Patch for some Compilers's mistake =============
#if defined(_MSC_VER)
#  pragma warning(push)
#  pragma warning(disable : 4702)
#elif defined(__GNUC__) && !defined(__clang__) && !defined(__apple_build_version__)
#  if (__GNUC__ * 100 + __GNUC_MINOR__ * 10) >= 460
#    pragma GCC diagnostic push
#  endif

#  pragma GCC diagnostic ignored "-Warray-bounds"
#  if (__GNUC__ * 100 + __GNUC_MINOR__ * 10) >= 710
#    pragma GCC diagnostic ignored "-Wstringop-overflow"
#  endif

#  if (__GNUC__ * 100 + __GNUC_MINOR__) == 1402
#    pragma GCC diagnostic ignored "-Wuninitialized"
#    pragma GCC diagnostic ignored "-Wmaybe-uninitialized"
#  endif

#  if defined(__SANITIZE_THREAD__)
#    pragma GCC diagnostic ignored "-Wtsan"
#  elif defined(__has_feature)
#    if __has_feature(thread_sanitizer)
#      pragma GCC diagnostic ignored "-Wtsan"
#    endif
#  endif
#elif defined(__clang__) || defined(__apple_build_version__)
#  pragma clang diagnostic push
#  pragma GCC diagnostic ignored "-Warray-bounds"
#endif

#endif

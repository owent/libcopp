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

#if defined(__GNUC__) && !defined(_WIN32) && !defined(__CYGWIN__)
#  pragma GCC visibility push(default)
#endif

#endif

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

#ifdef LIBCOPP_UTILS_CONFIG_STL_INCLUDE_PREFIX_H

#  if defined(__GNUC__) && !defined(__clang__) && !defined(__apple_build_version__)
#    if (__GNUC__ * 100 + __GNUC_MINOR__ * 10) >= 460
#      pragma GCC diagnostic pop
#    endif
#  endif

#  undef LIBCOPP_UTILS_CONFIG_STL_INCLUDE_PREFIX_H

#endif

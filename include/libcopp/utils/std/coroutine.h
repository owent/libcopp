/**
 * @file type_traits.h
 * @brief type_traits compatiable
 * Licensed under the MIT licenses.
 *
 * @version 1.0
 * @author OWenT, owt5008137@live.com
 * @date 2020-04-26
 *
 * @history
 *
 */
#ifndef LIBCOPP_UTILS_STD_COROUTINE_H
#define LIBCOPP_UTILS_STD_COROUTINE_H

#pragma once

#include <libcopp/utils/config/libcopp_build_features.h>

// clang-format off
#include <libcopp/utils/config/stl_include_prefix.h>  // NOLINT(build/include_order)
// clang-format on
#if defined(LIBCOPP_MACRO_ENABLE_STD_COROUTINE) && LIBCOPP_MACRO_ENABLE_STD_COROUTINE
#  if defined(LIBCOPP_MACRO_USE_STD_EXPERIMENTAL_COROUTINE) && LIBCOPP_MACRO_USE_STD_EXPERIMENTAL_COROUTINE
#    include <experimental/coroutine>
#    define LIBCOPP_MACRO_FUTURE_COROUTINE_VOID std::experimental::coroutine_handle<>
#    define LIBCOPP_MACRO_FUTURE_COROUTINE_TYPE(T) std::experimental::coroutine_handle<T>
#  else
#    include <coroutine>
#    define LIBCOPP_MACRO_FUTURE_COROUTINE_VOID std::coroutine_handle<>
#    define LIBCOPP_MACRO_FUTURE_COROUTINE_TYPE(T) std::coroutine_handle<T>
#  endif
#endif
// clang-format off
#include <libcopp/utils/config/stl_include_suffix.h>  // NOLINT(build/include_order)
// clang-format on

#if defined(LIBCOPP_MACRO_USE_STD_EXPERIMENTAL_COROUTINE) && LIBCOPP_MACRO_USE_STD_EXPERIMENTAL_COROUTINE
#  define LIBCOPP_MACRO_STD_COROUTINE_NAMESPACE std::experimental::
#else
#  define LIBCOPP_MACRO_STD_COROUTINE_NAMESPACE std::
#endif

#endif
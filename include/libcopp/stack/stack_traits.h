// Copyright 2023 owent

#pragma once

#include <libcopp/utils/config/libcopp_build_features.h>

#include <libcopp/utils/config/libcopp_build_features.h>
#include <libcopp/utils/features.h>

// clang-format off
#include <libcopp/utils/config/stl_include_prefix.h>  // NOLINT(build/include_order)
// clang-format on
#include <cstddef>
// clang-format off
#include <libcopp/utils/config/stl_include_suffix.h>  // NOLINT(build/include_order)
// clang-format on

#ifdef COPP_HAS_ABI_HEADERS
#  include COPP_ABI_PREFIX
#endif

LIBCOPP_COPP_NAMESPACE_BEGIN
struct stack_traits {
  static LIBCOPP_COPP_API bool is_unbounded() LIBCOPP_MACRO_NOEXCEPT;

  static LIBCOPP_COPP_API std::size_t page_size() LIBCOPP_MACRO_NOEXCEPT;

  static LIBCOPP_COPP_API std::size_t default_size() LIBCOPP_MACRO_NOEXCEPT;

  static LIBCOPP_COPP_API std::size_t minimum_size() LIBCOPP_MACRO_NOEXCEPT;

  static LIBCOPP_COPP_API std::size_t maximum_size() LIBCOPP_MACRO_NOEXCEPT;

  static LIBCOPP_COPP_API std::size_t round_to_page_size(std::size_t stacksize) LIBCOPP_MACRO_NOEXCEPT;
};

LIBCOPP_COPP_NAMESPACE_END

#ifdef COPP_HAS_ABI_HEADERS
#  include COPP_ABI_SUFFIX
#endif

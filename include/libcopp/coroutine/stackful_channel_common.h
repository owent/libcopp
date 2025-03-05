// Copyright 2025 owent

#pragma once

#include <libcopp/utils/config/compile_optimize.h>
#include <libcopp/utils/config/libcopp_build_features.h>

#include <libcopp/utils/errno.h>

// clang-format off
#include <libcopp/utils/config/stl_include_prefix.h>  // NOLINT(build/include_order)
// clang-format on

#include <type_traits>

// clang-format off
#include <libcopp/utils/config/stl_include_suffix.h>  // NOLINT(build/include_order)
// clang-format on

LIBCOPP_COPP_NAMESPACE_BEGIN

class stackful_channel_context_base;

template <class TVALUE, bool STATIC_CAST, bool IS_POINTER>
struct LIBCOPP_COPP_API_HEAD_ONLY basic_stackful_channel_error_transform;

template <class TAWAITABLE>
struct LIBCOPP_COPP_API_HEAD_ONLY stackful_inject_awaitable;

template <class TVALUE>
struct LIBCOPP_COPP_API_HEAD_ONLY basic_stackful_channel_error_transform<TVALUE, true, false> {
  using value_type = TVALUE;

  LIBCOPP_UTIL_FORCEINLINE value_type operator()(copp_error_code err_code) const noexcept {
    return static_cast<value_type>(err_code);
  }
};

template <class TVALUE>
struct LIBCOPP_COPP_API_HEAD_ONLY basic_stackful_channel_error_transform<TVALUE, false, false> {
  using value_type = TVALUE;

  LIBCOPP_UTIL_FORCEINLINE value_type operator()(copp_error_code err_code) const
      noexcept(noexcept(std::is_nothrow_constructible<value_type, copp_error_code>::value)) {
    return value_type{err_code};
  }
};

template <class TVALUE, bool STATIC_CAST>
struct LIBCOPP_COPP_API_HEAD_ONLY basic_stackful_channel_error_transform<TVALUE, STATIC_CAST, true> {
  using value_type = TVALUE;

  LIBCOPP_UTIL_FORCEINLINE value_type operator()(copp_error_code /*err_code*/) const noexcept { return nullptr; }
};

template <class TVALUE>
struct LIBCOPP_COPP_API_HEAD_ONLY stackful_channel_error_transform
    : public basic_stackful_channel_error_transform<
          TVALUE, std::is_enum<TVALUE>::value || std::is_integral<TVALUE>::value, std::is_pointer<TVALUE>::value> {};

template <class TAWAITABLE>
struct LIBCOPP_COPP_API_HEAD_ONLY stackful_inject_awaitable : ::std::false_type {};

LIBCOPP_COPP_NAMESPACE_END

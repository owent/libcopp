// Copyright 2022 owent

#pragma once

#include <libcopp/utils/config/libcopp_build_features.h>

// clang-format off
#include <libcopp/utils/config/stl_include_prefix.h>  // NOLINT(build/include_order)
// clang-format on
#include <assert.h>
#include <type_traits>
#include <vector>
// clang-format off
#include <libcopp/utils/config/stl_include_suffix.h>  // NOLINT(build/include_order)
// clang-format on

#include "libcopp/coroutine/std_coroutine_common.h"

#if defined(LIBCOPP_MACRO_ENABLE_STD_COROUTINE) && LIBCOPP_MACRO_ENABLE_STD_COROUTINE

LIBCOPP_COPP_NAMESPACE_BEGIN

template <class TFUTURE>
struct some_ready {
  using type = std::vector<std::reference_wrapper<TFUTURE>>;
};

template <class TFUTURE>
struct any_ready {
  using type = typename some_ready<TFUTURE>::type;
};

template <class TFUTURE>
struct all_ready {
  using type = typename some_ready<TFUTURE>::type;
};

template <class TCONTAINER>
struct some_ready_container {
  using container_type = typename std::decay<TCONTAINER>::type;
  using value_type = typename std::decay<typename container_type::value_type>::type;
};

template <class TCONTAINER>
struct some_ready_reference_container {
  using reference_wrapper_type = typename some_ready_container<TCONTAINER>::value_type;
  using value_type = typename reference_wrapper_type::type;
};

template <class TELEMENT>
struct LIBCOPP_COPP_API_HEAD_ONLY pick_some_reference;

template <class TELEMENT>
struct LIBCOPP_COPP_API_HEAD_ONLY pick_some_reference<std::reference_wrapper<TELEMENT>> {
  inline static TELEMENT& unwrap(std::reference_wrapper<TELEMENT>& input) noexcept { return input.get(); }
  inline static TELEMENT& unwrap(const std::reference_wrapper<TELEMENT>& input) noexcept { return input.get(); }
};

template <class TELEMENT>
struct LIBCOPP_COPP_API_HEAD_ONLY pick_some_reference {
  inline static TELEMENT& unwrap(TELEMENT& input) noexcept { return input; }
};

LIBCOPP_COPP_NAMESPACE_END

#endif

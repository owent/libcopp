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
struct LIBCOPP_COPP_API_HEAD_ONLY some_ready {
  using reference_type = std::reference_wrapper<TFUTURE>;
  using type = std::vector<reference_type>;
};

template <class TFUTURE>
struct LIBCOPP_COPP_API_HEAD_ONLY any_ready {
  using reference_type = typename some_ready<TFUTURE>::reference_type;
  using type = typename some_ready<TFUTURE>::type;
};

template <class TFUTURE>
struct LIBCOPP_COPP_API_HEAD_ONLY all_ready {
  using reference_type = typename some_ready<TFUTURE>::reference_type;
  using type = typename some_ready<TFUTURE>::type;
};

template <class TELEMENT>
struct LIBCOPP_COPP_API_HEAD_ONLY remove_reference_wrapper;

template <class TELEMENT>
struct LIBCOPP_COPP_API_HEAD_ONLY remove_reference_wrapper<std::reference_wrapper<TELEMENT>> {
  using type = TELEMENT;
};

template <class TELEMENT>
struct LIBCOPP_COPP_API_HEAD_ONLY remove_reference_wrapper {
  using type = TELEMENT;
};

template <class TCONTAINER>
struct LIBCOPP_COPP_API_HEAD_ONLY some_ready_container {
  using container_type = typename std::decay<TCONTAINER>::type;
  using value_type = typename std::decay<typename container_type::value_type>::type;
};

template <class TCONTAINER>
struct LIBCOPP_COPP_API_HEAD_ONLY some_ready_reference_container {
  using reference_wrapper_type = typename some_ready_container<TCONTAINER>::value_type;
  using value_type = typename remove_reference_wrapper<typename reference_wrapper_type::type>::type;
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

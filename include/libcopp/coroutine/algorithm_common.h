// Copyright 2022 owent

#pragma once

#include <libcopp/utils/config/libcopp_build_features.h>

// clang-format off
#include <libcopp/utils/config/stl_include_prefix.h>  // NOLINT(build/include_order)
// clang-format on
#include <assert.h>
#include <list>
#include <type_traits>
#include <vector>
// clang-format off
#include <libcopp/utils/config/stl_include_suffix.h>  // NOLINT(build/include_order)
// clang-format on

#include "libcopp/coroutine/std_coroutine_common.h"
#include "libcopp/utils/gsl/not_null.h"
#include "libcopp/utils/gsl/span.h"

#if defined(LIBCOPP_MACRO_ENABLE_STD_COROUTINE) && LIBCOPP_MACRO_ENABLE_STD_COROUTINE

LIBCOPP_COPP_NAMESPACE_BEGIN

template <class TFUTURE>
struct LIBCOPP_COPP_API_HEAD_ONLY some_ready {
  using element_type = gsl::not_null<TFUTURE*>;
  using type = std::vector<element_type>;
};

template <class TFUTURE>
struct LIBCOPP_COPP_API_HEAD_ONLY any_ready {
  using element_type = typename some_ready<TFUTURE>::element_type;
  using type = typename some_ready<TFUTURE>::type;
};

template <class TFUTURE>
struct LIBCOPP_COPP_API_HEAD_ONLY all_ready {
  using element_type = typename some_ready<TFUTURE>::element_type;
  using type = typename some_ready<TFUTURE>::type;
};

template <class TELEMENT>
struct LIBCOPP_COPP_API_HEAD_ONLY some_ready_remove_wrapper;

template <class TELEMENT>
struct LIBCOPP_COPP_API_HEAD_ONLY some_ready_remove_wrapper<std::reference_wrapper<TELEMENT>> {
  using type = TELEMENT;
};

template <class TELEMENT>
struct LIBCOPP_COPP_API_HEAD_ONLY some_ready_remove_wrapper<gsl::not_null<TELEMENT*>> {
  using type = TELEMENT;
};

template <class TELEMENT>
struct LIBCOPP_COPP_API_HEAD_ONLY some_ready_remove_wrapper {
  using type = TELEMENT;
};

template <class TCONTAINER>
struct LIBCOPP_COPP_API_HEAD_ONLY some_ready_container {
  using container_type = typename std::decay<TCONTAINER>::type;
  using value_type =
      typename std::decay<typename some_ready_remove_wrapper<typename container_type::value_type>::type>::type;
  using reference_type = value_type&;
  using const_reference_type = const value_type&;
  using pointer_type = value_type*;
  using const_pointer_type = const value_type*;
};

template <class TELEMENT>
struct LIBCOPP_COPP_API_HEAD_ONLY pick_some_reference;

template <class TELEMENT>
struct LIBCOPP_COPP_API_HEAD_ONLY pick_some_reference<std::reference_wrapper<TELEMENT>> {
  inline static TELEMENT& unwrap(std::reference_wrapper<TELEMENT>& input) noexcept { return input.get(); }
  inline static const TELEMENT& unwrap(std::reference_wrapper<const TELEMENT>& input) noexcept { return input.get(); }
};

template <class TELEMENT>
struct LIBCOPP_COPP_API_HEAD_ONLY pick_some_reference<gsl::not_null<TELEMENT*>> {
  inline static TELEMENT& unwrap(gsl::not_null<TELEMENT*>& input) noexcept { return *input.get(); }
  inline static const TELEMENT& unwrap(gsl::not_null<const TELEMENT*>& input) noexcept { return *input.get(); }
};

template <class TELEMENT>
struct LIBCOPP_COPP_API_HEAD_ONLY pick_some_reference {
  inline static TELEMENT& unwrap(TELEMENT& input) noexcept { return input; }
  inline static const TELEMENT& unwrap(const TELEMENT& input) noexcept { return input; }
};

LIBCOPP_COPP_NAMESPACE_END

#endif

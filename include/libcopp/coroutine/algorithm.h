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

#include "libcopp/coroutine/callable_promise.h"

#if defined(LIBCOPP_MACRO_ENABLE_STD_COROUTINE) && LIBCOPP_MACRO_ENABLE_STD_COROUTINE

LIBCOPP_COPP_NAMESPACE_BEGIN

#  if defined(LIBCOPP_MACRO_ENABLE_CONCEPTS) && LIBCOPP_MACRO_ENABLE_CONCEPTS
template <class TREADY_CONTAINER, class TCONTAINER>
    LIBCOPP_COPP_API_HEAD_ONLY callable_future<promise_status> some(
        TREADY_CONTAINER&&ready_futures, size_t ready_count, TCONTAINER&&pending_futures) requires std::convertible_to <
    typename std::decay<TREADY_CONTAINER>::type,
typename some_ready<typename some_ready_container<TCONTAINER>::value_type>::type > {
#  else
template <class TREADY_CONTAINER, class TCONTAINER,
          class = typename std::enable_if<std::is_same<
              typename std::decay<TREADY_CONTAINER>::type,
              typename some_ready<typename some_ready_container<TCONTAINER>::value_type>::type>::value>::type>
LIBCOPP_COPP_API_HEAD_ONLY callable_future<promise_status> some(TREADY_CONTAINER&& ready_futures, size_t ready_count,
                                                                TCONTAINER&& pending_futures) {
#  endif
  return some_delegate<typename some_ready_container<TCONTAINER>::value_type>::run(
      std::forward<TREADY_CONTAINER>(ready_futures), ready_count, std::forward<TCONTAINER>(pending_futures));
}

template <class TREADY_CONTAINER>
LIBCOPP_COPP_API_HEAD_ONLY callable_future<promise_status> some(
    TREADY_CONTAINER&& ready_futures, size_t ready_count,
    std::initializer_list<typename some_ready_reference_container<TREADY_CONTAINER>::reference_wrapper_type>
        pending_futures) {
  return some_delegate<typename some_ready_reference_container<TREADY_CONTAINER>::value_type>::run(
      std::forward<TREADY_CONTAINER>(ready_futures), ready_count, std::move(pending_futures));
}

#  if defined(LIBCOPP_MACRO_ENABLE_CONCEPTS) && LIBCOPP_MACRO_ENABLE_CONCEPTS
template <class TREADY_CONTAINER, class TCONTAINER>
    LIBCOPP_COPP_API_HEAD_ONLY callable_future<promise_status> any(
        TREADY_CONTAINER&&ready_futures, TCONTAINER&&pending_futures) requires std::convertible_to <
    typename std::decay<TREADY_CONTAINER>::type,
typename any_ready<typename some_ready_container<TCONTAINER>::value_type>::type > {
#  else
template <class TREADY_CONTAINER, class TCONTAINER,
          class = typename std::enable_if<std::is_same<
              typename std::decay<TREADY_CONTAINER>::type,
              typename any_ready<typename some_ready_container<TCONTAINER>::value_type>::type>::value>::type>
LIBCOPP_COPP_API_HEAD_ONLY callable_future<promise_status> any(TREADY_CONTAINER&& ready_futures,
                                                               TCONTAINER&& pending_futures) {
#  endif
  return some_delegate<typename some_ready_container<TCONTAINER>::value_type>::run(
      std::forward<TREADY_CONTAINER>(ready_futures), 1, std::forward<TCONTAINER>(pending_futures));
}

template <class TREADY_CONTAINER>
LIBCOPP_COPP_API_HEAD_ONLY callable_future<promise_status> any(
    TREADY_CONTAINER&& ready_futures,
    std::initializer_list<typename some_ready_reference_container<TREADY_CONTAINER>::reference_wrapper_type>
        pending_futures) {
  return some_delegate<typename some_ready_reference_container<TREADY_CONTAINER>::value_type>::run(
      std::forward<TREADY_CONTAINER>(ready_futures), 1, std::move(pending_futures));
}

#  if defined(LIBCOPP_MACRO_ENABLE_CONCEPTS) && LIBCOPP_MACRO_ENABLE_CONCEPTS
template <class TREADY_CONTAINER, class TCONTAINER>
    LIBCOPP_COPP_API_HEAD_ONLY callable_future<promise_status> all(
        TREADY_CONTAINER&&ready_futures, TCONTAINER&&pending_futures) requires std::convertible_to <
    typename std::decay<TREADY_CONTAINER>::type,
typename all_ready<typename some_ready_container<TCONTAINER>::value_type>::type > {
#  else
template <class TREADY_CONTAINER, class TCONTAINER,
          class = typename std::enable_if<std::is_same<
              typename std::decay<TREADY_CONTAINER>::type,
              typename all_ready<typename some_ready_container<TCONTAINER>::value_type>::type>::value>::type>
LIBCOPP_COPP_API_HEAD_ONLY callable_future<promise_status> all(TREADY_CONTAINER&& ready_futures,
                                                               TCONTAINER&& pending_futures) {
#  endif
  return some_delegate<typename some_ready_container<TCONTAINER>::value_type>::run(
      std::forward<TREADY_CONTAINER>(ready_futures), pending_futures.size(), std::forward<TCONTAINER>(pending_futures));
}

template <class TREADY_CONTAINER>
LIBCOPP_COPP_API_HEAD_ONLY callable_future<promise_status> all(
    TREADY_CONTAINER&& ready_futures,
    std::initializer_list<typename some_ready_reference_container<TREADY_CONTAINER>::reference_wrapper_type>
        pending_futures) {
  return some_delegate<typename some_ready_reference_container<TREADY_CONTAINER>::value_type>::run(
      std::forward<TREADY_CONTAINER>(ready_futures), pending_futures.size(), std::move(pending_futures));
}

LIBCOPP_COPP_NAMESPACE_END

#endif

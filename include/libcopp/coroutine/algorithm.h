// Copyright 2022 owent

#pragma once

#include <libcopp/utils/config/libcopp_build_features.h>

#include <libcopp/utils/std/span.h>

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

template <class TREADY_CONTAINER>
LIBCOPP_COPP_API_HEAD_ONLY callable_future<promise_status> some(
    TREADY_CONTAINER&& ready_futures, size_t ready_count,
    nostd::span<typename some_ready_reference_container<TREADY_CONTAINER>::value_type> pending_futures) {
  return some_delegate<typename some_ready_reference_container<TREADY_CONTAINER>::value_type>::run(
      std::forward<TREADY_CONTAINER>(ready_futures), ready_count, pending_futures);
}

template <class TREADY_CONTAINER>
LIBCOPP_COPP_API_HEAD_ONLY callable_future<promise_status> some(
    TREADY_CONTAINER&& ready_futures, size_t ready_count,
    nostd::span<typename some_ready_reference_container<TREADY_CONTAINER>::reference_wrapper_type> pending_futures) {
  return some_delegate<typename some_ready_reference_container<TREADY_CONTAINER>::value_type>::run(
      std::forward<TREADY_CONTAINER>(ready_futures), ready_count, pending_futures);
}

template <class TREADY_CONTAINER>
LIBCOPP_COPP_API_HEAD_ONLY callable_future<promise_status> any(
    TREADY_CONTAINER&& ready_futures,
    nostd::span<typename some_ready_reference_container<TREADY_CONTAINER>::value_type> pending_futures) {
  return some_delegate<typename some_ready_reference_container<TREADY_CONTAINER>::value_type>::run(
      std::forward<TREADY_CONTAINER>(ready_futures), 1, pending_futures);
}

template <class TREADY_CONTAINER>
LIBCOPP_COPP_API_HEAD_ONLY callable_future<promise_status> any(
    TREADY_CONTAINER&& ready_futures,
    nostd::span<typename some_ready_reference_container<TREADY_CONTAINER>::reference_wrapper_type> pending_futures) {
  return some_delegate<typename some_ready_reference_container<TREADY_CONTAINER>::value_type>::run(
      std::forward<TREADY_CONTAINER>(ready_futures), 1, pending_futures);
}

template <class TREADY_CONTAINER>
LIBCOPP_COPP_API_HEAD_ONLY callable_future<promise_status> all(
    TREADY_CONTAINER&& ready_futures,
    nostd::span<typename some_ready_reference_container<TREADY_CONTAINER>::value_type> pending_futures) {
  return some_delegate<typename some_ready_reference_container<TREADY_CONTAINER>::value_type>::run(
      std::forward<TREADY_CONTAINER>(ready_futures), pending_futures.size(), pending_futures);
}

template <class TREADY_CONTAINER>
LIBCOPP_COPP_API_HEAD_ONLY callable_future<promise_status> all(
    TREADY_CONTAINER&& ready_futures,
    nostd::span<typename some_ready_reference_container<TREADY_CONTAINER>::reference_wrapper_type> pending_futures) {
  return some_delegate<typename some_ready_reference_container<TREADY_CONTAINER>::value_type>::run(
      std::forward<TREADY_CONTAINER>(ready_futures), pending_futures.size(), pending_futures);
}

LIBCOPP_COPP_NAMESPACE_END

#endif

// Copyright 2022 owent

#pragma once

#include <libcopp/utils/config/libcopp_build_features.h>

// clang-format off
#include <libcopp/utils/config/stl_include_prefix.h>  // NOLINT(build/include_order)
// clang-format on
#include <type_traits>
#if defined(LIBCOPP_MACRO_ENABLE_CONCEPTS) && LIBCOPP_MACRO_ENABLE_CONCEPTS
#  include <concepts>
#endif
// clang-format off
#include <libcopp/utils/config/stl_include_suffix.h>  // NOLINT(build/include_order)
// clang-format on

#include "libcopp/coroutine/callable_promise.h"
#include "libcopp/utils/gsl/span.h"

#if defined(LIBCOPP_MACRO_ENABLE_STD_COROUTINE) && LIBCOPP_MACRO_ENABLE_STD_COROUTINE

LIBCOPP_COPP_NAMESPACE_BEGIN

template <class TWAITING_CONTAINER>
struct LIBCOPP_COPP_API_HEAD_ONLY some_waiting_container_traits {
  using element_type = typename std::remove_pointer<decltype(gsl::data(std::declval<TWAITING_CONTAINER>()))>::type;
  using value_type = typename std::remove_reference<
      typename std::decay<decltype(pick_some_reference<typename std::remove_reference<element_type>::type>::unwrap(
          std::declval<typename std::add_lvalue_reference<element_type>::type>()))>::type>::type;
};

#  if defined(LIBCOPP_MACRO_ENABLE_CONCEPTS) && LIBCOPP_MACRO_ENABLE_CONCEPTS
template <class TREADY_ELEMENT, class TWAITING_ELEMENT>
concept SomeContainerConvertible =
    std::same_as<TWAITING_ELEMENT, TREADY_ELEMENT> || std::convertible_to<TWAITING_ELEMENT*, TREADY_ELEMENT*>;
#  else
template <class TREADY_ELEMENT, class TWAITING_ELEMENT>
struct LIBCOPP_COPP_API_HEAD_ONLY some_container_convertible
    : std::enable_if<std::is_same<TWAITING_ELEMENT, TREADY_ELEMENT>::value ||
                     std::is_convertible<TWAITING_ELEMENT*, TREADY_ELEMENT*>::value> {};
#  endif

template <class TREADY_CONTAINER, class TWAITING_CONTAINER
#  if !(defined(LIBCOPP_MACRO_ENABLE_CONCEPTS) && LIBCOPP_MACRO_ENABLE_CONCEPTS)
          ,
          class = typename some_container_convertible<
              typename some_ready_container<TREADY_CONTAINER>::value_type,
              typename some_waiting_container_traits<TWAITING_CONTAINER>::value_type>::type
#  endif
          >
    LIBCOPP_COPP_API_HEAD_ONLY inline callable_future<promise_status> some(TREADY_CONTAINER&&ready_futures,
                                                                           size_t ready_count,
                                                                           TWAITING_CONTAINER&&pending_futures)
#  if defined(LIBCOPP_MACRO_ENABLE_CONCEPTS) && LIBCOPP_MACRO_ENABLE_CONCEPTS
    // clang-format off
        requires SomeContainerConvertible<
          typename some_ready_container<TREADY_CONTAINER>::value_type,
          typename some_waiting_container_traits<TWAITING_CONTAINER>::value_type
        >
// clang-format on
#  endif
{
  return some_delegate<typename some_ready_container<TREADY_CONTAINER>::value_type>::template run(
      std::forward<TREADY_CONTAINER>(ready_futures), ready_count, &pending_futures);
}

template <class TREADY_CONTAINER, class TWAITING_CONTAINER
#  if !(defined(LIBCOPP_MACRO_ENABLE_CONCEPTS) && LIBCOPP_MACRO_ENABLE_CONCEPTS)
          ,
          class = typename some_container_convertible<
              typename some_ready_container<TREADY_CONTAINER>::value_type,
              typename some_waiting_container_traits<TWAITING_CONTAINER>::value_type>::type
#  endif
          >
    LIBCOPP_COPP_API_HEAD_ONLY inline callable_future<promise_status> any(TREADY_CONTAINER&&ready_futures,
                                                                          TWAITING_CONTAINER&&pending_futures)
#  if defined(LIBCOPP_MACRO_ENABLE_CONCEPTS) && LIBCOPP_MACRO_ENABLE_CONCEPTS
    // clang-format off
        requires SomeContainerConvertible<
          typename some_ready_container<TREADY_CONTAINER>::value_type,
          typename some_waiting_container_traits<TWAITING_CONTAINER>::value_type
        >
// clang-format on
#  endif
{
  return some_delegate<typename some_ready_container<TREADY_CONTAINER>::value_type>::template run(
      std::forward<TREADY_CONTAINER>(ready_futures), 1, &pending_futures);
}

template <class TREADY_CONTAINER, class TWAITING_CONTAINER
#  if !(defined(LIBCOPP_MACRO_ENABLE_CONCEPTS) && LIBCOPP_MACRO_ENABLE_CONCEPTS)
          ,
          class = typename some_container_convertible<
              typename some_ready_container<TREADY_CONTAINER>::value_type,
              typename some_waiting_container_traits<TWAITING_CONTAINER>::value_type>::type
#  endif
          >
    LIBCOPP_COPP_API_HEAD_ONLY inline callable_future<promise_status> all(TREADY_CONTAINER&&ready_futures,
                                                                          TWAITING_CONTAINER&&pending_futures)
#  if defined(LIBCOPP_MACRO_ENABLE_CONCEPTS) && LIBCOPP_MACRO_ENABLE_CONCEPTS
    // clang-format off
        requires SomeContainerConvertible<
          typename some_ready_container<TREADY_CONTAINER>::value_type,
          typename some_waiting_container_traits<TWAITING_CONTAINER>::value_type
        >
// clang-format on
#  endif
{
  return some_delegate<typename some_ready_container<TREADY_CONTAINER>::value_type>::template run(
      std::forward<TREADY_CONTAINER>(ready_futures), gsl::size(pending_futures), &pending_futures);
}

LIBCOPP_COPP_NAMESPACE_END

#endif

// Copyright 2022 owent

#pragma once

#include <libcopp/utils/config/libcopp_build_features.h>

#include <assert.h>

#if defined(LIBCOPP_MACRO_ENABLE_STD_EXCEPTION_PTR) && LIBCOPP_MACRO_ENABLE_STD_EXCEPTION_PTR
#  include <exception>
#endif

#include "libcopp/future/future.h"

#if defined(LIBCOPP_MACRO_ENABLE_STD_COROUTINE) && LIBCOPP_MACRO_ENABLE_STD_COROUTINE

LIBCOPP_COPP_NAMESPACE_BEGIN

template <class TDATA>
struct LIBCOPP_COTASK_API_HEAD_ONLY callable_data_trait {
  using type = TDATA;
};

template <class TDATATRAIT>
class LIBCOPP_COPP_API_HEAD_ONLY callable_data_future
    : public LIBCOPP_COPP_NAMESPACE_ID::future::future<typename TDATATRAIT::type> {
 public:
  using data_trait = TDATATRAIT;
  using self_type = callable_data_future<data_trait>;
  using base_type = LIBCOPP_COPP_NAMESPACE_ID::future::future<typename TDATATRAIT::type>;
  using poller_type = typename base_type::poller_type;
  using storage_type = typename base_type::storage_type;
  using value_type = typename base_type::value_type;
  using ptr_type = typename base_type::ptr_type;
};

template <class TDATATRAIT>
class LIBCOPP_COPP_API_HEAD_ONLY callable_context;

template <class TDATATRAIT>
class LIBCOPP_COPP_API_HEAD_ONLY callable_promise;

namespace details {
template <class TDATATRAIT, class... TARGS>
LIBCOPP_COPP_API_HEAD_ONLY std::shared_ptr<callable_context<TDATATRAIT>> make_callable_context(
    callable_promise<TDATATRAIT> &, TARGS &&...);
}  // namespace details

template <class TDATATRAIT>
class LIBCOPP_COPP_API_HEAD_ONLY callable_future;

LIBCOPP_COPP_NAMESPACE_END

#endif

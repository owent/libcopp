// Copyright 2022 owent

#pragma once

#include <libcopp/utils/config/libcopp_build_features.h>

#include <assert.h>

#if defined(__cpp_impl_three_way_comparison)
#  include <compare>
#endif

#include <libcopp/utils/uint64_id_allocator.h>

#if defined(LIBCOPP_MACRO_ENABLE_STD_EXCEPTION_PTR) && LIBCOPP_MACRO_ENABLE_STD_EXCEPTION_PTR
#  include <exception>
#endif

#include "libcopp/future/future.h"

#if defined(LIBCOPP_MACRO_ENABLE_STD_COROUTINE) && LIBCOPP_MACRO_ENABLE_STD_COROUTINE

LIBCOPP_COTASK_NAMESPACE_BEGIN

template <class TDATA>
struct LIBCOPP_COTASK_API_HEAD_ONLY awaitable_macros {
  using data_type = TDATA;
};

template <class TAWAITABLE>
class LIBCOPP_COPP_API_HEAD_ONLY task_data_future
    : public LIBCOPP_COPP_NAMESPACE_ID::future::future<typename TAWAITABLE::data_type> {
 public:
  using awaitable_type = TAWAITABLE;
  using self_type = task_data_future<awaitable_type>;
  using base_type = LIBCOPP_COPP_NAMESPACE_ID::future::future<typename TAWAITABLE::data_type>;
  using poller_type = typename base_type::poller_type;
  using storage_type = typename base_type::storage_type;
  using value_type = typename base_type::value_type;
  using ptr_type = typename base_type::ptr_type;
};

template <class TAWAITABLE>
class LIBCOPP_COPP_API_HEAD_ONLY task_context;

template <class TAWAITABLE>
class LIBCOPP_COPP_API_HEAD_ONLY task_promise;

namespace details {
template <class TAWAITABLE, class... TARGS>
LIBCOPP_COPP_API_HEAD_ONLY std::shared_ptr<task_context<TAWAITABLE>> make_task_context(task_promise<TAWAITABLE> &,
                                                                                       TARGS &&...);
}  // namespace details

template <class TAWAITABLE>
class LIBCOPP_COPP_API_HEAD_ONLY task_future;

template <class TAWAITABLE>
class LIBCOPP_COPP_API_HEAD_ONLY task_data_generator;

LIBCOPP_COTASK_NAMESPACE_END

#endif

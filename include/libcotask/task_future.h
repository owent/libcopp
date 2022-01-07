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
#include "libcotask/std_coroutine_common.h"

#if defined(LIBCOPP_MACRO_ENABLE_STD_COROUTINE) && LIBCOPP_MACRO_ENABLE_STD_COROUTINE

LIBCOPP_COTASK_NAMESPACE_BEGIN

template <class TAWAITABLE>
class LIBCOPP_COPP_API_HEAD_ONLY task_future {
 public:
  using awaitable_type = TAWAITABLE;
  using self_type = task_future<awaitable_type>;
  using base_type = LIBCOPP_COPP_NAMESPACE_ID::future::future<typename TAWAITABLE::data_type>;
  using poller_type = typename base_type::poller_type;
  using storage_type = typename base_type::storage_type;
  using value_type = typename base_type::value_type;
  using ptr_type = typename base_type::ptr_type;

  // ============ C++20 coroutine ============
  using promise_type = task_promise<awaitable_type>;

 private:
  std::shared_ptr<promise_type> promise_;
};

LIBCOPP_COTASK_NAMESPACE_END

#endif

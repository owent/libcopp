// Copyright 2022 owent

#pragma once

#include <libcopp/utils/config/libcopp_build_features.h>

#include <libcopp/coroutine/coroutine_context_container.h>
#include <libcopp/coroutine/coroutine_context_fiber_container.h>
#include <libcopp/utils/errno.h>

#include <libcotask/core/standard_new_allocator.h>
#include <libcotask/impl/task_impl.h>

#include <stdint.h>

LIBCOPP_COTASK_NAMESPACE_BEGIN
struct LIBCOPP_COTASK_API_HEAD_ONLY macro_coroutine {
  using stack_allocator_type = LIBCOPP_COPP_NAMESPACE_ID::allocator::default_statck_allocator;
  using coroutine_type = LIBCOPP_COPP_NAMESPACE_ID::coroutine_context_container<stack_allocator_type>;
  using value_type = int;
};

template <class T>
struct LIBCOPP_COPP_API_HEAD_ONLY task_data_ptr_selector {
  using type = typename LIBCOPP_COPP_NAMESPACE_ID::future::poll_storage_ptr_selector<T>::type;
};
LIBCOPP_COTASK_NAMESPACE_END

// Copyright 2022 owent

#pragma once

#include <libcopp/utils/config/libcopp_build_features.h>

#include <stdint.h>

#include <libcopp/coroutine/coroutine_context_container.h>
#include <libcopp/coroutine/coroutine_context_fiber_container.h>
#include <libcopp/utils/errno.h>

#include <libcotask/core/standard_new_allocator.h>
#include <libcotask/impl/task_impl.h>

LIBCOPP_COTASK_NAMESPACE_BEGIN
struct LIBCOPP_COTASK_API_HEAD_ONLY macro_coroutine {
  using stack_allocator_t = LIBCOPP_COPP_NAMESPACE_ID::allocator::default_statck_allocator;
  using coroutine_t = LIBCOPP_COPP_NAMESPACE_ID::coroutine_context_container<stack_allocator_t>;
};
LIBCOPP_COTASK_NAMESPACE_END

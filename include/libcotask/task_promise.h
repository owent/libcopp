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
class LIBCOPP_COPP_API_HEAD_ONLY task_promise {
 public:
  using awaitable_type = TAWAITABLE;
  using self_type = task_promise<awaitable_type>;
  using future_type = task_future<awaitable_type>;

 public:
  template <class... TARGS>
  task_promise(TARGS &&...args) : context_{details::make_task_context(*this, std::forward<TARGS>(args)...)} {}

  task_future<awaitable_type> get_return_object() noexcept;
  auto initial_suspend() noexcept { return std::suspend_never{}; }

  void unhandled_exception() noexcept {
#  if defined(LIBCOPP_MACRO_ENABLE_STD_EXCEPTION_PTR) && LIBCOPP_MACRO_ENABLE_STD_EXCEPTION_PTR
    if (likely(runtime_)) {
      runtime_->unhandle_exception = std::current_exception();
    }
#  endif
  }

 private:
  std::shared_ptr<task_context<awaitable_type>> context_;
};

namespace details {
template <class TAWAITABLE, class... TARGS>
LIBCOPP_COPP_API_HEAD_ONLY std::shared_ptr<task_context<TAWAITABLE>> make_task_context(
    task_promise<TAWAITABLE> &promise, TARGS &&...) {
  return std::make_shared<task_context<TAWAITABLE>>(LIBCOPP_COPP_NAMESPACE_ID::util::uint64_id_allocator::allocate(),
                                                    std::forward<TARGS>(args)...);
}
}  // namespace details

template <class TAWAITABLE>
task_future<TAWAITABLE> task_promise<TAWAITABLE>::get_return_object() noexcept {
  return task_future<awaitable_type>{context_};
}

LIBCOPP_COTASK_NAMESPACE_END

#endif

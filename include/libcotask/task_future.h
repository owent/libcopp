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
#include "libcotask/task_context.h"
#include "libcotask/task_promise.h"

#if defined(LIBCOPP_MACRO_ENABLE_STD_COROUTINE) && LIBCOPP_MACRO_ENABLE_STD_COROUTINE

LIBCOPP_COTASK_NAMESPACE_BEGIN

template <class TDATATRAIT>
class LIBCOPP_COPP_API_HEAD_ONLY task_future {
 public:
  using data_trait = TDATATRAIT;
  using self_type = task_future<data_trait>;
  using base_type = LIBCOPP_COPP_NAMESPACE_ID::future::future<typename TDATATRAIT::type>;
  using poller_type = typename base_type::poller_type;
  using storage_type = typename base_type::storage_type;
  using value_type = typename base_type::value_type;
  using ptr_type = typename base_type::ptr_type;

  // ============ C++20 coroutine ============
  using promise_type = task_promise<data_trait>;
  using context_type = task_context<data_trait>;

 public:
  task_future(std::shared_ptr<context_type> &&ctx) : context_{std::move(ctx)} {}
  struct awaitable {
    bool await_ready() {
      if (!context) {
        return true;
      }

      if (context->is_exiting()) {
        return true;
      }

      return context->is_ready();
    }

    T await_resume() {
      if (context) {
        context->maybe_rethrow();
        auto data = context->data().data();
        if (nullptr != data) {
          return std::move(*data);
        }
      }

      // TODO(owent): Maybe resume and timeout/cancle/killed by parent task
    }

    auto await_suspend(std::coroutine_handle<> h) {
      coro.promise().previous = h;
      return coro;
    }

    std::shared_ptr<context_type> context;
  };
  awaitable operator co_await() noexcept { return awaitable{context_}; }

 private:
  std::shared_ptr<context_type> context_;
};

namespace details {
template <class TDATATRAIT, class... TARGS>
LIBCOPP_COPP_API_HEAD_ONLY std::shared_ptr<task_context<TDATATRAIT>> make_task_context(
    task_promise<TDATATRAIT> &promise, TARGS &&...) {
  // TODO(owent): maybe reuse task id of current parent
  return std::make_shared<task_context<TDATATRAIT>>(
      promise, LIBCOPP_COPP_NAMESPACE_ID::util::uint64_id_allocator::allocate(), std::forward<TARGS>(args)...);
}
}  // namespace details

template <class TDATATRAIT>
task_future<TDATATRAIT> task_promise<TDATATRAIT>::get_return_object() noexcept {
  std::shared_ptr<task_context<data_trait>> context = details::make_task_context(*this, std::forward<TARGS>(args)...);
  context_ = context;
  return task_future<data_trait>{std::move(context)};
}

LIBCOPP_COTASK_NAMESPACE_END

#endif

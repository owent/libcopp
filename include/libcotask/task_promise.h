// Copyright 2022 owent

#pragma once

#include <libcopp/utils/config/libcopp_build_features.h>

#include <assert.h>
#include <type_traits>

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

template <class TDATATRAIT, bool RETURN_VOID>
class LIBCOPP_COPP_API_HEAD_ONLY task_promise_base;

template <class TDATATRAIT>
class LIBCOPP_COPP_API_HEAD_ONLY task_promise_base<TDATATRAIT, true> {
 public:
  using data_trait = TDATATRAIT;

  void return_void() {
    // TODO(owent): reset current
    if (context_) {
      context_->reset_data(true);
      context_->reset_current_handle(nullptr);
    }
  }

 protected:
  std::shared_ptr<task_context<data_trait>> context_;
};

template <class TDATATRAIT>
class LIBCOPP_COPP_API_HEAD_ONLY task_promise_base<TDATATRAIT, false> {
 public:
  using data_trait = TDATATRAIT;

  template <class TARG>
  void return_value(TARG &&value) {
    // TODO(owent): reset current
    if (context_) {
      context_->reset_data(std::forward<TARG>(value));
      context_->reset_current_handle(nullptr);
    }
  }

 protected:
  std::shared_ptr<task_context<data_trait>> context_;
};

template <class TDATATRAIT>
class LIBCOPP_COPP_API_HEAD_ONLY task_promise
    : public task_promise_base<TDATATRAIT,
                               std::is_same<typename std::decay<typename TDATATRAIT::type>::type, void>::value> {
 public:
  using data_trait = TDATATRAIT;
  using self_type = task_promise<data_trait>;
  using future_type = task_future<data_trait>;
  using handle_type = LIBCOPP_MACRO_STD_COROUTINE_NAMESPACE coroutine_handle<self_type>;
  using anonymous_handle_type = LIBCOPP_MACRO_STD_COROUTINE_NAMESPACE coroutine_handle<>;

 public:
  template <class... TARGS>
  task_promise(TARGS &&...args) {}

  std::shared_ptr<task_context<data_trait>> &get_context() noexcept { return context_; }
  const std::shared_ptr<task_context<data_trait>> &get_context() const noexcept { return context_; }

 public:
  // ============ C++ 20 coroutine ============
  task_future<data_trait> get_return_object() noexcept;

  // We need to call start/resume to start a coroutine and setup current TLS variables.
  auto initial_suspend() noexcept { return LIBCOPP_MACRO_STD_COROUTINE_NAMESPACE::suspend_always{}; }
  auto final_suspend() noexcept { return LIBCOPP_MACRO_STD_COROUTINE_NAMESPACE::suspend_never{}; }

  void unhandled_exception() noexcept {
#  if defined(LIBCOPP_MACRO_ENABLE_STD_EXCEPTION_PTR) && LIBCOPP_MACRO_ENABLE_STD_EXCEPTION_PTR
    if (likely(context_)) {
      context_->unhandle_exception_ = std::current_exception();
      return;
    }
    throw;
#  endif
  }
};

LIBCOPP_COTASK_NAMESPACE_END

#endif

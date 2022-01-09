// Copyright 2022 owent

#pragma once

#include <libcopp/utils/config/libcopp_build_features.h>

#include <assert.h>

#include <libcopp/utils/uint64_id_allocator.h>

#if defined(LIBCOPP_MACRO_ENABLE_STD_EXCEPTION_PTR) && LIBCOPP_MACRO_ENABLE_STD_EXCEPTION_PTR
#  include <exception>
#endif

#include "libcopp/future/future.h"
#include "libcopp/future/std_coroutine_common.h"

#if defined(LIBCOPP_MACRO_ENABLE_STD_COROUTINE) && LIBCOPP_MACRO_ENABLE_STD_COROUTINE

LIBCOPP_COPP_NAMESPACE_BEGIN

template <class TDATATRAIT>
class LIBCOPP_COPP_API_HEAD_ONLY callable_context {
 public:
  using data_trait = TDATATRAIT;
  using self_type = callable_context<data_trait>;
  using type = typename data_trait::type;
  using future_type = callable_future<data_trait>;
  using promise_type = callable_promise<data_trait>;
  using handle_type = LIBCOPP_MACRO_STD_COROUTINE_NAMESPACE coroutine_handle<promise_type>;
  using anonymous_handle_type = LIBCOPP_MACRO_STD_COROUTINE_NAMESPACE coroutine_handle<>;

 private:
  // context can not be copy or moved.
  callable_context(const callable_context &) = delete;
  callable_context &operator=(const callable_context &) = delete;
  callable_context(callable_context &&) = delete;
  callable_context &operator=(callable_context &&) = delete;

 public:
  template <class... TARGS>
  callable_context(promise_type &promise, TARGS &&...args)
      : data_(std::forward<TARGS>(args)...),
        current_handle_{handle_type::from_promise(promise)},
        previous_handle_{nullptr} {}
  ~callable_context() {
    // If a coroutine is not been start, current_handle_ will not be reset by return_value/return_void of promise_type
    // In this case, we need to destroy the suspended handle, or it will leak
    if (current_handle_) {
      current_handle_.destroy();
      reset_current_handle(nullptr);
    }
  }

  inline callable_data_future<data_trait> &data() noexcept { return data_; }
  inline const callable_data_future<data_trait> &data() const noexcept { return data_; }

#  if defined(LIBCOPP_MACRO_ENABLE_STD_EXCEPTION_PTR) && LIBCOPP_MACRO_ENABLE_STD_EXCEPTION_PTR
  inline void maybe_rethrow() {
    if (unlikely(unhandle_exception_)) {
      std::exception_ptr eptr;
      std::swap(eptr, unhandle_exception_);
      std::rethrow_exception(eptr);
    }
  }
#  endif
 private:
  template <class TDATATRAIT>
  friend class LIBCOPP_COPP_API_HEAD_ONLY callable_promise;

  void reset_current_handle(handle_type handle) { current_handle_ = handle; }
  void reset_previous_handle(anonymous_handle_type handle) { previous_handle_ = handle; }
  template <class TARG>
  void reset_data(TARG &&args) {
    data_.reset_data(std::forward<TARG>(args));
  }

 private:
  callable_data_future<data_trait> data_;
  anonymous_handle_type current_handle_;
  LIBCOPP_MACRO_STD_COROUTINE_NAMESPACE coroutine_handle<> previous_handle_;
#  if defined(LIBCOPP_MACRO_ENABLE_STD_EXCEPTION_PTR) && LIBCOPP_MACRO_ENABLE_STD_EXCEPTION_PTR
  std::exception_ptr unhandle_exception_;
#  endif
};

LIBCOPP_COPP_NAMESPACE_END

#endif

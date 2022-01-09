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

enum class LIBCOPP_COPP_API_HEAD_ONLY task_status {
  kCreated = 0,
  kRunning = 1,
  kDone = 2,
  kCancle = 3,
  kKilled = 4,
  kTimeout = 5,
};

template <class TDATATRAIT>
class LIBCOPP_COPP_API_HEAD_ONLY task_context {
 public:
  using data_trait = TDATATRAIT;
  using self_type = task_context<data_trait>;
  using type = typename data_trait::type;
  using future_type = task_future<data_trait>;
  using promise_type = task_promise<TDATATRAIT>;
  using handle_type = LIBCOPP_MACRO_STD_COROUTINE_NAMESPACE coroutine_handle<promise_type>;
  using anonymous_handle_type = LIBCOPP_MACRO_STD_COROUTINE_NAMESPACE coroutine_handle<>;

 private:
  // context can not be copy or moved.
  task_context(const task_context &) = delete;
  task_context &operator=(const task_context &) = delete;
  task_context(task_context &&) = delete;
  task_context &operator=(task_context &&) = delete;

 public:
  template <class... TARGS>
  task_context(promise_type &promise, uint64_t task_id, TARGS &&...args)
      : task_id_(task_id),
        status_(task_status::kCreated),
        data_(std::forward<TARGS>(args)...),
        current_handle_{handle_type::from_promise(promise)},
        previous_handle_{nullptr} {}
  ~task_context() {
    // If a coroutine is not been start, current_handle_ will not be reset by return_value/return_void of promise_type
    // In this case, we need to destroy the suspended handle, or it will leak
    if (current_handle_) {
      current_handle_.destroy();
      reset_current_handle(nullptr);
    }
  }

  inline task_data_future<data_trait> &data() noexcept { return data_; }
  inline const task_data_future<data_trait> &data() const noexcept { return data_; }
  inline bool is_exiting() const noexcept { return status_ >= task_status::kDone; }
  inline bool is_exited() const noexcept { return is_exiting() && (!current_handle_ || current_handle_.done()); }
  inline bool is_ready() const noexcept { return data_.is_ready(); }

  template <class TAWAITABLE_L, class TAWAITABLE_R>
  friend inline bool operator==(const task_context<TAWAITABLE_L> &l, const task_context<TAWAITABLE_R> &r) noexcept {
    return l.task_id_ == r.task_id_;
  }
#  if defined(__cpp_impl_three_way_comparison)
  template <class TAWAITABLE_L, class TAWAITABLE_R>
  friend inline std::strong_ordering operator<=>(const task_context<TAWAITABLE_L> &l,
                                                 const task_context<TAWAITABLE_R> &r) noexcept {
    return l.task_id_ <=> r.task_id_;
  }
#  else
  template <class TAWAITABLE_L, class TAWAITABLE_R>
  friend inline bool operator!=(const task_context<TAWAITABLE_L> &l, const task_context<TAWAITABLE_R> &r) noexcept {
    return l.task_id_ != r.task_id_;
  }
  template <class TAWAITABLE_L, class TAWAITABLE_R>
  friend inline bool operator<(const task_context<TAWAITABLE_L> &l, const task_context<TAWAITABLE_R> &r) noexcept {
    return l.task_id_ < r.task_id_;
  }
  template <class TAWAITABLE_L, class TAWAITABLE_R>
  friend inline bool operator<=(const task_context<TAWAITABLE_L> &l, const task_context<TAWAITABLE_R> &r) noexcept {
    return l.task_id_ <= r.task_id_;
  }
  template <class TAWAITABLE_L, class TAWAITABLE_R>
  friend inline bool operator>(const task_context<TAWAITABLE_L> &l, const task_context<TAWAITABLE_R> &r) noexcept {
    return l.task_id_ > r.task_id_;
  }
  template <class TAWAITABLE_L, class TAWAITABLE_R>
  friend inline bool operator>=(const task_context<TAWAITABLE_L> &l, const task_context<TAWAITABLE_R> &r) noexcept {
    return l.task_id_ >= r.task_id_;
  }
#  endif

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
  friend class LIBCOPP_COPP_API_HEAD_ONLY task_promise;
  template <class TDATATRAIT>
  friend class LIBCOPP_COPP_API_HEAD_ONLY task_data_generator;

  void reset_current_handle(handle_type handle) { current_handle_ = handle; }
  void reset_previous_handle(anonymous_handle_type handle) { previous_handle_ = handle; }
  template <class TARG>
  void reset_data(TARG &&args) {
    data_.reset_data(std::forward<TARG>(args));
  }

 private:
  uint64_t task_id_;
  task_status status_;
  task_data_future<data_trait> data_;
  anonymous_handle_type current_handle_;
  LIBCOPP_MACRO_STD_COROUTINE_NAMESPACE coroutine_handle<> previous_handle_;
#  if defined(LIBCOPP_MACRO_ENABLE_STD_EXCEPTION_PTR) && LIBCOPP_MACRO_ENABLE_STD_EXCEPTION_PTR
  std::exception_ptr unhandle_exception_;
#  endif
};

LIBCOPP_COTASK_NAMESPACE_END

#endif

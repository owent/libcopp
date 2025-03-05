// Copyright 2025 owent

#pragma once

#include <libcopp/utils/config/compile_optimize.h>
#include <libcopp/utils/config/libcopp_build_features.h>

#include <libcopp/utils/memory/default_smart_ptr_trait.h>

// clang-format off
#include <libcopp/utils/config/stl_include_prefix.h>  // NOLINT(build/include_order)
// clang-format on

#include <functional>
#include <type_traits>
#include <unordered_set>

#if defined(LIBCOPP_MACRO_ENABLE_STD_EXCEPTION_PTR) && LIBCOPP_MACRO_ENABLE_STD_EXCEPTION_PTR
#  include <exception>
#endif

#if defined(LIBCOPP_MACRO_ENABLE_STD_VARIANT) && LIBCOPP_MACRO_ENABLE_STD_VARIANT
#  include <variant>
#endif

#ifdef __cpp_impl_three_way_comparison
#  include <compare>
#endif

// clang-format off
#include <libcopp/utils/config/stl_include_suffix.h>  // NOLINT(build/include_order)
// clang-format on

#include "libcopp/coroutine/stackful_channel_common.h"
#include "libcopp/future/future.h"

LIBCOPP_COPP_NAMESPACE_BEGIN

class coroutine_context_base;
class coroutine_context;
class coroutine_context_fiber;

class stackful_channel_context_base;

template <class TCONTEXT>
struct stackful_channel_resume_invoker;

template <class TCONTEXT>
struct stackful_channel_resume_handle;

template <class TVALUE>
class LIBCOPP_COPP_API_HEAD_ONLY stackful_channel_context;

template <class TVALUE>
class LIBCOPP_COPP_API_HEAD_ONLY stackful_channel_receiver;

template <class TVALUE>
class LIBCOPP_COPP_API_HEAD_ONLY stackful_channel_sender;

template <>
struct stackful_channel_resume_handle<coroutine_context> {
  LIBCOPP_COPP_API static int resume(coroutine_context_base *invoke_ctx, stackful_channel_context_base *priv_data);
};

template <>
struct stackful_channel_resume_handle<coroutine_context_fiber> {
  LIBCOPP_COPP_API static int resume(coroutine_context_base *invoke_ctx, stackful_channel_context_base *priv_data);
};

template <class TCONTEXT>
struct stackful_channel_resume_invoker {
  LIBCOPP_UTIL_FORCEINLINE static int resume(coroutine_context_base *invoke_ctx,
                                             stackful_channel_context_base *priv_data) {
    if (nullptr != invoke_ctx) {
      return static_cast<TCONTEXT *>(invoke_ctx)->resume(reinterpret_cast<void *>(priv_data));
    }

    return 0;
  }
};

template <class TCONTEXT>
struct stackful_channel_resume_handle {
  LIBCOPP_COPP_API_HEAD_ONLY inline static int resume(coroutine_context_base *invoke_ctx,
                                                      stackful_channel_context_base *priv_data) {
    return stackful_channel_resume_invoker<TCONTEXT>::resume(invoke_ctx, priv_data);
  }
};

struct LIBCOPP_COPP_API_HEAD_ONLY stackful_channel_handle_delegate {
  coroutine_context_base *context = nullptr;
  int (*resume_handle)(coroutine_context_base *, stackful_channel_context_base *priv_data) = nullptr;

  LIBCOPP_UTIL_FORCEINLINE stackful_channel_handle_delegate() noexcept = default;
  LIBCOPP_UTIL_FORCEINLINE stackful_channel_handle_delegate(const stackful_channel_handle_delegate &) noexcept =
      default;
  LIBCOPP_UTIL_FORCEINLINE stackful_channel_handle_delegate(stackful_channel_handle_delegate &&) noexcept = default;
  LIBCOPP_UTIL_FORCEINLINE stackful_channel_handle_delegate &operator=(
      const stackful_channel_handle_delegate &) noexcept = default;
  LIBCOPP_UTIL_FORCEINLINE stackful_channel_handle_delegate &operator=(stackful_channel_handle_delegate &&) noexcept =
      default;

  template <class TCONTEXT>
  explicit inline stackful_channel_handle_delegate(TCONTEXT *ctx) noexcept : context{ctx}, resume_handle{nullptr} {
    if (context != nullptr) {
      resume_handle = stackful_channel_resume_handle<TCONTEXT>::resume;
    }
  }

  explicit LIBCOPP_UTIL_FORCEINLINE stackful_channel_handle_delegate(std::nullptr_t) noexcept
      : context{nullptr}, resume_handle{nullptr} {}

  friend LIBCOPP_UTIL_FORCEINLINE bool operator==(const stackful_channel_handle_delegate &l,
                                                  const stackful_channel_handle_delegate &r) noexcept {
    return l.context == r.context;
  }
#ifdef __cpp_impl_three_way_comparison
  friend LIBCOPP_UTIL_FORCEINLINE auto operator<=>(const stackful_channel_handle_delegate &l,
                                                   const stackful_channel_handle_delegate &r) noexcept {
    return l.context <=> r.context;
  }
#else
  friend LIBCOPP_UTIL_FORCEINLINE bool operator!=(const stackful_channel_handle_delegate &l,
                                                  const stackful_channel_handle_delegate &r) noexcept {
    return l.context != r.context;
  }
  friend LIBCOPP_UTIL_FORCEINLINE bool operator<(const stackful_channel_handle_delegate &l,
                                                 const stackful_channel_handle_delegate &r) noexcept {
    return l.context < r.context;
  }
  friend LIBCOPP_UTIL_FORCEINLINE bool operator<=(const stackful_channel_handle_delegate &l,
                                                  const stackful_channel_handle_delegate &r) noexcept {
    return l.context <= r.context;
  }
  friend LIBCOPP_UTIL_FORCEINLINE bool operator>(const stackful_channel_handle_delegate &l,
                                                 const stackful_channel_handle_delegate &r) noexcept {
    return l.context > r.context;
  }
  friend LIBCOPP_UTIL_FORCEINLINE bool operator>=(const stackful_channel_handle_delegate &l,
                                                  const stackful_channel_handle_delegate &r) noexcept {
    return l.context >= r.context;
  }
#endif
  LIBCOPP_UTIL_FORCEINLINE operator bool() const noexcept { return !!context; }

  template <class TCONTEXT>
  LIBCOPP_UTIL_FORCEINLINE stackful_channel_handle_delegate &operator=(TCONTEXT *ctx) noexcept {
    context = ctx;
    if (context != nullptr) {
      resume_handle = stackful_channel_resume_handle<TCONTEXT>::resume;
    }

    return *this;
  }

  LIBCOPP_UTIL_FORCEINLINE stackful_channel_handle_delegate &operator=(std::nullptr_t) noexcept {
    context = nullptr;
    resume_handle = nullptr;
    return *this;
  }
};

// hash for stackful_channel_handle_delegate
struct LIBCOPP_COPP_API_HEAD_ONLY stackful_channel_handle_delegate_hash {
  inline size_t operator()(const stackful_channel_handle_delegate &stackful_channel_handle_delegate) const noexcept {
    return std::hash<void *>()(stackful_channel_handle_delegate.context);
  }
};

class stackful_channel_context_base {
 public:
  using handle_delegate = stackful_channel_handle_delegate;
  using handle_delegate_hash = stackful_channel_handle_delegate_hash;

 private:
  stackful_channel_context_base(const stackful_channel_context_base &) = delete;
  stackful_channel_context_base(stackful_channel_context_base &&) = delete;
  stackful_channel_context_base &operator=(const stackful_channel_context_base &) = delete;
  stackful_channel_context_base &operator=(stackful_channel_context_base &&) = delete;

 protected:
  LIBCOPP_COPP_API stackful_channel_context_base() noexcept;
  LIBCOPP_COPP_API ~stackful_channel_context_base();

 public:
  LIBCOPP_COPP_API void add_caller(handle_delegate handle) noexcept;

  LIBCOPP_COPP_API bool remove_caller(handle_delegate handle) noexcept;

  LIBCOPP_COPP_API size_t resume_callers();

  LIBCOPP_COPP_API bool has_multiple_callers() const noexcept;

 protected:
  LIBCOPP_COPP_API void wake();

 private:
  using multi_caller_set = std::unordered_set<handle_delegate, handle_delegate_hash>;
#if defined(LIBCOPP_MACRO_ENABLE_STD_VARIANT) && LIBCOPP_MACRO_ENABLE_STD_VARIANT
  std::variant<handle_delegate, multi_caller_set> callers_;
#else
  handle_delegate unique_caller_;
  // Mostly, there is only one caller for a promise, we needn't hash map to store one handle
  std::unique_ptr<multi_caller_set> multiple_callers_;
#endif
};

template <class TVALUE>
class LIBCOPP_COPP_API_HEAD_ONLY stackful_channel_context : public stackful_channel_context_base {
 public:
  using value_type = TVALUE;
  using handle_delegate = stackful_channel_context_base::handle_delegate;
  using handle_delegate_hash = stackful_channel_context_base::handle_delegate_hash;

 public:
  LIBCOPP_UTIL_FORCEINLINE stackful_channel_context() = default;
  LIBCOPP_UTIL_FORCEINLINE ~stackful_channel_context() = default;

 public:
  LIBCOPP_UTIL_FORCEINLINE bool is_ready() const noexcept { return data_.is_ready(); }

  LIBCOPP_UTIL_FORCEINLINE bool is_pending() const noexcept { return data_.is_pending(); }

  LIBCOPP_UTIL_FORCEINLINE void reset_value() noexcept(noexcept(data_.reset_data())) { data_.reset_data(); }

  template <class U>
  LIBCOPP_UTIL_FORCEINLINE void set_value(U &&in) noexcept(noexcept(data_.reset_data(std::forward<U>(in)))) {
    data_.reset_data(std::forward<U>(in));

    // Wakeup all waiting coroutines
    resume_callers();
  }

  LIBCOPP_UTIL_FORCEINLINE const value_type *get_value() const noexcept { return data_.data(); }

  LIBCOPP_UTIL_FORCEINLINE value_type *get_value() noexcept { return data_.data(); }

  template <
      class TCONTEXT, class TERROR_TRANSFORM,
      class = nostd::enable_if_t<!std::is_base_of<coroutine_context, nostd::remove_cvref_t<TCONTEXT>>::value &&
                                 !std::is_base_of<coroutine_context_fiber, nostd::remove_cvref_t<TCONTEXT>>::value>>
  LIBCOPP_UTIL_FORCEINLINE value_type inject_await(TCONTEXT *ctx, TERROR_TRANSFORM &&error_transform) noexcept(
      std::is_nothrow_copy_constructible<value_type>::value && noexcept(error_transform(COPP_EC_ARGS_ERROR))) {
    return internal_inject_await<TCONTEXT>(ctx, std::forward<TERROR_TRANSFORM>(error_transform));
  }

  template <class TERROR_TRANSFORM>
  LIBCOPP_UTIL_FORCEINLINE value_type inject_await(coroutine_context *ctx, TERROR_TRANSFORM &&error_transform) noexcept(
      std::is_nothrow_copy_constructible<value_type>::value && noexcept(error_transform(COPP_EC_ARGS_ERROR))) {
    return internal_inject_await<coroutine_context>(ctx, std::forward<TERROR_TRANSFORM>(error_transform));
  }

  template <class TERROR_TRANSFORM>
  LIBCOPP_UTIL_FORCEINLINE value_type
  inject_await(coroutine_context_fiber *ctx,
               TERROR_TRANSFORM &&error_transform) noexcept(std::is_nothrow_copy_constructible<value_type>::value &&
                                                            noexcept(error_transform(COPP_EC_ARGS_ERROR))) {
    return internal_inject_await<coroutine_context_fiber>(ctx, std::forward<TERROR_TRANSFORM>(error_transform));
  }

 private:
  template <class TCONTEXT, class TERROR_TRANSFORM>
  inline value_type internal_inject_await(TCONTEXT *ctx, TERROR_TRANSFORM &&error_transform) noexcept(
      std::is_nothrow_copy_constructible<value_type>::value && noexcept(error_transform(COPP_EC_ARGS_ERROR))) {
    if (is_ready()) {
      return *get_value();
    }

    if LIBCOPP_UTIL_UNLIKELY_CONDITION (nullptr == ctx) {
      return error_transform(COPP_EC_ARGS_ERROR);
    }

    add_caller(handle_delegate{ctx});
    void *check_ptr = nullptr;
    int res = ctx->yield(&check_ptr);
    remove_caller(handle_delegate{ctx});

    if (COPP_EC_SUCCESS != res) {
      return error_transform(static_cast<copp_error_code>(res));
    }

    // This means the context is not resume by the sender of this context/receiver
    // Maybe the context is killed
    if (check_ptr != reinterpret_cast<void *>(this)) {
      return error_transform(COPP_EC_OPERATION_CANCLE);
    }

    if LIBCOPP_UTIL_LIKELY_CONDITION (is_ready()) {
      return *get_value();
    }

    return error_transform(COPP_EC_NOT_READY);
  }

 private:
  future::future<TVALUE> data_;
};

template <class TVALUE>
class LIBCOPP_COPP_API_HEAD_ONLY stackful_channel_receiver {
 public:
  using context_type = stackful_channel_context<TVALUE>;
  using context_pointer_type = LIBCOPP_COPP_NAMESPACE_ID::memory::default_strong_rc_ptr<context_type>;
  using value_type = typename context_type::value_type;

 public:
  inline stackful_channel_receiver() noexcept
      : context_{LIBCOPP_COPP_NAMESPACE_ID::memory::make_strong_rc<context_type>()} {}

  LIBCOPP_UTIL_FORCEINLINE bool is_ready() const noexcept {
    if LIBCOPP_UTIL_UNLIKELY_CONDITION (!context_) {
      return false;
    }
    return context_->is_ready();
  }

  LIBCOPP_UTIL_FORCEINLINE bool is_pending() const noexcept {
    if LIBCOPP_UTIL_UNLIKELY_CONDITION (!context_) {
      return false;
    }
    return context_->is_pending();
  }

  LIBCOPP_UTIL_FORCEINLINE void reset_value() noexcept(noexcept(context_->reset_data())) {
    if LIBCOPP_UTIL_UNLIKELY_CONDITION (!context_) {
      return;
    }

    context_->reset_data();
  }

  LIBCOPP_UTIL_FORCEINLINE const value_type *get_value() const noexcept {
    if LIBCOPP_UTIL_UNLIKELY_CONDITION (!context_) {
      return nullptr;
    }

    return context_->get_value();
  }

  LIBCOPP_UTIL_FORCEINLINE value_type *get_value() noexcept {
    if LIBCOPP_UTIL_UNLIKELY_CONDITION (!context_) {
      return nullptr;
    }

    return context_->get_value();
  }

  template <class TCONTEXT, class TERROR_TRANSFORM>
  LIBCOPP_UTIL_FORCEINLINE value_type inject_await(TCONTEXT *ctx, TERROR_TRANSFORM &&error_transform) noexcept(
      std::is_nothrow_copy_constructible<value_type>::value && noexcept(error_transform(COPP_EC_ARGS_ERROR))) {
    if LIBCOPP_UTIL_UNLIKELY_CONDITION (!context_) {
      return error_transform(COPP_EC_NOT_INITED);
    }

    return context_->inject_await(ctx, std::forward<TERROR_TRANSFORM>(error_transform));
  }

  LIBCOPP_UTIL_FORCEINLINE const context_pointer_type &get_context() const noexcept { return context_; }

  LIBCOPP_UTIL_FORCEINLINE context_pointer_type &get_context() noexcept { return context_; }

 private:
  context_pointer_type context_;
};

template <class TVALUE>
struct LIBCOPP_COPP_API_HEAD_ONLY stackful_inject_awaitable<stackful_channel_receiver<TVALUE>> : ::std::true_type {};

template <class TVALUE>
class LIBCOPP_COPP_API_HEAD_ONLY stackful_channel_sender {
 public:
  using context_type = stackful_channel_context<TVALUE>;
  using context_pointer_type = LIBCOPP_COPP_NAMESPACE_ID::memory::default_strong_rc_ptr<context_type>;
  using value_type = typename context_type::value_type;

 public:
  inline stackful_channel_sender(const context_pointer_type &context) noexcept : context_{context} {}

  LIBCOPP_UTIL_FORCEINLINE bool is_ready() const noexcept {
    if LIBCOPP_UTIL_UNLIKELY_CONDITION (!context_) {
      return false;
    }
    return context_->is_ready();
  }

  LIBCOPP_UTIL_FORCEINLINE bool is_pending() const noexcept {
    if LIBCOPP_UTIL_UNLIKELY_CONDITION (!context_) {
      return false;
    }
    return context_->is_pending();
  }

  LIBCOPP_UTIL_FORCEINLINE void reset_value() noexcept(noexcept(context_->reset_data())) {
    if LIBCOPP_UTIL_UNLIKELY_CONDITION (!context_) {
      return;
    }

    context_->reset_data();
  }

  template <class U>
  LIBCOPP_UTIL_FORCEINLINE void set_value(U &&in) noexcept(noexcept(context_->set_value(std::forward<U>(in)))) {
    if LIBCOPP_UTIL_UNLIKELY_CONDITION (!context_) {
      return;
    }

    context_->set_value(std::forward<U>(in));
  }

  LIBCOPP_UTIL_FORCEINLINE const context_pointer_type &get_context() const noexcept { return context_; }

  LIBCOPP_UTIL_FORCEINLINE context_pointer_type &get_context() noexcept { return context_; }

 private:
  context_pointer_type context_;
};

template <class TVALUE>
inline std::pair<stackful_channel_receiver<TVALUE>, stackful_channel_sender<TVALUE>> make_stackful_channel() {
  stackful_channel_receiver<TVALUE> receiver;
  stackful_channel_sender<TVALUE> sender{receiver.get_context()};
  return std::make_pair(std::move(receiver), std::move(sender));
}

LIBCOPP_COPP_NAMESPACE_END

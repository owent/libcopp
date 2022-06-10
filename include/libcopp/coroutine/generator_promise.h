// Copyright 2022 owent

#pragma once

#include <libcopp/utils/config/libcopp_build_features.h>

// clang-format off
#include <libcopp/utils/config/stl_include_prefix.h>  // NOLINT(build/include_order)
// clang-format on
#include <assert.h>
#include <functional>
#include <type_traits>

#if defined(LIBCOPP_MACRO_ENABLE_STD_EXCEPTION_PTR) && LIBCOPP_MACRO_ENABLE_STD_EXCEPTION_PTR
#  include <exception>
#endif
// clang-format off
#include <libcopp/utils/config/stl_include_suffix.h>  // NOLINT(build/include_order)
// clang-format on

#include "libcopp/coroutine/std_coroutine_common.h"
#include "libcopp/future/future.h"

#if defined(LIBCOPP_MACRO_ENABLE_STD_COROUTINE) && LIBCOPP_MACRO_ENABLE_STD_COROUTINE

LIBCOPP_COPP_NAMESPACE_BEGIN

template <class TVALUE>
class LIBCOPP_COPP_API_HEAD_ONLY generator_context_base;

template <class TVALUE, bool RETURN_VOID>
class LIBCOPP_COPP_API_HEAD_ONLY generator_context_delegate;

template <class TVALUE>
class LIBCOPP_COPP_API_HEAD_ONLY generator_context;

template <class TVALUE>
class LIBCOPP_COPP_API_HEAD_ONLY generator_future;

template <class TPROMISE, bool RETURN_VOID>
class LIBCOPP_COPP_API_HEAD_ONLY generator_awaitable;

template <class TVALUE>
class LIBCOPP_COPP_API_HEAD_ONLY generator_context_base {
 public:
  using value_type = TVALUE;
  using handle_delegate = promise_caller_manager::handle_delegate;

 private:
  generator_context_base(const generator_context_base&) = delete;
  generator_context_base(generator_context_base&&) = delete;
  generator_context_base& operator=(const generator_context_base&) = delete;
  generator_context_base& operator=(generator_context_base&&) = delete;

 protected:
  generator_context_base() = default;
  ~generator_context_base() { wake(); }

 public:
  UTIL_FORCEINLINE bool is_ready() const noexcept { return data_.is_ready(); }

  UTIL_FORCEINLINE bool is_pending() const noexcept { return data_.is_pending(); }

  UTIL_FORCEINLINE void reset_value() { data_.reset_data(); }

  UTIL_FORCEINLINE void add_caller(handle_delegate handle) noexcept { caller_manager_.add_caller(handle); }

#  if defined(LIBCOPP_MACRO_ENABLE_CONCEPTS) && LIBCOPP_MACRO_ENABLE_CONCEPTS
  template <DerivedPromiseBaseType TPROMISE>
#  else
  template <class TPROMISE, typename = std::enable_if_t<std::is_base_of<promise_base_type, TPROMISE>::value> >
#  endif
  UTIL_FORCEINLINE LIBCOPP_COPP_API_HEAD_ONLY void add_caller(
      const LIBCOPP_MACRO_STD_COROUTINE_NAMESPACE coroutine_handle<TPROMISE>& handle) noexcept {
    add_caller(handle_delegate{handle});
  }

  UTIL_FORCEINLINE void remove_caller(handle_delegate handle) noexcept { caller_manager_.remove_caller(handle); }

#  if defined(LIBCOPP_MACRO_ENABLE_CONCEPTS) && LIBCOPP_MACRO_ENABLE_CONCEPTS
  template <DerivedPromiseBaseType TPROMISE>
#  else
  template <class TPROMISE, typename = std::enable_if_t<std::is_base_of<promise_base_type, TPROMISE>::value> >
#  endif
  UTIL_FORCEINLINE LIBCOPP_COPP_API_HEAD_ONLY void remove_caller(
      const LIBCOPP_MACRO_STD_COROUTINE_NAMESPACE coroutine_handle<TPROMISE>& handle, bool inherit_status) noexcept {
    remove_caller(handle_delegate{handle}, inherit_status);
  }

  UTIL_FORCEINLINE bool has_multiple_callers() const noexcept { return caller_manager_.has_multiple_callers(); }

 protected:
  UTIL_FORCEINLINE void wake() { caller_manager_.resume_callers(); }

 protected:
  future::future<TVALUE> data_;
  // caller manager
  promise_caller_manager caller_manager_;
};

template <class TVALUE>
class LIBCOPP_COPP_API_HEAD_ONLY generator_context_delegate<TVALUE, true> : public generator_context_base<TVALUE> {
 public:
  using base_type = generator_context_base<TVALUE>;
  using value_type = typename base_type::value_type;

 public:
  template <class... TARGS>
  generator_context_delegate(TARGS&&... args) : base_type(std::forward<TARGS>(args)...) {}

  ~generator_context_delegate() { wake(); }

  using base_type::add_caller;
  using base_type::is_pending;
  using base_type::is_ready;
  using base_type::remove_caller;
  using base_type::reset_value;

  UTIL_FORCEINLINE void set_value() {
    data_.reset_data(true);
    wake();
  }

 private:
  using base_type::data_;
  using base_type::wake;
};

template <class TVALUE>
class LIBCOPP_COPP_API_HEAD_ONLY generator_context_delegate<TVALUE, false> : public generator_context_base<TVALUE> {
 public:
  using base_type = generator_context_base<TVALUE>;
  using value_type = typename base_type::value_type;

 public:
  template <class... TARGS>
  generator_context_delegate(TARGS&&... args) : base_type(std::forward<TARGS>(args)...) {}

  using base_type::add_caller;
  using base_type::is_pending;
  using base_type::is_ready;
  using base_type::remove_caller;
  using base_type::reset_value;

  ~generator_context_delegate() {
    if (is_pending()) {
      set_value(promise_error_transform<value_type>()(promise_status::kKilled));
    } else {
      wake();
    }
  }

  UTIL_FORCEINLINE const value_type* data() const noexcept {
    if (!is_ready()) {
      return nullptr;
    }

    return data_.data();
  }

  UTIL_FORCEINLINE value_type* data() noexcept {
    if (!is_ready()) {
      return nullptr;
    }

    return data_.data();
  }

  template <class U>
  UTIL_FORCEINLINE void set_value(U&& in) {
    data_.reset_data(std::forward<U>(in));
    wake();
  }

 private:
  using base_type::data_;
  using base_type::wake;
};

template <class TVALUE>
class LIBCOPP_COPP_API_HEAD_ONLY generator_context
    : public generator_context_delegate<TVALUE, std::is_void<typename std::decay<TVALUE>::type>::value>,
      public std::enable_shared_from_this<generator_context<TVALUE> > {
 public:
  using base_type = generator_context_delegate<TVALUE, std::is_void<typename std::decay<TVALUE>::type>::value>;
  using value_type = typename base_type::value_type;

 public:
  template <class... TARGS>
  generator_context(TARGS&&... args) : base_type(std::forward<TARGS>(args)...) {}

  using base_type::is_pending;
  using base_type::is_ready;
  using base_type::reset_value;
  using base_type::set_value;
};

template <class TCONTEXT>
class LIBCOPP_COPP_API_HEAD_ONLY generator_awaitable_base : public awaitable_base_type {
 public:
  using context_type = TCONTEXT;
  using context_pointer_type = std::shared_ptr<context_type>;
  using value_type = typename context_type::value_type;
  using await_suspend_callback_type = std::function<void(context_pointer_type)>;
  using await_resume_callback_type = std::function<void(const context_type&)>;

 public:
  generator_awaitable_base(context_type* context, const await_suspend_callback_type& await_suspend_callback,
                           const await_resume_callback_type& await_resume_callback)
      : context_{context},
        await_suspend_callback_(await_suspend_callback),
        await_resume_callback_(await_resume_callback) {}

  inline bool await_ready() noexcept {
    if (nullptr == context_) {
      return true;
    }

    return context_->is_ready();
  }

#  if defined(LIBCOPP_MACRO_ENABLE_CONCEPTS) && LIBCOPP_MACRO_ENABLE_CONCEPTS
  template <DerivedPromiseBaseType TCPROMISE>
#  else
  template <class TCPROMISE, typename = std::enable_if_t<std::is_base_of<promise_base_type, TCPROMISE>::value> >
#  endif
  inline void await_suspend(LIBCOPP_MACRO_STD_COROUTINE_NAMESPACE coroutine_handle<TCPROMISE> caller) noexcept {
    if (nullptr != context_ && caller.promise().get_status() < promise_status::kDone) {
      set_caller(caller);
      context_->add_caller(caller);

      // Allow kill resume to forward error information
      caller.promise().set_flag(promise_flag::kInternalWaitting, true);

      // Custom event. awaitable object may be deleted after this call
      if (await_suspend_callback_) {
        await_suspend_callback_(context_->shared_from_this());
      }
    } else {
      // Already done and can not suspend again
      caller.resume();
    }
  }

 protected:
  inline promise_status detach() noexcept {
    promise_status result_status;
    COPP_UNLIKELY_IF (nullptr == context_) {
      result_status = promise_status::kInvalid;
    } else if (context_->is_ready()) {
      result_status = promise_status::kDone;
    } else {
      result_status = promise_status::kKilled;
    }

    // caller maybe null if the callable is already ready when co_await
    auto caller = get_caller();

    if (caller) {
      if (nullptr != caller.promise) {
        caller.promise->set_flag(promise_flag::kInternalWaitting, false);
      }
      if (nullptr != context_) {
        if (!context_->is_ready() && nullptr != caller.promise) {
          result_status = caller.promise->get_status();
        }

        context_->remove_caller(caller);
        set_caller(nullptr);

        // Custom event
        if (await_resume_callback_) {
          await_resume_callback_(*context_);
        }
      } else {
        set_caller(nullptr);
      }
    }

    return result_status;
  }

  /**
   * @brief Get the context object
   * @note this can only be called in await_resume()
   *
   * @return context
   */
  inline context_type* get_context() noexcept { return context_; }

 private:
  context_type* context_;
  await_suspend_callback_type await_suspend_callback_;
  await_resume_callback_type await_resume_callback_;
};

template <class TCONTEXT>
class LIBCOPP_COPP_API_HEAD_ONLY generator_awaitable<TCONTEXT, true> : public generator_awaitable_base<TCONTEXT> {
 public:
  using base_type = generator_awaitable_base<TCONTEXT>;
  using value_type = typename base_type::value_type;
  using context_type = typename base_type::context_type;
  using context_pointer_type = typename base_type::context_pointer_type;
  using await_suspend_callback_type = typename base_type::await_suspend_callback_type;
  using await_resume_callback_type = typename base_type::await_resume_callback_type;

 public:
  using base_type::await_ready;
  using base_type::await_suspend;
  using base_type::get_caller;
  using base_type::set_caller;
  generator_awaitable(context_type* context, const await_suspend_callback_type& await_suspend_callback,
                      const await_resume_callback_type& await_resume_callback)
      : base_type(context, await_suspend_callback, await_resume_callback) {}

  inline void await_resume() { detach(); }

 private:
  using base_type::detach;
  using base_type::get_context;
};

template <class TCONTEXT>
class LIBCOPP_COPP_API_HEAD_ONLY generator_awaitable<TCONTEXT, false> : public generator_awaitable_base<TCONTEXT> {
 public:
  using base_type = generator_awaitable_base<TCONTEXT>;
  using value_type = typename base_type::value_type;
  using context_type = typename base_type::context_type;
  using context_pointer_type = typename base_type::context_pointer_type;
  using await_suspend_callback_type = typename base_type::await_suspend_callback_type;
  using await_resume_callback_type = typename base_type::await_resume_callback_type;

 public:
  using base_type::await_ready;
  using base_type::await_suspend;
  using base_type::get_caller;
  using base_type::set_caller;
  generator_awaitable(context_type* context, const await_suspend_callback_type& await_suspend_callback,
                      const await_resume_callback_type& await_resume_callback)
      : base_type(context, await_suspend_callback, await_resume_callback) {}

  inline value_type await_resume() {
    bool has_multiple_callers;
    COPP_LIKELY_IF (nullptr != get_context()) {
      has_multiple_callers = get_context()->has_multiple_callers();
    } else {
      has_multiple_callers = false;
    }
    promise_status result_status = detach();

    if (promise_status::kDone != result_status) {
      return promise_error_transform<value_type>()(result_status);
    }

    COPP_LIKELY_IF (nullptr != get_context()) {
      if (has_multiple_callers) {
        return *get_context()->data();
      } else {
        return std::move(*get_context()->data());
      }
    } else {
      return promise_error_transform<value_type>()(promise_status::kInvalid);
    }
  }

 private:
  using base_type::detach;
  using base_type::get_context;
};

template <class TVALUE>
class LIBCOPP_COPP_API_HEAD_ONLY generator_future {
 public:
  using value_type = TVALUE;
  using self_type = generator_future<value_type>;
  using context_type = generator_context<value_type>;
  using context_pointer_type = std::shared_ptr<context_type>;
  using awaitable_type = generator_awaitable<context_type, std::is_void<typename std::decay<value_type>::type>::value>;
  using await_suspend_callback_type = typename awaitable_type::await_suspend_callback_type;
  using await_resume_callback_type = typename awaitable_type::await_resume_callback_type;

 public:
  template <class TSUSPEND, class TRESUME>
  generator_future(TSUSPEND&& await_suspend_callback, TRESUME&& await_resume_callback)
      : context_(std::make_shared<context_type>()),
        await_suspend_callback_(std::forward<TSUSPEND>(await_suspend_callback)),
        await_resume_callback_(std::forward<TRESUME>(await_resume_callback)) {}

  template <class TSUSPEND>
  generator_future(TSUSPEND&& await_suspend_callback)
      : context_(std::make_shared<context_type>()),
        await_suspend_callback_(std::forward<TSUSPEND>(await_suspend_callback)) {}

  generator_future(generator_future&&) = default;
  generator_future(const generator_future&) = default;
  generator_future& operator=(generator_future&&) = default;
  generator_future& operator=(const generator_future&) = default;

  ~generator_future() {
    if (context_) {
      context_.reset();
    }
  }

  awaitable_type operator co_await() {
    return awaitable_type{context_.get(), await_suspend_callback_, await_resume_callback_};
  }

  inline bool is_ready() const noexcept {
    if (!context_) {
      return false;
    }

    return context_->is_ready();
  }

  inline bool is_pending() const noexcept {
    if (!context_) {
      return false;
    }

    return context_->is_pending();
  }

  inline promise_status get_status() const noexcept {
    if (!context_) {
      return promise_status::kInvalid;
    }

    if (context_->is_ready()) {
      return promise_status::kDone;
    }

    return promise_status::kRunning;
  }

  UTIL_FORCEINLINE const std::shared_ptr<context_type>& get_context() const noexcept { return context_; }

  UTIL_FORCEINLINE std::shared_ptr<context_type>& get_context() noexcept { return context_; }

 private:
  std::shared_ptr<context_type> context_;
  await_suspend_callback_type await_suspend_callback_;
  await_resume_callback_type await_resume_callback_;
};

LIBCOPP_COPP_NAMESPACE_END

#endif

// Copyright 2023 owent

#pragma once

#include <libcopp/utils/config/libcopp_build_features.h>

// clang-format off
#include <libcopp/utils/config/stl_include_prefix.h>  // NOLINT(build/include_order)
// clang-format on
#include <cstdlib>
#include <type_traits>
// clang-format off
#include <libcopp/utils/config/stl_include_suffix.h>  // NOLINT(build/include_order)
// clang-format on

#if defined(LIBCOPP_MACRO_ENABLE_STD_EXCEPTION_PTR) && LIBCOPP_MACRO_ENABLE_STD_EXCEPTION_PTR
#  include <exception>
#endif

#include "libcopp/coroutine/algorithm_common.h"
#include "libcopp/coroutine/std_coroutine_common.h"
#include "libcopp/future/future.h"
#include "libcopp/utils/uint64_id_allocator.h"

#if defined(LIBCOPP_MACRO_ENABLE_STD_COROUTINE) && LIBCOPP_MACRO_ENABLE_STD_COROUTINE

LIBCOPP_COPP_NAMESPACE_BEGIN

template <class TFUTURE>
class LIBCOPP_COPP_API_HEAD_ONLY some_delegate;

template <class TVALUE, class TERROR_TRANSFORM>
class LIBCOPP_COPP_API_HEAD_ONLY callable_future;

template <class TVALUE, bool RETURN_VOID>
class LIBCOPP_COPP_API_HEAD_ONLY callable_promise_base;

template <class TPROMISE, class TERROR_TRANSFORM, bool RETURN_VOID>
class LIBCOPP_COPP_API_HEAD_ONLY callable_awaitable;

template <class TVALUE>
class LIBCOPP_COPP_API_HEAD_ONLY callable_promise_base<TVALUE, true> : public promise_base_type {
 public:
  using value_type = TVALUE;

  template <class... TARGS>
  callable_promise_base(TARGS&&...) {}

  callable_promise_base() = default;

  void return_void() noexcept {
    set_flag(promise_flag::kHasReturned, true);
    if (get_status() < promise_status::kDone) {
      set_status(promise_status::kDone);
    }
  }
};

template <class TVALUE, bool IS_DEFAULT_CONSTRUCTIBLE>
class LIBCOPP_COPP_API_HEAD_ONLY callable_promise_value_constructor;

template <class TVALUE>
class LIBCOPP_COPP_API_HEAD_ONLY callable_promise_value_constructor<TVALUE, true> {
 public:
  template <class... TARGS>
  inline static TVALUE construct(TARGS&&...) {
    return TVALUE{};
  }
};

template <class TVALUE>
class LIBCOPP_COPP_API_HEAD_ONLY callable_promise_value_constructor<TVALUE, false> {
 public:
  template <class... TARGS>
  inline static TVALUE construct(TARGS&&... args) {
    return TVALUE{std::forward<TARGS>(args)...};
  }
};

template <class TVALUE>
class LIBCOPP_COPP_API_HEAD_ONLY callable_promise_base<TVALUE, false> : public promise_base_type {
 public:
  using value_type = TVALUE;

  template <class... TARGS>
  callable_promise_base(TARGS&&... args)
      : data_(callable_promise_value_constructor<value_type, !std::is_constructible<value_type, TARGS...>::value>::
                  construct(std::forward<TARGS>(args)...)) {}

  void return_value(value_type value) {
    set_flag(promise_flag::kHasReturned, true);
    if (get_status() < promise_status::kDone) {
      set_status(promise_status::kDone);
    }
    data_ = std::move(value);
  }

  UTIL_FORCEINLINE value_type& data() noexcept { return data_; }
  UTIL_FORCEINLINE const value_type& data() const noexcept { return data_; }

 protected:
  value_type data_;
};

#  if defined(LIBCOPP_MACRO_ENABLE_CONCEPTS) && LIBCOPP_MACRO_ENABLE_CONCEPTS
template <DerivedPromiseBaseType TPROMISE>
#  else
template <class TPROMISE, typename = std::enable_if_t<std::is_base_of<promise_base_type, TPROMISE>::value>>
#  endif
class LIBCOPP_COPP_API_HEAD_ONLY callable_awaitable_base : public awaitable_base_type {
 public:
  using promise_type = TPROMISE;
  using value_type = typename promise_type::value_type;
  using handle_type = LIBCOPP_MACRO_STD_COROUTINE_NAMESPACE coroutine_handle<promise_type>;

 public:
  callable_awaitable_base(handle_type handle) : callee_{handle} {}

  UTIL_FORCEINLINE bool await_ready() noexcept {
    if (!callee_) {
      return true;
    }

    // callee_ should not be destroyed, this only works with callable_future<T>::promise_type
    if (callee_.done()) {
      return true;
    }

    if (callee_.promise().get_status() >= promise_status::kDone) {
      return true;
    }

    if (callee_.promise().check_flag(promise_flag::kHasReturned)) {
      return true;
    }

    return callee_.done();
  }

#  if defined(LIBCOPP_MACRO_ENABLE_CONCEPTS) && LIBCOPP_MACRO_ENABLE_CONCEPTS
  template <DerivedPromiseBaseType TCPROMISE>
#  else
  template <class TCPROMISE, typename = std::enable_if_t<std::is_base_of<promise_base_type, TCPROMISE>::value>>
#  endif
  inline bool await_suspend(LIBCOPP_MACRO_STD_COROUTINE_NAMESPACE coroutine_handle<TCPROMISE> caller) noexcept {
    if (caller.promise().get_status() < promise_status::kDone) {
      set_caller(caller);
      caller.promise().set_waiting_handle(callee_);
      callee_.promise().add_caller(caller);

      caller.promise().set_flag(promise_flag::kInternalWaitting, true);
      return true;
    } else {
      // Already done and can not suspend again
      auto& callee_promise = get_callee().promise();
      // If callee is killed when running, we need inherit status from caller
      if (callee_promise.get_status() < promise_status::kDone &&
          callee_promise.check_flag(promise_flag::kInternalWaitting)) {
        callee_promise.set_status(caller.promise().get_status());
        callee_.resume();
      }
      // caller.resume();
      return false;
    }
  }

  UTIL_FORCEINLINE handle_type& get_callee() noexcept { return callee_; }
  UTIL_FORCEINLINE const handle_type& get_callee() const noexcept { return callee_; }

 protected:
  void detach() noexcept {
    // caller maybe null if the callable is already ready when co_await
    auto caller = get_caller();
    auto& callee_promise = get_callee().promise();

    if (caller) {
      if (nullptr != caller.promise) {
        caller.promise->set_flag(promise_flag::kInternalWaitting, false);
        caller.promise->set_waiting_handle(nullptr);
      }
      callee_promise.remove_caller(caller, true);
      set_caller(nullptr);
    }

    if (callee_promise.get_status() < promise_status::kDone) {
      if (!caller) {
        callee_promise.set_status(promise_status::kKilled);
      } else {
        callee_promise.set_status(caller.promise->get_status());
      }
    }
  }

 private:
  handle_type callee_;
};

template <class TPROMISE, class TERROR_TRANSFORM>
class LIBCOPP_COPP_API_HEAD_ONLY callable_awaitable<TPROMISE, TERROR_TRANSFORM, true>
    : public callable_awaitable_base<TPROMISE> {
 public:
  using base_type = callable_awaitable_base<TPROMISE>;
  using promise_type = typename base_type::promise_type;
  using value_type = typename base_type::value_type;
  using handle_type = typename base_type::handle_type;

 public:
  using base_type::await_ready;
  using base_type::await_suspend;
  using base_type::detach;
  using base_type::get_callee;
  using base_type::get_caller;
  using base_type::set_caller;
  callable_awaitable(handle_type handle) : base_type(handle) {}

  UTIL_FORCEINLINE void await_resume() {
    detach();
    get_callee().promise().resume_waiting(get_callee(), true);
  }
};

template <class TPROMISE, class TERROR_TRANSFORM>
class LIBCOPP_COPP_API_HEAD_ONLY callable_awaitable<TPROMISE, TERROR_TRANSFORM, false>
    : public callable_awaitable_base<TPROMISE> {
 public:
  using base_type = callable_awaitable_base<TPROMISE>;
  using promise_type = typename base_type::promise_type;
  using value_type = typename base_type::value_type;
  using handle_type = typename base_type::handle_type;

 public:
  using base_type::await_ready;
  using base_type::await_suspend;
  using base_type::detach;
  using base_type::get_callee;
  using base_type::get_caller;
  using base_type::set_caller;
  callable_awaitable(handle_type handle) : base_type(handle) {}

  inline value_type await_resume() {
    detach();
    auto& callee_promise = get_callee().promise();
    callee_promise.resume_waiting(get_callee(), true);

    if (!callee_promise.check_flag(promise_flag::kHasReturned)) {
      return TERROR_TRANSFORM()(callee_promise.get_status());
    }

    return std::move(callee_promise.data());
  }
};

template <class TVALUE, class TERROR_TRANSFORM = promise_error_transform<TVALUE>>
class LIBCOPP_COPP_API_HEAD_ONLY callable_future {
 public:
  using value_type = TVALUE;
  using error_transform = TERROR_TRANSFORM;
  using self_type = callable_future<value_type, error_transform>;
  class promise_type
      : public callable_promise_base<value_type, std::is_void<typename std::decay<value_type>::type>::value> {
   public:
#  if defined(__GNUC__) && !defined(__clang__)
    template <class... TARGS>
    promise_type(TARGS&&... args)
        : callable_promise_base<value_type, std::is_void<typename std::decay<value_type>::type>::value>(args...) {}
#  else
    template <class... TARGS>
    promise_type(TARGS&&... args)
        : callable_promise_base<value_type, std::is_void<typename std::decay<value_type>::type>::value>(
              std::forward<TARGS>(args)...) {}
#  endif

    auto get_return_object() noexcept {
      return self_type{LIBCOPP_MACRO_STD_COROUTINE_NAMESPACE coroutine_handle<promise_type>::from_promise(*this)};
    }

    struct initial_awaitable {
      inline bool await_ready() const noexcept { return false; }

      inline void await_resume() const noexcept {
        if (handle.promise().get_status() == promise_status::kCreated) {
          promise_status excepted = promise_status::kCreated;
          handle.promise().set_status(promise_status::kRunning, &excepted);
        }
      }

      inline bool await_suspend(LIBCOPP_MACRO_STD_COROUTINE_NAMESPACE coroutine_handle<promise_type> caller) noexcept {
        handle = caller;

        // Return false to resume the caller
        return false;
      }

      LIBCOPP_MACRO_STD_COROUTINE_NAMESPACE coroutine_handle<promise_type> handle;
    };
    initial_awaitable initial_suspend() noexcept { return {}; }
#  if defined(LIBCOPP_MACRO_ENABLE_EXCEPTION) && LIBCOPP_MACRO_ENABLE_EXCEPTION
    void unhandled_exception() { throw; }
#  elif defined(LIBCOPP_MACRO_HAS_EXCEPTION) && LIBCOPP_MACRO_HAS_EXCEPTION
    void unhandled_exception() { throw; }
#  else
    void unhandled_exception() { std::abort(); }
#  endif
  };
  using handle_type = LIBCOPP_MACRO_STD_COROUTINE_NAMESPACE coroutine_handle<promise_type>;
  using awaitable_type =
      callable_awaitable<promise_type, error_transform, std::is_void<typename std::decay<value_type>::type>::value>;

 public:
  callable_future(handle_type handle) noexcept : current_handle_{handle} {}

  callable_future(const callable_future&) = delete;
  callable_future(callable_future&& other) noexcept {
    current_handle_ = other.current_handle_;
    other.current_handle_ = nullptr;
  };

  callable_future& operator=(const callable_future&) = delete;
  callable_future& operator=(callable_future&& other) noexcept {
    current_handle_ = other.current_handle_;
    other.current_handle_ = nullptr;
    return *this;
  }

  ~callable_future() {
    // Move current_handle_ to stack here to allow recursive call of force_destroy
    handle_type current_handle = current_handle_;
    current_handle_ = nullptr;

    while (current_handle && !current_handle.done() &&
           !current_handle.promise().check_flag(promise_flag::kHasReturned)) {
      if (current_handle.promise().get_status() < promise_status::kDone) {
        current_handle.promise().set_status(promise_status::kKilled);
      }
      current_handle.resume();
    }

    if (current_handle) {
      current_handle.promise().set_flag(promise_flag::kDestroying, true);
      current_handle.destroy();
    }
  }

  awaitable_type operator co_await() { return awaitable_type{current_handle_}; }

  inline bool is_ready() const noexcept {
    if (!current_handle_) {
      return true;
    }

    return current_handle_.done() || current_handle_.promise().check_flag(promise_flag::kHasReturned);
  }

  UTIL_FORCEINLINE promise_status get_status() const noexcept { return current_handle_.promise().get_status(); }

  static auto yield_status() noexcept { return promise_base_type::pick_current_status(); }

  /**
   * @brief Kill callable
   * @param target_status status to set
   * @param force_resume force resume and ignore if there is no waiting handle
   * @note This function is safe only when bith call and callee are copp components
   *
   * @return true if killing or killed
   */
  bool kill(promise_status target_status = promise_status::kKilled, bool force_resume = false) noexcept {
    if (target_status < promise_status::kDone) {
      return false;
    }

    bool ret = true;
    while (true) {
      if (!current_handle_) {
        ret = false;
        break;
      }

      if (current_handle_.done()) {
        ret = false;
        break;
      }

      promise_status current_status = get_status();
      if (current_status >= promise_status::kDone) {
        ret = false;
        break;
      }

      if (!current_handle_.promise().set_status(target_status, &current_status)) {
        continue;
      }

      if ((force_resume || current_handle_.promise().is_waiting()) &&
          !current_handle_.promise().check_flag(promise_flag::kDestroying) &&
          !current_handle_.promise().check_flag(promise_flag::kHasReturned)) {
        // rethrow a exception in c++20 coroutine will crash when using MSVC now(VS2022)
        // We may enable exception in the future
#  if 0 && defined(LIBCOPP_MACRO_ENABLE_STD_EXCEPTION_PTR) && LIBCOPP_MACRO_ENABLE_STD_EXCEPTION_PTR
        std::exception_ptr unhandled_exception;
        try {
#  endif
        current_handle_.resume();

        // rethrow a exception in c++20 coroutine will crash when using MSVC now(VS2022)
        // We may enable exception in the future
#  if 0 && defined(LIBCOPP_MACRO_ENABLE_STD_EXCEPTION_PTR) && LIBCOPP_MACRO_ENABLE_STD_EXCEPTION_PTR
        } catch (...) {
          unhandled_exception = std::current_exception();
        }
        if (unhandled_exception) {
          std::rethrow_exception(unhandled_exception);
        }
#  endif
      }
      break;
    }

    return ret;
  }

  /**
   * @brief Get the internal handle object
   * @note This function is only for internal use(testing), do not use it in your code.
   *
   * @return internal handle
   */
  UTIL_FORCEINLINE const handle_type& get_internal_handle() const noexcept { return current_handle_; }

  /**
   * @brief Get the internal handle object
   * @note This function is only for internal use(testing), do not use it in your code.
   *
   * @return internal handle
   */
  UTIL_FORCEINLINE handle_type& get_internal_handle() noexcept { return current_handle_; }

  /**
   * @brief Get the internal promise object
   * @note This function is only for internal use(testing), do not use it in your code.
   *
   * @return internal promise object
   */
  UTIL_FORCEINLINE const promise_type& get_internal_promise() const noexcept { return current_handle_.promise(); }

  /**
   * @brief Get the internal promise object
   * @note This function is only for internal use(testing), do not use it in your code.
   *
   * @return internal promise object
   */
  UTIL_FORCEINLINE promise_type& get_internal_promise() noexcept { return current_handle_.promise(); }

 private:
  handle_type current_handle_;
};

// some delegate
template <class TFUTURE>
struct LIBCOPP_COPP_API_HEAD_ONLY some_delegate_context {
  using future_type = TFUTURE;
  using ready_output_type = typename some_ready<future_type>::type;

  std::list<future_type*> pending;
  ready_output_type ready;
  size_t ready_bound = 0;
  size_t scan_bound = 0;
  promise_status status = promise_status::kCreated;
  promise_caller_manager::handle_delegate caller_handle = promise_caller_manager::handle_delegate(nullptr);
};

template <class TFUTURE, class TDELEGATE_ACTION>
class LIBCOPP_COPP_API_HEAD_ONLY some_delegate_base {
 public:
  using future_type = TFUTURE;
  using value_type = typename future_type::value_type;
  using context_type = some_delegate_context<future_type>;
  using ready_output_type = typename context_type::ready_output_type;
  using delegate_action_type = TDELEGATE_ACTION;

 private:
  static void force_resume_all(context_type& context) {
    for (auto& pending_future : context.pending) {
      delegate_action_type::resume_future(context.caller_handle, *pending_future);
    }

    if (context.status < promise_status::kDone && nullptr != context.caller_handle.promise) {
      context.status = context.caller_handle.promise->get_status();
    }

    context.caller_handle = nullptr;
    if (context.status < promise_status::kDone) {
      context.status = promise_status::kKilled;
    }
  }

  static void scan_ready(context_type& context) {
    auto iter = context.pending.begin();

    while (iter != context.pending.end()) {
      if (delegate_action_type::is_pending(**iter)) {
        ++iter;
        continue;
      }
      future_type& future = **iter;
      context.ready.push_back(gsl::make_not_null(&future));
      iter = context.pending.erase(iter);

      delegate_action_type::resume_future(context.caller_handle, future);
    }
  }

 public:
  class awaitable_type : public awaitable_base_type {
   public:
    awaitable_type(context_type* context) : context_(context) {}

    inline bool await_ready() noexcept {
      if (nullptr == context_) {
        return true;
      }

      if (context_->status >= promise_status::kDone) {
        return true;
      }

      return context_->pending.empty();
    }

#  if defined(LIBCOPP_MACRO_ENABLE_CONCEPTS) && LIBCOPP_MACRO_ENABLE_CONCEPTS
    template <DerivedPromiseBaseType TCPROMISE>
#  else
    template <class TCPROMISE, typename = std::enable_if_t<std::is_base_of<promise_base_type, TCPROMISE>::value>>
#  endif
    inline bool await_suspend(LIBCOPP_MACRO_STD_COROUTINE_NAMESPACE coroutine_handle<TCPROMISE> caller) noexcept {
      if (nullptr == context_ || caller.promise().get_status() >= promise_status::kDone) {
        // Already done and can not suspend again
        // caller.resume();
        return false;
      }

      set_caller(caller);

      // Allow kill resume to forward error information
      caller.promise().set_flag(promise_flag::kInternalWaitting, true);

      // set caller for all futures
      if (!context_->caller_handle) {
        context_->caller_handle = caller;
        // Copy pending here, the callback may call resume and will change the pending list
        std::list<future_type*> copy_pending = context_->pending;
        for (auto& pending_future : copy_pending) {
          delegate_action_type::suspend_future(context_->caller_handle, *pending_future);
        }
      }

      return true;
    }

    void await_resume() {
      // caller maybe null if the callable is already ready when co_await
      auto caller = get_caller();
      if (caller) {
        if (nullptr != caller.promise) {
          caller.promise->set_flag(promise_flag::kInternalWaitting, false);
        }
        set_caller(nullptr);
      }

      if (nullptr == context_) {
        return;
      }

      ++context_->scan_bound;
      if (context_->scan_bound >= context_->ready_bound) {
        scan_ready(*context_);
        context_->scan_bound = context_->ready.size();

        if (context_->scan_bound >= context_->ready_bound && context_->status < promise_status::kDone) {
          context_->status = promise_status::kDone;
        }
      }
    }

   private:
    context_type* context_;
  };

 public:
  struct promise_type {
    context_type* context_;

    promise_type(context_type* context) : context_(context) {}
    promise_type(const promise_type&) = delete;
    promise_type(promise_type&&) = delete;
    promise_type& operator=(const promise_type&) = delete;
    promise_type& operator=(promise_type&&) = delete;
    ~promise_type() {
      COPP_LIKELY_IF (nullptr != context_ && !!context_->caller_handle) {
        force_resume_all(*context_);
      }
    }

    inline awaitable_type operator co_await() & { return awaitable_type{context_}; }
  };

  template <class TCONTAINER>
  static callable_future<promise_status> run(ready_output_type& ready_futures, size_t ready_count,
                                             TCONTAINER* futures) {
    using container_type = typename std::decay<typename std::remove_pointer<TCONTAINER>::type>::type;
    context_type context;
    context.ready.reserve(gsl::size(*futures));

    for (auto& future_object : *futures) {
      auto& future_ref =
          pick_some_reference<typename std::remove_reference<decltype(future_object)>::type>::unwrap(future_object);
      if (delegate_action_type::is_pending(future_ref)) {
        context.pending.push_back(&future_ref);
      } else {
        context.ready.push_back(gsl::make_not_null(&future_ref));
      }
    }

    if (context.ready.size() >= ready_count) {
      context.ready.swap(ready_futures);
      co_return promise_status::kDone;
    }

    if (ready_count >= context.pending.size() + ready_futures.size()) {
      ready_count = context.pending.size() + ready_futures.size();
    }
    context.ready_bound = ready_count;
    context.scan_bound = context.ready.size();
    context.status = promise_status::kRunning;

    {
      promise_type some_promise{&context};
      while (context.status < promise_status::kDone) {
        // Killed by caller
        auto current_status = co_yield callable_future<promise_status>::yield_status();
        if (current_status >= promise_status::kDone) {
          context.status = current_status;
          break;
        }

        co_await some_promise;
      }

      // destroy promise object and detach handles
    }

    context.ready.swap(ready_futures);
    co_return context.status;
  }

 private:
  std::shared_ptr<context_type> context_;
};

// some
template <class TVALUE, class TERROR_TRANSFORM>
struct LIBCOPP_COPP_API_HEAD_ONLY some_delegate_callable_action {
  using future_type = callable_future<TVALUE, TERROR_TRANSFORM>;
  using context_type = some_delegate_context<future_type>;

  inline static void suspend_future(const promise_caller_manager::handle_delegate& caller, future_type& callee) {
    callee.get_internal_promise().add_caller(caller);
  }

  inline static void resume_future(const promise_caller_manager::handle_delegate& caller, future_type& callee) {
    callee.get_internal_promise().remove_caller(caller, false);
    // Do not force resume callee here, we allow to await the unready callable later.
  }

  inline static bool is_pending(future_type& future_object) noexcept {
    auto future_status = future_object.get_status();
    return future_status >= promise_status::kCreated && future_status < promise_status::kDone;
  }
};

template <class TVALUE, class TERROR_TRANSFORM>
class LIBCOPP_COPP_API_HEAD_ONLY some_delegate<callable_future<TVALUE, TERROR_TRANSFORM>>
    : public some_delegate_base<callable_future<TVALUE, TERROR_TRANSFORM>,
                                some_delegate_callable_action<TVALUE, TERROR_TRANSFORM>> {
 public:
  using base_type = some_delegate_base<callable_future<TVALUE, TERROR_TRANSFORM>,
                                       some_delegate_callable_action<TVALUE, TERROR_TRANSFORM>>;
  using future_type = typename base_type::future_type;
  using value_type = typename base_type::value_type;
  using ready_output_type = typename base_type::ready_output_type;
  using context_type = typename base_type::context_type;

  using base_type::run;
};

LIBCOPP_COPP_NAMESPACE_END

#endif

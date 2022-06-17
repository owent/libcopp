// Copyright 2022 owent

#pragma once

#include <libcopp/utils/config/libcopp_build_features.h>

// clang-format off
#include <libcopp/utils/config/stl_include_prefix.h>  // NOLINT(build/include_order)
// clang-format on
#include <assert.h>
#include <type_traits>
#include <vector>
// clang-format off
#include <libcopp/utils/config/stl_include_suffix.h>  // NOLINT(build/include_order)
// clang-format on

#if defined(LIBCOPP_MACRO_ENABLE_STD_EXCEPTION_PTR) && LIBCOPP_MACRO_ENABLE_STD_EXCEPTION_PTR
#  include <exception>
#endif

#include "libcopp/coroutine/std_coroutine_common.h"
#include "libcopp/future/future.h"
#include "libcopp/utils/uint64_id_allocator.h"

#if defined(LIBCOPP_MACRO_ENABLE_STD_COROUTINE) && LIBCOPP_MACRO_ENABLE_STD_COROUTINE

LIBCOPP_COPP_NAMESPACE_BEGIN

template <class TFUTURE>
class LIBCOPP_COPP_API_HEAD_ONLY some_delegate;

template <class TVALUE>
class LIBCOPP_COPP_API_HEAD_ONLY callable_future;

template <class TVALUE, bool RETURN_VOID>
class LIBCOPP_COPP_API_HEAD_ONLY callable_promise_base;

template <class TPROMISE, bool RETURN_VOID>
class LIBCOPP_COPP_API_HEAD_ONLY callable_awaitable;

template <class TVALUE>
class LIBCOPP_COPP_API_HEAD_ONLY callable_promise_base<TVALUE, true> : public promise_base_type {
 public:
  using value_type = TVALUE;

  template <class... TARGS>
  callable_promise_base(TARGS&&...) {}

  callable_promise_base() = default;

  void return_void() noexcept {
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
                  construct(std::forward<TARGS>(args)...)),
        has_return_(false) {}

  void return_value(value_type value) {
    if (get_status() < promise_status::kDone) {
      set_status(promise_status::kDone);
    }
    data_ = std::move(value);
    has_return_ = true;
  }

  UTIL_FORCEINLINE value_type& data() noexcept { return data_; }
  UTIL_FORCEINLINE const value_type& data() const noexcept { return data_; }

  UTIL_FORCEINLINE bool has_return() const noexcept { return has_return_; }

 protected:
  value_type data_;
  bool has_return_;
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

    if (callee_.promise().get_status() == promise_status::kCreated) {
      callee_.resume();
    }

    if (callee_.promise().get_status() >= promise_status::kDone) {
      return true;
    }

    if (callee_.promise().check_flag(promise_flag::kFinalSuspend)) {
      return true;
    }

    return callee_.done();
  }

#  if defined(LIBCOPP_MACRO_ENABLE_CONCEPTS) && LIBCOPP_MACRO_ENABLE_CONCEPTS
  template <DerivedPromiseBaseType TCPROMISE>
#  else
  template <class TCPROMISE, typename = std::enable_if_t<std::is_base_of<promise_base_type, TCPROMISE>::value>>
#  endif
  inline void await_suspend(LIBCOPP_MACRO_STD_COROUTINE_NAMESPACE coroutine_handle<TCPROMISE> caller) noexcept {
    if (caller.promise().get_status() < promise_status::kDone) {
      set_caller(caller);
      caller.promise().set_waiting_handle(callee_);
      callee_.promise().add_caller(caller);

      caller.promise().set_flag(promise_flag::kInternalWaitting, true);
    } else {
      // Already done and can not suspend again
      auto& callee_promise = get_callee().promise();
      // If callee is killed when running, we need inherit status from caller
      if (callee_promise.get_status() < promise_status::kDone &&
          callee_promise.check_flag(promise_flag::kInternalWaitting)) {
        callee_promise.set_status(caller.promise().get_status());
        callee_.resume();
      }
      caller.resume();
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

template <class TPROMISE>
class LIBCOPP_COPP_API_HEAD_ONLY callable_awaitable<TPROMISE, true> : public callable_awaitable_base<TPROMISE> {
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

template <class TPROMISE>
class LIBCOPP_COPP_API_HEAD_ONLY callable_awaitable<TPROMISE, false> : public callable_awaitable_base<TPROMISE> {
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

    if (!callee_promise.has_return()) {
      return promise_error_transform<value_type>()(callee_promise.get_status());
    }

    return std::move(callee_promise.data());
  }
};

template <class TVALUE>
class LIBCOPP_COPP_API_HEAD_ONLY callable_future {
 public:
  using value_type = TVALUE;
  using self_type = callable_future<value_type>;
  class promise_type
      : public callable_promise_base<value_type, std::is_void<typename std::decay<value_type>::type>::value> {
   public:
#  if defined(__GNUC__) && !defined(__clang__)
    template <class... TARGS>
    promise_type(TARGS&&... args)
        : callable_promise_base<value_type, std::is_void<typename std::decay<value_type>::type>::value>(
              std::move(args)...) {}
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

      inline void await_suspend(LIBCOPP_MACRO_STD_COROUTINE_NAMESPACE coroutine_handle<promise_type> caller) noexcept {
        handle = caller;

        caller.resume();
      }

      LIBCOPP_MACRO_STD_COROUTINE_NAMESPACE coroutine_handle<promise_type> handle;
    };
    initial_awaitable initial_suspend() noexcept { return {}; }
#  if defined(LIBCOPP_MACRO_ENABLE_EXCEPTION) && LIBCOPP_MACRO_ENABLE_EXCEPTION
    void unhandled_exception() { throw; }
#  endif
  };
  using handle_type = LIBCOPP_MACRO_STD_COROUTINE_NAMESPACE coroutine_handle<promise_type>;
  using awaitable_type = callable_awaitable<promise_type, std::is_void<typename std::decay<value_type>::type>::value>;

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
    while (current_handle_ && !current_handle_.done() &&
           !current_handle_.promise().check_flag(promise_flag::kFinalSuspend)) {
      current_handle_.resume();
    }

    if (current_handle_) {
      current_handle_.promise().set_flag(promise_flag::kDestroying, true);
      current_handle_.destroy();
    }
  }

  awaitable_type operator co_await() { return awaitable_type{current_handle_}; }

  inline bool is_ready() const noexcept {
    if (!current_handle_) {
      return true;
    }

    return current_handle_.done() || current_handle_.promise().check_flag(promise_flag::kFinalSuspend);
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
          !current_handle_.promise().check_flag(promise_flag::kFinalSuspend)) {
        current_handle_.resume();
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

template <class TFUTURE>
struct some_ready {
  using type = std::vector<std::reference_wrapper<TFUTURE>>;
};

template <class TCONTAINER>
struct some_ready_container {
  using container_type = typename std::decay<TCONTAINER>::type;
  using value_type = typename std::decay<typename container_type::value_type>::type;
};

#  if defined(LIBCOPP_MACRO_ENABLE_CONCEPTS) && LIBCOPP_MACRO_ENABLE_CONCEPTS
template <class TREADY_CONTAINER, class TCONTAINER>
    LIBCOPP_COPP_API_HEAD_ONLY callable_future<promise_status> some(
        TREADY_CONTAINER&&ready_futures, size_t ready_count, TCONTAINER&&pending_futures) requires std::convertible_to <
    typename std::decay<TREADY_CONTAINER>::type,
typename some_ready<typename some_ready_container<TCONTAINER>::value_type>::type > {
#  else
template <class TREADY_CONTAINER, class TCONTAINER,
          class = typename std::enable_if<std::is_same<
              typename std::decay<TREADY_CONTAINER>::type,
              typename some_ready<typename some_ready_container<TCONTAINER>::value_type>::type>::value>::type>
LIBCOPP_COPP_API_HEAD_ONLY callable_future<promise_status> some(TREADY_CONTAINER&& ready_futures, size_t ready_count,
                                                                TCONTAINER&& pending_futures) {
#  endif
  return some_delegate<typename some_ready_container<TCONTAINER>::value_type>::run(
      std::forward<TREADY_CONTAINER>(ready_futures), ready_count, std::forward<TCONTAINER>(pending_futures));
}

LIBCOPP_COPP_NAMESPACE_END

#endif

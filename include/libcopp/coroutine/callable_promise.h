// Copyright 2022 owent

#pragma once

#include <libcopp/utils/config/libcopp_build_features.h>

#include <assert.h>
#include <type_traits>

#if defined(LIBCOPP_MACRO_ENABLE_STD_EXCEPTION_PTR) && LIBCOPP_MACRO_ENABLE_STD_EXCEPTION_PTR
#  include <exception>
#endif

#include "libcopp/coroutine/std_coroutine_common.h"
#include "libcopp/future/future.h"
#include "libcopp/utils/uint64_id_allocator.h"

#if defined(LIBCOPP_MACRO_ENABLE_STD_COROUTINE) && LIBCOPP_MACRO_ENABLE_STD_COROUTINE

LIBCOPP_COPP_NAMESPACE_BEGIN

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

  void return_void() noexcept {
    if (get_status() < promise_status::kDone) {
      set_status(promise_status::kDone);
    }
  }
};

template <class TVALUE>
class LIBCOPP_COPP_API_HEAD_ONLY callable_promise_base<TVALUE, false> : public promise_base_type {
 public:
  using value_type = TVALUE;

  void return_value(value_type value) {
    if (get_status() < promise_status::kDone) {
      set_status(promise_status::kDone);
    }
    data_ = std::move(value);
    has_return_ = true;
  }

  inline value_type& data() noexcept { return data_; }
  inline const value_type& data() const noexcept { return data_; }

  inline bool has_return() const noexcept { return has_return_; }

 protected:
  value_type data_;
  bool has_return_ = false;
};

#  if defined(LIBCOPP_MACRO_ENABLE_CONCEPTS) && LIBCOPP_MACRO_ENABLE_CONCEPTS
template <DerivedPromiseBaseType TPROMISE>
#  else
template <class TPROMISE, typename = std::enable_if_t<std::is_base_of<promise_base_type, TPROMISE>::value> >
#  endif
class LIBCOPP_COPP_API_HEAD_ONLY callable_awaitable_base : public awaitable_base_type {
 public:
  using promise_type = TPROMISE;
  using value_type = typename promise_type::value_type;
  using handle_type = LIBCOPP_MACRO_STD_COROUTINE_NAMESPACE coroutine_handle<promise_type>;

 public:
  callable_awaitable_base(handle_type handle) : callee_{handle} {}

  inline bool await_ready() noexcept {
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

    return callee_.done();
  }

#  if defined(LIBCOPP_MACRO_ENABLE_CONCEPTS) && LIBCOPP_MACRO_ENABLE_CONCEPTS
  template <DerivedPromiseBaseType TCPROMISE>
#  else
  template <class TCPROMISE, typename = std::enable_if_t<std::is_base_of<promise_base_type, TCPROMISE>::value> >
#  endif
  inline void await_suspend(LIBCOPP_MACRO_STD_COROUTINE_NAMESPACE coroutine_handle<TCPROMISE> caller) noexcept {
    if (caller.promise().get_status() < promise_status::kDone) {
      set_caller(caller);
      caller.promise().set_waiting_handle(callee_);
      callee_.promise().add_caller(caller);
    } else {
      // Already done and can not suspend again
      caller.resume();
    }
  }

  inline handle_type& get_callee() noexcept { return callee_; }
  inline const handle_type& get_callee() const noexcept { return callee_; }

 protected:
  inline void detach() noexcept {
    // caller maybe null if the callable is already ready when co_await
    auto caller = get_caller();
    auto& callee_promise = get_callee().promise();

    if (caller) {
      callee_promise.remove_caller(caller, true);
      caller.promise->set_waiting_handle(nullptr);
      set_caller(nullptr);
    }

    if (callee_promise.get_status() < promise_status::kDone) {
      if (await_ready() || !caller) {
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

  inline void await_resume() {
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
      }
      LIBCOPP_MACRO_STD_COROUTINE_NAMESPACE coroutine_handle<promise_type> handle;
    };
    initial_awaitable initial_suspend() noexcept { return {}; }
    void unhandled_exception() { throw; }
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
    if (current_handle_) {
      current_handle_.destroy();
    }
  }

  awaitable_type operator co_await() { return awaitable_type{current_handle_}; }

  inline bool is_ready() const noexcept { return current_handle_.done(); }

  inline promise_status get_status() const noexcept { return current_handle_.promise().get_status(); }

  static auto yield_status() noexcept { return promise_base_type::pick_current_status(); }

  /**
   * @brief Get the internal handle object
   * @note This function is only for internal use(testing), do not use it in your code.
   *
   * @return internal handle
   */
  inline const handle_type& get_internal_handle() const noexcept { return current_handle_; }

  /**
   * @brief Get the internal handle object
   * @note This function is only for internal use(testing), do not use it in your code.
   *
   * @return internal handle
   */
  inline handle_type& get_internal_handle() noexcept { return current_handle_; }

  /**
   * @brief Get the internal promise object
   * @note This function is only for internal use(testing), do not use it in your code.
   *
   * @return internal promise object
   */
  inline const promise_type& get_internal_promise() const noexcept { return current_handle_.promise(); }

  /**
   * @brief Get the internal promise object
   * @note This function is only for internal use(testing), do not use it in your code.
   *
   * @return internal promise object
   */
  inline promise_type& get_internal_promise() noexcept { return current_handle_.promise(); }

 private:
  handle_type current_handle_;
};

LIBCOPP_COPP_NAMESPACE_END

#endif

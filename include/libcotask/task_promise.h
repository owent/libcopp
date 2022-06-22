// Copyright 2022 owent

#pragma once

#include <libcopp/utils/config/libcopp_build_features.h>

#include <libcopp/coroutine/algorithm_common.h>
#include <libcopp/coroutine/callable_promise.h>
#include <libcopp/coroutine/std_coroutine_common.h>
#include <libcopp/future/future.h>
#include <libcopp/utils/uint64_id_allocator.h>

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

#if defined(LIBCOPP_MACRO_ENABLE_STD_COROUTINE) && LIBCOPP_MACRO_ENABLE_STD_COROUTINE

LIBCOPP_COTASK_NAMESPACE_BEGIN

template <class TVALUE>
class LIBCOPP_COPP_API_HEAD_ONLY task_context_base;

template <class TVALUE, bool RETURN_VOID>
class LIBCOPP_COPP_API_HEAD_ONLY task_context_delegate;

template <class TVALUE, class TPRIVATE_DATA>
class LIBCOPP_COPP_API_HEAD_ONLY task_context;

template <class TVALUE, class TPRIVATE_DATA>
class LIBCOPP_COPP_API_HEAD_ONLY task_future_base;

template <class TVALUE, class TPRIVATE_DATA>
class LIBCOPP_COPP_API_HEAD_ONLY task_future;

template <class TVALUE, class TPRIVATE_DATA, bool RETURN_VOID>
class LIBCOPP_COPP_API_HEAD_ONLY task_promise_base;

template <class TCONTEXT>
class LIBCOPP_COPP_API_HEAD_ONLY task_awaitable_base;

template <class TCONTEXT, bool RETURN_VOID>
class LIBCOPP_COPP_API_HEAD_ONLY task_awaitable;

template <class TVALUE>
class LIBCOPP_COPP_API_HEAD_ONLY task_context_base {
 public:
  using value_type = TVALUE;
  using id_type = LIBCOPP_COPP_NAMESPACE_ID::util::uint64_id_allocator::value_type;
  using id_allocator_type = LIBCOPP_COPP_NAMESPACE_ID::util::uint64_id_allocator;
  using handle_delegate = LIBCOPP_COPP_NAMESPACE_ID::promise_caller_manager::handle_delegate;
  using task_status_type = LIBCOPP_COPP_NAMESPACE_ID::promise_status;
  using promise_flag = LIBCOPP_COPP_NAMESPACE_ID::promise_flag;

 private:
  task_context_base(const task_context_base&) = delete;
  task_context_base(task_context_base&&) = delete;
  task_context_base& operator=(const task_context_base&) = delete;
  task_context_base& operator=(task_context_base&&) = delete;

 public:
  task_context_base() noexcept : current_handle_(nullptr) {
    id_allocator_type id_allocator;
    id_ = id_allocator.allocate();
  }

  ~task_context_base() noexcept {
    force_finish();

    if (nullptr != current_handle_.promise) {
      current_handle_.promise->set_flag(promise_flag::kDestroying, true);
    }
    if (current_handle_.handle) {
      current_handle_.handle.destroy();
    }

#  if defined(LIBCOPP_MACRO_ENABLE_STD_EXCEPTION_PTR) && LIBCOPP_MACRO_ENABLE_STD_EXCEPTION_PTR
    if (last_exception_) {
      std::rethrow_exception(last_exception_);
    }
#  endif
  }

  UTIL_FORCEINLINE bool is_ready() const noexcept {
    if (nullptr != current_handle_.promise && current_handle_.promise->check_flag(promise_flag::kFinalSuspend)) {
      return true;
    }

    return data_.is_ready();
  }

  UTIL_FORCEINLINE bool is_pending() const noexcept { return data_.is_pending(); }

  inline task_status_type get_status() const noexcept {
    if (current_handle_.promise) {
      return current_handle_.promise->get_status();
    }

    return task_status_type::kInvalid;
  }

  UTIL_FORCEINLINE id_type get_id() const noexcept { return id_; }

  UTIL_FORCEINLINE bool has_multiple_callers() const noexcept { return caller_manager_.has_multiple_callers(); }

 protected:
  UTIL_FORCEINLINE void add_caller(handle_delegate handle) noexcept { caller_manager_.add_caller(handle); }

#  if defined(LIBCOPP_MACRO_ENABLE_CONCEPTS) && LIBCOPP_MACRO_ENABLE_CONCEPTS
  template <LIBCOPP_COPP_NAMESPACE_ID::DerivedPromiseBaseType TPROMISE>
#  else
  template <class TPROMISE,
            typename = std::enable_if_t<std::is_base_of<LIBCOPP_COPP_NAMESPACE_ID::promise_base_type, TPROMISE>::value>>
#  endif
  UTIL_FORCEINLINE LIBCOPP_COPP_API_HEAD_ONLY void add_caller(
      const LIBCOPP_MACRO_STD_COROUTINE_NAMESPACE coroutine_handle<TPROMISE>& handle) noexcept {
    add_caller(handle_delegate{handle});
  }

  UTIL_FORCEINLINE void remove_caller(handle_delegate handle) noexcept { caller_manager_.remove_caller(handle); }

#  if defined(LIBCOPP_MACRO_ENABLE_CONCEPTS) && LIBCOPP_MACRO_ENABLE_CONCEPTS
  template <LIBCOPP_COPP_NAMESPACE_ID::DerivedPromiseBaseType TPROMISE>
#  else
  template <class TPROMISE,
            typename = std::enable_if_t<std::is_base_of<LIBCOPP_COPP_NAMESPACE_ID::promise_base_type, TPROMISE>::value>>
#  endif
  UTIL_FORCEINLINE LIBCOPP_COPP_API_HEAD_ONLY void remove_caller(
      const LIBCOPP_MACRO_STD_COROUTINE_NAMESPACE coroutine_handle<TPROMISE>& handle, bool inherit_status) noexcept {
    remove_caller(handle_delegate{handle}, inherit_status);
  }

  UTIL_FORCEINLINE void wake() { caller_manager_.resume_callers(); }

 private:
  template <class, class, bool>
  friend class LIBCOPP_COPP_API_HEAD_ONLY task_promise_base;

  template <class, class>
  friend class LIBCOPP_COPP_API_HEAD_ONLY task_future_base;

  template <class, class>
  friend class LIBCOPP_COPP_API_HEAD_ONLY task_future;

  template <class TCONTEXT>
  friend class LIBCOPP_COPP_API_HEAD_ONLY task_awaitable_base;

  inline void force_finish() noexcept {
    COPP_LIKELY_IF (nullptr != current_handle_.promise) {
      if (current_handle_.promise->get_status() < task_status_type::kDone) {
        current_handle_.promise->set_status(task_status_type::kKilled);
      }
    }

    // Move unhandled_exception
    while (current_handle_.handle && !current_handle_.handle.done() && current_handle_.promise &&
           !current_handle_.promise->check_flag(promise_flag::kFinalSuspend)
#  if defined(LIBCOPP_MACRO_ENABLE_STD_EXCEPTION_PTR) && LIBCOPP_MACRO_ENABLE_STD_EXCEPTION_PTR
           && !last_exception_
#  endif
    ) {
      current_handle_.handle.resume();
    }

    wake();
  }

  UTIL_FORCEINLINE void initialize_handle(handle_delegate handle) noexcept { current_handle_ = handle; }

  UTIL_FORCEINLINE handle_delegate& get_handle_delegate() noexcept { return current_handle_; }
  UTIL_FORCEINLINE const handle_delegate& get_handle_delegate() const noexcept { return current_handle_; }

 protected:
  LIBCOPP_COPP_NAMESPACE_ID::future::future<TVALUE> data_;
  // caller manager
  LIBCOPP_COPP_NAMESPACE_ID::promise_caller_manager caller_manager_;

 private:
  id_type id_;
  std::atomic<size_t> future_counter_;
  handle_delegate current_handle_;
#  if defined(LIBCOPP_MACRO_ENABLE_STD_EXCEPTION_PTR) && LIBCOPP_MACRO_ENABLE_STD_EXCEPTION_PTR
  std::exception_ptr last_exception_;
#  endif
};

template <class TVALUE>
class LIBCOPP_COPP_API_HEAD_ONLY task_context_delegate<TVALUE, true> : public task_context_base<TVALUE> {
 public:
  using base_type = task_context_base<TVALUE>;
  using id_type = typename base_type::id_type;
  using value_type = typename base_type::value_type;
  using handle_delegate = typename base_type::handle_delegate;
  using task_status_type = typename base_type::task_status_type;
  using promise_flag = typename base_type::promise_flag;

 public:
  ~task_context_delegate() { wake(); }

  using base_type::is_pending;
  using base_type::is_ready;

  UTIL_FORCEINLINE void set_value() {
    data_.reset_data(true);
    wake();
  }

 private:
  template <class TCONTEXT>
  friend class LIBCOPP_COPP_API_HEAD_ONLY task_awaitable_base;

  template <class TSOME_DELEGATE_TASK_VALUE, class TSOME_DELEGATE_TASK_PRIVATE_DATA>
  friend struct LIBCOPP_COPP_API_HEAD_ONLY some_delegate_task_action;

  using base_type::add_caller;
  using base_type::data_;
  using base_type::remove_caller;
  using base_type::wake;
};

template <class TVALUE>
class LIBCOPP_COPP_API_HEAD_ONLY task_context_delegate<TVALUE, false> : public task_context_base<TVALUE> {
 public:
  using base_type = task_context_base<TVALUE>;
  using id_type = typename base_type::id_type;
  using value_type = typename base_type::value_type;
  using handle_delegate = typename base_type::handle_delegate;
  using task_status_type = typename base_type::task_status_type;
  using promise_flag = typename base_type::promise_flag;

 public:
  using base_type::is_pending;
  using base_type::is_ready;

  ~task_context_delegate() {
    if (is_pending()) {
      set_value(LIBCOPP_COPP_NAMESPACE_ID::promise_error_transform<value_type>()(task_status_type::kKilled));
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
  template <class TCONTEXT>
  friend class LIBCOPP_COPP_API_HEAD_ONLY task_awaitable_base;

  template <class TSOME_DELEGATE_TASK_VALUE, class TSOME_DELEGATE_TASK_PRIVATE_DATA>
  friend struct LIBCOPP_COPP_API_HEAD_ONLY some_delegate_task_action;

  using base_type::add_caller;
  using base_type::data_;
  using base_type::remove_caller;
  using base_type::wake;
};

template <class TPRIVATE_DATA>
class LIBCOPP_COPP_API_HEAD_ONLY task_private_data {
 public:
  inline task_private_data() noexcept : data_(nullptr) {}
  inline task_private_data(TPRIVATE_DATA* input) noexcept : data_(input) {}
  inline task_private_data(task_private_data&& other) noexcept = default;
  inline task_private_data& operator=(task_private_data&&) noexcept = default;
  inline task_private_data(const task_private_data&) = delete;
  inline task_private_data& operator=(const task_private_data&) = delete;
  inline ~task_private_data() {}

  inline bool await_ready() const noexcept { return true; }
  inline TPRIVATE_DATA* await_resume() const noexcept { return data_; }
  inline void await_suspend(LIBCOPP_COPP_NAMESPACE_ID::promise_base_type::type_erased_handle_type) noexcept {}

 private:
  template <class, class>
  friend class LIBCOPP_COPP_API_HEAD_ONLY task_context;

  template <class, class>
  friend class LIBCOPP_COPP_API_HEAD_ONLY task_future;

  TPRIVATE_DATA* data_;
};

template <class TVALUE>
class LIBCOPP_COPP_API_HEAD_ONLY task_context<TVALUE, void>
    : public task_context_delegate<TVALUE, std::is_void<typename std::decay<TVALUE>::type>::value>,
      public std::enable_shared_from_this<task_context<TVALUE, void>> {
 public:
  using base_type = task_context_delegate<TVALUE, std::is_void<typename std::decay<TVALUE>::type>::value>;
  using value_type = typename base_type::value_type;
  using id_type = typename base_type::id_type;
  using private_data_type = void;
  using handle_delegate = typename base_type::handle_delegate;
  using task_status_type = typename base_type::task_status_type;
  using promise_flag = typename base_type::promise_flag;

 public:
  using base_type::is_pending;
  using base_type::is_ready;
  using base_type::set_value;

  template <class... TARGS>
  task_context(TARGS&&...) {}
};

template <class TVALUE, class TPRIVATE_DATA>
class LIBCOPP_COPP_API_HEAD_ONLY task_context
    : public task_context_delegate<TVALUE, std::is_void<typename std::decay<TVALUE>::type>::value>,
      public std::enable_shared_from_this<task_context<TVALUE, TPRIVATE_DATA>> {
 public:
  using base_type = task_context_delegate<TVALUE, std::is_void<typename std::decay<TVALUE>::type>::value>;
  using value_type = typename base_type::value_type;
  using private_data_type = TPRIVATE_DATA;
  using handle_delegate = typename base_type::handle_delegate;
  using task_status_type = typename base_type::task_status_type;
  using promise_flag = typename base_type::promise_flag;

 public:
  using base_type::is_pending;
  using base_type::is_ready;
  using base_type::set_value;

  template <class... TARGS>
  task_context(TARGS&&... args)
      : private_data_(
            LIBCOPP_COPP_NAMESPACE_ID::callable_promise_value_constructor<
                private_data_type,
                !std::is_constructible<private_data_type, TARGS...>::value>::construct(std::forward<TARGS>(args)...)) {}

  private_data_type& get_private_data() noexcept { return private_data_; }
  const private_data_type& get_private_data() const noexcept { return private_data_; }

 private:
  private_data_type private_data_;
};

template <class TVALUE, class TPRIVATE_DATA>
class LIBCOPP_COPP_API_HEAD_ONLY task_promise_base<TVALUE, TPRIVATE_DATA, true>
    : public LIBCOPP_COPP_NAMESPACE_ID::promise_base_type {
 public:
  using value_type = TVALUE;
  using context_type = task_context<value_type, TPRIVATE_DATA>;
  using private_data_type = typename context_type::private_data_type;
  using context_pointer_type = std::shared_ptr<context_type>;
  using handle_delegate = typename context_type::handle_delegate;
  using task_status_type = LIBCOPP_COPP_NAMESPACE_ID::promise_status;

  template <class... TARGS>
  task_promise_base(TARGS&&... args) : context_(std::make_shared<context_type>(std::forward<TARGS>(args)...)) {}

  void return_void() noexcept {
    if (get_status() < task_status_type::kDone) {
      set_status(task_status_type::kDone);
    }
    COPP_LIKELY_IF (context_) {
      context_->set_value();
    }
  }

  UTIL_FORCEINLINE bool has_return() const noexcept { return context_ && context_->is_ready(); }

  UTIL_FORCEINLINE const context_pointer_type& get_context() const noexcept { return context_; }

 protected:
  template <class TPROMISE, typename = std::enable_if_t<
                                std::is_base_of<task_promise_base<TVALUE, TPRIVATE_DATA, true>, TPROMISE>::value>>
  UTIL_FORCEINLINE void initialize_promise(
      const LIBCOPP_MACRO_STD_COROUTINE_NAMESPACE coroutine_handle<TPROMISE>& origin_handle) noexcept {
    COPP_LIKELY_IF (context_) {
      context_->initialize_handle(handle_delegate{origin_handle});
    }
  }

 private:
  context_pointer_type context_;
};

template <class TVALUE, class TPRIVATE_DATA>
class LIBCOPP_COPP_API_HEAD_ONLY task_promise_base<TVALUE, TPRIVATE_DATA, false>
    : public LIBCOPP_COPP_NAMESPACE_ID::promise_base_type {
 public:
  using value_type = TVALUE;
  using context_type = task_context<value_type, TPRIVATE_DATA>;
  using private_data_type = typename context_type::private_data_type;
  using context_pointer_type = std::shared_ptr<context_type>;
  using task_status_type = LIBCOPP_COPP_NAMESPACE_ID::promise_status;

  template <class... TARGS>
  task_promise_base(TARGS&&... args) : context_(std::make_shared<context_type>(std::forward<TARGS>(args)...)) {}

  void return_value(value_type value) {
    if (get_status() < task_status_type::kDone) {
      set_status(task_status_type::kDone);
    }
    COPP_LIKELY_IF (context_) {
      context_->set_value(std::move(value));
    }
  }

  UTIL_FORCEINLINE value_type* data() noexcept {
    COPP_LIKELY_IF (context_) {
      return context_->data();
    }
    return nullptr;
  }
  UTIL_FORCEINLINE const value_type* data() const noexcept {
    COPP_LIKELY_IF (context_) {
      return context_->data();
    }
    return nullptr;
  }

  UTIL_FORCEINLINE bool has_return() const noexcept { return context_ && context_->is_ready(); }

  UTIL_FORCEINLINE const context_pointer_type& get_context() const noexcept { return context_; }

 protected:
  template <class TPROMISE, typename = std::enable_if_t<
                                std::is_base_of<task_promise_base<TVALUE, TPRIVATE_DATA, false>, TPROMISE>::value>>
  UTIL_FORCEINLINE void initialize_promise(
      const LIBCOPP_MACRO_STD_COROUTINE_NAMESPACE coroutine_handle<TPROMISE>& origin_handle) noexcept {
    COPP_LIKELY_IF (context_) {
      context_->initialize_handle(handle_delegate{origin_handle});
    }
  }

 private:
  context_pointer_type context_;
};

template <class TCONTEXT>
class LIBCOPP_COPP_API_HEAD_ONLY task_awaitable_base : public LIBCOPP_COPP_NAMESPACE_ID::awaitable_base_type {
 public:
  using context_type = TCONTEXT;
  using context_pointer_type = std::shared_ptr<context_type>;
  using value_type = typename context_type::value_type;
  using task_status_type = LIBCOPP_COPP_NAMESPACE_ID::promise_status;
  using promise_flag = LIBCOPP_COPP_NAMESPACE_ID::promise_flag;

 public:
  task_awaitable_base(context_type* context) : context_{context} {}

  inline bool await_ready() noexcept {
    COPP_UNLIKELY_IF (nullptr == context_) {
      return true;
    }

    if (context_->is_ready()) {
      return true;
    }

    if (nullptr == context_->get_handle_delegate().promise) {
      return true;
    }

    if (context_->get_handle_delegate().promise->get_status() >= task_status_type::kDone) {
      return true;
    }

    return false;
  }

#  if defined(LIBCOPP_MACRO_ENABLE_CONCEPTS) && LIBCOPP_MACRO_ENABLE_CONCEPTS
  template <LIBCOPP_COPP_NAMESPACE_ID::DerivedPromiseBaseType TCPROMISE>
#  else
  template <class TCPROMISE, typename = std::enable_if_t<
                                 std::is_base_of<LIBCOPP_COPP_NAMESPACE_ID::promise_base_type, TCPROMISE>::value>>
#  endif
  inline void await_suspend(LIBCOPP_MACRO_STD_COROUTINE_NAMESPACE coroutine_handle<TCPROMISE> caller) noexcept {
    if (nullptr != context_ && caller.promise().get_status() < task_status_type::kDone) {
      set_caller(caller);
      context_->add_caller(caller);

      // Allow kill resume to forward error information
      caller.promise().set_flag(promise_flag::kInternalWaitting, true);
    } else {
      // Already done and can not suspend again
      caller.resume();
    }
  }

 protected:
  task_status_type detach() noexcept {
    task_status_type result_status;
    COPP_UNLIKELY_IF (nullptr == context_) {
      result_status = task_status_type::kInvalid;
    } else if (context_->is_ready()) {
      result_status = task_status_type::kDone;
    } else {
      result_status = task_status_type::kKilled;
    }

    // caller maybe null if the callable is already ready when co_await
    auto caller = get_caller();

    if (caller) {
      if (nullptr != caller.promise) {
        caller.promise->set_flag(promise_flag::kInternalWaitting, false);
      }
      COPP_LIKELY_IF (nullptr != context_) {
        if (!context_->is_ready() && nullptr != caller.promise) {
          result_status = caller.promise->get_status();
        }

        context_->remove_caller(caller);
        set_caller(nullptr);
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
  UTIL_FORCEINLINE context_type* get_context() noexcept { return context_; }

 private:
  context_type* context_;
};

template <class TCONTEXT>
class LIBCOPP_COPP_API_HEAD_ONLY task_awaitable<TCONTEXT, true> : public task_awaitable_base<TCONTEXT> {
 public:
  using base_type = task_awaitable_base<TCONTEXT>;
  using value_type = typename base_type::value_type;
  using context_type = typename base_type::context_type;
  using context_pointer_type = typename base_type::context_pointer_type;
  using task_status_type = typename base_type::task_status_type;
  using promise_flag = typename base_type::promise_flag;

 public:
  using base_type::await_ready;
  using base_type::await_suspend;
  using base_type::get_caller;
  using base_type::set_caller;
  task_awaitable(context_type* context) : base_type(context) {}

  inline void await_resume() { detach(); }

 private:
  using base_type::detach;
  using base_type::get_context;
};

template <class TCONTEXT>
class LIBCOPP_COPP_API_HEAD_ONLY task_awaitable<TCONTEXT, false> : public task_awaitable_base<TCONTEXT> {
 public:
  using base_type = task_awaitable_base<TCONTEXT>;
  using value_type = typename base_type::value_type;
  using context_type = typename base_type::context_type;
  using context_pointer_type = typename base_type::context_pointer_type;
  using task_status_type = typename base_type::task_status_type;
  using promise_flag = typename base_type::promise_flag;

 public:
  using base_type::await_ready;
  using base_type::await_suspend;
  using base_type::get_caller;
  using base_type::set_caller;
  task_awaitable(context_type* context) : base_type(context) {}

  inline value_type await_resume() {
    bool has_multiple_callers;
    COPP_LIKELY_IF (nullptr != get_context()) {
      has_multiple_callers = get_context()->has_multiple_callers();
    } else {
      has_multiple_callers = false;
    }
    task_status_type result_status = detach();

    if (task_status_type::kDone != result_status) {
      return LIBCOPP_COPP_NAMESPACE_ID::promise_error_transform<value_type>()(result_status);
    }

    COPP_LIKELY_IF (nullptr != get_context()) {
      if (has_multiple_callers) {
        return *get_context()->data();
      } else {
        return LIBCOPP_COPP_NAMESPACE_ID::multiple_callers_constructor<value_type>::return_value(
            *get_context()->data());
      }
    } else {
      return LIBCOPP_COPP_NAMESPACE_ID::promise_error_transform<value_type>()(task_status_type::kInvalid);
    }
  }

 private:
  using base_type::detach;
  using base_type::get_context;
};

template <class TVALUE, class TPRIVATE_DATA>
class LIBCOPP_COPP_API_HEAD_ONLY task_future_base {
 public:
  using value_type = TVALUE;
  using context_type = task_context<value_type, TPRIVATE_DATA>;
  using id_type = typename context_type::id_type;
  using private_data_type = typename context_type::private_data_type;
  using context_pointer_type = std::shared_ptr<context_type>;
  using task_status_type = typename context_type::task_status_type;
  using promise_flag = typename context_type::promise_flag;

 public:
  task_future_base() noexcept = default;

  task_future_base(context_pointer_type context) noexcept : context_{context} {
    COPP_LIKELY_IF (context_) {
      ++context_->future_counter_;
    }
  }

  task_future_base(const task_future_base& other) noexcept : context_{other.context_} {
    COPP_LIKELY_IF (context_) {
      ++context_->future_counter_;
    }
  }

  task_future_base(task_future_base&& other) noexcept : context_{std::move(other.context_)} { other.context_.reset(); }

  task_future_base& operator=(const task_future_base& other) noexcept {
    assign(other);
    return *this;
  }

  task_future_base& operator=(task_future_base&& other) noexcept {
    assign(std::move(other));
    return *this;
  }

  ~task_future_base() { reset(); }

  void assign(const task_future_base& other) noexcept {
    if (this == &other || context_ == other.context_) {
      return;
    }

    reset();

    COPP_LIKELY_IF (other.context_) {
      ++other.context_->future_counter_;
    }
    context_ = other.context_;
  }

  void assign(task_future_base&& other) noexcept {
    if (this == &other || context_ == other.context_) {
      return;
    }

    reset();
    context_.swap(other.context_);

    return;
  }

  void reset() {
    if (context_) {
      size_t future_counter = --context_->future_counter_;
      // Destroy context when future_counter decrease to 0
      if (0 == future_counter) {
        context_->force_finish();
      }
      context_.reset();
    }
  }

  size_t get_ref_future_count() const noexcept {
    COPP_LIKELY_IF (context_) {
      return context_->future_counter_.load();
    }

    return 0;
  }

  inline task_status_type get_status() const noexcept {
    COPP_UNLIKELY_IF (!context_) {
      return task_status_type::kInvalid;
    }

    return context_->get_status();
  }

  UTIL_FORCEINLINE bool is_canceled() const noexcept { return task_status_type::kCancle == get_status(); }
  inline bool is_completed() const noexcept {
    if (false == is_exiting()) {
      return false;
    }

    if (!context_) {
      return true;
    }

    auto& handle = context_->get_handle_delegate().handle;
    COPP_UNLIKELY_IF (!handle) {
      return true;
    }

    if (handle.done()) {
      return true;
    }

    auto promise = context_->get_handle_delegate().promise;
    COPP_UNLIKELY_IF (nullptr == promise) {
      return true;
    }

    if (promise->check_flag(promise_flag::kFinalSuspend)) {
      return true;
    }

    return false;
  }

  UTIL_FORCEINLINE bool is_faulted() const noexcept { return task_status_type::kKilled <= get_status(); }
  UTIL_FORCEINLINE bool is_timeout() const noexcept { return task_status_type::kTimeout <= get_status(); }
  UTIL_FORCEINLINE bool is_exiting() const noexcept {
    task_status_type status = get_status();
    return task_status_type::kDone <= status || task_status_type::kInvalid == status;
  }

  UTIL_FORCEINLINE static auto yield_status() noexcept {
    return LIBCOPP_COPP_NAMESPACE_ID::promise_base_type::pick_current_status();
  }

  /**
   * @brief Custom start run callable
   * @note This function should not be called when it's co_await by another callable or task
   *
   * @return true if start run success
   */
  bool start() noexcept {
    COPP_UNLIKELY_IF (!context_) {
      return false;
    }

    auto& handle = context_->get_handle_delegate().handle;
    COPP_UNLIKELY_IF (!handle) {
      return false;
    }

    if (handle.done()) {
      return false;
    }

    auto promise = context_->get_handle_delegate().promise;
    COPP_UNLIKELY_IF (nullptr == promise) {
      return false;
    }

    task_status_type expect_status = task_status_type::kCreated;
    if (!promise->set_status(task_status_type::kRunning, &expect_status)) {
      return false;
    }

    if (!promise->check_flag(promise_flag::kFinalSuspend) && !promise->check_flag(promise_flag::kDestroying)) {
      handle.resume();
    }

    return true;
  }

  /**
   * @brief Kill callable
   * @param target_status status to set
   * @param force_resume force resume and ignore if there is no waiting handle
   * @note This function is safe only when bith call and callee are copp components
   *
   * @return true if killing or killed
   */
  bool kill(task_status_type target_status = task_status_type::kKilled, bool force_resume = false) noexcept {
    if (target_status < task_status_type::kDone) {
      return false;
    }

    COPP_UNLIKELY_IF (!context_) {
      return false;
    }

    auto& handle = context_->get_handle_delegate().handle;
    COPP_UNLIKELY_IF (!handle) {
      return false;
    }

    bool ret = true;
    while (context_) {
      COPP_UNLIKELY_IF (!handle) {
        ret = false;
        break;
      }

      if (handle.done()) {
        ret = false;
        break;
      }

      task_status_type current_status = get_status();
      if (current_status >= task_status_type::kDone) {
        ret = false;
        break;
      }

      auto promise = context_->get_handle_delegate().promise;
      COPP_UNLIKELY_IF (nullptr == promise) {
        return false;
      }

      if (!promise->set_status(target_status, &current_status)) {
        continue;
      }

      if ((force_resume || promise->is_waiting()) && !promise->check_flag(promise_flag::kFinalSuspend) &&
          !promise->check_flag(promise_flag::kDestroying)) {
        handle.resume();
      }
      break;
    }

    return ret;
  }

  UTIL_FORCEINLINE bool kill(bool force_resume) { return kill(promise_flag::kKilled, force_resume); }

  UTIL_FORCEINLINE bool cancel(bool force_resume = false) { return kill(promise_flag::kCancle, force_resume); }

  UTIL_FORCEINLINE const context_pointer_type& get_context() const noexcept { return context_; }

  UTIL_FORCEINLINE context_pointer_type& get_context() noexcept { return context_; }

  inline id_type get_id() const noexcept {
    COPP_UNLIKELY_IF (!context_) {
      return 0;
    }

    return context_->get_id();
  }

 private:
  context_pointer_type context_;
};

template <class TVALUE, class TPRIVATE_DATA, bool NO_PRIVATE_DATA>
class LIBCOPP_COPP_API_HEAD_ONLY task_future_delegate;

template <class TVALUE, class TPRIVATE_DATA>
class LIBCOPP_COPP_API_HEAD_ONLY task_future_delegate<TVALUE, TPRIVATE_DATA, false>
    : public task_future_base<TVALUE, TPRIVATE_DATA> {
 public:
  using base_type = task_future_base<TVALUE, TPRIVATE_DATA>;
  using value_type = typename base_type::value_type;
  using context_type = typename base_type::context_type;
  using id_type = typename base_type::id_type;
  using private_data_type = typename base_type::private_data_type;
  using context_pointer_type = typename base_type::context_pointer_type;
  using task_status_type = typename base_type::task_status_type;
  using promise_flag = typename base_type::promise_flag;

 public:
  task_future_delegate() noexcept = default;
  task_future_delegate(context_pointer_type context) noexcept : base_type{context} {}
  task_future_delegate(const task_future_delegate& other) noexcept : base_type{other} {}
  task_future_delegate(task_future_delegate&& other) noexcept : base_type{std::move(other)} {}
  task_future_delegate& operator=(const task_future_delegate& other) noexcept {
    assign(other);
    return *this;
  }

  task_future_delegate& operator=(task_future_delegate&& other) noexcept {
    assign(std::move(other));
    return *this;
  }

  using base_type::assign;
  using base_type::get_context;

  inline private_data_type* get_private_data() noexcept {
    COPP_UNLIKELY_IF (!get_context()) {
      return nullptr;
    }

    return &get_context()->get_private_data();
  }

  inline const private_data_type* get_private_data() const noexcept {
    COPP_UNLIKELY_IF (!get_context()) {
      return nullptr;
    }

    return &get_context()->get_private_data();
  }
};

template <class TVALUE, class TPRIVATE_DATA>
class LIBCOPP_COPP_API_HEAD_ONLY task_future_delegate<TVALUE, TPRIVATE_DATA, true>
    : public task_future_base<TVALUE, TPRIVATE_DATA> {
 public:
  using base_type = task_future_base<TVALUE, TPRIVATE_DATA>;
  using value_type = typename base_type::value_type;
  using context_type = typename base_type::context_type;
  using id_type = typename base_type::id_type;
  using private_data_type = typename base_type::private_data_type;
  using context_pointer_type = typename base_type::context_pointer_type;
  using task_status_type = typename base_type::task_status_type;
  using promise_flag = typename base_type::promise_flag;

 public:
  task_future_delegate() noexcept = default;
  task_future_delegate(context_pointer_type context) noexcept : base_type{context} {}
  task_future_delegate(const task_future_delegate& other) noexcept : base_type{other} {}
  task_future_delegate(task_future_delegate&& other) noexcept : base_type{std::move(other)} {}
  task_future_delegate& operator=(const task_future_delegate& other) noexcept {
    assign(other);
    return *this;
  }

  task_future_delegate& operator=(task_future_delegate&& other) noexcept {
    assign(std::move(other));
    return *this;
  }

  using base_type::assign;
  using base_type::get_context;
};

template <class TVALUE, class TPRIVATE_DATA>
class LIBCOPP_COPP_API_HEAD_ONLY task_future
    : public task_future_delegate<TVALUE, TPRIVATE_DATA, std::is_same<TPRIVATE_DATA, void>::value> {
 public:
  using self_type = task_future<TVALUE, TPRIVATE_DATA>;
  using base_type = task_future_delegate<TVALUE, TPRIVATE_DATA, std::is_same<TPRIVATE_DATA, void>::value>;
  using value_type = typename base_type::value_type;
  using context_type = typename base_type::context_type;
  using id_type = typename base_type::id_type;
  using private_data_type = typename base_type::private_data_type;
  using context_pointer_type = typename base_type::context_pointer_type;
  using task_status_type = typename base_type::task_status_type;
  using promise_flag = typename base_type::promise_flag;

  using promise_base_type =
      task_promise_base<value_type, private_data_type, std::is_void<typename std::decay<value_type>::type>::value>;
  class promise_type : public promise_base_type {
   public:
#  if defined(__GNUC__) && !defined(__clang__)
    template <class... TARGS>
    promise_type(TARGS&&... args) : promise_base_type(args...) {}
#  else
    template <class... TARGS>
    promise_type(TARGS&&... args) : promise_base_type(std::forward<TARGS>(args)...) {}
#  endif

    using promise_base_type::get_context;

    auto get_return_object() noexcept { return self_type{get_context()}; }

    struct initial_awaitable {
      inline bool await_ready() const noexcept { return false; }

      inline void await_resume() const noexcept {
        if (handle.promise().get_status() == task_status_type::kCreated) {
          task_status_type excepted = task_status_type::kCreated;
          handle.promise().set_status(task_status_type::kRunning, &excepted);
        }
      }

      inline void await_suspend(LIBCOPP_MACRO_STD_COROUTINE_NAMESPACE coroutine_handle<promise_type> caller) noexcept {
        handle = caller;
        caller.promise().initialize_promise(caller);
      }

      LIBCOPP_MACRO_STD_COROUTINE_NAMESPACE coroutine_handle<promise_type> handle;
    };
    initial_awaitable initial_suspend() noexcept { return {}; }
#  if defined(LIBCOPP_MACRO_ENABLE_STD_EXCEPTION_PTR) && LIBCOPP_MACRO_ENABLE_STD_EXCEPTION_PTR
    void unhandled_exception() { get_context()->last_exception_ = std::current_exception(); }
#  endif

    template <class TCONVERT_PRIVATE_DATA>
    inline task_private_data<TCONVERT_PRIVATE_DATA> yield_value(
        task_private_data<TCONVERT_PRIVATE_DATA>&& input) noexcept {
      if (get_context()) {
        input.data_ = &get_context()->get_private_data();
      } else {
        input.data_ = nullptr;
      }
      return task_private_data<TCONVERT_PRIVATE_DATA>(input.data_);
    }

    using promise_base_type::yield_value;
  };
  using handle_type = LIBCOPP_MACRO_STD_COROUTINE_NAMESPACE coroutine_handle<promise_type>;
  using awaitable_type = task_awaitable<context_type, std::is_void<typename std::decay<value_type>::type>::value>;

 public:
  task_future() noexcept = default;
  task_future(context_pointer_type context) noexcept : base_type{context} {}
  task_future(const task_future& other) noexcept : base_type{other} {}
  task_future(task_future&& other) noexcept : base_type{std::move(other)} {}
  task_future& operator=(const task_future& other) noexcept {
    assign(other);
    return *this;
  }

  task_future& operator=(task_future&& other) noexcept {
    assign(std::move(other));
    return *this;
  }

  using base_type::assign;
  using base_type::get_context;

  inline awaitable_type operator co_await() { return awaitable_type{get_context().get()}; }

 private:
  template <class TTHENABLE_VALUE>
  struct _thenable_return_traits_value_type;

  template <class TCALLABLE_VALUE>
  struct _thenable_return_traits_value_type<LIBCOPP_COPP_NAMESPACE_ID::callable_future<TCALLABLE_VALUE>>
      : public std::true_type {
    using type = typename LIBCOPP_COPP_NAMESPACE_ID::callable_future<TCALLABLE_VALUE>::value_type;

    template <class TINPUT>
    UTIL_FORCEINLINE static LIBCOPP_COPP_NAMESPACE_ID::callable_future<TCALLABLE_VALUE> start_thenable(TINPUT&& c) {
      return {std::forward<TINPUT>(c)};
    }
  };

  template <class TTASK_VALUE, class TTASK_PRIVATE_DATA>
  struct _thenable_return_traits_value_type<task_future<TTASK_VALUE, TTASK_PRIVATE_DATA>> : public std::true_type {
    using type = typename task_future<TTASK_VALUE, TTASK_PRIVATE_DATA>::value_type;

    template <class TINPUT>
    UTIL_FORCEINLINE static task_future<TTASK_VALUE, TTASK_PRIVATE_DATA> start_thenable(TINPUT&& task) {
      task.start();
      return {std::forward<TINPUT>(task)};
    }
  };

  template <class TTHENABLE_VALUE>
  struct _thenable_return_traits_value_type : public std::false_type {
    using type = TTHENABLE_VALUE;
  };

  template <class TTHENABLE, bool TASK_RETURN_VOID>
  struct _thenable_return_traits_return_type;

  template <class TTHENABLE>
  struct _thenable_return_traits_return_type<TTHENABLE, true>
      : public _thenable_return_traits_value_type<typename std::invoke_result<TTHENABLE, context_pointer_type>::type> {
  };

  template <class TTHENABLE>
  struct _thenable_return_traits_return_type<TTHENABLE, false>
      : public _thenable_return_traits_value_type<
            typename std::invoke_result<TTHENABLE, context_pointer_type, value_type>::type> {};

  template <class TTHENABLE>
  struct thenable_return_traits
      : public _thenable_return_traits_return_type<TTHENABLE, std::is_void<value_type>::value> {};

  template <class TTHENABLE, bool THENABLE_RETURN_COROUTINE, bool THENABLE_RETURN_VOID, bool TASK_RETURN_VOID>
  struct thenable_traits;

  template <class TTHENABLE>
  struct thenable_traits<TTHENABLE, true, false, false> {
    using callable_thenable_type =
        LIBCOPP_COPP_NAMESPACE_ID::callable_future<typename thenable_return_traits<TTHENABLE>::type>;

    template <class TTHENABLE_PRIVATE_DATA>
    struct task_thenable_type {
      using task_type = task_future<typename thenable_return_traits<TTHENABLE>::type, TTHENABLE_PRIVATE_DATA>;
    };

    inline static callable_thenable_type invoke_callable(self_type self, TTHENABLE thenable) {
      context_pointer_type ctx = self.get_context();
      co_return (co_await thenable_return_traits<TTHENABLE>::start_thenable(thenable(std::move(ctx), co_await self)));
    }

    template <class TTHENABLE_PRIVATE_DATA>
    inline static typename task_thenable_type<TTHENABLE_PRIVATE_DATA>::task_type invoke_task(self_type self,
                                                                                             TTHENABLE thenable) {
      context_pointer_type ctx = self.get_context();
      co_return (co_await thenable_return_traits<TTHENABLE>::start_thenable(thenable(std::move(ctx), co_await self)));
    }
  };

  template <class TTHENABLE>
  struct thenable_traits<TTHENABLE, true, false, true> {
    using callable_thenable_type =
        LIBCOPP_COPP_NAMESPACE_ID::callable_future<typename thenable_return_traits<TTHENABLE>::type>;

    template <class TTHENABLE_PRIVATE_DATA>
    struct task_thenable_type {
      using task_type = task_future<typename thenable_return_traits<TTHENABLE>::type, TTHENABLE_PRIVATE_DATA>;
    };

    inline static callable_thenable_type invoke_callable(self_type self, TTHENABLE thenable) {
      context_pointer_type ctx = self.get_context();
      co_await self;
      co_return (co_await thenable_return_traits<TTHENABLE>::start_thenable(thenable(std::move(ctx))));
    }

    template <class TTHENABLE_PRIVATE_DATA>
    inline static typename task_thenable_type<TTHENABLE_PRIVATE_DATA>::task_type invoke_task(self_type self,
                                                                                             TTHENABLE thenable) {
      context_pointer_type ctx = self.get_context();
      co_await self;
      co_return (co_await thenable_return_traits<TTHENABLE>::start_thenable(thenable(std::move(ctx))));
    }
  };

  template <class TTHENABLE>
  struct thenable_traits<TTHENABLE, true, true, false> {
    using callable_thenable_type =
        LIBCOPP_COPP_NAMESPACE_ID::callable_future<typename thenable_return_traits<TTHENABLE>::type>;

    template <class TTHENABLE_PRIVATE_DATA>
    struct task_thenable_type {
      using task_type = task_future<typename thenable_return_traits<TTHENABLE>::type, TTHENABLE_PRIVATE_DATA>;
    };

    inline static callable_thenable_type invoke_callable(self_type self, TTHENABLE thenable) {
      context_pointer_type ctx = self.get_context();
      co_await thenable_return_traits<TTHENABLE>::start_thenable(thenable(std::move(ctx), co_await self));
      co_return;
    }

    template <class TTHENABLE_PRIVATE_DATA>
    inline static typename task_thenable_type<TTHENABLE_PRIVATE_DATA>::task_type invoke_task(self_type self,
                                                                                             TTHENABLE thenable) {
      context_pointer_type ctx = self.get_context();
      co_await thenable_return_traits<TTHENABLE>::start_thenable(thenable(std::move(ctx), co_await self));
      co_return;
    }
  };

  template <class TTHENABLE>
  struct thenable_traits<TTHENABLE, true, true, true> {
    using callable_thenable_type =
        LIBCOPP_COPP_NAMESPACE_ID::callable_future<typename thenable_return_traits<TTHENABLE>::type>;

    template <class TTHENABLE_PRIVATE_DATA>
    struct task_thenable_type {
      using task_type = task_future<typename thenable_return_traits<TTHENABLE>::type, TTHENABLE_PRIVATE_DATA>;
    };

    inline static callable_thenable_type invoke_callable(self_type self, TTHENABLE thenable) {
      context_pointer_type ctx = self.get_context();
      co_await self;
      co_await thenable_return_traits<TTHENABLE>::start_thenable(thenable(std::move(ctx)));
      co_return;
    }

    template <class TTHENABLE_PRIVATE_DATA>
    inline static typename task_thenable_type<TTHENABLE_PRIVATE_DATA>::task_type invoke_task(self_type self,
                                                                                             TTHENABLE thenable) {
      context_pointer_type ctx = self.get_context();
      co_await self;
      co_await thenable_return_traits<TTHENABLE>::start_thenable(thenable(std::move(ctx)));
      co_return;
    }
  };

  template <class TTHENABLE>
  struct thenable_traits<TTHENABLE, false, true, false> {
    using callable_thenable_type =
        LIBCOPP_COPP_NAMESPACE_ID::callable_future<typename thenable_return_traits<TTHENABLE>::type>;

    template <class TTHENABLE_PRIVATE_DATA>
    struct task_thenable_type {
      using task_type = task_future<typename thenable_return_traits<TTHENABLE>::type, TTHENABLE_PRIVATE_DATA>;
    };

    inline static callable_thenable_type invoke_callable(self_type self, TTHENABLE thenable) {
      context_pointer_type ctx = self.get_context();
      thenable(std::move(ctx), co_await self);
      co_return;
    }

    template <class TTHENABLE_PRIVATE_DATA>
    inline static typename task_thenable_type<TTHENABLE_PRIVATE_DATA>::task_type invoke_task(self_type self,
                                                                                             TTHENABLE thenable) {
      context_pointer_type ctx = self.get_context();
      thenable(std::move(ctx), co_await self);
      co_return;
    }
  };

  template <class TTHENABLE>
  struct thenable_traits<TTHENABLE, false, true, true> {
    using callable_thenable_type =
        LIBCOPP_COPP_NAMESPACE_ID::callable_future<typename thenable_return_traits<TTHENABLE>::type>;

    template <class TTHENABLE_PRIVATE_DATA>
    struct task_thenable_type {
      using task_type = task_future<typename thenable_return_traits<TTHENABLE>::type, TTHENABLE_PRIVATE_DATA>;
    };

    inline static callable_thenable_type invoke_callable(self_type self, TTHENABLE thenable) {
      context_pointer_type ctx = self.get_context();
      co_await self;
      thenable(std::move(ctx));
      co_return;
    }

    template <class TTHENABLE_PRIVATE_DATA>
    inline static typename task_thenable_type<TTHENABLE_PRIVATE_DATA>::task_type invoke_task(self_type self,
                                                                                             TTHENABLE thenable) {
      context_pointer_type ctx = self.get_context();
      co_await self;
      thenable(std::move(ctx));
      co_return;
    }
  };

  template <class TTHENABLE>
  struct thenable_traits<TTHENABLE, false, false, false> {
    using callable_thenable_type =
        LIBCOPP_COPP_NAMESPACE_ID::callable_future<typename thenable_return_traits<TTHENABLE>::type>;

    template <class TTHENABLE_PRIVATE_DATA>
    struct task_thenable_type {
      using task_type = task_future<typename thenable_return_traits<TTHENABLE>::type, TTHENABLE_PRIVATE_DATA>;
    };

    inline static callable_thenable_type invoke_callable(self_type self, TTHENABLE thenable) {
      context_pointer_type ctx = self.get_context();
      co_return thenable(std::move(ctx), co_await self);
    }

    template <class TTHENABLE_PRIVATE_DATA>
    inline static typename task_thenable_type<TTHENABLE_PRIVATE_DATA>::task_type invoke_task(self_type self,
                                                                                             TTHENABLE thenable) {
      context_pointer_type ctx = self.get_context();
      co_return thenable(std::move(ctx), co_await self);
    }
  };

  template <class TTHENABLE>
  struct thenable_traits<TTHENABLE, false, false, true> {
    using callable_thenable_type =
        LIBCOPP_COPP_NAMESPACE_ID::callable_future<typename thenable_return_traits<TTHENABLE>::type>;

    template <class TTHENABLE_PRIVATE_DATA>
    struct task_thenable_type {
      using task_type = task_future<typename thenable_return_traits<TTHENABLE>::type, TTHENABLE_PRIVATE_DATA>;
    };

    inline static callable_thenable_type invoke_callable(self_type self, TTHENABLE thenable) {
      context_pointer_type ctx = self.get_context();
      co_await self;
      co_return thenable(std::move(ctx));
    }

    template <class TTHENABLE_PRIVATE_DATA>
    inline static typename task_thenable_type<TTHENABLE_PRIVATE_DATA>::task_type invoke_task(self_type self,
                                                                                             TTHENABLE thenable) {
      context_pointer_type ctx = self.get_context();
      co_await self;
      co_return thenable(std::move(ctx));
    }
  };

 public:
  template <class TTHENABLE>
  struct callable_thenable_trait {
    using trait_type = thenable_traits<
        typename std::decay<TTHENABLE>::type, thenable_return_traits<typename std::decay<TTHENABLE>::type>::value,
        std::is_same<typename thenable_return_traits<typename std::decay<TTHENABLE>::type>::type, void>::value,
        std::is_void<value_type>::value>;

    using callable_thenable_type = typename trait_type::callable_thenable_type;
  };

  template <class TTHENABLE, class TTHENABLE_PRIVATE_DATA>
  struct task_thenable_trait {
    using trait_type = thenable_traits<
        typename std::decay<TTHENABLE>::type, thenable_return_traits<typename std::decay<TTHENABLE>::type>::value,
        std::is_same<typename thenable_return_traits<typename std::decay<TTHENABLE>::type>::type, void>::value,
        std::is_void<value_type>::value>;

    using task_thenable_wrapper = typename trait_type::template task_thenable_type<TTHENABLE_PRIVATE_DATA>;
    using task_thenable_type = typename task_thenable_wrapper::task_type;
  };

  /**
   * @brief Run callback after this task is done.
   *
   * @param thenable callback which can return callable_future<T>, task_future<T> or custom types.
   *   The parameter of callback is (task_future<value_type>::context_pointer_type) when return type is
   *   void, or (task_future<value_type>::context_pointer_type, T&&) when return type is T.
   *
   * @example
   *  task_future<int> t = create_task();
   *  auto st1 = t.then([](task_future<int>::context_pointer_type&&, int v) { return 1 + v; });
   *  auto st2 = t.then([](task_future<int>::context_pointer_type, int v) -> callable_future<int> { co_return 2 + v; });
   *  t.start();
   *
   * @example
   *  task_future<void> t = create_task();
   *  auto st1 = t.then([](task_future<int>::context_pointer_type&&) { return; });
   *  auto st2 = t.then([](task_future<int>::context_pointer_type) -> task_future<void> { co_return; });
   *  t.start();
   *
   * @return callable_future<T>
   */
  template <class TTHENABLE>
  typename callable_thenable_trait<TTHENABLE>::callable_thenable_type then(TTHENABLE&& thenable) {
    using trait_type = typename callable_thenable_trait<TTHENABLE>::trait_type;
    return trait_type::invoke_callable(*this, std::forward<TTHENABLE>(thenable));
  }

  /**
   * @brief Run callback after this task is done.
   *
   * @param thenable callback which can return callable_future<T>, task_future<T> or custom types.
   *   The parameter of callback is (task_future<value_type>::context_pointer_type) when return type is
   *   void, or (task_future<value_type>::context_pointer_type, T&&) when return type is T.
   * @param private_data private data of task
   *
   * @example
   *  task_future<int> t = create_task();
   *  auto sub_task = t
   *    .then([](task_future<int>::context_pointer_type&&, int v) { return v; }, 1);
   *    .then([](task_future<int, int>::context_pointer_type ctx, int v) -> callable_future<int> {
   *       co_return v + ctx.get_private_data();
   *    }, 20);
   *  t.start();
   *
   * @example
   *  task_future<void> t = create_task();
   *  auto sub_task = t
   *    .then([](task_future<int>::context_pointer_type&&) { return; }, nullptr);
   *    .then([](task_future<int>::context_pointer_type) -> task_future<void> { co_return; }, nullptr);
   *  t.start();
   *
   * @return task_future<T>
   */
  template <class TTHENABLE, class TTHENABLE_PRIVATE_DATA>
  typename task_thenable_trait<TTHENABLE, typename std::decay<TTHENABLE_PRIVATE_DATA>::type>::task_thenable_type then(
      TTHENABLE&& thenable, TTHENABLE_PRIVATE_DATA&& private_data) {
    using trait_type =
        typename task_thenable_trait<TTHENABLE, typename std::decay<TTHENABLE_PRIVATE_DATA>::type>::trait_type;
    auto result = trait_type::template invoke_task<typename std::decay<TTHENABLE_PRIVATE_DATA>::type>(
        *this, std::forward<TTHENABLE>(thenable));
    *result.get_private_data() = std::forward<TTHENABLE_PRIVATE_DATA>(private_data);
    result.start();
    return result;
  }

  /**
   * @brief Run callback after this task is done.
   *
   * @param thenable callback which can return callable_future<T>, task_future<T> or custom types.
   *   The parameter of callback is (task_future<value_type>::context_pointer_type) when return type is
   *   void, or (task_future<value_type>::context_pointer_type, T&&) when return type is T.
   * @param nullptr_t this task has no private data
   *
   * @example
   *  task_future<int> t = create_task();
   *  auto sub_task = t
   *    .then([](task_future<int>::context_pointer_type&&, int v) { return v; }, 1);
   *    .then([](task_future<int, int>::context_pointer_type ctx, int v) -> callable_future<int> {
   *       co_return v + ctx.get_private_data();
   *    }, 20);
   *  t.start();
   *
   * @example
   *  task_future<void> t = create_task();
   *  auto sub_task = t
   *    .then([](task_future<int>::context_pointer_type&&) { return; }, nullptr);
   *    .then([](task_future<int>::context_pointer_type) -> task_future<void> { co_return; }, nullptr);
   *  t.start();
   *
   * @return task_future<T>
   */
  template <class TTHENABLE>
  typename task_thenable_trait<TTHENABLE, void>::task_thenable_type then(TTHENABLE&& thenable, std::nullptr_t) {
    using trait_type = typename task_thenable_trait<TTHENABLE, void>::trait_type;
    auto result = trait_type::template invoke_task<void>(*this, std::forward<TTHENABLE>(thenable));
    result.start();
    return result;
  }
};

// some
template <class TVALUE, class TPRIVATE_DATA>
struct LIBCOPP_COPP_API_HEAD_ONLY some_delegate_task_action {
  using future_type = task_future<TVALUE, TPRIVATE_DATA>;
  using context_type = LIBCOPP_COPP_NAMESPACE_ID::some_delegate_context<future_type>;

  inline static void suspend_future(const LIBCOPP_COPP_NAMESPACE_ID::promise_caller_manager::handle_delegate& caller,
                                    future_type& task_object) {
    if (task_object.get_context()) {
      task_object.get_context()->add_caller(caller);
    }
  }

  inline static void resume_future(const LIBCOPP_COPP_NAMESPACE_ID::promise_caller_manager::handle_delegate& caller,
                                   future_type& task_object) {
    if (task_object.get_context()) {
      task_object.get_context()->remove_caller(caller);
    }
  }

  inline static bool is_pending(future_type& future_object) noexcept { return !future_object.is_exiting(); }
};

LIBCOPP_COTASK_NAMESPACE_END

LIBCOPP_COPP_NAMESPACE_BEGIN
// some
template <class TVALUE, class TPRIVATE_DATA>
class LIBCOPP_COPP_API_HEAD_ONLY some_delegate<cotask::task_future<TVALUE, TPRIVATE_DATA>>
    : public some_delegate_base<cotask::task_future<TVALUE, TPRIVATE_DATA>,
                                cotask::some_delegate_task_action<TVALUE, TPRIVATE_DATA>> {
 public:
  using base_type = some_delegate_base<cotask::task_future<TVALUE, TPRIVATE_DATA>,
                                       cotask::some_delegate_task_action<TVALUE, TPRIVATE_DATA>>;
  using future_type = typename base_type::future_type;
  using value_type = typename base_type::value_type;
  using ready_output_type = typename base_type::ready_output_type;
  using context_type = typename base_type::context_type;

  using base_type::run;
};

LIBCOPP_COPP_NAMESPACE_END

#endif

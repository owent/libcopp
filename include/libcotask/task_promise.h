// Copyright 2022 owent

#pragma once

#include <libcopp/utils/config/libcopp_build_features.h>

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

  UTIL_FORCEINLINE void add_caller(handle_delegate handle) noexcept { caller_manager_.add_caller(handle); }

#  if defined(LIBCOPP_MACRO_ENABLE_CONCEPTS) && LIBCOPP_MACRO_ENABLE_CONCEPTS
  template <LIBCOPP_COPP_NAMESPACE_ID::DerivedPromiseBaseType TPROMISE>
#  else
  template <class TPROMISE, typename = std::enable_if_t<
                                std::is_base_of<LIBCOPP_COPP_NAMESPACE_ID::promise_base_type, TPROMISE>::value> >
#  endif
  UTIL_FORCEINLINE LIBCOPP_COPP_API_HEAD_ONLY void add_caller(
      const LIBCOPP_MACRO_STD_COROUTINE_NAMESPACE coroutine_handle<TPROMISE>& handle) noexcept {
    add_caller(handle_delegate{handle});
  }

  UTIL_FORCEINLINE void remove_caller(handle_delegate handle) noexcept { caller_manager_.remove_caller(handle); }

#  if defined(LIBCOPP_MACRO_ENABLE_CONCEPTS) && LIBCOPP_MACRO_ENABLE_CONCEPTS
  template <LIBCOPP_COPP_NAMESPACE_ID::DerivedPromiseBaseType TPROMISE>
#  else
  template <class TPROMISE, typename = std::enable_if_t<
                                std::is_base_of<LIBCOPP_COPP_NAMESPACE_ID::promise_base_type, TPROMISE>::value> >
#  endif
  UTIL_FORCEINLINE LIBCOPP_COPP_API_HEAD_ONLY void remove_caller(
      const LIBCOPP_MACRO_STD_COROUTINE_NAMESPACE coroutine_handle<TPROMISE>& handle, bool inherit_status) noexcept {
    remove_caller(handle_delegate{handle}, inherit_status);
  }

  UTIL_FORCEINLINE bool has_multiple_callers() const noexcept { return caller_manager_.has_multiple_callers(); }

 protected:
  UTIL_FORCEINLINE void wake() { caller_manager_.resume_callers(); }

 private:
  template <class, class, bool>
  friend class LIBCOPP_COPP_API_HEAD_ONLY task_promise_base;

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

  using base_type::add_caller;
  using base_type::is_pending;
  using base_type::is_ready;
  using base_type::remove_caller;

  UTIL_FORCEINLINE void set_value() {
    data_.reset_data(true);
    wake();
  }

 private:
  using base_type::data_;
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
  using base_type::add_caller;
  using base_type::is_pending;
  using base_type::is_ready;
  using base_type::remove_caller;

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
  using base_type::data_;
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
      public std::enable_shared_from_this<task_context<TVALUE, void> > {
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
      public std::enable_shared_from_this<task_context<TVALUE, TPRIVATE_DATA> > {
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
                                std::is_base_of<task_promise_base<TVALUE, TPRIVATE_DATA, true>, TPROMISE>::value> >
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
                                std::is_base_of<task_promise_base<TVALUE, TPRIVATE_DATA, false>, TPROMISE>::value> >
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
                                 std::is_base_of<LIBCOPP_COPP_NAMESPACE_ID::promise_base_type, TCPROMISE>::value> >
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
  inline task_status_type detach() noexcept {
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
        return std::move(*get_context()->data());
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
class LIBCOPP_COPP_API_HEAD_ONLY task_future {
 public:
  using value_type = TVALUE;
  using self_type = task_future<value_type, TPRIVATE_DATA>;
  using context_type = task_context<value_type, TPRIVATE_DATA>;
  using id_type = typename context_type::id_type;
  using private_data_type = typename context_type::private_data_type;
  using context_pointer_type = std::shared_ptr<context_type>;
  using task_status_type = typename context_type::task_status_type;
  using promise_flag = typename context_type::promise_flag;

  using promise_base_type =
      task_promise_base<value_type, private_data_type, std::is_void<typename std::decay<value_type>::type>::value>;
  class promise_type : public promise_base_type {
   public:
    template <class... TARGS>
    promise_type(TARGS&&... args) : promise_base_type(std::forward<TARGS>(args)...) {}

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

  task_future(context_pointer_type context) noexcept : context_{context} {
    COPP_LIKELY_IF (context_) {
      ++context_->future_counter_;
    }
  }

  task_future(const task_future& other) noexcept : context_{other.context_} {
    COPP_LIKELY_IF (context_) {
      ++context_->future_counter_;
    }
  }

  task_future(task_future&& other) noexcept : context_{std::move(other.context_)} { other.context_.reset(); }

  task_future& operator=(const task_future& other) noexcept {
    if (this == &other || context_ == other.context_) {
      return *this;
    }

    reset();

    COPP_LIKELY_IF (other.context_) {
      ++other.context_->future_counter_;
    }
    context_ = other.context_;

    return *this;
  }

  task_future& operator=(task_future&& other) noexcept {
    if (this == &other || context_ == other.context_) {
      return *this;
    }

    reset();
    context_.swap(other.context_);

    return *this;
  }

  ~task_future() { reset(); }

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

  inline awaitable_type operator co_await() { return awaitable_type{context_.get()}; }

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

  template <class = typename std::enable_if<!std::is_same<private_data_type, void>::value>::type>
  inline private_data_type* get_private_data() noexcept {
    COPP_UNLIKELY_IF (!context_) {
      return nullptr;
    }

    return &context_->get_private_data();
  }

  template <class = typename std::enable_if<!std::is_same<private_data_type, void>::value>::type>
  inline const private_data_type* get_private_data() const noexcept {
    COPP_UNLIKELY_IF (!context_) {
      return nullptr;
    }

    return &context_->get_private_data();
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

  UTIL_FORCEINLINE bool kill(bool force_resume = false) { return kill(promise_flag::kKilled, force_resume); }

  UTIL_FORCEINLINE bool cancel(bool force_resume = false) { return kill(promise_flag::kCancle, force_resume); }

  UTIL_FORCEINLINE const context_pointer_type& get_context() const noexcept { return context_; }

  UTIL_FORCEINLINE context_pointer_type& get_context() noexcept { return context_; }

  inline id_type get_id() const noexcept {
    COPP_UNLIKELY_IF (!context_) {
      return 0;
    }

    return context_->get_id();
  }

  // TODO then(RETURN (*CLAZZ::fn)(TARGS...), CLAZZ*, TARGS...)
  // TODO then(functor, TARGS&&...)

 private:
  context_pointer_type context_;
};

LIBCOPP_COTASK_NAMESPACE_END

#endif

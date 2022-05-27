// Copyright 2022 owent

#pragma once

#include <libcopp/utils/config/libcopp_build_features.h>

// clang-format off
#include <libcopp/utils/config/stl_include_prefix.h>  // NOLINT(build/include_order)
// clang-format on
#include <assert.h>

#ifdef __cpp_impl_three_way_comparison
#  include <compare>
#endif

#include <libcopp/utils/uint64_id_allocator.h>

#if defined(LIBCOPP_MACRO_ENABLE_STD_EXCEPTION_PTR) && LIBCOPP_MACRO_ENABLE_STD_EXCEPTION_PTR
#  include <exception>
#endif
// clang-format off
#include <libcopp/utils/config/stl_include_suffix.h>  // NOLINT(build/include_order)
// clang-format on

#include "libcopp/future/future.h"

LIBCOPP_COPP_NAMESPACE_BEGIN
namespace future {

#if defined(LIBCOPP_MACRO_ENABLE_STD_COROUTINE) && LIBCOPP_MACRO_ENABLE_STD_COROUTINE
struct macro_task {
  using coroutine_handle_allocator = std::allocator<LIBCOPP_MACRO_FUTURE_COROUTINE_VOID>;
};

template <class TPD>
class LIBCOPP_COPP_API_HEAD_ONLY task_context : public context<TPD> {
 public:
  using self_type = task_context<TPD>;

 private:
  // context can not be copy or moved.
  task_context(const task_context &) = delete;
  task_context &operator=(const task_context &) = delete;
  task_context(task_context &&) = delete;
  task_context &operator=(task_context &&) = delete;

 public:
  template <class... TARGS>
  task_context(uint64_t tid, TARGS &&...args) : context<TPD>(std::forward<TARGS>(args)...), task_id_(tid) {}

  inline uint64_t get_task_id() const LIBCOPP_MACRO_NOEXCEPT { return task_id_; }

 private:
  uint64_t task_id_;
};

template <class T, class TPTR = typename poll_storage_ptr_selector<T>::type>
class LIBCOPP_COPP_API_HEAD_ONLY task_future_data : public future_with_waker<T, TPTR> {
 public:
  using future_with_waker<T, TPTR>::future_with_waker;
};

template <class T, class TPD, class TPTR, class TMACRO>
class LIBCOPP_COPP_API_HEAD_ONLY task_future;

template <class T, class TPD, class TPTR, class TMACRO>
class LIBCOPP_COPP_API_HEAD_ONLY task_promise_base_type;

template <class T, class TPD, class TPTR, class TMACRO>
class LIBCOPP_COPP_API_HEAD_ONLY task_promise;

template <class T, class TPD, class TPTR, class TMACRO>
struct LIBCOPP_COPP_API_HEAD_ONLY task_runtime;

enum class LIBCOPP_COPP_API_HEAD_ONLY task_status {
  kCreated = 0,
  kRunning = 1,
  kDone = 2,
};

template <class T, class TPD, class TPTR, class TMACRO>
struct LIBCOPP_COPP_API_HEAD_ONLY task_common_types {
  using future_data_type = task_future_data<T, TPTR>;
  using poller_type = typename future_data_type::poller_type;
  using waker_type = typename future_data_type::waker_type;
  using context_type = task_context<TPD>;
  using wake_list_type =
      std::list<LIBCOPP_MACRO_FUTURE_COROUTINE_VOID, typename macro_task::coroutine_handle_allocator>;
};

template <class T, class TPD, class TPTR, class TMACRO>
struct LIBCOPP_COPP_API_HEAD_ONLY task_runtime {
  using self_type = task_promise_base_type<T, TPD, TPTR, TMACRO>;
  using future_data_type = typename task_common_types<T, TPD, TPTR, TMACRO>::future_data_type;
  using waker_type = typename task_common_types<T, TPD, TPTR, TMACRO>::waker_type;
  using poller_type = typename task_common_types<T, TPD, TPTR, TMACRO>::poller_type;
  using context_type = typename task_common_types<T, TPD, TPTR, TMACRO>::context_type;
  using wake_list_type = typename task_common_types<T, TPD, TPTR, TMACRO>::wake_list_type;

  task_runtime() : task_id(0), status(task_status::kCreated), handle(nullptr) {}

#  ifdef __cpp_impl_three_way_comparison
  friend inline std::strong_ordering operator<=>(const task_runtime &l, const task_runtime &r) LIBCOPP_MACRO_NOEXCEPT {
    return l.task_id <=> r.task_id;
  }
#  else
  friend inline bool operator!=(const task_runtime &l, const task_runtime &r) LIBCOPP_MACRO_NOEXCEPT {
    return l.task_id != r.task_id;
  }
  friend inline bool operator<(const task_runtime &l, const task_runtime &r) LIBCOPP_MACRO_NOEXCEPT {
    return l.task_id < r.task_id;
  }
  friend inline bool operator<=(const task_runtime &l, const task_runtime &r) LIBCOPP_MACRO_NOEXCEPT {
    return l.task_id <= r.task_id;
  }
  friend inline bool operator>(const task_runtime &l, const task_runtime &r) LIBCOPP_MACRO_NOEXCEPT {
    return l.task_id > r.task_id;
  }
  friend inline bool operator>=(const task_runtime &l, const task_runtime &r) LIBCOPP_MACRO_NOEXCEPT {
    return l.task_id >= r.task_id;
  }
#  endif

  inline bool done() const LIBCOPP_MACRO_NOEXCEPT { return status == task_status::kDone || !handle || handle.done(); }

  uint64_t task_id;
  task_status status;
  future_data_type future;
  LIBCOPP_MACRO_STD_COROUTINE_NAMESPACE
  coroutine_handle<task_promise<T, TPD, TPTR, TMACRO> > handle;
#  if defined(LIBCOPP_MACRO_ENABLE_STD_EXCEPTION_PTR) && LIBCOPP_MACRO_ENABLE_STD_EXCEPTION_PTR
  std::exception_ptr unhandle_exception;
#  endif
 private:
  task_runtime(const task_runtime &) = delete;
  task_runtime &operator=(const task_runtime &) = delete;
  task_runtime(task_runtime &&) = delete;
  task_runtime &operator=(task_runtime &&) = delete;
};

template <class TPROMISE>
struct task_waker {
  using context_type = typename TPROMISE::context_type;
  std::shared_ptr<typename TPROMISE::runtime_type> runtime;
  task_waker(std::shared_ptr<typename TPROMISE::runtime_type> &r) : runtime(r) {}

  void operator()(context_type &ctx) {
    // if waker->self == nullptr, the future is already destroyed, then handle is also invalid
    COPP_LIKELY_IF(runtime) {
      if (!runtime->done() && !runtime->future.is_ready()) {
        runtime->future.template poll_as<typename TPROMISE::future_data_type>(ctx);
      }

      // once set ready, it must be polled to the end
      if (runtime->done() || runtime->future.is_ready()) {
        while (runtime->handle && !runtime->handle.done()) {
          runtime->handle.resume();
        }
      }
    }
    // TODO check type
  }

  template <class UPD>
  void operator()(context<UPD> &ctx) {
    static_assert(!std::is_same<context_type, context<UPD> >::value, "task context type must be drive of task_context");

    (*this)(*static_cast<context_type *>(&ctx));
  }
};

template <class T, class TPD, class TPTR, class TMACRO>
class LIBCOPP_COPP_API_HEAD_ONLY task_promise_base_type {
 public:
  using runtime_type = task_runtime<T, TPD, TPTR, TMACRO>;
  using self_type = task_promise_base_type<T, TPD, TPTR, TMACRO>;
  using future_data_type = typename task_common_types<T, TPD, TPTR, TMACRO>::future_data_type;
  using waker_type = typename task_common_types<T, TPD, TPTR, TMACRO>::waker_type;
  using poller_type = typename task_common_types<T, TPD, TPTR, TMACRO>::poller_type;
  using context_type = typename task_common_types<T, TPD, TPTR, TMACRO>::context_type;
  using wake_list_type = typename task_common_types<T, TPD, TPTR, TMACRO>::wake_list_type;

 private:
  // ================= C++20 Coroutine Support =================
  struct initial_awaitable {
    self_type *promise;
    initial_awaitable(self_type &s) : promise(&s) {}

    inline bool await_ready() const LIBCOPP_MACRO_NOEXCEPT {
      // only setup handle once
      // return promise && promise->runtime_ && promise->runtime_->handle;
      return false;
    }

    template <typename U>
    void await_suspend(LIBCOPP_MACRO_STD_COROUTINE_NAMESPACE coroutine_handle<U> handle) {
      COPP_LIKELY_IF(promise && promise->runtime_) {
        COPP_UNLIKELY_IF(promise->runtime_->handle) { promise->runtime_->handle.resume(); }
        promise->runtime_->handle = handle;
      }

      COPP_LIKELY_IF(handle) {
        COPP_LIKELY_IF(handle.promise().runtime_) {
          runtime_type &runtime = *handle.promise().runtime_;
          handle.promise().get_context().set_wake_fn(task_waker<U>{handle.promise().runtime_});
          // Can not set waker clear functor, because even future is polled outside
          //   we still need to resume handle after event finished
          // runtime.future.set_ctx_waker(handle.promise().get_context());

          if (!runtime.future.is_ready()) {
            runtime.future.template poll_as<future_data_type>(handle.promise().get_context());
          }
        }

        handle.resume();
      }
    }

    inline void await_resume() LIBCOPP_MACRO_NOEXCEPT {
      COPP_LIKELY_IF(promise && promise->runtime_) { promise->runtime_->status = task_status::kRunning; }
    }
  };

  struct final_awaitable {
    self_type *promise;
    final_awaitable(self_type &s) : promise(&s) {}

    inline bool await_ready() const LIBCOPP_MACRO_NOEXCEPT { return true; }

    template <typename U>
    inline void await_suspend(LIBCOPP_MACRO_STD_COROUTINE_NAMESPACE coroutine_handle<U> /*handle*/)
        LIBCOPP_MACRO_NOEXCEPT {}

    void await_resume() LIBCOPP_MACRO_NOEXCEPT {
      COPP_LIKELY_IF(promise) {
        COPP_LIKELY_IF(promise->runtime_) {
          promise->runtime_->status = task_status::kDone;
          promise->runtime_->handle = nullptr;
        }
        // clear waker
        promise->get_context().set_wake_fn(nullptr);

        // wake all co_await handles
        promise->wake_all();
      }
    }
  };

 private:
  // future can not be copy or moved.
  task_promise_base_type(const task_promise_base_type &) = delete;
  task_promise_base_type &operator=(const task_promise_base_type &) = delete;
  task_promise_base_type(task_promise_base_type &&) = delete;
  task_promise_base_type &operator=(task_promise_base_type &&) = delete;

  template <class U>
  struct pick_pointer_awaitable {
    U *data;
    pick_pointer_awaitable() : data(nullptr) {}

    bool await_ready() const LIBCOPP_MACRO_NOEXCEPT { return true; }

    template <typename TPROMISE>
    void await_suspend(LIBCOPP_MACRO_STD_COROUTINE_NAMESPACE coroutine_handle<TPROMISE> /*handle*/)
        LIBCOPP_MACRO_NOEXCEPT {}

    U *await_resume() LIBCOPP_MACRO_NOEXCEPT { return data; }
  };

 public:
  template <class U>
  struct pick_context_awaitable : pick_pointer_awaitable<U> {};

  template <class U>
  struct pick_future_awaitable : pick_pointer_awaitable<U> {};

 public:
  template <class... TARGS>
  task_promise_base_type(TARGS &&...args)
      : context_(LIBCOPP_COPP_NAMESPACE_ID::util::uint64_id_allocator::allocate(), std::forward<TARGS>(args)...),
        runtime_(std::make_shared<runtime_type>()) {
    if (runtime_) {
      runtime_->task_id = context_.get_task_id();
    }
  }
  ~task_promise_base_type() {
    // printf("~task_promise_base_type %p - %s\n", this, typeid(typename future_data_type::value_type).name());
    // cleanup: await_handles_ should be already cleanup in final_awaitable::await_suspend
    wake_all();
  }

  auto initial_suspend() LIBCOPP_MACRO_NOEXCEPT { return initial_awaitable{*this}; }
  auto final_suspend() LIBCOPP_MACRO_NOEXCEPT { return final_awaitable{*this}; }

  void unhandled_exception() LIBCOPP_MACRO_NOEXCEPT {
#  if defined(LIBCOPP_MACRO_ENABLE_STD_EXCEPTION_PTR) && LIBCOPP_MACRO_ENABLE_STD_EXCEPTION_PTR
    COPP_LIKELY_IF(runtime_) { runtime_->unhandle_exception = std::current_exception(); }
#  endif
  }

  // template <class... TARGS>
  // auto await_transform(TARGS &&...) LIBCOPP_MACRO_NOEXCEPT;
  template <class TARGS>
  auto yield_value(TARGS &&args) {
    return std::forward<TARGS>(args);
  }

  template <class U>
  auto yield_value(pick_context_awaitable<U> &&args) {
    args.data = &context_;
    return std::move(args);
  }

  template <class U>
  auto yield_value(pick_future_awaitable<U> &&args) {
    COPP_LIKELY_IF(runtime_) { args.data = &runtime_->future; }
    else {
      args.data = nullptr;
    }

    return std::move(args);
  }

  inline const context_type &get_context() const LIBCOPP_MACRO_NOEXCEPT { return context_; }
  inline context_type &get_context() LIBCOPP_MACRO_NOEXCEPT { return context_; }
  inline const std::shared_ptr<runtime_type> &get_runtime() const LIBCOPP_MACRO_NOEXCEPT { return runtime_; }
  inline std::shared_ptr<runtime_type> &get_runtime() LIBCOPP_MACRO_NOEXCEPT { return runtime_; }

  inline typename wake_list_type::iterator begin_wake_handles() LIBCOPP_MACRO_NOEXCEPT {
    return await_handles_.begin();
  }
  inline typename wake_list_type::iterator end_wake_handles() LIBCOPP_MACRO_NOEXCEPT { return await_handles_.end(); }
  inline typename wake_list_type::iterator add_wake_handle(LIBCOPP_MACRO_FUTURE_COROUTINE_VOID h) {
    if (h) {
      return await_handles_.insert(await_handles_.end(), h);
    }

    return end_wake_handles();
  }
  inline void remove_wake_handle(typename wake_list_type::iterator &iter) {
    if (iter != await_handles_.end()) {
      await_handles_.erase(iter);
      iter = await_handles_.end();
    }
  }

  void wake_all() {
    for (typename wake_list_type::iterator iter = await_handles_.begin(); iter != await_handles_.end();) {
      typename wake_list_type::iterator wake_iter = iter++;
      if (*wake_iter) {
        (*wake_iter).resume();
      }
    }

    await_handles_.clear();
  }

  static inline pick_context_awaitable<context_type> current_context() {
    return pick_context_awaitable<context_type>{};
  }
  static inline pick_future_awaitable<future_data_type> current_future_data() {
    return pick_future_awaitable<future_data_type>{};
  }

 private:
  wake_list_type await_handles_;
  context_type context_;
  std::shared_ptr<runtime_type> runtime_;
};

template <class T, class TPD, class TPTR, class TMACRO>
class LIBCOPP_COPP_API_HEAD_ONLY task_promise : public task_promise_base_type<T, TPD, TPTR, TMACRO> {
 public:
  using self_type = task_promise<T, TPD, TPTR, TMACRO>;

 public:
  task_future<T, TPD, TPTR, TMACRO> get_return_object() LIBCOPP_MACRO_NOEXCEPT;
  using task_promise_base_type<T, TPD, TPTR, TMACRO>::get_runtime;

  template <class U>
  void return_value(U &&in) LIBCOPP_MACRO_NOEXCEPT {
    // Maybe set error data on custom poller, ignore co_return here.
    if (get_runtime() && !get_runtime()->future.is_ready()) {
      get_runtime()->future.poll_data() = std::forward<U>(in);
    }
  }
};

template <class TPD, class TPTR, class TMACRO>
class LIBCOPP_COPP_API_HEAD_ONLY task_promise<void, TPD, TPTR, TMACRO>
    : public task_promise_base_type<void, TPD, TPTR, TMACRO> {
 public:
  using self_type = task_promise<void, TPD, TPTR, TMACRO>;

 public:
  task_future<void, TPD, TPTR, TMACRO> get_return_object() LIBCOPP_MACRO_NOEXCEPT;
  using task_promise_base_type<void, TPD, TPTR, TMACRO>::get_runtime;

  void return_void() LIBCOPP_MACRO_NOEXCEPT {
    // Maybe set error data on custom poller, ignore co_return here.
    if (get_runtime() && !get_runtime()->future.is_ready()) {
      get_runtime()->future.poll_data() = true;
    }
  }
};

template <class T, class TPD = void, class TPTR = typename poll_storage_ptr_selector<T>::type,
          class TMACRO = std::allocator<LIBCOPP_MACRO_FUTURE_COROUTINE_VOID> >
class LIBCOPP_COPP_API_HEAD_ONLY task_future {
 public:
  using self_type = task_future<T, TPD, TPTR, TMACRO>;
  using promise_type = task_promise<T, TPD, TPTR, TMACRO>;
  using runtime_type = typename promise_type::runtime_type;
  using future_data_type = typename promise_type::future_data_type;
  using context_type = typename promise_type::context_type;
  using poller_type = typename promise_type::poller_type;
  using wake_list_type = typename promise_type::wake_list_type;
  using storage_type = typename poller_type::storage_type;
  using value_type = typename poller_type::value_type;
  using status_type = task_status;

 private:
  class awaitable_base_type {
   private:
    awaitable_base_type(const awaitable_base_type &) = delete;
    awaitable_base_type &operator=(const awaitable_base_type &) = delete;

   public:
    awaitable_base_type(self_type *task) : refer_task_(task) {
      if (refer_task_ && refer_task_->runtime_ && !refer_task_->runtime_->done()) {
        await_iterator_ = refer_task_->runtime_->handle.promise().end_wake_handles();
      }
    }

    awaitable_base_type(awaitable_base_type &&other)
        : await_iterator_(other.await_iterator_), refer_task_(other.refer_task_) {
      other.refer_task_ = nullptr;
    }
    awaitable_base_type &operator=(awaitable_base_type &&other) {
      await_iterator_ = other.await_iterator_;
      refer_task_ = other.refer_task_;

      other.refer_task_ = nullptr;
      if (refer_task_ && refer_task_->runtime_ && !refer_task_->runtime_->done()) {
        refer_task_->runtime_->handle.promise().end_wake_handles();
      }
    }

    bool await_ready() const LIBCOPP_MACRO_NOEXCEPT { return !refer_task_ || refer_task_->done(); }

    void await_suspend(LIBCOPP_MACRO_FUTURE_COROUTINE_VOID h) {
      if (await_ready()) {
        h.resume();
        return;
      }

      await_iterator_ = refer_task_->runtime_->handle.promise().add_wake_handle(h);
    }

    void await_resume() LIBCOPP_MACRO_NOEXCEPT {
      if (refer_task_ && refer_task_->runtime_ && refer_task_->runtime_->handle &&
          !refer_task_->runtime_->handle.done() &&
          await_iterator_ != refer_task_->runtime_->handle.promise().end_wake_handles()) {
        refer_task_->runtime_->handle.promise().remove_wake_handle(await_iterator_);
      }
    }

   protected:
    typename wake_list_type::iterator await_iterator_;
    self_type *refer_task_;
  };

 public:
  task_future() {}
  task_future(std::shared_ptr<runtime_type> r) : runtime_(r) {}

#  ifdef __cpp_impl_three_way_comparison
  friend inline std::strong_ordering operator<=>(const task_future &l, const task_future &r) LIBCOPP_MACRO_NOEXCEPT {
    return l.get_task_id() <=> r.get_task_id();
  }
#  else
  friend inline bool operator!=(const task_future &l, const task_future &r) LIBCOPP_MACRO_NOEXCEPT {
    return l.get_task_id() != r.get_task_id();
  }
  friend inline bool operator<(const task_future &l, const task_future &r) LIBCOPP_MACRO_NOEXCEPT {
    return l.get_task_id() < r.get_task_id();
  }
  friend inline bool operator<=(const task_future &l, const task_future &r) LIBCOPP_MACRO_NOEXCEPT {
    return l.get_task_id() <= r.get_task_id();
  }
  friend inline bool operator>(const task_future &l, const task_future &r) LIBCOPP_MACRO_NOEXCEPT {
    return l.get_task_id() > r.get_task_id();
  }
  friend inline bool operator>=(const task_future &l, const task_future &r) LIBCOPP_MACRO_NOEXCEPT {
    return l.get_task_id() >= r.get_task_id();
  }
#  endif

  // co_await a temporary task_future in GCC 10.1.0 will destroy task_future first, which may cause all resources
  // unavailable auto operator co_await() && LIBCOPP_MACRO_NOEXCEPT {
  //     struct awaitable : awaitable_base_type {
  //         using awaitable_base_type::awaitable_base_type;
  //         using awaitable_base_type::refer_task_;
  //
  //         poller_type await_resume() {
  //             awaitable_base_type::await_resume();
  //             COPP_LIKELY_IF(refer_task_) {
  //                 poller_type *ret = refer_task_->poll_data();
  //                 if (nullptr != ret) {
  //                     return std::move(*ret);
  //                 }
  //             }
  //
  //
  //             return poller_type{};
  //         }
  //     };
  //
  //     return awaitable{this};
  // }

  auto operator co_await() & LIBCOPP_MACRO_NOEXCEPT {
    struct awaitable : awaitable_base_type {
      using awaitable_base_type::awaitable_base_type;
      using awaitable_base_type::refer_task_;

      value_type *await_resume() {
        awaitable_base_type::await_resume();
        COPP_LIKELY_IF(refer_task_) { return refer_task_->data(); }

        return nullptr;
      }
    };

    return awaitable{this};
  }

  inline bool done() const LIBCOPP_MACRO_NOEXCEPT {
    COPP_LIKELY_IF(runtime_) { return runtime_->done(); }

    return true;
  }

#  if defined(LIBCOPP_MACRO_ENABLE_STD_EXCEPTION_PTR) && LIBCOPP_MACRO_ENABLE_STD_EXCEPTION_PTR
  inline const value_type *data() const {
#  else
  inline const value_type *data() const LIBCOPP_MACRO_NOEXCEPT {
#  endif
    COPP_LIKELY_IF(runtime_) {
#  if defined(LIBCOPP_MACRO_ENABLE_STD_EXCEPTION_PTR) && LIBCOPP_MACRO_ENABLE_STD_EXCEPTION_PTR
      maybe_rethrow();
#  endif
      return runtime_->future.data();
    }

    return nullptr;
  }

#  if defined(LIBCOPP_MACRO_ENABLE_STD_EXCEPTION_PTR) && LIBCOPP_MACRO_ENABLE_STD_EXCEPTION_PTR
  inline value_type *data() {
#  else
  inline value_type *data() LIBCOPP_MACRO_NOEXCEPT {
#  endif
    COPP_LIKELY_IF(runtime_) {
#  if defined(LIBCOPP_MACRO_ENABLE_STD_EXCEPTION_PTR) && LIBCOPP_MACRO_ENABLE_STD_EXCEPTION_PTR
      maybe_rethrow();
#  endif
      return runtime_->future.data();
    }

    return nullptr;
  }

  inline const poller_type *poll_data() const LIBCOPP_MACRO_NOEXCEPT {
    COPP_LIKELY_IF(runtime_) { return &runtime_->future.poll_data(); }

    return nullptr;
  }

  inline poller_type *poll_data() LIBCOPP_MACRO_NOEXCEPT {
    COPP_LIKELY_IF(runtime_) { return &runtime_->future.poll_data(); }

    return nullptr;
  }

  inline status_type get_status() const LIBCOPP_MACRO_NOEXCEPT {
    if (runtime_) {
      return runtime_->status;
    }

    return status_type::kDone;
  }

  inline context_type *get_context() LIBCOPP_MACRO_NOEXCEPT {
    if (done()) {
      return nullptr;
    }

    return &runtime_->handle.promise().get_context();
  }

  inline const context_type *get_context() const LIBCOPP_MACRO_NOEXCEPT {
    if (done()) {
      return nullptr;
    }

    return &runtime_->handle.promise().get_context();
  }

  inline uint64_t get_task_id() const LIBCOPP_MACRO_NOEXCEPT {
    COPP_LIKELY_IF(runtime_) { return runtime_->task_id; }

    const context_type *ctx = get_context();
    if (ctx) {
      return ctx->get_task_id();
    }

    return 0;
  }

  static typename promise_type::template pick_context_awaitable<context_type> current_context() {
    return promise_type::current_context();
  }
  static typename promise_type::template pick_future_awaitable<future_data_type> current_future_data() {
    return promise_type::current_future_data();
  }

 private:
#  if defined(LIBCOPP_MACRO_ENABLE_STD_EXCEPTION_PTR) && LIBCOPP_MACRO_ENABLE_STD_EXCEPTION_PTR
  inline void maybe_rethrow() {
    COPP_LIKELY_IF(runtime_) {
      COPP_UNLIKELY_IF(runtime_->unhandle_exception) {
        std::exception_ptr eptr;
        std::swap(eptr, runtime_->unhandle_exception);
        std::rethrow_exception(eptr);
      }
    }
  }
#  endif

 private:
  std::shared_ptr<runtime_type> runtime_;
};

template <typename T, class TPD, class TPTR, class TMACRO>
task_future<T, TPD, TPTR, TMACRO> task_promise<T, TPD, TPTR, TMACRO>::get_return_object() LIBCOPP_MACRO_NOEXCEPT {
  // if (get_runtime() && !get_runtime()->handle) {
  //     get_runtime()->handle = LIBCOPP_MACRO_STD_COROUTINE_NAMESPACE
  //         coroutine_handle<typename task_future<T, TPD, TPTR, TMACRO>::promise_type>::from_promise(*this);
  // }
  return task_future<T, TPD, TPTR, TMACRO>{get_runtime()};
}

template <class TPD, class TPTR, class TMACRO>
task_future<void, TPD, TPTR, TMACRO> task_promise<void, TPD, TPTR, TMACRO>::get_return_object() LIBCOPP_MACRO_NOEXCEPT {
  // if (get_runtime() && !get_runtime()->handle) {
  //     get_runtime()->handle = LIBCOPP_MACRO_STD_COROUTINE_NAMESPACE
  //     coroutine_handle<typename task_future<void, TPD, TPTR, TMACRO>::promise_type>::from_promise(*this);
  // }
  return task_future<void, TPD, TPTR, TMACRO>{get_runtime()};
}
#endif
}  // namespace future
LIBCOPP_COPP_NAMESPACE_END

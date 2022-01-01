// Copyright 2022 owent

#pragma once

#include <libcopp/utils/config/libcopp_build_features.h>

#include "future.h"

LIBCOPP_COPP_NAMESPACE_BEGIN
namespace future {

#if defined(LIBCOPP_MACRO_ENABLE_STD_COROUTINE) && LIBCOPP_MACRO_ENABLE_STD_COROUTINE
template <class T, class TPTR = typename poll_storage_ptr_selector<T>::type>
class LIBCOPP_COPP_API_HEAD_ONLY generator_future_data : public future_with_waker<T, TPTR> {
 public:
  using self_type = generator_future_data<T, TPTR>;

 public:
  template <class... TARGS>
  generator_future_data(TARGS &&...args)
      : future_with_waker<T, TPTR>(std::forward<TARGS>(args)...), await_handle_(nullptr) {}
  ~generator_future_data() {}

  UTIL_FORCEINLINE const LIBCOPP_MACRO_FUTURE_COROUTINE_VOID &get_handle() const { return await_handle_; }
  UTIL_FORCEINLINE LIBCOPP_MACRO_FUTURE_COROUTINE_VOID &get_handle() { return await_handle_; }
  UTIL_FORCEINLINE void set_handle(LIBCOPP_MACRO_FUTURE_COROUTINE_VOID h) { await_handle_ = h; }

  template <class TCONTEXT>
  void poll(TCONTEXT &&ctx) {
    future_with_waker<T, TPTR>::template poll_as<self_type>(*this, std::forward<TCONTEXT>(ctx));
  }

 private:
  LIBCOPP_MACRO_FUTURE_COROUTINE_VOID await_handle_;
};

template <class TPD>
class LIBCOPP_COPP_API_HEAD_ONLY generator_context : public context<TPD> {
 public:
  using self_type = generator_context<TPD>;

 private:
  // context can not be copy or moved.
  generator_context(const generator_context &) = delete;
  generator_context &operator=(const generator_context &) = delete;
  generator_context(generator_context &&) = delete;
  generator_context &operator=(generator_context &&) = delete;

 public:
  template <class... TARGS>
  generator_context(TARGS &&...args) : context<TPD>(std::forward<TARGS>(args)...) {}
};

template <class T, class TPD = void, class TPTR = typename poll_storage_ptr_selector<T>::type>
class LIBCOPP_COPP_API_HEAD_ONLY generator {
 public:
  using self_type = generator<T, TPD, TPTR>;
  using context_type = generator_context<TPD>;
  using future_data_type = generator_future_data<T, TPTR>;
  using poller_type = typename future_data_type::poller_type;
  using value_type = typename future_data_type::value_type;

 private:
  class awaitable_base_type {
   private:
    awaitable_base_type(const awaitable_base_type &) = delete;
    awaitable_base_type &operator=(const awaitable_base_type &) = delete;

   public:
    awaitable_base_type(future_data_type &fut, context_type &ctx) : future_(&fut), context_(&ctx) {}

    awaitable_base_type(awaitable_base_type &&other) : future_(other.future_), context_(&other.context_) {
      other.future_ = nullptr;
      other.context_ = nullptr;
    }
    awaitable_base_type &operator=(awaitable_base_type &&other) {
      future_ = other.future_;
      context_ = other.context_;

      other.future_ = nullptr;
      other.context_ = nullptr;
    }

    inline bool await_ready() const LIBCOPP_MACRO_NOEXCEPT { return !future_ || future_->is_ready(); }

    inline void await_suspend(LIBCOPP_MACRO_FUTURE_COROUTINE_VOID h) LIBCOPP_MACRO_NOEXCEPT {
      if (future_) {
        // This should never triggered, because a generator can only be co_await once
        if (future_->get_handle() && !future_->get_handle().done()) {
          future_->get_handle().resume();
        }

        future_->set_handle(h);
      }
    }

    inline future_data_type *await_resume() LIBCOPP_MACRO_NOEXCEPT {
      if (likely(future_)) {
        future_->set_handle(nullptr);

        if (future_->is_ready()) {
          future_data_type *fut = future_;
          future_ = nullptr;

          return fut;
        }
      }

      return nullptr;
    }

   protected:
    future_data_type *future_;
    context_type *context_;
  };

  class reference_awaitable : public awaitable_base_type {
   public:
    using awaitable_base_type::awaitable_base_type;

    value_type *await_resume() LIBCOPP_MACRO_NOEXCEPT {
      future_data_type *fut = awaitable_base_type::await_resume();
      if (nullptr != fut) {
        return fut->data();
      }
      return nullptr;
    }
  };

  class rvalue_awaitable : public awaitable_base_type {
   public:
    using awaitable_base_type::awaitable_base_type;

    poller_type await_resume() LIBCOPP_MACRO_NOEXCEPT {
      future_data_type *fut = awaitable_base_type::await_resume();
      if (nullptr != fut) {
        return std::move(fut->poll_data());
      }
      return nullptr;
    }
  };

  struct generator_waker {
    future_data_type *future;
    generator_waker(future_data_type &fut) : future(&fut) {}

    void operator()(context_type &ctx) {
      if (likely(future)) {
        if (!future->is_ready()) {
          future->template poll_as<future_data_type>(ctx);
        }

        // waker may be destroyed when call poll, so copy waker and future into stack
        if (future->is_ready() && future->get_handle() && !future->get_handle().done()) {
          // This may lead to deallocation of generator_waker later, and we need not to resume again
          future->get_handle().resume();
        }
      }
    }

    void operator()(context<TPD> &ctx) { (*this)(*static_cast<context_type *>(&ctx)); }
  };

 public:
  template <class... TARGS>
  generator(TARGS &&...args) : context_(std::forward<TARGS>(args)...) {
    context_.set_wake_fn(generator_waker{future_});
    // Can not set waker clear functor, because even future is polled outside
    //   we still need to resume handle after event finished
    // future_.set_ctx_waker(context_);

    future_.template poll_as<future_data_type>(context_);
  }
  ~generator() { context_.set_wake_fn(nullptr); }

  auto operator co_await() & LIBCOPP_MACRO_NOEXCEPT { return reference_awaitable{future_, context_}; }

  // co_await a temporary generator in GCC 10.1.0 will destroy generator first, which will cause all resources
  // unavailable auto operator co_await() && LIBCOPP_MACRO_NOEXCEPT { return rvalue_awaitable{future_, context_}; }

  UTIL_FORCEINLINE value_type *data() LIBCOPP_MACRO_NOEXCEPT { return future_.data(); }
  UTIL_FORCEINLINE const value_type *data() const LIBCOPP_MACRO_NOEXCEPT { return future_.data(); }
  UTIL_FORCEINLINE poller_type &poll_data() LIBCOPP_MACRO_NOEXCEPT { return future_.poll_data(); }
  UTIL_FORCEINLINE const poller_type &poll_data() const LIBCOPP_MACRO_NOEXCEPT { return future_.poll_data(); }
  UTIL_FORCEINLINE context_type &get_context() LIBCOPP_MACRO_NOEXCEPT { return context_; }
  UTIL_FORCEINLINE const context_type &get_context() const LIBCOPP_MACRO_NOEXCEPT { return context_; }

  UTIL_FORCEINLINE operator bool() const LIBCOPP_MACRO_NOEXCEPT { return has_data(); }
  UTIL_FORCEINLINE bool has_data() const LIBCOPP_MACRO_NOEXCEPT { return future_.is_ready(); }
  UTIL_FORCEINLINE void reset_data() { future_.reset_data(); }

 private:
  // generator can not be copy or moved.
  generator(const generator &) = delete;
  generator &operator=(const generator &) = delete;
  generator(generator &&) = delete;
  generator &operator=(generator &&) = delete;

 protected:
  context_type context_;
  future_data_type future_;
};

template <class T, class... TARGS>
inline T make_generator(TARGS &&...args) {
  return T{std::forward<TARGS>(args)...};
}
#endif
}  // namespace future
LIBCOPP_COPP_NAMESPACE_END

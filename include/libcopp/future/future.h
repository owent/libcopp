// Copyright 2022 owent

#pragma once

#include <libcopp/utils/config/libcopp_build_features.h>

// clang-format off
#include <libcopp/utils/config/stl_include_prefix.h>  // NOLINT(build/include_order)
// clang-format on
#include <list>
// clang-format off
#include <libcopp/utils/config/stl_include_suffix.h>  // NOLINT(build/include_order)
// clang-format on

#include "libcopp/future/context.h"
#include "libcopp/future/poller.h"

LIBCOPP_COPP_NAMESPACE_BEGIN
namespace future {

template <class T, class TPTR = typename poll_storage_ptr_selector<T>::type>
class LIBCOPP_COPP_API_HEAD_ONLY future {
 public:
  using self_type = future<T, TPTR>;
  using poller_type = poller<T, TPTR>;
  using storage_type = typename poller_type::storage_type;
  using value_type = typename poller_type::value_type;
  using ptr_type = typename poller_type::ptr_type;

 public:
  future() {}
  ~future() {}

  UTIL_FORCEINLINE bool is_ready() const LIBCOPP_MACRO_NOEXCEPT { return poll_data_.is_ready(); }

  UTIL_FORCEINLINE bool is_pending() const LIBCOPP_MACRO_NOEXCEPT { return poll_data_.is_pending(); }

  UTIL_FORCEINLINE const value_type *data() const LIBCOPP_MACRO_NOEXCEPT {
    if (!is_ready()) {
      return nullptr;
    }

    return poll_data_.data();
  }

  UTIL_FORCEINLINE value_type *data() LIBCOPP_MACRO_NOEXCEPT {
    if (!is_ready()) {
      return nullptr;
    }

    return poll_data_.data();
  }

  UTIL_FORCEINLINE const ptr_type &raw_ptr() const LIBCOPP_MACRO_NOEXCEPT { return poll_data_.raw_ptr(); }
  UTIL_FORCEINLINE ptr_type &raw_ptr() LIBCOPP_MACRO_NOEXCEPT { return poll_data_.raw_ptr(); }
  UTIL_FORCEINLINE const poller_type &poll_data() const LIBCOPP_MACRO_NOEXCEPT { return poll_data_; }
  UTIL_FORCEINLINE poller_type &poll_data() LIBCOPP_MACRO_NOEXCEPT { return poll_data_; }
  UTIL_FORCEINLINE void reset_data() { poll_data_.reset(); }

  template <class U>
  UTIL_FORCEINLINE void reset_data(U &&in) {
    poll_data_ = std::forward<U>(in);
  }

 private:
  poller_type poll_data_;
};

template <class T, class TPTR = typename poll_storage_ptr_selector<T>::type>
class LIBCOPP_COPP_API_HEAD_ONLY future_with_waker : public future<T, TPTR> {
 public:
  using self_type = future_with_waker<T, TPTR>;
  using poller_type = typename future<T, TPTR>::poller_type;
  using storage_type = typename poller_type::storage_type;
  using value_type = typename poller_type::value_type;
  using ptr_type = typename poller_type::ptr_type;

  struct waker_type {
    self_type *self;
  };

 private:
  // future_with_waker can not be copyed or moved.waker may reference to poll_data_
  future_with_waker(const future_with_waker &) = delete;
  future_with_waker &operator=(const future_with_waker &) = delete;
  future_with_waker(future_with_waker &&) = delete;
  future_with_waker &operator=(future_with_waker &&) = delete;

 private:
  struct _test_context_functor_t {
    // template <class U>
    // int operator()(U *);

    template <class U>
    bool operator()(context<U> *);
  };

 protected:
  template <class TSELF, class TCONTEXT>
  static void poll_as(TSELF &self, TCONTEXT &&ctx) {
    using decay_context_type = typename std::decay<TCONTEXT>::type;
    static_assert(std::is_same<bool, COPP_RETURN_VALUE_DECAY(_test_context_functor_t, decay_context_type *)>::value,
                  "ctx must be drive of context");
    static_assert(std::is_base_of<self_type, typename std::decay<TSELF>::type>::value,
                  "self must be drive of future_with_waker<T, TPTR>");

    if (self.is_ready()) {
      return;
    }

    // Set waker first, and then context can be moved or copyed in private data callback
    // If two or more context poll the same future, we just use the last one
    if (!ctx.get_wake_fn()) {
      ctx.set_wake_fn(future_waker_type<TSELF, typename std::decay<TCONTEXT>::type>(self));
      self.set_ctx_waker(std::forward<TCONTEXT>(ctx));
    }

    ctx.template poll_as<TCONTEXT>(self);

    if (self.is_ready() && self.clear_ctx_waker_) {
      self.clear_ctx_waker();
    }
  }

 public:
  future_with_waker() {}
  ~future_with_waker() { clear_ctx_waker(); }

  template <class TCONTEXT>
  UTIL_FORCEINLINE void poll(TCONTEXT &&ctx) {
    poll_as(*this, std::forward<TCONTEXT>(ctx));
  }

  template <class TSELF, class TCONTEXT>
  UTIL_FORCEINLINE void poll_as(TCONTEXT &&ctx) {
    poll_as(*static_cast<typename std::decay<TSELF>::type *>(this), std::forward<TCONTEXT>(ctx));
  }

  inline void clear_ctx_waker() LIBCOPP_MACRO_NOEXCEPT {
    if (clear_ctx_waker_) {
      clear_ctx_waker_();
      clear_ctx_waker_ = nullptr;
    }
  }

  template <class TCONTEXT>
  inline void set_ctx_waker(TCONTEXT &&ctx) {
    clear_ctx_waker();
    clear_ctx_waker_ = clear_context_waker_type<typename std::decay<TCONTEXT>::type>(ctx);
  }

 protected:
  template <class TCONTEXT>
  struct clear_context_waker_type {
    TCONTEXT *context;
    clear_context_waker_type(TCONTEXT &ctx) : context(&ctx) {}

    void operator()() LIBCOPP_MACRO_NOEXCEPT {
      COPP_LIKELY_IF (context) {
        context->set_wake_fn(nullptr);
      }
    }
  };

 private:
  // Allow to auto set waker for any child class of context
  template <class TSELF, class TCONTEXT>
  struct future_waker_type {
    TSELF *self;
    future_waker_type(TSELF &s) : self(&s) {}

    void operator()(TCONTEXT &ctx) {
      COPP_LIKELY_IF (self) {
        self->template poll_as<TSELF>(ctx);
      }
    }

    // convert type
    template <class TPD>
    inline void operator()(context<TPD> &ctx) {
      static_assert(std::is_base_of<context<TPD>, TCONTEXT>::value, "TCONTEXT must be drive of context<T, TPTR>");

      (*this)(*static_cast<TCONTEXT *>(&ctx));
    }
  };

  template <class TSELF, class TPD>
  struct future_waker_type<TSELF, context<TPD> > {
    TSELF *self;
    future_waker_type(TSELF &s) : self(&s) {}

    void operator()(context<TPD> &ctx) {
      COPP_LIKELY_IF (self) {
        self->template poll_as<TSELF>(ctx);
      }
    }
  };

 private:
  std::function<void()> clear_ctx_waker_;
};
}  // namespace future
LIBCOPP_COPP_NAMESPACE_END

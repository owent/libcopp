#ifndef COPP_FUTURE_CONTEXT_H
#define COPP_FUTURE_CONTEXT_H

#pragma once

#include <assert.h>

#include "poll.h"

namespace copp {
namespace future {
template <class TFUNCTION>
struct LIBCOPP_COPP_API_HEAD_ONLY context_event_function {
  TFUNCTION func;
  TFUNCTION **call_backup;

  context_event_function() : call_backup(nullptr) {}
  ~context_event_function() {
    if (call_backup) {
      *call_backup = nullptr;
    }
  }

  template <class... TARGS>
  inline void invoke(TARGS &&...args) {
    // can not call recursive
    if (call_backup) {
      return;
    }

    if (func) {
      // move into stack in case of removed when call func
      TFUNCTION move_to_stack;
      TFUNCTION *move_to_stack_ptr = &move_to_stack;
      move_to_stack.swap(func);
      call_backup = &move_to_stack_ptr;

      move_to_stack(std::forward<TARGS>(args)...);

      // restore functor, if move_to_stack_ptr is still not nullptr, this object must still exists
      //   and not call context_event_function::set(arg)
      // FIXME: Maybe there is visibillity problem?
      if (move_to_stack_ptr) {
        move_to_stack_ptr->swap(func);
        call_backup = nullptr;
      }
    }
  }

  template <class TARG>
  inline void set(TARG &&arg) {
    func = std::forward<TARG>(arg);
    if (call_backup) {
      *call_backup = nullptr;
      call_backup = nullptr;
    }
  }

  inline TFUNCTION &get() LIBCOPP_MACRO_NOEXCEPT {
    if (nullptr != call_backup) {
      return **call_backup;
    }

    return func;
  }

  inline const TFUNCTION &get() const LIBCOPP_MACRO_NOEXCEPT {
    if (nullptr != call_backup) {
      return **call_backup;
    }

    return func;
  }
};
/**
 * @brief context
 * @note TPD::operator()(future_with_waker<T>&, context<TPD>&) must be declared
 *         for private data type 'TPD' (template argument).
 *       TPD::operator()(future_with_waker<T>&, context<TPD>&) will be called with private_data_(future, *this) when
 * related event state changes. and the "future.poll_data()" must be set if all related asynchronous jobs is done.
 *
 */
template <class TPD>
class LIBCOPP_COPP_API_HEAD_ONLY context {
 public:
  using self_type = context<TPD>;
  using wake_fn_t = std::function<void(self_type &)>;
  using value_type = TPD;

 private:
  // context can not be copy or moved.
  context(const context &) = delete;
  context &operator=(const context &) = delete;
  context(context &&) = delete;
  context &operator=(context &&) = delete;

 public:
  template <class... TARGS>
  context(TARGS &&...args) : private_data_(std::forward<TARGS>(args)...) {}

  template <class TCONTEXT, class TFUTURE>
  void poll_as(TFUTURE &&fut) {
    static_assert(std::is_base_of<self_type, typename std::decay<TCONTEXT>::type>::value,
                  "The context type must be drive of context<TPD>");

    private_data_(std::forward<TFUTURE>(fut), *static_cast<typename std::decay<TCONTEXT>::type *>(this));
  }

  void wake() { wake_fn_.invoke(*this); }

  inline void set_wake_fn(wake_fn_t fn) { wake_fn_.set(std::move(fn)); }
  inline const wake_fn_t &get_wake_fn() const LIBCOPP_MACRO_NOEXCEPT { return wake_fn_.get(); }
  inline wake_fn_t &get_wake_fn() LIBCOPP_MACRO_NOEXCEPT { return wake_fn_.get(); }

  inline value_type &get_private_data() LIBCOPP_MACRO_NOEXCEPT { return private_data_; }
  inline const value_type &get_private_data() const LIBCOPP_MACRO_NOEXCEPT { return private_data_; }

 private:
  value_type private_data_;
  context_event_function<wake_fn_t> wake_fn_;
};

template <bool>
struct context_construct_helper_assign;

template <>
struct context_construct_helper_assign<true> {
  template <class TCONTEXT, class U>
  static inline void assign(TCONTEXT &&self, U &&helper) {
    self.set_private_data(helper.private_data);
    self.set_poll_fn(std::move(helper.pool_fn));
  }
};

template <>
struct context_construct_helper_assign<false> {
  template <class TCONTEXT, class U>
  static inline void assign(TCONTEXT &&, U &&) {}
};

/**
 * @brief context<void>
 * @note pool_fn(self_type&, poll_event_data) will be called with pool_fn(*this, {&out, private_data_}) when related
 * event state changes, and the "out" must be set if the action is ready
 */
template <>
class LIBCOPP_COPP_API_HEAD_ONLY context<void> {
 public:
  struct poll_event_data {
    void *future_ptr;    // set to address of **fut** when call poll(future_with_waker<T> &fut)
    void *private_data;  // set to ptr passed by constructor
  };

  using self_type = context<void>;
  using wake_fn_t = std::function<void(self_type &)>;
  using poll_fn_t = std::function<void(self_type &, poll_event_data)>;
  using value_type = void *;

 private:
  // context can not be copy or moved.
  context(const context &) = delete;
  context &operator=(const context &) = delete;
  context(context &&) = delete;
  context &operator=(context &&) = delete;

  struct construct_helper_t {
    poll_fn_t pool_fn;
    void *private_data;

    template <class U>
    inline construct_helper_t(U &&fn, void *ptr = nullptr) : pool_fn(std::forward<U>(fn)), private_data(ptr) {}
  };

 public:
  template <class U>
  static inline construct_helper_t construct(U &&fn, void *ptr = nullptr) {
    return construct_helper_t(std::forward<U>(fn), ptr);
  }

  context() : private_data_(nullptr) {}
  template <class TFIRST, class... TARGS>
  context(TFIRST &&helper, TARGS &&...) : private_data_(nullptr) {
    context_construct_helper_assign<std::is_same<construct_helper_t, typename std::decay<TFIRST>::type>::value>::assign(
        *this, std::forward<TFIRST>(helper));
  }

  ~context() { on_destroy_fn_.invoke(*this); }

  template <class TCONTEXT, class TFUTURE>
  void poll_as(TFUTURE &&fut) {
    static_assert(std::is_base_of<self_type, typename std::decay<TCONTEXT>::type>::value,
                  "The context type must be drive of context<TPD>");

    poll_event_data data;
    data.future_ptr = reinterpret_cast<void *>(&fut);
    data.private_data = private_data_;
    poll_fn_.invoke(*static_cast<typename std::decay<TCONTEXT>::type *>(this), data);
  }

  void wake() { wake_fn_.invoke(*this); }

  inline void set_poll_fn(poll_fn_t fn) { poll_fn_.set(std::move(fn)); }
  inline const poll_fn_t &get_poll_fn() const LIBCOPP_MACRO_NOEXCEPT { return poll_fn_.get(); }
  inline poll_fn_t &get_poll_fn() LIBCOPP_MACRO_NOEXCEPT { return poll_fn_.get(); }

  inline void set_wake_fn(wake_fn_t fn) { wake_fn_.set(std::move(fn)); }
  inline const wake_fn_t &get_wake_fn() const LIBCOPP_MACRO_NOEXCEPT { return wake_fn_.get(); }
  inline wake_fn_t &get_wake_fn() LIBCOPP_MACRO_NOEXCEPT { return wake_fn_.get(); }

  inline void set_on_destroy(wake_fn_t fn) { on_destroy_fn_.set(std::move(fn)); }
  inline const wake_fn_t &get_on_destroy() const LIBCOPP_MACRO_NOEXCEPT { return on_destroy_fn_.get(); }
  inline wake_fn_t &get_on_destroy() LIBCOPP_MACRO_NOEXCEPT { return on_destroy_fn_.get(); }

  inline void set_private_data(void *ptr) { private_data_ = ptr; }
  inline void *get_private_data() LIBCOPP_MACRO_NOEXCEPT { return private_data_; }
  inline const void *get_private_data() const LIBCOPP_MACRO_NOEXCEPT { return private_data_; }

 private:
  value_type private_data_;
  context_event_function<wake_fn_t> wake_fn_;
  context_event_function<poll_fn_t> poll_fn_;
  context_event_function<wake_fn_t> on_destroy_fn_;
};
}  // namespace future
}  // namespace copp

#endif
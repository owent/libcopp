// Copyright 2022 owent

#pragma once

#include <libcopp/utils/config/libcopp_build_features.h>

#include <libcopp/utils/std/coroutine.h>

LIBCOPP_COTASK_NAMESPACE_BEGIN

namespace impl {

class task_impl;

class LIBCOPP_COTASK_API task_action_future_base {
#if defined(LIBCOPP_MACRO_ENABLE_STD_COROUTINE) && LIBCOPP_MACRO_ENABLE_STD_COROUTINE
  // TODO(owent): promise type for C++20 coroutine
  struct promise_type {
    // std::suspend_never initial_suspend() const LIBCOPP_MACRO_NOEXCEPT { return {}; }
    // std::suspend_never final_suspend() const LIBCOPP_MACRO_NOEXCEPT { return {}; }

    // task_action_result<T> get_return_object() LIBCOPP_MACRO_NOEXCEPT { return this->get_future(); }

    // void return_value(const T &value) LIBCOPP_MACRO_NOEXCEPT {
    // this->set_value(value); }

    // void return_value(T &&value) LIBCOPP_MACRO_NOEXCEPT {
    //   this->set_value(std::move(value));
    // }

    // void return_void() LIBCOPP_MACRO_NOEXCEPT {}

    // template <class TARGS>
    // auto yield_value(TARGS &&args) {
    //   return std::forward<TARGS>(args);
    // }
  };
#endif
};

class LIBCOPP_COTASK_API task_action_impl {
 public:
  task_action_impl();
  virtual ~task_action_impl();

  task_action_impl(const task_action_impl &);
  task_action_impl &operator=(const task_action_impl &);

  task_action_impl(const task_action_impl &&);
  task_action_impl &operator=(const task_action_impl &&);

  virtual int operator()(void *) = 0;
  virtual int on_finished(task_impl &);
};
}  // namespace impl
LIBCOPP_COTASK_NAMESPACE_END

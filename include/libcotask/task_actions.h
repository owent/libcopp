// Copyright 2023 owent

#pragma once

#include <libcopp/utils/config/libcopp_build_features.h>

#include <libcotask/impl/task_action_impl.h>

LIBCOPP_COTASK_NAMESPACE_BEGIN

namespace detail {

struct task_action_functor_check {
  // ================================================
  template <typename TR, typename TF>
  static LIBCOPP_COTASK_API_HEAD_ONLY int call(TR (TF::*)(void *), TF &fn, void *priv_data) {
    fn(priv_data);
    return 0;
  }

  template <typename TR, typename TF>
  static LIBCOPP_COTASK_API_HEAD_ONLY int call(TR (TF::*)(void *) const, const TF &fn, void *priv_data) {
    fn(priv_data);
    return 0;
  }

  // ------------------------------------------------
  template <typename TF>
  static LIBCOPP_COTASK_API_HEAD_ONLY int call(int (TF::*)(void *), TF &fn, void *priv_data) {
    return fn(priv_data);
  }

  template <typename TF>
  static LIBCOPP_COTASK_API_HEAD_ONLY int call(int (TF::*)(void *) const, const TF &fn, void *priv_data) {
    return fn(priv_data);
  }

  // ------------------------------------------------
  template <typename TR, typename TF>
  static LIBCOPP_COTASK_API_HEAD_ONLY int call(TR (TF::*)(), TF &fn, void * /*priv_data*/) {
    fn();
    return 0;
  }

  template <typename TR, typename TF>
  static LIBCOPP_COTASK_API_HEAD_ONLY int call(TR (TF::*)() const, const TF &fn, void * /*priv_data*/) {
    fn();
    return 0;
  }

  // ------------------------------------------------
  template <typename TF>
  static LIBCOPP_COTASK_API_HEAD_ONLY int call(int (TF::*)(), TF &fn, void * /*priv_data*/) {
    return fn();
  }

  template <typename TF>
  static LIBCOPP_COTASK_API_HEAD_ONLY int call(int (TF::*)() const, const TF &fn, void * /*priv_data*/) {
    return fn();
  }
};
}  // namespace detail

// functor
template <typename Ty>
class LIBCOPP_COTASK_API_HEAD_ONLY task_action_functor : public impl::task_action_impl {
 public:
  using value_type = typename std::decay<Ty>::type;

  template <typename... TARG>
  task_action_functor(TARG &&...arg) : functor_(std::forward<TARG>(arg)...) {}

  template <typename... TARG>
  task_action_functor(task_action_functor &&other, TARG &&...) : functor_(std::move(other.functor_)) {}

  template <typename... TARG>
  task_action_functor(const task_action_functor &other, TARG &&...) : functor_(other.functor_) {}

  ~task_action_functor() {}
  int operator()(void *priv_data) override {
    return detail::task_action_functor_check::call(&value_type::operator(), functor_, priv_data);
  }

  static void placement_destroy(void *selfp) {
    if (nullptr == selfp) {
      return;
    }

    task_action_functor<Ty> *self = reinterpret_cast<task_action_functor<Ty> *>(selfp);
    self->~task_action_functor();
  }

 private:
  value_type functor_;
};

// function
template <typename Ty>
class LIBCOPP_COTASK_API_HEAD_ONLY task_action_function : public impl::task_action_impl {
 public:
  using value_type = Ty (*)(void *);

 private:
  template <class Tz, bool IS_INTEGRAL>
  struct invoker;

  template <class Tz>
  struct invoker<Tz, true> {
    UTIL_FORCEINLINE static int invoke(task_action_function &in, void *priv_data) {
      return static_cast<int>((*in.func_)(priv_data));
    }
  };

  template <class Tz>
  struct invoker<Tz, false> {
    UTIL_FORCEINLINE static int invoke(task_action_function &in, void *priv_data) {
      (*in.func_)(priv_data);
      return 0;
    }
  };

 public:
  task_action_function(value_type func) : func_(func) {}
  ~task_action_function() {}

  int operator()(void *priv_data) override {
    return invoker<Ty, std::is_integral<typename std::decay<Ty>::type>::value>::invoke(*this, priv_data);
  }

  static void placement_destroy(void *selfp) {
    if (nullptr == selfp) {
      return;
    }

    task_action_function<Ty> *self = reinterpret_cast<task_action_function<Ty> *>(selfp);
    self->~task_action_function();
  }

 private:
  value_type func_;
};

// mem function
template <typename Ty, typename Tc>
class LIBCOPP_COTASK_API_HEAD_ONLY task_action_mem_function : public impl::task_action_impl {
 public:
  using value_type = Ty Tc::*;

 private:
  template <class Tz, bool IS_INTEGRAL>
  struct invoker;

  template <class Tz>
  struct invoker<Tz, true> {
    UTIL_FORCEINLINE static int invoke(task_action_mem_function &in, void *priv_data) {
      return static_cast<int>((in.instacne_->*in.func_)(priv_data));
    }
  };

  template <class Tz>
  struct invoker<Tz, false> {
    UTIL_FORCEINLINE static int invoke(task_action_mem_function &in, void *priv_data) {
      (in.instacne_->*in.func_)(priv_data);
      return 0;
    }
  };

 public:
  task_action_mem_function(value_type func, Tc *inst) : instacne_(inst), func_(func) {}
  ~task_action_mem_function() {}

  int operator()(void *priv_data) override {
    return invoker<Ty, std::is_integral<typename std::decay<Ty>::type>::value>::invoke(*this, priv_data);
  }

  static void placement_destroy(void *selfp) {
    if (nullptr == selfp) {
      return;
    }

    task_action_mem_function<Ty, Tc> *self = reinterpret_cast<task_action_mem_function<Ty, Tc> *>(selfp);
    self->~task_action_mem_function();
  }

 private:
  Tc *instacne_;
  value_type func_;
};

template <typename Ty>
LIBCOPP_COTASK_API_HEAD_ONLY void placement_destroy(void *selfp) {
  if (nullptr == selfp) {
    return;
  }

  Ty *self = reinterpret_cast<Ty *>(selfp);
  self->~Ty();
}

using placement_destroy_fn_t = void (*)(void *);

template <typename Ty>
LIBCOPP_COTASK_API_HEAD_ONLY placement_destroy_fn_t get_placement_destroy(task_action_functor<Ty> * /*selfp*/) {
  return &task_action_functor<Ty>::placement_destroy;
}

template <typename Ty>
LIBCOPP_COTASK_API_HEAD_ONLY placement_destroy_fn_t get_placement_destroy(task_action_function<Ty> * /*selfp*/) {
  return &task_action_function<Ty>::placement_destroy;
}

template <typename Ty, typename Tc>
LIBCOPP_COTASK_API_HEAD_ONLY placement_destroy_fn_t
get_placement_destroy(task_action_mem_function<Ty, Tc> * /*selfp*/) {
  return &task_action_mem_function<Ty, Tc>::placement_destroy;
}

template <typename Ty>
LIBCOPP_COTASK_API_HEAD_ONLY placement_destroy_fn_t get_placement_destroy(Ty * /*selfp*/) {
  return &placement_destroy<Ty>;
}
LIBCOPP_COTASK_NAMESPACE_END

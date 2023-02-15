// Copyright 2023 owent

#pragma once

#include <libcopp/utils/config/libcopp_build_features.h>

#include <libcopp/future/future.h>
#include <libcopp/utils/std/coroutine.h>

LIBCOPP_COTASK_NAMESPACE_BEGIN

namespace impl {

class task_impl;

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

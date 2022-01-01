// Copyright 2022 owent

#include <libcopp/utils/config/libcopp_build_features.h>

#include <libcotask/impl/task_action_impl.h>

LIBCOPP_COTASK_NAMESPACE_BEGIN
namespace impl {
LIBCOPP_COTASK_API task_action_impl::task_action_impl() {}

LIBCOPP_COTASK_API task_action_impl::~task_action_impl() {}

task_action_impl::task_action_impl(const task_action_impl &) {}
task_action_impl &task_action_impl::operator=(const task_action_impl &) { return *this; }

task_action_impl::task_action_impl(const task_action_impl &&) {}
task_action_impl &task_action_impl::operator=(const task_action_impl &&) { return *this; }

}  // namespace impl
LIBCOPP_COTASK_NAMESPACE_END

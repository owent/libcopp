// Copyright 2023 owent

#include <libcopp/utils/config/libcopp_build_features.h>

#include <libcotask/impl/task_action_impl.h>

LIBCOPP_COTASK_NAMESPACE_BEGIN
namespace impl {
LIBCOPP_COTASK_API task_action_impl::task_action_impl() {}

LIBCOPP_COTASK_API task_action_impl::~task_action_impl() {}

LIBCOPP_COTASK_API task_action_impl::task_action_impl(const task_action_impl &) {}
LIBCOPP_COTASK_API task_action_impl &task_action_impl::operator=(const task_action_impl &) { return *this; }

LIBCOPP_COTASK_API task_action_impl::task_action_impl(const task_action_impl &&) {}
LIBCOPP_COTASK_API task_action_impl &task_action_impl::operator=(const task_action_impl &&) { return *this; }

LIBCOPP_COTASK_API int task_action_impl::on_finished(task_impl &) { return 0; }

}  // namespace impl
LIBCOPP_COTASK_NAMESPACE_END

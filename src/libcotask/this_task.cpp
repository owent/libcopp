// Copyright 2023 owent

#include <libcopp/utils/config/libcopp_build_features.h>

#include <libcotask/this_task.h>

LIBCOPP_COTASK_NAMESPACE_BEGIN
namespace this_task {
LIBCOPP_COTASK_API impl::task_impl *get_task() LIBCOPP_MACRO_NOEXCEPT { return impl::task_impl::this_task(); }
}  // namespace this_task
LIBCOPP_COTASK_NAMESPACE_END

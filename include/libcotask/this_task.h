// Copyright 2023 owent

#pragma once

#include <libcopp/utils/config/libcopp_build_features.h>

#include <libcotask/impl/task_impl.h>

LIBCOPP_COTASK_NAMESPACE_BEGIN
namespace this_task {

/**
 * @brief get current running task
 * @return current running task or empty pointer when not in task
 */
LIBCOPP_COTASK_API impl::task_impl *get_task() LIBCOPP_MACRO_NOEXCEPT;

/**
 * @brief get current running task and try to convert type
 * @return current running task or empty pointer when not in task or fail to convert type
 */
template <typename Tt>
LIBCOPP_COTASK_API_HEAD_ONLY Tt *get() {
#if defined(LIBCOPP_MACRO_ENABLE_RTTI) && LIBCOPP_MACRO_ENABLE_RTTI
  return dynamic_cast<Tt *>(get_task());
#else
  return static_cast<Tt *>(get_task());
#endif
}
}  // namespace this_task
LIBCOPP_COTASK_NAMESPACE_END

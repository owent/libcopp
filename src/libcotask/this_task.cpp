/*
 * this_task.cpp
 *
 *  Created on: 2014年4月2日
 *      Author: owent
 *
 *  Released under the MIT license
 */

#include <libcotask/this_task.h>

namespace cotask {
namespace this_task {
LIBCOPP_COTASK_API impl::task_impl *get_task() LIBCOPP_MACRO_NOEXCEPT { return impl::task_impl::this_task(); }
}  // namespace this_task
}  // namespace cotask

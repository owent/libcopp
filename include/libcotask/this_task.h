/*
 * this_task.h
 *
 *  Created on: 2014年4月2日
 *      Author: owent
 *
 *  Released under the MIT license
 */

#ifndef COTASK_THIS_TASK_H
#define COTASK_THIS_TASK_H

#pragma once

#include <libcotask/impl/task_impl.h>

namespace cotask {
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
            return dynamic_cast<Tt *>(get_task());
        }
    } // namespace this_task
} // namespace cotask

#endif /* _COTASK_THIS_TASK_H_ */

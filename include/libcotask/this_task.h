/*
 * this_task.h
 *
 *  Created on: 2014年4月2日
 *      Author: owent
 *
 *  Released under the MIT license
 */

#ifndef _COTASK_THIS_TASK_H_
#define _COTASK_THIS_TASK_H_

#include <libcotask/impl/task_impl.h>

namespace cotask {
    namespace this_task {

        /**
         * @brief get current running task
         * @return current running task or empty pointer when not in task
         */
        impl::task_impl* get_task();

        /**
         * @brief get current running task and try to convert type
         * @return current running task or empty pointer when not in task or fail to convert type
         */
        template<typename Tt>
        Tt* get() {
            return dynamic_cast<Tt*>(get_task());
        }
    }
}

#endif /* _COTASK_THIS_TASK_H_ */

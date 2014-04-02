/*
 * task_action_impl.h
 *
 *  Created on: 2014年4月1日
 *      Author: owent
 *
 *  Released under the MIT license
 */

#ifndef _COTASK_IMPL_TASK_ACTION_IMPL_H_
#define _COTASK_IMPL_TASK_ACTION_IMPL_H_

#include <libcopp/coroutine/coroutine_runnable_base.h>

namespace cotask {

    namespace impl {

        class task_impl;

        class task_action_impl : public copp::coroutine_runnable_base
        {
        public:
            task_impl* get_task() { return ref_task_; }
            const task_impl* get_task() const { return ref_task_; }

            virtual int on_finished() { return 0; }

        private:
            friend class task_impl;
            task_impl* ref_task_;
        };
    }
}


#endif /* _COTASK_IMPL_TASK_ACTION_IMPL_H_ */

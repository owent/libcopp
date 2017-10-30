/*
 * task_action_impl.h
 *
 *  Created on: 2014年4月1日
 *      Author: owent
 *
 *  Released under the MIT license
 */

#ifndef COTASK_IMPL_TASK_ACTION_IMPL_H
#define COTASK_IMPL_TASK_ACTION_IMPL_H

#pragma once

#include <libcopp/coroutine/coroutine_runnable_base.h>

namespace cotask {

    namespace impl {

        class task_impl;

        class task_action_impl : public copp::coroutine_runnable_base
        {
        public:
            virtual int on_finished(task_impl&) { return 0; }
        };
    }
}


#endif /* _COTASK_IMPL_TASK_ACTION_IMPL_H_ */

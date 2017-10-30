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

namespace cotask {

    namespace impl {

        class task_impl;

        class task_action_impl {
        public:
            virtual ~task_action_impl() {}
            virtual int operator()(void *) = 0;
            virtual int on_finished(task_impl &) { return 0; }
        };
    }
}


#endif /* _COTASK_IMPL_TASK_ACTION_IMPL_H_ */

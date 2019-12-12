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

#include <libcopp/utils/config/compiler_features.h>
#include <libcopp/utils/config/libcopp_build_features.h>

namespace cotask {

    namespace impl {

        class task_impl;

        class LIBCOPP_COTASK_API task_action_impl {
        public:
            task_action_impl();
            virtual ~task_action_impl();

            task_action_impl(const task_action_impl &);
            task_action_impl &operator=(const task_action_impl &);

#if defined(UTIL_CONFIG_COMPILER_CXX_RVALUE_REFERENCES) && UTIL_CONFIG_COMPILER_CXX_RVALUE_REFERENCES
            task_action_impl(const task_action_impl &&);
            task_action_impl &operator=(const task_action_impl &&);
#endif


            virtual int operator()(void *) = 0;
            virtual int on_finished(task_impl &) { return 0; }
        };
    } // namespace impl
} // namespace cotask


#endif /* _COTASK_IMPL_TASK_ACTION_IMPL_H_ */

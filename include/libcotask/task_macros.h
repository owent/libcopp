/*
 * task_macros.h
 *
 *  Created on: 2014年5月6日
 *      Author: owent
 *
 *  Released under the MIT license
 */

#ifndef COTASK_TASK_MACROS_H
#define COTASK_TASK_MACROS_H

#pragma once

#include <stdint.h>

#include <libcopp/coroutine/coroutine_context_container.h>
#include <libcopp/coroutine/coroutine_context_fiber_container.h>
#include <libcopp/utils/errno.h>

#include <libcotask/core/standard_new_allocator.h>
#include <libcotask/impl/task_impl.h>


namespace cotask {
    struct LIBCOPP_COTASK_API_HEAD_ONLY macro_coroutine {
        typedef copp::allocator::default_statck_allocator            stack_allocator_t;
        typedef copp::coroutine_context_container<stack_allocator_t> coroutine_t;
    };
} // namespace cotask

#endif /* _COTASK_THIS_TASK_H_ */

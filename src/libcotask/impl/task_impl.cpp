/*
 * task_impl.cpp
 *
 *  Created on: 2014年4月2日
 *      Author: owent
 *
 *  Released under the MIT license
 */

#include <algorithm>
#include <cstdlib>

#include <libcopp/utils/std/thread.h>

#include <libcotask/impl/task_action_impl.h>
#include <libcotask/impl/task_impl.h>


namespace cotask {
    namespace impl {

        static THREAD_TLS task_impl::ptr_t g_current_task_ = task_impl::ptr_t(NULL);

        task_impl::task_impl(): action_(NULL), status_(EN_TS_CREATED) {}

        task_impl::~task_impl(){}

        bool task_impl::is_canceled() const {
            return EN_TS_CANCELED == get_status();
        }

        bool task_impl::is_completed() const {
            return EN_TS_DONE <= get_status();
        }

        bool task_impl::is_faulted() const {
            return EN_TS_KILLED == get_status();
        }

        int task_impl::on_finished() {
            return 0;
        }

        task_impl::ptr_t task_impl::this_task() {
            return g_current_task_;
        }

        void task_impl::_set_action(action_ptr_t action) {
            action_ = action;
        }

        task_impl::action_ptr_t task_impl::_get_action() {
            return action_;
        }

        task_impl::ptr_t task_impl::_set_active_task(ptr_t task) {
            ptr_t ret = g_current_task_;
            g_current_task_ = task;
            return ret;
        }
    }
}

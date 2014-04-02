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

        static THREAD_TLS task_impl* g_current_task_ = NULL;

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

        task_impl* task_impl::this_task() {
            return g_current_task_;
        }

        void task_impl::_set_action(task_action_impl* action) {
            action_ = action;
            if (NULL != action_)
                action_->ref_task_ = this;
        }

        impl::task_action_impl* task_impl::_get_action() {
            return action_;
        }

        task_impl* task_impl::_set_active_task(task_impl* task) {
            task_impl* ret = g_current_task_;
            g_current_task_ = task;
            return ret;
        }
    }
}

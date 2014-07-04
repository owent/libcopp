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

        // VC do not support to set TLS property of not POD type 
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
            return EN_TS_KILLED <= get_status();
        }

        task_impl::ptr_t task_impl::next(ptr_t next_task) {
            // can not refers to self
            if(this == next_task.get())
                return shared_from_this();

            // can not add next task when finished
            if (get_status() >= EN_TS_DONE)
                return next_task;

            next_list_.member_list_.push_back(next_task);
            return next_task;
        }

        int task_impl::kill() {
            return kill(EN_TS_KILLED);
        }

        int task_impl::on_finished() {
            return 0;
        }

        task_impl::ptr_t task_impl::this_task() {
            task_impl* ret = g_current_task_;
            return NULL == ret?
                ptr_t():
                ret->shared_from_this();
        }

        void task_impl::_set_action(action_ptr_t action) {
            action_ = action;
        }

        task_impl::action_ptr_t task_impl::_get_action() {
            return action_;
        }

        task_impl::ptr_t task_impl::_set_active_task(task_impl* task) {
            task_impl* ret = g_current_task_;
            g_current_task_ = task;
            
            return NULL == ret ? task_impl::ptr_t(): ret->shared_from_this();
        }

        void task_impl::active_next_tasks() {
            // do next task
            for(std::list<ptr_t>::iterator iter = next_list_.member_list_.begin();
                iter != next_list_.member_list_.end(); ++ iter) {
                if (!(*iter) || EN_TS_INVALID == (*iter)->get_status())
                    continue;

                if ((*iter)->get_status() < EN_TS_RUNNING)
                    (*iter)->start();
                else
                    (*iter)->resume();
            }
            next_list_.member_list_.clear();
        }

        int task_impl::_notify_finished() {
            _get_action()->on_finished(*this);
            int ret = on_finished();

            // next tasks
            active_next_tasks();
            return ret;
        }
    }
}

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

#include <assert.h>

#include <libcopp/coroutine/coroutine_context.h>
#include <libcotask/impl/task_action_impl.h>
#include <libcotask/impl/task_impl.h>


namespace cotask {
    namespace impl {
        task_impl::task_impl() : action_(), status_(EN_TS_CREATED), finish_priv_data_(UTIL_CONFIG_NULLPTR) {}

        task_impl::~task_impl() { assert(status_ <= EN_TS_CREATED || status_ >= EN_TS_DONE); }

        EN_TASK_STATUS task_impl::get_status() const UTIL_CONFIG_NOEXCEPT { return static_cast<EN_TASK_STATUS>(status_.load()); }

        bool task_impl::is_canceled() const UTIL_CONFIG_NOEXCEPT { return EN_TS_CANCELED == get_status(); }

        bool task_impl::is_completed() const UTIL_CONFIG_NOEXCEPT { return EN_TS_DONE <= get_status(); }

        bool task_impl::is_faulted() const UTIL_CONFIG_NOEXCEPT { return EN_TS_KILLED <= get_status(); }

        task_impl::ptr_t task_impl::next(ptr_t next_task, void *priv_data) {
            // can not refers to self
            if (this == next_task.get()) return shared_from_this();

            // can not add next task when finished
            if (get_status() >= EN_TS_DONE) return next_task;

            util::lock::lock_holder<util::lock::spin_lock> lock_guard(next_list_lock_);
            next_list_.member_list_.push_back(std::make_pair(next_task, priv_data));
            return next_task;
        }

        int task_impl::on_finished() { return 0; }

        task_impl *task_impl::this_task() {
            copp::coroutine_context *this_co = copp::this_coroutine::get_coroutine();
            if (UTIL_CONFIG_NULLPTR == this_co) {
                return UTIL_CONFIG_NULLPTR;
            }

            return *((task_impl **)this_co->get_private_buffer());
        }

        void task_impl::_set_action(const action_ptr_t &action) { action_ = action; }

        const task_impl::action_ptr_t &task_impl::_get_action() { return action_; }

        bool task_impl::_cas_status(EN_TASK_STATUS &expected, EN_TASK_STATUS desired) {
            uint32_t expected_int = expected;
            bool ret = status_.compare_exchange_weak(expected_int, desired);
            expected = static_cast<EN_TASK_STATUS>(expected_int);
            return ret;
        }

        void task_impl::active_next_tasks() {
            std::list<std::pair<ptr_t, void *> > next_list;

            // first, lock and swap container
            {
                util::lock::lock_holder<util::lock::spin_lock> lock_guard(next_list_lock_);
                next_list.swap(next_list_.member_list_);
            }

            // then, do all the pending tasks
            for (std::list<std::pair<ptr_t, void *> >::iterator iter = next_list.begin(); iter != next_list.end(); ++iter) {
                if (!iter->first || EN_TS_INVALID == iter->first->get_status()) {
                    continue;
                }

                if (iter->first->get_status() < EN_TS_RUNNING)
                    iter->first->start(iter->second);
                else
                    iter->first->resume(iter->second);
            }
        }

        int task_impl::_notify_finished(void *priv_data) {
            finish_priv_data_ = priv_data;

            _get_action()->on_finished(*this);
            int ret = on_finished();

            // next tasks
            active_next_tasks();
            return ret;
        }
    }
}

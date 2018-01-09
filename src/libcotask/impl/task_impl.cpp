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
        task_impl::task_impl() : action_(UTIL_CONFIG_NULLPTR), finish_priv_data_(UTIL_CONFIG_NULLPTR), status_(EN_TS_CREATED) {}

        task_impl::~task_impl() { assert(status_ <= EN_TS_CREATED || status_ >= EN_TS_DONE); }

        bool task_impl::is_canceled() const UTIL_CONFIG_NOEXCEPT { return EN_TS_CANCELED == get_status(); }

        bool task_impl::is_completed() const UTIL_CONFIG_NOEXCEPT { return EN_TS_DONE <= get_status(); }

        bool task_impl::is_faulted() const UTIL_CONFIG_NOEXCEPT { return EN_TS_KILLED <= get_status(); }

        int task_impl::on_finished() { return 0; }

        task_impl *task_impl::this_task() {
            copp::coroutine_context *this_co = copp::this_coroutine::get_coroutine();
            if (UTIL_CONFIG_NULLPTR == this_co) {
                return UTIL_CONFIG_NULLPTR;
            }

            if (false == this_co->check_flags(ext_coroutine_flag_t::EN_ECFT_COTASK)) {
                return UTIL_CONFIG_NULLPTR;
            }

            return *((task_impl **)this_co->get_private_buffer());
        }

        void task_impl::_set_action(action_ptr_t action) { action_ = action; }

        task_impl::action_ptr_t task_impl::_get_action() { return action_; }

        bool task_impl::_cas_status(EN_TASK_STATUS &expected, EN_TASK_STATUS desired) {
            uint32_t expected_int = expected;
            bool ret =
                status_.compare_exchange_weak(expected_int, desired, util::lock::memory_order_acq_rel, util::lock::memory_order_acquire);
            expected = static_cast<EN_TASK_STATUS>(expected_int);
            return ret;
        }

        int task_impl::_notify_finished(void *priv_data) {
            finish_priv_data_ = priv_data;

            _get_action()->on_finished(*this);
            int ret = on_finished();

            return ret;
        }
    } // namespace impl
} // namespace cotask

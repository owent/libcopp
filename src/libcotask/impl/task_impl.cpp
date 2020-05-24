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
        LIBCOPP_COTASK_API task_impl::task_impl()
            : action_(UTIL_CONFIG_NULLPTR), finish_priv_data_(UTIL_CONFIG_NULLPTR), status_(EN_TS_CREATED) {}

        LIBCOPP_COTASK_API task_impl::~task_impl() { assert(status_ <= EN_TS_CREATED || status_ >= EN_TS_DONE); }

        LIBCOPP_COTASK_API bool task_impl::is_canceled() const LIBCOPP_MACRO_NOEXCEPT { return EN_TS_CANCELED == get_status(); }

        LIBCOPP_COTASK_API bool task_impl::is_completed() const LIBCOPP_MACRO_NOEXCEPT { return is_exiting(); }

        LIBCOPP_COTASK_API bool task_impl::is_faulted() const LIBCOPP_MACRO_NOEXCEPT { return EN_TS_KILLED <= get_status(); }

        LIBCOPP_COTASK_API bool task_impl::is_timeout() const LIBCOPP_MACRO_NOEXCEPT { return EN_TS_TIMEOUT == get_status(); }

        LIBCOPP_COTASK_API bool task_impl::is_exiting() const LIBCOPP_MACRO_NOEXCEPT { return EN_TS_DONE <= get_status(); }

        LIBCOPP_COTASK_API int task_impl::on_finished() { return 0; }

        LIBCOPP_COTASK_API task_impl *task_impl::this_task() {
            copp::coroutine_context *this_co = copp::this_coroutine::get_coroutine();
            if (UTIL_CONFIG_NULLPTR == this_co) {
                return UTIL_CONFIG_NULLPTR;
            }

            if (false == this_co->check_flags(ext_coroutine_flag_t::EN_ECFT_COTASK)) {
                return UTIL_CONFIG_NULLPTR;
            }

            return *((task_impl **)this_co->get_private_buffer());
        }

        LIBCOPP_COTASK_API void task_impl::_set_action(action_ptr_t action) { action_ = action; }

        LIBCOPP_COTASK_API task_impl::action_ptr_t task_impl::_get_action() { return action_; }

        LIBCOPP_COTASK_API bool task_impl::_cas_status(EN_TASK_STATUS &expected, EN_TASK_STATUS desired) {
            uint32_t expected_int = expected;
            bool     ret          = status_.compare_exchange_weak(expected_int, desired, libcopp::util::lock::memory_order_acq_rel,
                                                     libcopp::util::lock::memory_order_acquire);
            expected              = static_cast<EN_TASK_STATUS>(expected_int);
            return ret;
        }

#if defined(LIBCOPP_MACRO_ENABLE_STD_EXCEPTION_PTR) && LIBCOPP_MACRO_ENABLE_STD_EXCEPTION_PTR
        LIBCOPP_COTASK_API int task_impl::_notify_finished(std::list<std::exception_ptr>& unhandled, void *priv_data) {
#else
        LIBCOPP_COTASK_API int task_impl::_notify_finished(void *priv_data) {
#endif
            finish_priv_data_ = priv_data;

#if defined(LIBCOPP_MACRO_ENABLE_STD_EXCEPTION_PTR) && LIBCOPP_MACRO_ENABLE_STD_EXCEPTION_PTR
            try {
#endif
                _get_action()->on_finished(*this);
                int ret = on_finished();
#if defined(LIBCOPP_MACRO_ENABLE_STD_EXCEPTION_PTR) && LIBCOPP_MACRO_ENABLE_STD_EXCEPTION_PTR
                return ret;
            } catch(...) {
                unhandled.emplace_back(std::current_exception());
                return copp::COPP_EC_HAS_UNHANDLE_EXCEPTION;
            }
#else
            return ret;
#endif
        }
    } // namespace impl
} // namespace cotask

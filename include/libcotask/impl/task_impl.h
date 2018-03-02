/*
 * task_impl.h
 *
 *  Created on: 2014年4月2日
 *      Author: owent
 *
 *  Released under the MIT license
 */

#ifndef COTASK_IMPL_TASK_IMPL_H
#define COTASK_IMPL_TASK_IMPL_H

#pragma once

#include <list>
#include <stdint.h>

#include <libcopp/utils/atomic_int_type.h>
#include <libcopp/utils/config/compiler_features.h>
#include <libcopp/utils/std/smart_ptr.h>

#include <libcopp/utils/lock_holder.h>
#include <libcopp/utils/spin_lock.h>

#include <libcotask/task_actions.h>

namespace cotask {
    enum EN_TASK_STATUS {
        EN_TS_INVALID = 0,
        EN_TS_CREATED,
        EN_TS_RUNNING,
        EN_TS_WAITING,
        EN_TS_DONE,
        EN_TS_CANCELED,
        EN_TS_KILLED,
        EN_TS_TIMEOUT,
    };

    namespace impl {

        class task_impl {
        protected:
            typedef task_action_impl *action_ptr_t;

            struct ext_coroutine_flag_t {
                enum type {
                    EN_ECFT_UNKNOWN = 0,
                    EN_ECFT_COTASK = 0x0100,
                    EN_ECFT_MASK = 0xFF00,
                };
            };

        private:
            task_impl(const task_impl &);

        public:
            task_impl();
            virtual ~task_impl() = 0;

            /**
             * get task status
             * @return task status
             */
            EN_TASK_STATUS get_status() const UTIL_CONFIG_NOEXCEPT {
                return static_cast<EN_TASK_STATUS>(status_.load(util::lock::memory_order_acquire));
            }

            virtual bool is_canceled() const UTIL_CONFIG_NOEXCEPT;
            virtual bool is_completed() const UTIL_CONFIG_NOEXCEPT;
            virtual bool is_faulted() const UTIL_CONFIG_NOEXCEPT;
            virtual bool is_timeout() const UTIL_CONFIG_NOEXCEPT;
            /**
             * @brief check if a cotask is exiting
             * @note cotask is exiting means the cotask is is_completed() or is killed.
             *       if a cotask is killed and is running, then is_completed() == false but is_exiting() == true,
             *       and after the cotask finished, is_completed() == true
             * @return return true if a cotask is exiting.
             */
            bool is_exiting() const UTIL_CONFIG_NOEXCEPT;

        public:
            virtual int get_ret_code() const = 0;

            virtual int start(void *priv_data, EN_TASK_STATUS expected_status = EN_TS_CREATED) = 0;
            virtual int resume(void *priv_data, EN_TASK_STATUS expected_status = EN_TS_WAITING) = 0;
            virtual int yield(void **priv_data) = 0;
            virtual int cancel(void *priv_data) = 0;
            virtual int kill(enum EN_TASK_STATUS status, void *priv_data) = 0;
            inline int kill(void *priv_data) { return kill(EN_TS_KILLED, priv_data); }

            inline int start() { return start(UTIL_CONFIG_NULLPTR); };
            inline int resume() { return resume(UTIL_CONFIG_NULLPTR); };
            inline int yield() { return yield(UTIL_CONFIG_NULLPTR); };
            inline int cancel() { return cancel(UTIL_CONFIG_NULLPTR); };
            inline int kill(enum EN_TASK_STATUS status) { return kill(status, UTIL_CONFIG_NULLPTR); };
            inline int kill() { return kill(UTIL_CONFIG_NULLPTR); };

            virtual int on_finished();

            /**
             * get current running task
             * @return current running task or empty pointer
             */
            static task_impl *this_task();

        protected:
            void _set_action(action_ptr_t action);
            action_ptr_t _get_action();

            bool _cas_status(EN_TASK_STATUS &expected, EN_TASK_STATUS desired);

            int _notify_finished(void *priv_data);

        private:
            action_ptr_t action_;

        protected:
            void *finish_priv_data_;

        private:
#if !defined(PROJECT_DISABLE_MT) || !(PROJECT_DISABLE_MT)
            ::util::lock::atomic_int_type<uint32_t> status_;
#else
            ::util::lock::atomic_int_type< ::util::lock::unsafe_int_type<uint32_t> > status_;
#endif
        };
    } // namespace impl
} // namespace cotask


#endif /* TASK_IMPL_H_ */

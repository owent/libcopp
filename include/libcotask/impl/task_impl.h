/*
 * task_impl.h
 *
 *  Created on: 2014年4月2日
 *      Author: owent
 *
 *  Released under the MIT license
 */

#ifndef _COTASK_IMPL_TASK_IMPL_H_
#define _COTASK_IMPL_TASK_IMPL_H_

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

        class task_impl : public std::enable_shared_from_this<task_impl> {
        public:
            typedef std::shared_ptr<task_impl> ptr_t;
            typedef std::shared_ptr<task_action_impl> action_ptr_t;

            struct task_group {
                std::list<std::pair<ptr_t, void *> > member_list_;
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
            EN_TASK_STATUS get_status() const UTIL_CONFIG_NOEXCEPT;

            virtual bool is_canceled() const UTIL_CONFIG_NOEXCEPT;
            virtual bool is_completed() const UTIL_CONFIG_NOEXCEPT;
            virtual bool is_faulted() const UTIL_CONFIG_NOEXCEPT;

            /**
             * @brief add next task to run when task finished
             * @note please not to make tasks refer to each other. [it will lead to memory leak]
             * @note [don't do that] ptr_t a = ..., b = ...; a.next(b); b.next(a);
             * @param next_task next stack
             * @param priv_data priv_data passed to resume or start next stack
             * @return next_task
             */
            ptr_t next(ptr_t next_task, void *priv_data = UTIL_CONFIG_NULLPTR);

        public:
            virtual int get_ret_code() const = 0;

            virtual int start(void *priv_data) = 0;
            virtual int resume(void *priv_data) = 0;
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
            void _set_action(const action_ptr_t &action);
            const action_ptr_t &_get_action();

            bool _cas_status(EN_TASK_STATUS &expected, EN_TASK_STATUS desired);

            void active_next_tasks();

            int _notify_finished(void *priv_data);

        private:
            action_ptr_t action_;
            ::util::lock::atomic_int_type<uint32_t> status_;

            util::lock::spin_lock next_list_lock_;
            task_group next_list_;

        protected:
            void *finish_priv_data_;
        };
    }
}


#endif /* TASK_IMPL_H_ */

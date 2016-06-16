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
#include <libcopp/utils/std/smart_ptr.h>
#include <libcopp/utils/config/compiler_features.h>

#include <libcotask/task_actions.h>

namespace cotask {
    enum EN_TASK_STATUS
    {
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

        class task_impl: public std::enable_shared_from_this<task_impl>
        {
        public:
            typedef std::shared_ptr<task_impl> ptr_t;
            typedef std::shared_ptr<task_action_impl> action_ptr_t;

            struct task_group {
                std::list<ptr_t> member_list_;
            };

        private:
            task_impl(const task_impl&);

        public:
            task_impl();
            virtual ~task_impl() = 0;

            /**
             * get task status
             * @return task status
             */
            inline EN_TASK_STATUS get_status() const UTIL_CONFIG_NOEXCEPT { return status_; }

            virtual bool is_canceled() const UTIL_CONFIG_NOEXCEPT;
            virtual bool is_completed() const UTIL_CONFIG_NOEXCEPT;
            virtual bool is_faulted() const UTIL_CONFIG_NOEXCEPT;

            /**
             * @brief add next task to run when task finished
             * @note please not to make tasks refer to each other. [it will lead to memory leak]
             * @note [don't do that] ptr_t a = ..., b = ...; a.next(b); b.next(a);
             * @param next_task next stack
             * @return next_task
             */
            ptr_t next(ptr_t next_task);

        public:
            virtual int get_ret_code() const = 0;

            virtual int start() = 0;
            virtual int resume() = 0;
            virtual int yield() = 0;
            virtual int cancel() = 0;
            virtual int kill(enum EN_TASK_STATUS status) = 0;
            virtual int kill();

            virtual int on_finished();

            /**
             * get current running task
             * @return current running task or empty pointer
             */
            static task_impl* this_task();

        protected:
            void _set_action(action_ptr_t action);
            action_ptr_t _get_action();

            inline void _set_status(EN_TASK_STATUS status) { status_ = status; }

            void active_next_tasks();

            int _notify_finished();

        private:
            action_ptr_t action_;
            EN_TASK_STATUS status_;

            task_group next_list_;
        };
    }
}


#endif /* TASK_IMPL_H_ */

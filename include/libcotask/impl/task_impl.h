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

#include <libcopp/utils/std/smart_ptr.h>

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
    };

    namespace impl {

        class task_impl: public std::enable_shared_from_this<task_impl>
        {
        public:
            typedef std::shared_ptr<task_impl> ptr_t;
            typedef std::shared_ptr<task_action_impl> action_ptr_t;

        private:
            task_impl(const task_impl&);

        public:
            task_impl();
            virtual ~task_impl() = 0;

            inline EN_TASK_STATUS get_status() const { return status_; }

            virtual bool is_canceled() const;
            virtual bool is_completed() const;
            virtual bool is_faulted() const;

        public:
            virtual int start() = 0;
            virtual int resume() = 0;
            virtual int yield() = 0;
            virtual int cancel() = 0;
            virtual int kill() = 0;
            virtual int on_finished();

            static ptr_t this_task();

        protected:
            void _set_action(action_ptr_t action);
            action_ptr_t _get_action();

            inline void _set_status(EN_TASK_STATUS status) { status_ = status; }

            static ptr_t _set_active_task(ptr_t);

        private:
            action_ptr_t action_;
            EN_TASK_STATUS status_;
        };
    }
}


#endif /* TASK_IMPL_H_ */

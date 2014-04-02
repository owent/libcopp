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

        class task_action_impl;

        class task_impl
        {
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

            static task_impl* this_task();

        protected:
            void _set_action(task_action_impl* action);
            impl::task_action_impl* _get_action();

            inline void _set_status(EN_TASK_STATUS status) { status_ = status; }

            static task_impl* _set_active_task(task_impl*);

        private:
            impl::task_action_impl* action_;
            EN_TASK_STATUS status_;
        };
    }
}


#endif /* TASK_IMPL_H_ */

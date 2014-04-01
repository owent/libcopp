/*
 * task.h
 *
 *  Created on: 2014年4月1日
 *      Author: owent
 *
 *  Released under the MIT license
 */

#ifndef _COTASK_TASK_H_
#define _COTASK_TASK_H_

#include <stdint.h>

#include <libcopp/utils/errno.h>
#include <libcopp/utils/features.h>
#include <libcopp/coroutine/coroutine_context_container.h>
#include <libcotask/core/standard_new_allocator.h>

#include <libcotask/task_actions.h>

namespace cotask {

    enum EN_TASK_STATUS
    {
        EN_TS_CREATED = 0,
        EN_TS_RUNNING,
        EN_TS_WAITING,
        EN_TS_DONE,
        EN_TS_CANCELED,
        EN_TS_KILLED,
    };

    template<typename TID = uint64_t,
        typename TACT_ALLOC = core::standard_new_allocator,
        typename TCO_CONTAINER = copp::coroutine_context_default
    >
    class task
    {
    public:
        typedef TID id_t;
        typedef TACT_ALLOC action_alloc_t;
        typedef TCO_CONTAINER co_t;

    public:

        /**
         * @brief create task with action
         * @param action
         * @param stack_size
         */
        task(impl::task_action_impl* action,
            size_t stack_size = co_t::allocator_type::default_stacksize()
        ): action_(action), status_(EN_TS_CREATED) {
            coroutine_obj_.create(action_, stack_size);
        }

        /**
         * @brief create task with functor
         * @param action
         * @param stack_size
         */
        template<typename Ty>
        task(Ty functor, size_t stack_size = co_t::allocator_type::default_stacksize()
        ): status_(EN_TS_CREATED) {
            action_ = action_alloc_.allocate<task_action_functor<Ty> >(functor);
            coroutine_obj_.create(action_, stack_size);
        }

        /**
         * @brief create task with function
         * @param action
         * @param stack_size
         */
        template<typename Ty>
        task(Ty (*func)(), size_t stack_size = co_t::allocator_type::default_stacksize()
        ): status_(EN_TS_CREATED) {
            action_ = action_alloc_.allocate<task_action_function<Ty> >(func);
            coroutine_obj_.create(action_, stack_size);
        }

        virtual ~task() {
            if (NULL != action_) {
                action_alloc_.deallocate(action_);
                action_ = NULL;
            }
        }

        inline action_alloc_t& get_action_allocator() { return action_alloc_; }
        inline const action_alloc_t& get_action_allocator() const { return action_alloc_; }

        inline co_t& get_coroutine_context() { return coroutine_obj_; }
        inline const co_t& get_coroutine_context() const { return coroutine_obj_; }

        inline id_t get_id() const { return id_; }

        inline EN_TASK_STATUS get_status() const { return status_; }

        inline bool is_canceled() const { return EN_TS_CANCELED == get_status(); }
        inline bool is_completed() const { return coroutine_obj_.is_finished(); }
        inline bool is_faulted() const { return EN_TS_KILLED == get_status(); }
        inline bool is_killed() const { return EN_TS_KILLED == get_status(); }

    public:
        int start() {
            if (status_ >= EN_TS_DONE) {
                return copp::COPP_EC_ALREADY_FINISHED;
            }
// TODO
//#ifdef COPP_MACRO_ENABLE_MULTI_THREAD
//#endif
            status_ = EN_TS_RUNNING;
            int ret = coroutine_obj_.start();
            if (is_completed()) {
                _notify_finished();
                status_ = EN_TS_DONE;
            }

            return ret;
        }

        int resume() {
            return start();
        }

        int yield() {
            status_ = EN_TS_WAITING;
            return coroutine_obj_.yield();
        }

        int cancel() {
            if (EN_TS_RUNNING == status_) {
                return copp::COPP_EC_IS_RUNNING;
            }

            status_ = EN_TS_CANCELED;

            _notify_finished();
            return copp::COPP_EC_SUCCESS;
        }

    public:
        virtual int on_finished() { return copp::COPP_EC_SUCCESS; }

    private:
        int _notify_finished() {
            action_->on_finished();
            return on_finished();
        }

    private:
        action_alloc_t action_alloc_;
        id_t id_;
        impl::task_action_impl* action_;
        co_t coroutine_obj_;
        EN_TASK_STATUS status_;
    };
}



#endif /* _COTASK_TASK_H_ */

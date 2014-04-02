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
#include <algorithm>

#include <libcopp/utils/errno.h>
#include <libcopp/utils/features.h>
#include <libcopp/coroutine/coroutine_context_container.h>
#include <libcotask/core/standard_new_allocator.h>

#include <libcotask/task_actions.h>
#include <libcotask/impl/task_impl.h>
#include <libcotask/this_task.h>

namespace cotask {

    template<typename TID = uint64_t,
        typename ALLOC = core::standard_new_allocator,
        typename TCO_CONTAINER = copp::coroutine_context_default
    >
    class task: public impl::task_impl
    {
    public:
        typedef TID id_t;
        typedef ALLOC alloc_t;
        typedef TCO_CONTAINER co_t;

    public:

        /**
         * @brief create task with action
         * @param action
         * @param stack_size
         */
        task(impl::task_action_impl* action,
            size_t stack_size = co_t::allocator_type::default_stacksize()
        ) {
            _set_action(action);
            coroutine_obj_.create(_get_action(), stack_size);
        }

        /**
         * @brief create task with functor
         * @param action
         * @param stack_size
         */
        template<typename Ty>
        task(Ty* action,
            size_t stack_size = co_t::allocator_type::default_stacksize()
        ) {
            _set_action(action);
            coroutine_obj_.create(_get_action(), stack_size);
        }

        /**
         * @brief create task with functor
         * @param action
         * @param stack_size
         */
        template<typename Ty>
        task(Ty functor,
            size_t stack_size = co_t::allocator_type::default_stacksize()
        ) {
            typedef Ty ap_t;
            typedef task_action_functor<Ty> a_t;
            _set_action(
                alloc_.allocate(reinterpret_cast<a_t*>(NULL), functor)
            );
            coroutine_obj_.create(_get_action(), stack_size);
        }

        /**
         * @brief create task with function
         * @param action
         * @param stack_size
         */
        template<typename Ty>
        task(Ty (*func)(),
            size_t stack_size = co_t::allocator_type::default_stacksize()
        ) {
            typedef Ty (*ap_t)();
            typedef task_action_function<Ty> a_t;
            _set_action(
                alloc_.allocate(reinterpret_cast<a_t*>(NULL), func)
            );
            coroutine_obj_.create(_get_action(), stack_size);
        }

        virtual ~task() {
            if (NULL != _get_action()) {
                alloc_.deallocate(_get_action());
                _set_action(NULL);
            }
        }

        inline alloc_t& get_allocator() { return alloc_; }
        inline const alloc_t& get_allocator() const { return alloc_; }

        inline co_t& get_coroutine_context() { return coroutine_obj_; }
        inline const co_t& get_coroutine_context() const { return coroutine_obj_; }

        inline id_t get_id() const { return id_; }

    public:
        virtual int start() {
            if (get_status() >= EN_TS_DONE) {
                return copp::COPP_EC_ALREADY_FINISHED;
            }
// TODO
//#ifdef COPP_MACRO_ENABLE_MULTI_THREAD
//#endif
            _set_status(EN_TS_RUNNING);

            impl::task_impl* origin_task = _set_active_task(this);
            int ret = coroutine_obj_.start();
            _set_active_task(origin_task);

            if (get_status() > EN_TS_DONE) { // canceled or killed
                _notify_finished();
            } else if (is_completed()) { // completed
                _set_status(EN_TS_DONE);
                _notify_finished();
            } else { // waiting
                _set_status(EN_TS_WAITING);
            }

            return ret;
        }

        virtual int resume() {
            return start();
        }

        virtual int yield() {
            return coroutine_obj_.yield();
        }

        virtual int cancel() {
            if (EN_TS_RUNNING == get_status()) {
                return copp::COPP_EC_IS_RUNNING;
            }

            _set_status(EN_TS_CANCELED);

            _notify_finished();
            return copp::COPP_EC_SUCCESS;
        }

        virtual int kill() {
            _set_status(EN_TS_KILLED);

            if (EN_TS_RUNNING != get_status()) {
                _notify_finished();
            }

            return copp::COPP_EC_SUCCESS;
        }

    public:
        virtual bool is_completed() const { return coroutine_obj_.is_finished(); }

    private:
        int _notify_finished() {
            _get_action()->on_finished();
            return on_finished();
        }

        task(const task&);

    private:
        alloc_t alloc_;
        id_t id_;
        co_t coroutine_obj_;
    };
}



#endif /* _COTASK_TASK_H_ */

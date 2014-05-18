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

#include <libcotask/task_macros.h>
#include <libcotask/this_task.h>

namespace cotask {

    template<typename TCO_MACRO = macro_coroutine,
        typename TTASK_MACRO = macro_task
    >
    class task: public impl::task_impl
    {
    public:
        typedef task<TCO_MACRO, TTASK_MACRO> self_t;
        typedef TCO_MACRO macro_coroutine_t;
        typedef TTASK_MACRO macro_task_t;

        typedef typename macro_coroutine_t::coroutine_t coroutine_t;
        typedef typename macro_coroutine_t::stack_allocator_t stack_allocator_t;
        typedef typename macro_coroutine_t::coroutine_container_t coroutine_container_t;

        typedef typename macro_task_t::id_t id_t;
        typedef typename macro_task_t::id_allocator_t id_allocator_t;

        typedef typename macro_task_t::action_allocator_t action_allocator_t;
        typedef typename macro_task_t::task_allocator_t task_allocator_t;

        typedef std::shared_ptr<self_t> ptr_t;
        typedef impl::task_impl::action_ptr_t action_ptr_t;

        friend task_allocator_t;
    private:
        task() {
            id_allocator_t id_alloc_;
            id_ = id_alloc_.allocate();
        }

        static void _action_deleter(impl::task_action_impl* action) {
            action_allocator_t::deallocate(action);
        }

    public:
        /**
         * @brief create task with action
         * @param action
         * @param stack_size stack size
         * @return task smart pointer
         */
        static ptr_t create(action_ptr_t action,
            size_t stack_size = stack_allocator_t::default_stacksize()
        ) {
            // step 1. create task instance
            self_t* inst = task_allocator_t::allocate(static_cast<self_t*>(NULL));
            ptr_t ret = ptr_t(inst);
            if (NULL == inst)
                return ret;

            // step 2. set action
            ret->_set_action(action);

            // step 3. init coroutine context
            int res = ret->get_coroutine_context().create(ret->_get_action().get(), stack_size);
            if (res < 0)
                return ptr_t();

            return ret;
        }

        /**
         * @brief create task with functor
         * @param action
         * @param stack_size stack size
         * @return task smart pointer
         */
        template<typename Ty>
        static ptr_t create(Ty functor,
            size_t stack_size = stack_allocator_t::default_stacksize()
        ) {
            typedef task_action_functor<Ty> a_t;

            // step 1. create task instance
            self_t* inst = task_allocator_t::allocate(static_cast<self_t*>(NULL));
            ptr_t ret = ptr_t(inst);
            if (NULL == inst)
                return ret;

            // step 2. create action
            action_ptr_t action = action_ptr_t(
                action_allocator_t::allocate(
                    reinterpret_cast<a_t*>(NULL),
                    functor
                ),
                _action_deleter
            );

            if (!action)
                return ptr_t();
            ret->_set_action(action);

            // step 3. init coroutine context
            int res = ret->get_coroutine_context().create(ret->_get_action().get(), stack_size);
            if (res < 0)
                return ptr_t();

            return ret;
        }

        /**
         * @brief create task with function
         * @param action
         * @param stack_size stack size
         * @return task smart pointer
         */
        template<typename Ty>
        static ptr_t create(Ty (*func)(),
            size_t stack_size = stack_allocator_t::default_stacksize()
        ) {
            typedef task_action_function<Ty> a_t;

            // step 1. create task instance
            self_t* inst = task_allocator_t::allocate(static_cast<self_t*>(NULL));
            ptr_t ret = ptr_t(inst);
            if (NULL == inst)
                return ret;

            // step 2. create action
            action_ptr_t action = action_ptr_t(
                action_allocator_t::allocate(
                    reinterpret_cast<a_t*>(NULL),
                    func
                ),
                _action_deleter
            );

            if (!action)
                return ptr_t();
            ret->_set_action(action);

            // step 3. init coroutine context
            int res = ret->get_coroutine_context().create(ret->_get_action().get(), stack_size);
            if (res < 0)
                return ptr_t();

            return ret;
        }

        /**
         * @brief create task with function
         * @param action
         * @param stack_size stack size
         * @return task smart pointer
         */
        template<typename Ty, typename TInst>
        static ptr_t create(Ty (TInst::*func), TInst* instance,
            size_t stack_size = stack_allocator_t::default_stacksize()
        ) {
            typedef task_action_mem_function<Ty, TInst> a_t;

            // step 1. create task instance
            self_t* inst = task_allocator_t::allocate(static_cast<self_t*>(NULL));
            ptr_t ret = ptr_t(inst);
            if (NULL == inst)
                return ret;

            // step 2. create action
            action_ptr_t action = action_ptr_t(
                action_allocator_t::allocate(
                    reinterpret_cast<a_t*>(NULL),
                    func,
                    instance
                ),
                _action_deleter
            );

            if (!action)
                return ptr_t();
            ret->_set_action(action);

            // step 3. init coroutine context
            int res = ret->get_coroutine_context().create(ret->_get_action().get(), stack_size);
            if (res < 0)
                return ptr_t();

            return ret;
        }


        /**
         * get current running task and convert to task object
         * @return task smart pointer
         */
        static ptr_t this_task() {
            return std::dynamic_pointer_cast<self_t>(impl::task_impl::this_task());
        }
    public:
        virtual ~task() {
            id_allocator_t id_alloc_;
            id_alloc_.deallocate(id_);
        }

        inline coroutine_container_t& get_coroutine_context() { return coroutine_obj_; }
        inline const coroutine_container_t& get_coroutine_context() const { return coroutine_obj_; }

        inline id_t get_id() const { return id_; }

    public:
        virtual int start() {
            if (get_status() >= EN_TS_DONE) {
                return copp::COPP_EC_ALREADY_FINISHED;
            }
// TODO
//#ifdef COPP_MACRO_ENABLE_MULTI_THREAD
//#endif
            if (get_status() == EN_TS_RUNNING) {
                return copp::COPP_EC_IS_RUNNING;
            }
            _set_status(EN_TS_RUNNING);

            impl::task_impl::ptr_t self = shared_from_this();
            impl::task_impl::ptr_t origin_task = _set_active_task(self.get());
            int ret = coroutine_obj_.start();
            _set_active_task(origin_task.get());

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
        id_t id_;
        coroutine_container_t coroutine_obj_;
    };
}



#endif /* _COTASK_TASK_H_ */

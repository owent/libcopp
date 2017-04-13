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

#include <algorithm>
#include <stdint.h>


#include <libcopp/stack/stack_traits.h>
#include <libcotask/task_macros.h>
#include <libcotask/this_task.h>

namespace cotask {

    template <typename TCO_MACRO = macro_coroutine, typename TTASK_MACRO = macro_task>
    class task : public impl::task_impl {
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
        typedef impl::task_impl::ptr_t task_ptr_t;

    private:
        static void _action_deleter(impl::task_action_impl *action) { action_allocator_t::deallocate(action); }

    public:
        /**
         * @brief constuctor
         * @note should not be called directly
         */
        task() {
            id_allocator_t id_alloc_;
            id_ = id_alloc_.allocate();

            coroutine_obj_.set_private_data(static_cast<task_impl *>(this));
        }

        /**
         * @brief create task with action
         * @param action
         * @param stack_size stack size
         * @return task smart pointer
         */
        static ptr_t create(action_ptr_t action, size_t stack_size = copp::stack_traits::default_size()) {
            // step 1. create task instance
            self_t *inst = task_allocator_t::allocate(static_cast<self_t *>(NULL));
            ptr_t ret = ptr_t(inst);
            if (NULL == inst) {
                return ret;
            }

            // step 2. set action
            ret->_set_action(action);

            // step 3. init coroutine context
            int res = ret->get_coroutine_context().create(ret->_get_action().get(), stack_size);
            if (res < 0) {
                return ptr_t();
            }

            return ret;
        }

/**
 * @brief create task with functor
 * @param action
 * @param stack_size stack size
 * @return task smart pointer
 */
#if defined(UTIL_CONFIG_COMPILER_CXX_RVALUE_REFERENCES) && UTIL_CONFIG_COMPILER_CXX_RVALUE_REFERENCES
        template <typename Ty>
        static ptr_t create(Ty &&functor, size_t stack_size = copp::stack_traits::default_size()) {
#else
        template <typename Ty>
        static ptr_t create(const Ty &functor, size_t stack_size = copp::stack_traits::default_size()) {
#endif
            typedef task_action_functor<Ty> a_t;

            // step 1. create task instance
            self_t *inst = task_allocator_t::allocate(static_cast<self_t *>(NULL));
            ptr_t ret = ptr_t(inst);
            if (NULL == inst) {
                return ret;
            }

            // step 2. create action
            action_ptr_t action = action_ptr_t(action_allocator_t::allocate(reinterpret_cast<a_t *>(NULL),
#if defined(UTIL_CONFIG_COMPILER_CXX_RVALUE_REFERENCES) && UTIL_CONFIG_COMPILER_CXX_RVALUE_REFERENCES
                                                                            std::forward<Ty>(functor)
#else
                                                                            functor
#endif
                                                                                ),
                                               _action_deleter);

            if (!action) {
                return ptr_t();
            }

            ret->_set_action(action);

            // step 3. init coroutine context
            int res = ret->get_coroutine_context().create(ret->_get_action().get(), stack_size);
            if (res < 0) return ptr_t();

            return ret;
        }

        /**
         * @brief create task with function
         * @param action
         * @param stack_size stack size
         * @return task smart pointer
         */
        template <typename Ty>
        static ptr_t create(Ty (*func)(), size_t stack_size = copp::stack_traits::default_size()) {
            typedef task_action_function<Ty> a_t;

            // step 1. create task instance
            self_t *inst = task_allocator_t::allocate(static_cast<self_t *>(NULL));
            ptr_t ret = ptr_t(inst);
            if (NULL == inst) return ret;

            // step 2. create action
            action_ptr_t action = action_ptr_t(action_allocator_t::allocate(reinterpret_cast<a_t *>(NULL), func), _action_deleter);

            if (!action) {
                return ptr_t();
            }

            ret->_set_action(action);

            // step 3. init coroutine context
            int res = ret->get_coroutine_context().create(ret->_get_action().get(), stack_size);
            if (res < 0) return ptr_t();

            return ret;
        }

        /**
         * @brief create task with function
         * @param action
         * @param stack_size stack size
         * @return task smart pointer
         */
        template <typename Ty, typename TInst>
        static ptr_t create(Ty(TInst::*func), TInst *instance, size_t stack_size = copp::stack_traits::default_size()) {
            typedef task_action_mem_function<Ty, TInst> a_t;

            // step 1. create task instance
            self_t *inst = task_allocator_t::allocate(static_cast<self_t *>(NULL));
            ptr_t ret = ptr_t(inst);
            if (NULL == inst) {
                return ret;
            }

            // step 2. create action
            action_ptr_t action =
                action_ptr_t(action_allocator_t::allocate(reinterpret_cast<a_t *>(NULL), func, instance), _action_deleter);

            if (!action) {
                return ptr_t();
            }

            ret->_set_action(action);

            // step 3. init coroutine context
            int res = ret->get_coroutine_context().create(ret->_get_action().get(), stack_size);
            if (res < 0) return ptr_t();

            return ret;
        }

#if defined(UTIL_CONFIG_COMPILER_CXX_RVALUE_REFERENCES) && UTIL_CONFIG_COMPILER_CXX_RVALUE_REFERENCES
        /**
        * @brief create task with functor type and parameters
        * @param stack_size stack size
        * @param args all parameters passed to construtor of type Ty
        * @return task smart pointer
        */
        template <typename Ty, typename... TParams>
        static ptr_t create_with(size_t stack_size, TParams &&... args) {
            typedef Ty a_t;

            // step 1. create task instance
            self_t *inst = task_allocator_t::allocate(static_cast<self_t *>(NULL));
            ptr_t ret = ptr_t(inst);
            if (NULL == inst) return ret;

            // step 2. create action
            action_ptr_t action =
                action_ptr_t(action_allocator_t::allocate(reinterpret_cast<a_t *>(NULL), std::forward<TParams>(args)...), _action_deleter);

            if (!action) {
                return ptr_t();
            }

            ret->_set_action(action);

            // step 3. init coroutine context
            int res = ret->get_coroutine_context().create(ret->_get_action().get(), stack_size);
            if (res < 0) return ptr_t();

            return ret;
        }
#endif


        using impl::task_impl::next;

        /**
         * @brief add next task to run when task finished
         * @note please not to make tasks refer to each other. [it will lead to memory leak]
         * @note [don't do that] ptr_t a = ..., b = ...; a.next(b); b.next(a);
         * @see impl::task_impl::next
         * @param next_task next stack
         * @param priv_data priv_data passed to resume or start next stack
         * @return next_task
         */
        inline ptr_t next(ptr_t next_task, void *priv_data = UTIL_CONFIG_NULLPTR) {
            return std::static_pointer_cast<self_t>(impl::task_impl::next(std::static_pointer_cast<impl::task_impl>(next_task), priv_data));
        }

        /**
         * @brief create next task with action
         * @see next
         * @param action task action
         * @param priv_data priv_data passed to start task action
         * @param stack_size stack size
         * @return task smart pointer
         */
        inline ptr_t next(action_ptr_t action, void *priv_data = UTIL_CONFIG_NULLPTR,
                          size_t stack_size = copp::stack_traits::default_size()) {
            return next(create(action, stack_size), priv_data);
        }

/**
 * @brief create next task with functor
 * @see next
 * @param functor
 * @param priv_data priv_data passed to start functor
 * @param stack_size stack size
 * @return task smart pointer
 */
#if defined(UTIL_CONFIG_COMPILER_CXX_RVALUE_REFERENCES) && UTIL_CONFIG_COMPILER_CXX_RVALUE_REFERENCES
        template <typename Ty>
        inline ptr_t next(Ty &&functor, void *priv_data = UTIL_CONFIG_NULLPTR, size_t stack_size = copp::stack_traits::default_size()) {
            return next(create(std::forward<Ty>(functor), stack_size), priv_data);
        }
#else
        template <typename Ty>
        inline ptr_t next(Ty functor, void *priv_data = UTIL_CONFIG_NULLPTR, size_t stack_size = copp::stack_traits::default_size()) {
            return next(create(functor, stack_size), priv_data);
        }
#endif

        /**
         * @brief create next task with function
         * @see next
         * @param func function
         * @param priv_data priv_data passed to start function
         * @param stack_size stack size
         * @return task smart pointer
         */
        template <typename Ty>
        inline ptr_t next(Ty (*func)(void *), void *priv_data = UTIL_CONFIG_NULLPTR,
                          size_t stack_size = copp::stack_traits::default_size()) {
            return next(create(func, stack_size), priv_data);
        }

        /**
         * @brief create next task with function
         * @see next
         * @param func member function
         * @param instance instance
         * @param priv_data priv_data passed to start (instance->*func)(priv_data)
         * @param stack_size stack size
         * @return task smart pointer
         */
        template <typename Ty, typename TInst>
        inline ptr_t next(Ty(TInst::*func), TInst *instance, void *priv_data = UTIL_CONFIG_NULLPTR,
                          size_t stack_size = copp::stack_traits::default_size()) {
            return next(create(func, instance, stack_size), priv_data);
        }

        /**
         * get current running task and convert to task object
         * @return task smart pointer
         */
        static self_t *this_task() { return dynamic_cast<self_t *>(impl::task_impl::this_task()); }

    public:
        virtual ~task() {
            // inited but not finished will trigger timeout or finish other actor
            if (get_status() < EN_TS_DONE && get_status() > EN_TS_CREATED) {
                kill(EN_TS_TIMEOUT);
            }

            // free resource
            id_allocator_t id_alloc_;
            id_alloc_.deallocate(id_);
        }

        inline coroutine_container_t &get_coroutine_context() UTIL_CONFIG_NOEXCEPT { return coroutine_obj_; }
        inline const coroutine_container_t &get_coroutine_context() const UTIL_CONFIG_NOEXCEPT { return coroutine_obj_; }

        inline id_t get_id() const UTIL_CONFIG_NOEXCEPT { return id_; }

    public:
        virtual int get_ret_code() const UTIL_CONFIG_OVERRIDE { return coroutine_obj_.get_ret_code(); }

        virtual int start(void *priv_data) UTIL_CONFIG_OVERRIDE {
            EN_TASK_STATUS from_status = get_status();

            do {
                if (from_status >= EN_TS_DONE) {
                    return copp::COPP_EC_ALREADY_FINISHED;
                }

                if (from_status == EN_TS_RUNNING) {
                    return copp::COPP_EC_IS_RUNNING;
                }

                if (likely(_cas_status(from_status, EN_TS_RUNNING))) {
                    break;
                }
            } while (true);

            int ret = coroutine_obj_.start(priv_data);

            from_status = get_status();
            if (from_status > EN_TS_DONE) { // canceled or killed
                _notify_finished(finish_priv_data_);

            } else if (is_completed()) { // completed
                while (!likely(_cas_status(from_status, EN_TS_DONE)))
                    ;

                _notify_finished(finish_priv_data_ = priv_data);
            } else { // waiting
                while (!likely(_cas_status(from_status, EN_TS_WAITING)))
                    ;
            }

            return ret;
        }

        virtual int resume(void *priv_data) UTIL_CONFIG_OVERRIDE { return start(priv_data); }

        virtual int yield(void **priv_data) UTIL_CONFIG_OVERRIDE { return coroutine_obj_.yield(priv_data); }

        virtual int cancel(void *priv_data) UTIL_CONFIG_OVERRIDE {
            EN_TASK_STATUS from_status = get_status();

            do {
                if (EN_TS_RUNNING == from_status) {
                    return copp::COPP_EC_IS_RUNNING;
                }

                if (likely(_cas_status(from_status, EN_TS_CANCELED))) {
                    break;
                }
            } while (true);

            _notify_finished(priv_data);
            return copp::COPP_EC_SUCCESS;
        }

        virtual int kill(enum EN_TASK_STATUS status, void *priv_data) UTIL_CONFIG_OVERRIDE {
            EN_TASK_STATUS from_status = get_status();

            do {
                if (likely(_cas_status(from_status, status))) {
                    break;
                }
            } while (true);

            if (EN_TS_RUNNING != from_status) {
                _notify_finished(priv_data);
            } else {
                finish_priv_data_ = priv_data;
            }

            return copp::COPP_EC_SUCCESS;
        }

        using impl::task_impl::kill;
        using impl::task_impl::start;
        using impl::task_impl::resume;
        using impl::task_impl::yield;
        using impl::task_impl::cancel;

    public:
        virtual bool is_completed() const UTIL_CONFIG_NOEXCEPT UTIL_CONFIG_OVERRIDE { return coroutine_obj_.is_finished(); }

    private:
        task(const task &) UTIL_CONFIG_DELETED_FUNCTION;

        int _notify_finished(void *priv_data) {
            // first, make sure coroutine finished.
            if (false == coroutine_obj_.is_finished()) {
                // make sure this task will not be destroyed when running
                // because this function may be called by destructor and shared_ptr is already free
                // so any function that use shared_ptr or weak_ptr of this task should be deny
                while (false == coroutine_obj_.is_finished()) {
                    coroutine_obj_.resume(priv_data);
                }
            }

            return impl::task_impl::_notify_finished(priv_data);
        }

    private:
        id_t id_;
        coroutine_container_t coroutine_obj_;
    };
}


#endif /* _COTASK_TASK_H_ */

/*
 * task.h
 *
 *  Created on: 2014年4月1日
 *      Author: owent
 *
 *  Released under the MIT license
 */

#ifndef COTASK_TASK_H
#define COTASK_TASK_H

#pragma once

#include <algorithm>
#include <cstddef>
#include <list>
#include <stdint.h>

#include <libcopp/utils/uint64_id_allocator.h>
#include <libcopp/utils/config/libcopp_build_features.h>
#include <libcopp/future/std_coroutine_generator.h>

#if defined(LIBCOPP_MACRO_ENABLE_STD_EXCEPTION_PTR) && LIBCOPP_MACRO_ENABLE_STD_EXCEPTION_PTR
#include <exception>
#endif

#if defined(LIBCOPP_MACRO_ENABLE_WIN_FIBER) && LIBCOPP_MACRO_ENABLE_WIN_FIBER
#include <type_traits>
#endif

#include <libcopp/stack/stack_traits.h>
#include <libcopp/utils/errno.h>
#include <libcotask/task_macros.h>
#include <libcotask/this_task.h>

namespace cotask {

    template <typename TCO_MACRO = macro_coroutine>
    class LIBCOPP_COTASK_API_HEAD_ONLY task : public impl::task_impl {
    public:
        typedef task<TCO_MACRO>                      self_t;
        typedef libcopp::util::intrusive_ptr<self_t> ptr_t;
        typedef TCO_MACRO                            macro_coroutine_t;

        typedef typename macro_coroutine_t::coroutine_t       coroutine_t;
        typedef typename macro_coroutine_t::stack_allocator_t stack_allocator_t;

        typedef typename copp::util::uint64_id_allocator::value_type id_t;
        typedef typename copp::util::uint64_id_allocator id_allocator_t;

        struct task_group {
            std::list<std::pair<ptr_t, void *> > member_list_;
        };

    private:
        typedef impl::task_impl::action_ptr_t action_ptr_t;

    public:
        /**
         * @brief constuctor
         * @note should not be called directly
         */
        task(size_t stack_sz)
            : id_(0), stack_size_(stack_sz), action_destroy_fn_(UTIL_CONFIG_NULLPTR)
#if defined(LIBCOTASK_MACRO_AUTO_CLEANUP_MANAGER) && LIBCOTASK_MACRO_AUTO_CLEANUP_MANAGER
              ,
              binding_manager_ptr_(UTIL_CONFIG_NULLPTR), binding_manager_fn_(UTIL_CONFIG_NULLPTR)
#endif
        {
            id_allocator_t id_alloc_;
            ((void)id_alloc_);
            id_ = id_alloc_.allocate();
            ref_count_.store(0);
        }


/**
 * @brief create task with functor
 * @param action
 * @param stack_size stack size
 * @param private_buffer_size buffer size to store private data
 * @return task smart pointer
 */
#if defined(UTIL_CONFIG_COMPILER_CXX_RVALUE_REFERENCES) && UTIL_CONFIG_COMPILER_CXX_RVALUE_REFERENCES
        template <typename TAct, typename Ty>
        static LIBCOPP_COTASK_API_HEAD_ONLY ptr_t create_with_delegate(Ty &&callable, typename coroutine_t::allocator_type &alloc,
                                                                       size_t stack_size = 0, size_t private_buffer_size = 0) {
#else
        template <typename TAct, typename Ty>
        static LIBCOPP_COTASK_API_HEAD_ONLY ptr_t create_with_delegate(const Ty &callable, typename coroutine_t::allocator_type &alloc,
                                                                       size_t stack_size = 0, size_t private_buffer_size = 0) {
#endif
            typedef TAct a_t;

            if (0 == stack_size) {
                stack_size = copp::stack_traits::default_size();
            }

            size_t action_size = coroutine_t::align_address_size(sizeof(a_t));
            size_t task_size   = coroutine_t::align_address_size(sizeof(self_t));

            if (stack_size <= sizeof(impl::task_impl *) + private_buffer_size + action_size + task_size) {
                return ptr_t();
            }

            typename coroutine_t::ptr_t coroutine = coroutine_t::create(
                (a_t *)(UTIL_CONFIG_NULLPTR), alloc, stack_size, sizeof(impl::task_impl *) + private_buffer_size, action_size + task_size);
            if (!coroutine) {
                return ptr_t();
            }

            void *action_addr = sub_buffer_offset(coroutine.get(), action_size);
            void *task_addr   = sub_buffer_offset(action_addr, task_size);

            // placement new task
            ptr_t ret(new (task_addr) self_t(stack_size));
            if (!ret) {
                return ret;
            }

            *(reinterpret_cast<impl::task_impl **>(coroutine->get_private_buffer())) = ret.get();
            ret->coroutine_obj_                                                      = coroutine;
            ret->coroutine_obj_->set_flags(impl::task_impl::ext_coroutine_flag_t::EN_ECFT_COTASK);

            // placement new action
            a_t *action = new (action_addr) a_t(std::forward<Ty>(callable));
            if (UTIL_CONFIG_NULLPTR == action) {
                return ret;
            }

            typedef int (a_t::*a_t_fn_t)(void *);
            a_t_fn_t a_t_fn = &a_t::operator();

            // redirect runner
            coroutine->set_runner(
#if defined(UTIL_CONFIG_COMPILER_CXX_RVALUE_REFERENCES) && UTIL_CONFIG_COMPILER_CXX_RVALUE_REFERENCES
                std::move(std::bind(a_t_fn, action, std::placeholders::_1))
#else
                std::bind(a_t_fn, action, std::placeholders::_1)
#endif
            );

            ret->action_destroy_fn_ = get_placement_destroy(action);
            ret->_set_action(action);

            return ret;
        }


/**
 * @brief create task with functor
 * @param action
 * @param stack_size stack size
 * @param private_buffer_size buffer size to store private data
 * @return task smart pointer
 */
#if defined(UTIL_CONFIG_COMPILER_CXX_RVALUE_REFERENCES) && UTIL_CONFIG_COMPILER_CXX_RVALUE_REFERENCES
        template <typename Ty>
        static inline ptr_t create(Ty &&functor, size_t stack_size = 0, size_t private_buffer_size = 0) {
            typename coroutine_t::allocator_type alloc;
            return create(std::forward<Ty>(functor), alloc, stack_size, private_buffer_size);
        }

        template <typename Ty>
        static inline ptr_t create(Ty &&functor, typename coroutine_t::allocator_type &alloc, size_t stack_size = 0,
                                   size_t private_buffer_size = 0) {
            typedef typename std::conditional<std::is_base_of<impl::task_action_impl, Ty>::value, Ty, task_action_functor<Ty> >::type a_t;
            return create_with_delegate<a_t>(std::forward<Ty>(functor), alloc, stack_size, private_buffer_size);
        }
#else
        template <typename Ty>
        static inline ptr_t create(const Ty &functor, size_t stack_size = 0, size_t private_buffer_size = 0) {
            typename coroutine_t::allocator_type alloc;
            return create(functor, alloc, stack_size, private_buffer_size);
        }

        template <typename Ty>
        static LIBCOPP_COTASK_API_HEAD_ONLY ptr_t create(const Ty &functor, typename coroutine_t::allocator_type &alloc,
                                                         size_t stack_size = 0, size_t private_buffer_size = 0) {
            typedef typename std::conditional<std::is_base_of<impl::task_action_impl, Ty>::value, Ty, task_action_functor<Ty> >::type a_t;
            return create_with_delegate<a_t>(std::forward<Ty>(functor), alloc, stack_size, private_buffer_size);
        }
#endif

        /**
         * @brief create task with function
         * @param action
         * @param stack_size stack size
         * @return task smart pointer
         */
        template <typename Ty>
        static inline ptr_t create(Ty (*func)(void *), typename coroutine_t::allocator_type &alloc, size_t stack_size = 0,
                                   size_t private_buffer_size = 0) {
            typedef task_action_function<Ty> a_t;

            return create_with_delegate<a_t>(func, alloc, stack_size, private_buffer_size);
        }

        template <typename Ty>
        static inline ptr_t create(Ty (*func)(void *), size_t stack_size = 0, size_t private_buffer_size = 0) {
            typename coroutine_t::allocator_type alloc;
            return create(func, alloc, stack_size, private_buffer_size);
        }

        /**
         * @brief create task with function
         * @param action
         * @param stack_size stack size
         * @return task smart pointer
         */
        template <typename Ty, typename TInst>
        static LIBCOPP_COTASK_API_HEAD_ONLY ptr_t create(Ty(TInst::*func), TInst *instance, typename coroutine_t::allocator_type &alloc,
                                                         size_t stack_size = 0, size_t private_buffer_size = 0) {
            typedef task_action_mem_function<Ty, TInst> a_t;

            return create<a_t>(a_t(func, instance), alloc, stack_size, private_buffer_size);
        }

        template <typename Ty, typename TInst>
        static inline ptr_t create(Ty(TInst::*func), TInst *instance, size_t stack_size = 0, size_t private_buffer_size = 0) {
            typename coroutine_t::allocator_type alloc;
            return create(func, instance, alloc, stack_size, private_buffer_size);
        }

#if defined(UTIL_CONFIG_COMPILER_CXX_VARIADIC_TEMPLATES) && UTIL_CONFIG_COMPILER_CXX_VARIADIC_TEMPLATES
        /**
         * @brief create task with functor type and parameters
         * @param stack_size stack size
         * @param args all parameters passed to construtor of type Ty
         * @return task smart pointer
         */
        template <typename Ty, typename... TParams>
        static LIBCOPP_COTASK_API_HEAD_ONLY ptr_t create_with(typename coroutine_t::allocator_type &alloc, size_t stack_size,
                                                              size_t private_buffer_size, TParams &&... args) {
            typedef Ty a_t;

            return create(std::move(a_t(std::forward<TParams>(args)...)), alloc, stack_size, private_buffer_size);
        }
#endif

        /**
         * @brief add next task to run when task finished
         * @note please not to make tasks refer to each other. [it will lead to memory leak]
         * @note [don't do that] ptr_t a = ..., b = ...; a.next(b); b.next(a);
         * @param next_task next stack
         * @param priv_data priv_data passed to resume or start next stack
         * @return next_task if success , or self if failed
         */
        inline ptr_t next(ptr_t next_task, void *priv_data = UTIL_CONFIG_NULLPTR) {
            // can not refers to self
            if (this == next_task.get() || !next_task) {
                return ptr_t(this);
            }

            // can not add next task when finished
            if (is_exiting() || is_completed()) {
                // run next task immedialy
                EN_TASK_STATUS next_task_status = next_task->get_status();
                if (EN_TS_CREATED == next_task_status) {
                    next_task->start(priv_data);
                } else if (EN_TS_WAITING == next_task_status) {
                    next_task->resume(priv_data);
                }
                return next_task;
            }

#if !defined(LIBCOPP_DISABLE_ATOMIC_LOCK) || !(LIBCOPP_DISABLE_ATOMIC_LOCK)
            libcopp::util::lock::lock_holder<libcopp::util::lock::spin_lock> lock_guard(inner_action_lock_);
#endif

            next_list_.member_list_.push_back(std::make_pair(next_task, priv_data));
            return next_task;
        }

/**
 * @brief create next task with functor
 * @see next
 * @param functor
 * @param priv_data priv_data passed to start functor
 * @param stack_size stack size
 * @return the created task if success , or self if failed
 */
#if defined(UTIL_CONFIG_COMPILER_CXX_RVALUE_REFERENCES) && UTIL_CONFIG_COMPILER_CXX_RVALUE_REFERENCES
        template <typename Ty>
        inline ptr_t next(Ty &&functor, void *priv_data = UTIL_CONFIG_NULLPTR, size_t stack_size = 0, size_t private_buffer_size = 0) {
            return next(create(std::forward<Ty>(functor), stack_size, private_buffer_size), priv_data);
        }

        template <typename Ty>
        inline ptr_t next(Ty &&functor, typename coroutine_t::allocator_type &alloc, void *priv_data = UTIL_CONFIG_NULLPTR,
                          size_t stack_size = 0, size_t private_buffer_size = 0) {
            return next(create(std::forward<Ty>(functor), alloc, stack_size, private_buffer_size), priv_data);
        }
#else
        template <typename Ty>
        inline ptr_t next(const Ty &functor, void *priv_data = UTIL_CONFIG_NULLPTR, size_t stack_size = 0, size_t private_buffer_size = 0) {
            return next(create(functor, stack_size, private_buffer_size), priv_data);
        }

        template <typename Ty>
        inline ptr_t next(const Ty &functor, typename coroutine_t::allocator_type &alloc, void *priv_data = UTIL_CONFIG_NULLPTR,
                          size_t stack_size = 0, size_t private_buffer_size = 0) {
            return next(create(std::forward<Ty>(functor), alloc, stack_size, private_buffer_size), priv_data);
        }
#endif

        /**
         * @brief create next task with function
         * @see next
         * @param func function
         * @param priv_data priv_data passed to start function
         * @param stack_size stack size
         * @return the created task if success , or self if failed
         */
        template <typename Ty>
        inline ptr_t next(Ty (*func)(void *), void *priv_data = UTIL_CONFIG_NULLPTR, size_t stack_size = 0,
                          size_t private_buffer_size = 0) {
            return next(create(func, stack_size, private_buffer_size), priv_data);
        }

        template <typename Ty>
        inline ptr_t next(Ty (*func)(void *), typename coroutine_t::allocator_type &alloc, void *priv_data = UTIL_CONFIG_NULLPTR,
                          size_t stack_size = 0, size_t private_buffer_size = 0) {
            return next(create(func, alloc, stack_size, private_buffer_size), priv_data);
        }

        /**
         * @brief create next task with function
         * @see next
         * @param func member function
         * @param instance instance
         * @param priv_data priv_data passed to start (instance->*func)(priv_data)
         * @param stack_size stack size
         * @return the created task if success , or self if failed
         */
        template <typename Ty, typename TInst>
        inline ptr_t next(Ty(TInst::*func), TInst *instance, void *priv_data = UTIL_CONFIG_NULLPTR, size_t stack_size = 0,
                          size_t private_buffer_size = 0) {
            return next(create(func, instance, stack_size, private_buffer_size), priv_data);
        }

        template <typename Ty, typename TInst>
        inline ptr_t next(Ty(TInst::*func), TInst *instance, typename coroutine_t::allocator_type &alloc,
                          void *priv_data = UTIL_CONFIG_NULLPTR, size_t stack_size = 0, size_t private_buffer_size = 0) {
            return next(create(func, instance, alloc, stack_size, private_buffer_size), priv_data);
        }

        /**
         * @brief await_task another cotask to finish
         * @note please not to make tasks refer to each other. [it will lead to memory leak]
         * @note [don't do that] ptr_t a = ..., b = ...; a.await_task(b); b.await_task(a);
         * @param wait_task which stack to wait for
         * @note we will loop wait and ignore any resume(...) call
         * @return 0 or error code
         */
        inline int await_task(ptr_t wait_task) {
            if (!wait_task) {
                return copp::COPP_EC_ARGS_ERROR;
            }

            if (this == wait_task.get()) {
                return copp::COPP_EC_TASK_CAN_NOT_WAIT_SELF;
            }

            // if target is exiting or completed, just return
            if (wait_task->is_exiting() || wait_task->is_completed()) {
                return copp::COPP_EC_TASK_IS_EXITING;
            }

            if (is_exiting()) {
                return copp::COPP_EC_TASK_IS_EXITING;
            }

            if (this_task() != this) {
                return copp::COPP_EC_TASK_NOT_IN_ACTION;
            }

            // add to next list failed
            if (wait_task->next(ptr_t(this)).get() != this) {
                return copp::COPP_EC_TASK_ADD_NEXT_FAILED;
            }

            int ret = 0;
            while (!(wait_task->is_exiting() || wait_task->is_completed())) {
                if (is_exiting()) {
                    return copp::COPP_EC_TASK_IS_EXITING;
                }

                ret = yield();
            }

            return ret;
        }

        template <typename TTask>
        inline int await_task(TTask *wait_task) {
            return await_task(ptr_t(wait_task));
        }

        /**
         * @brief add task to run when task finished
         * @note please not to make tasks refer to each other. [it will lead to memory leak]
         * @note [don't do that] ptr_t a = ..., b = ...; a.then(b); b.then(a);
         * @param next_task then stack
         * @param priv_data priv_data passed to resume or start the stack
         * @return next_task if success , or self if failed
         */
        inline ptr_t then(ptr_t next_task, void *priv_data = UTIL_CONFIG_NULLPTR) { return next(next_task, priv_data); }

        /**
         * @brief create next task with functor using the same allocator and private buffer size as this task
         * @see next
         * @param functor
         * @param priv_data priv_data passed to start functor
         * @return the created task if success , or self if failed
         */
#if defined(UTIL_CONFIG_COMPILER_CXX_RVALUE_REFERENCES) && UTIL_CONFIG_COMPILER_CXX_RVALUE_REFERENCES
        template <typename Ty>
        inline ptr_t then(Ty &&functor, void *priv_data = UTIL_CONFIG_NULLPTR) {
            if (!coroutine_obj_) {
                then(create(std::forward<Ty>(functor), stack_size_, get_private_buffer_size()), priv_data);
            }

            return then(create(std::forward<Ty>(functor), coroutine_obj_->get_allocator(), stack_size_, get_private_buffer_size()),
                        priv_data);
        }
#else
        template <typename Ty>
        inline ptr_t then(const Ty &functor, typename coroutine_t::allocator_type &alloc, void *priv_data = UTIL_CONFIG_NULLPTR,
                          size_t stack_size = 0, size_t private_buffer_size = 0) {
            if (!coroutine_obj_) {
                return then(create(std::forward<Ty>(functor), stack_size_, get_private_buffer_size()), priv_data);
            }

            return then(create(std::forward<Ty>(functor), coroutine_obj_->get_allocator(), stack_size_, get_private_buffer_size()),
                        priv_data);
        }
#endif

        template <typename Ty>
        inline ptr_t then(Ty (*func)(void *), void *priv_data = UTIL_CONFIG_NULLPTR) {
            if (!coroutine_obj_) {
                return then(create(func, stack_size_, get_private_buffer_size()), priv_data);
            }

            return then(create(func, coroutine_obj_->get_allocator(), stack_size_, get_private_buffer_size()), priv_data);
        }


        /**
         * get current running task and convert to task object
         * @return task smart pointer
         */
        static self_t *this_task() { return dynamic_cast<self_t *>(impl::task_impl::this_task()); }

    public:
        virtual ~task() {
            EN_TASK_STATUS status = get_status();
            // inited but not finished will trigger timeout or finish other actor
            if (status < EN_TS_DONE && status > EN_TS_CREATED) {
                kill(EN_TS_TIMEOUT);
            }

            // free resource
            id_allocator_t id_alloc_;
            ((void)id_alloc_);
            id_alloc_.deallocate(id_);
        }

        inline typename coroutine_t::ptr_t &      get_coroutine_context() LIBCOPP_MACRO_NOEXCEPT { return coroutine_obj_; }
        inline const typename coroutine_t::ptr_t &get_coroutine_context() const LIBCOPP_MACRO_NOEXCEPT { return coroutine_obj_; }

        inline id_t get_id() const LIBCOPP_MACRO_NOEXCEPT { return id_; }

    public:
        virtual int get_ret_code() const UTIL_CONFIG_OVERRIDE {
            if (!coroutine_obj_) {
                return 0;
            }

            return coroutine_obj_->get_ret_code();
        }

#if defined(LIBCOPP_MACRO_ENABLE_STD_EXCEPTION_PTR) && LIBCOPP_MACRO_ENABLE_STD_EXCEPTION_PTR
        virtual int start(void *priv_data, EN_TASK_STATUS expected_status = EN_TS_CREATED) UTIL_CONFIG_OVERRIDE {
            std::list<std::exception_ptr> eptrs;
            int ret = start(eptrs, priv_data, expected_status);
            maybe_rethrow(eptrs);
            return ret;
        }

        virtual int start(std::list<std::exception_ptr>& unhandled, void *priv_data, EN_TASK_STATUS expected_status = EN_TS_CREATED) LIBCOPP_MACRO_NOEXCEPT {
#else
        virtual int start(void *priv_data, EN_TASK_STATUS expected_status = EN_TS_CREATED) UTIL_CONFIG_OVERRIDE {
#endif
            if (!coroutine_obj_) {
                return copp::COPP_EC_NOT_INITED;
            }

            EN_TASK_STATUS from_status = expected_status;

            do {
                if (unlikely(from_status >= EN_TS_DONE)) {
                    return copp::COPP_EC_ALREADY_FINISHED;
                }

                if (unlikely(from_status == EN_TS_RUNNING)) {
                    return copp::COPP_EC_IS_RUNNING;
                }

                if (likely(_cas_status(from_status, EN_TS_RUNNING))) { // Atomic.CAS here
                    break;
                }
            } while (true);

            // use this smart ptr to avoid destroy of this
            // ptr_t protect_from_destroy(this);

#if defined(LIBCOPP_MACRO_ENABLE_STD_EXCEPTION_PTR) && LIBCOPP_MACRO_ENABLE_STD_EXCEPTION_PTR
            std::exception_ptr eptr;
            int ret = coroutine_obj_->start(eptr, priv_data);
            if (eptr) {
                unhandled.emplace_back(std::move(eptr));
            }
#else
            int ret = coroutine_obj_->start(priv_data);
#endif

            from_status = EN_TS_RUNNING;
            if (is_completed()) { // Atomic.CAS here
                while (from_status < EN_TS_DONE) {
                    if (likely(_cas_status(from_status, EN_TS_DONE))) { // Atomic.CAS here
                        break;
                    }
                }

                finish_priv_data_ = priv_data;
#if defined(LIBCOPP_MACRO_ENABLE_STD_EXCEPTION_PTR) && LIBCOPP_MACRO_ENABLE_STD_EXCEPTION_PTR
                _notify_finished(unhandled, priv_data);
#else
                _notify_finished(priv_data);
#endif
                return ret;
            }

            while (true) {
                if (from_status >= EN_TS_DONE) { // canceled or killed
#if defined(LIBCOPP_MACRO_ENABLE_STD_EXCEPTION_PTR) && LIBCOPP_MACRO_ENABLE_STD_EXCEPTION_PTR
                    _notify_finished(unhandled, finish_priv_data_);
#else
                    _notify_finished(finish_priv_data_);
#endif
                    break;
                }

                if (likely(_cas_status(from_status, EN_TS_WAITING))) { // Atomic.CAS here
                    break;
                    // waiting
                }
            }

            return ret;
        }

#if defined(LIBCOPP_MACRO_ENABLE_STD_EXCEPTION_PTR) && LIBCOPP_MACRO_ENABLE_STD_EXCEPTION_PTR
        virtual int resume(void *priv_data, EN_TASK_STATUS expected_status = EN_TS_WAITING) UTIL_CONFIG_OVERRIDE {
            return start(priv_data, expected_status);
        }

        virtual int resume(std::list<std::exception_ptr>& unhandled, void *priv_data, EN_TASK_STATUS expected_status = EN_TS_WAITING) LIBCOPP_MACRO_NOEXCEPT {
            return start(unhandled, priv_data, expected_status);
        }
#else
        virtual int resume(void *priv_data, EN_TASK_STATUS expected_status = EN_TS_WAITING) UTIL_CONFIG_OVERRIDE {
            return start(priv_data, expected_status);
        }
#endif

        virtual int yield(void **priv_data) UTIL_CONFIG_OVERRIDE {
            if (!coroutine_obj_) {
                return copp::COPP_EC_NOT_INITED;
            }

            return coroutine_obj_->yield(priv_data);
        }


#if defined(LIBCOPP_MACRO_ENABLE_STD_EXCEPTION_PTR) && LIBCOPP_MACRO_ENABLE_STD_EXCEPTION_PTR
        virtual int cancel(void *priv_data) UTIL_CONFIG_OVERRIDE {
            std::list<std::exception_ptr> eptrs;
            int ret = cancel(eptrs, priv_data);
            maybe_rethrow(eptrs);
            return ret;
        }

        virtual int cancel(std::list<std::exception_ptr>& unhandled, void *priv_data) LIBCOPP_MACRO_NOEXCEPT {
#else
        virtual int cancel(void *priv_data) UTIL_CONFIG_OVERRIDE {
#endif
        
            EN_TASK_STATUS from_status = get_status();

            do {
                if (EN_TS_RUNNING == from_status) {
                    return copp::COPP_EC_IS_RUNNING;
                }

                if (likely(_cas_status(from_status, EN_TS_CANCELED))) {
                    break;
                }
            } while (true);

#if defined(LIBCOPP_MACRO_ENABLE_STD_EXCEPTION_PTR) && LIBCOPP_MACRO_ENABLE_STD_EXCEPTION_PTR
            _notify_finished(unhandled, priv_data);
#else
            _notify_finished(priv_data);
#endif
            return copp::COPP_EC_SUCCESS;
        }

#if defined(LIBCOPP_MACRO_ENABLE_STD_EXCEPTION_PTR) && LIBCOPP_MACRO_ENABLE_STD_EXCEPTION_PTR
        virtual int kill(enum EN_TASK_STATUS status, void *priv_data) UTIL_CONFIG_OVERRIDE {
            std::list<std::exception_ptr> eptrs;
            int ret = kill(eptrs, status, priv_data);
            maybe_rethrow(eptrs);
            return ret;
        }

        virtual int kill(std::list<std::exception_ptr>& unhandled, enum EN_TASK_STATUS status, void *priv_data) LIBCOPP_MACRO_NOEXCEPT {
#else
        virtual int kill(enum EN_TASK_STATUS status, void *priv_data) UTIL_CONFIG_OVERRIDE {
#endif
            EN_TASK_STATUS from_status = get_status();

            do {
                if (likely(_cas_status(from_status, status))) {
                    break;
                }
            } while (true);

            if (EN_TS_RUNNING != from_status) {
#if defined(LIBCOPP_MACRO_ENABLE_STD_EXCEPTION_PTR) && LIBCOPP_MACRO_ENABLE_STD_EXCEPTION_PTR
                _notify_finished(unhandled, priv_data);
#else
                _notify_finished(priv_data);
#endif
            } else {
                finish_priv_data_ = priv_data;
            }

            return copp::COPP_EC_SUCCESS;
        }

        using impl::task_impl::cancel;
        using impl::task_impl::kill;
        using impl::task_impl::resume;
        using impl::task_impl::start;
        using impl::task_impl::yield;

    public:
        virtual bool is_completed() const LIBCOPP_MACRO_NOEXCEPT UTIL_CONFIG_OVERRIDE {
            if (!coroutine_obj_) {
                return false;
            }

            return coroutine_obj_->is_finished();
        }

#if defined(LIBCOPP_MACRO_ENABLE_WIN_FIBER) && LIBCOPP_MACRO_ENABLE_WIN_FIBER
        virtual bool is_fiber() const LIBCOPP_MACRO_NOEXCEPT {
            return std::is_base_of<copp::coroutine_context_fiber, coroutine_t>::value;
        }
#endif

        static inline void *add_buffer_offset(void *in, size_t off) {
            return reinterpret_cast<void *>(reinterpret_cast<unsigned char *>(in) + off);
        }

        static inline void *sub_buffer_offset(void *in, size_t off) {
            return reinterpret_cast<void *>(reinterpret_cast<unsigned char *>(in) - off);
        }

        void *get_private_buffer() {
            if (!coroutine_obj_) {
                return UTIL_CONFIG_NULLPTR;
            }

            return add_buffer_offset(coroutine_obj_->get_private_buffer(), sizeof(impl::task_impl *));
        }

        size_t get_private_buffer_size() {
            if (!coroutine_obj_) {
                return 0;
            }

            return coroutine_obj_->get_private_buffer_size() - sizeof(impl::task_impl *);
        }


        inline size_t use_count() const { return ref_count_.load(); }

#if defined(LIBCOPP_MACRO_ENABLE_STD_EXCEPTION_PTR) && LIBCOPP_MACRO_ENABLE_STD_EXCEPTION_PTR
        static UTIL_FORCEINLINE void maybe_rethrow(std::list<std::exception_ptr>& eptrs) { 
            for (std::list<std::exception_ptr>::iterator iter = eptrs.begin(); iter != eptrs.end(); ++ iter) {
                coroutine_t::maybe_rethrow(*iter);
            }
        }
#endif
    private:
        task(const task &) UTIL_CONFIG_DELETED_FUNCTION;

#if defined(LIBCOPP_MACRO_ENABLE_STD_EXCEPTION_PTR) && LIBCOPP_MACRO_ENABLE_STD_EXCEPTION_PTR
        void active_next_tasks(std::list<std::exception_ptr>& unhandled) LIBCOPP_MACRO_NOEXCEPT {
#else
        void active_next_tasks() {
#endif
            std::list<std::pair<ptr_t, void *> > next_list;
#if defined(LIBCOTASK_MACRO_AUTO_CLEANUP_MANAGER) && LIBCOTASK_MACRO_AUTO_CLEANUP_MANAGER
            void *manager_ptr;
            void (*manager_fn)(void *, self_t &);
#endif
            // first, lock and swap container
            {
#if !defined(LIBCOPP_DISABLE_ATOMIC_LOCK) || !(LIBCOPP_DISABLE_ATOMIC_LOCK)
                libcopp::util::lock::lock_holder<libcopp::util::lock::spin_lock> lock_guard(inner_action_lock_);
#endif
                next_list.swap(next_list_.member_list_);
#if defined(LIBCOTASK_MACRO_AUTO_CLEANUP_MANAGER) && LIBCOTASK_MACRO_AUTO_CLEANUP_MANAGER
                manager_ptr          = binding_manager_ptr_;
                manager_fn           = binding_manager_fn_;
                binding_manager_ptr_ = UTIL_CONFIG_NULLPTR;
                binding_manager_fn_  = UTIL_CONFIG_NULLPTR;
#endif
            }

            // then, do all the pending tasks
            for (typename std::list<std::pair<ptr_t, void *> >::iterator iter = next_list.begin(); iter != next_list.end(); ++iter) {
                if (!iter->first || EN_TS_INVALID == iter->first->get_status()) {
                    continue;
                }

#if defined(LIBCOPP_MACRO_ENABLE_STD_EXCEPTION_PTR) && LIBCOPP_MACRO_ENABLE_STD_EXCEPTION_PTR
                if (iter->first->get_status() < EN_TS_RUNNING) {
                    iter->first->start(unhandled, iter->second);
                } else {
                    iter->first->resume(unhandled, iter->second);
                }
#else
                if (iter->first->get_status() < EN_TS_RUNNING) {
                    iter->first->start(iter->second);
                } else {
                    iter->first->resume(iter->second);
                }
#endif
            }

#if defined(LIBCOPP_MACRO_ENABLE_STD_COROUTINE) && LIBCOPP_MACRO_ENABLE_STD_COROUTINE
            // and then, resume all std coroutine handles
            while (!next_std_handles_.empty()) {
                if(!*next_std_handles_.begin()) {
                    next_std_handles_.pop_front();
                    continue;
                }

                if((*next_std_handles_.begin()).done()) {
                    next_std_handles_.pop_front();
                    continue;
                }

                (*next_std_handles_.begin()).resume();
            }
#endif

            // finally, notify manager to cleanup(maybe start or resume with task's API but not task_manager's)
#if defined(LIBCOTASK_MACRO_AUTO_CLEANUP_MANAGER) && LIBCOTASK_MACRO_AUTO_CLEANUP_MANAGER
            if (UTIL_CONFIG_NULLPTR != manager_ptr && UTIL_CONFIG_NULLPTR != manager_fn) {
                (*manager_fn)(manager_ptr, *this);
            }
#endif
        }

#if defined(LIBCOPP_MACRO_ENABLE_STD_EXCEPTION_PTR) && LIBCOPP_MACRO_ENABLE_STD_EXCEPTION_PTR
        int _notify_finished(std::list<std::exception_ptr>& unhandled, void *priv_data) LIBCOPP_MACRO_NOEXCEPT {
#else
        int _notify_finished(void *priv_data) {
#endif
            // first, make sure coroutine finished.
            if (coroutine_obj_ && false == coroutine_obj_->is_finished()) {
                // make sure this task will not be destroyed when running
                while (false == coroutine_obj_->is_finished()) {
#if defined(LIBCOPP_MACRO_ENABLE_STD_EXCEPTION_PTR) && LIBCOPP_MACRO_ENABLE_STD_EXCEPTION_PTR
                    std::exception_ptr eptr;
                    coroutine_obj_->resume(eptr, priv_data);
                    if (eptr) {
                        unhandled.emplace_back(std::move(eptr));
                    }
#else
                    coroutine_obj_->resume(priv_data);
#endif
                }
            }

#if defined(LIBCOPP_MACRO_ENABLE_STD_EXCEPTION_PTR) && LIBCOPP_MACRO_ENABLE_STD_EXCEPTION_PTR
            int ret = impl::task_impl::_notify_finished(unhandled, priv_data);
            // next tasks
            active_next_tasks(unhandled);
#else
            int ret = impl::task_impl::_notify_finished(priv_data);
            // next tasks
            active_next_tasks();
#endif
            return ret;
        }


        friend void intrusive_ptr_add_ref(self_t *p) {
            if (p == UTIL_CONFIG_NULLPTR) {
                return;
            }

            ++p->ref_count_;
        }

        friend void intrusive_ptr_release(self_t *p) {
            if (p == UTIL_CONFIG_NULLPTR) {
                return;
            }

            size_t left = --p->ref_count_;
            if (0 == left) {
                // save coroutine context first, make sure it's still available after destroy task
                typename coroutine_t::ptr_t coro = p->coroutine_obj_;

                // then, find and destroy action
                void *action_ptr = reinterpret_cast<void *>(p->_get_action());
                if (UTIL_CONFIG_NULLPTR != p->action_destroy_fn_ && UTIL_CONFIG_NULLPTR != action_ptr) {
                    (*p->action_destroy_fn_)(action_ptr);
                }

                // then, destruct task
                p->~task();

                // at last, destroy the coroutine and maybe recycle the stack space
                coro.reset();
            }
        }

#if defined(LIBCOTASK_MACRO_AUTO_CLEANUP_MANAGER) && LIBCOTASK_MACRO_AUTO_CLEANUP_MANAGER
    public:
        class LIBCOPP_COTASK_API_HEAD_ONLY task_manager_helper {
        private:
            template <typename, typename>
            friend class LIBCOPP_COTASK_API_HEAD_ONLY task_manager;
            static bool setup_task_manager(self_t &task_inst, void *manager_ptr, void (*fn)(void *, self_t &)) {
#if !defined(LIBCOPP_DISABLE_ATOMIC_LOCK) || !(LIBCOPP_DISABLE_ATOMIC_LOCK)
                libcopp::util::lock::lock_holder<libcopp::util::lock::spin_lock> lock_guard(task_inst.inner_action_lock_);
#endif
                if (task_inst.binding_manager_ptr_ != UTIL_CONFIG_NULLPTR) {
                    return false;
                }

                task_inst.binding_manager_ptr_ = manager_ptr;
                task_inst.binding_manager_fn_  = fn;
                return true;
            }

            static bool cleanup_task_manager(self_t &task_inst, void *manager_ptr) {
#if !defined(LIBCOPP_DISABLE_ATOMIC_LOCK) || !(LIBCOPP_DISABLE_ATOMIC_LOCK)
                libcopp::util::lock::lock_holder<libcopp::util::lock::spin_lock> lock_guard(task_inst.inner_action_lock_);
#endif
                if (task_inst.binding_manager_ptr_ != manager_ptr) {
                    return false;
                }

                task_inst.binding_manager_ptr_ = UTIL_CONFIG_NULLPTR;
                task_inst.binding_manager_fn_  = UTIL_CONFIG_NULLPTR;
                return true;
            }
        };
#endif

#if defined(LIBCOPP_MACRO_ENABLE_STD_COROUTINE) && LIBCOPP_MACRO_ENABLE_STD_COROUTINE
    public:
        class awaitable_base_t {
        private:
            awaitable_base_t(const awaitable_base_t &) UTIL_CONFIG_DELETED_FUNCTION;
            awaitable_base_t &operator=(const awaitable_base_t &) UTIL_CONFIG_DELETED_FUNCTION;

        public:
            awaitable_base_t(ptr_t t) : refer_task_(t), await_handle_iter_(t->next_std_handles_.end()) {}

            awaitable_base_t(awaitable_base_t &&other) : refer_task_(other.refer_task_), await_handle_iter_(other.await_handle_iter_) {
                if (other.refer_task_) {
                    other.await_handle_iter_ = other.refer_task_->next_std_handles_.end();
                    other.refer_task_.reset();
                }
            }
            awaitable_base_t &operator=(awaitable_base_t &&other) {
                refer_task_        = other.refer_task_;
                await_handle_iter_ = other.await_handle_iter_;

                if (other.refer_task_) {
                    other.await_handle_iter_ = other.refer_task_->next_std_handles_.end();
                    other.refer_task_.reset();
                }
            }

            inline bool await_ready() const LIBCOPP_MACRO_NOEXCEPT { return !refer_task_ || refer_task_->is_completed(); }

            inline void await_suspend(LIBCOPP_MACRO_FUTURE_COROUTINE_VOID h) LIBCOPP_MACRO_NOEXCEPT {
                if (likely(refer_task_)) {
                    await_handle_iter_ = refer_task_->next_std_handles_.insert(refer_task_->next_std_handles_.end(), h);
                }
            }

            inline int await_resume() LIBCOPP_MACRO_NOEXCEPT {
                if (likely(refer_task_ && await_handle_iter_ != refer_task_->next_std_handles_.end())) {
                    refer_task_->next_std_handles_.erase(await_handle_iter_);
                    await_handle_iter_ = refer_task_->next_std_handles_.end();
                }

                if (likely(refer_task_)) {
                    return refer_task_->get_ret_code();
                }

                return copp::COPP_EC_NOT_FOUND;
            }

        protected:
            ptr_t refer_task_;
            typename std::list<LIBCOPP_MACRO_FUTURE_COROUTINE_VOID>::iterator await_handle_iter_;
        };

        auto operator co_await() & LIBCOPP_MACRO_NOEXCEPT { return awaitable_base_t{ptr_t(this)}; }
#endif
    private:
        id_t                        id_;
        size_t                      stack_size_;
        typename coroutine_t::ptr_t coroutine_obj_;
        task_group                  next_list_;

        // ============== action information ==============
        void (*action_destroy_fn_)(void *);

#if !defined(LIBCOPP_DISABLE_ATOMIC_LOCK) || !(LIBCOPP_DISABLE_ATOMIC_LOCK)
        libcopp::util::lock::atomic_int_type<size_t> ref_count_; /** ref_count **/
        libcopp::util::lock::spin_lock               inner_action_lock_;
#else
        libcopp::util::lock::atomic_int_type<libcopp::util::lock::unsafe_int_type<size_t> > ref_count_; /** ref_count **/
#endif

        // ============== binding to task manager ==============
#if defined(LIBCOTASK_MACRO_AUTO_CLEANUP_MANAGER) && LIBCOTASK_MACRO_AUTO_CLEANUP_MANAGER
        void *binding_manager_ptr_;
        void (*binding_manager_fn_)(void *, self_t &);
#endif

#if defined(LIBCOPP_MACRO_ENABLE_STD_COROUTINE) && LIBCOPP_MACRO_ENABLE_STD_COROUTINE
        std::list<LIBCOPP_MACRO_FUTURE_COROUTINE_VOID> next_std_handles_;
#endif
    };

#if defined(LIBCOPP_MACRO_ENABLE_STD_COROUTINE) && LIBCOPP_MACRO_ENABLE_STD_COROUTINE
    template <typename TCO_MACRO>
    auto operator co_await(libcopp::util::intrusive_ptr<task<TCO_MACRO> > t) LIBCOPP_MACRO_NOEXCEPT {
        typedef typename task<TCO_MACRO>::awaitable_base_t awaitable_t;
        return awaitable_t{t}; 
    }
#endif
} // namespace cotask


#endif /* _COTASK_TASK_H_ */

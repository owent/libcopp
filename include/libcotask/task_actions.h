/*
 * task_actions.h
 *
 *  Created on: 2014年4月1日
 *      Author: owent
 *
 *  Released under the MIT license
 */

#ifndef COTASK_TASK_ACTIONS_H
#define COTASK_TASK_ACTIONS_H

#pragma once

#include <libcotask/impl/task_action_impl.h>

namespace cotask {

    namespace detail {

        struct task_action_functor_check {
            // ================================================
            template <typename TR, typename TF>
            static LIBCOPP_COTASK_API_HEAD_ONLY int call(TR (TF::*)(void *), TF &fn, void *priv_data) {
                fn(priv_data);
                return 0;
            }

            template <typename TR, typename TF>
            static LIBCOPP_COTASK_API_HEAD_ONLY int call(TR (TF::*)(void *) const, const TF &fn, void *priv_data) {
                fn(priv_data);
                return 0;
            }

            // ------------------------------------------------
            template <typename TF>
            static LIBCOPP_COTASK_API_HEAD_ONLY int call(int (TF::*)(void *), TF &fn, void *priv_data) {
                return fn(priv_data);
            }

            template <typename TF>
            static LIBCOPP_COTASK_API_HEAD_ONLY int call(int (TF::*)(void *) const, const TF &fn, void *priv_data) {
                return fn(priv_data);
            }

            // ------------------------------------------------
            template <typename TR, typename TF>
            static LIBCOPP_COTASK_API_HEAD_ONLY int call(TR (TF::*)(), TF &fn, void * /*priv_data*/) {
                fn();
                return 0;
            }

            template <typename TR, typename TF>
            static LIBCOPP_COTASK_API_HEAD_ONLY int call(TR (TF::*)() const, const TF &fn, void * /*priv_data*/) {
                fn();
                return 0;
            }

            // ------------------------------------------------
            template <typename TF>
            static LIBCOPP_COTASK_API_HEAD_ONLY int call(int (TF::*)(), TF &fn, void * /*priv_data*/) {
                return fn();
            }

            template <typename TF>
            static LIBCOPP_COTASK_API_HEAD_ONLY int call(int (TF::*)() const, const TF &fn, void * /*priv_data*/) {
                return fn();
            }
        };
    } // namespace detail

    // functor
    template <typename Ty>
    class LIBCOPP_COTASK_API_HEAD_ONLY task_action_functor : public impl::task_action_impl {
#if defined(UTIL_CONFIG_COMPILER_CXX_RVALUE_REFERENCES) && UTIL_CONFIG_COMPILER_CXX_RVALUE_REFERENCES
    public:
        typedef typename std::remove_cv<Ty>::type value_type;

        task_action_functor(value_type &&functor) : functor_(functor) {}

        task_action_functor(task_action_functor &&other) : functor_(std::move(other.functor_)) {}
        inline task_action_functor &operator=(task_action_functor &&other) { functor_ = std::move(other.functor_); }

#else
    public:
        typedef Ty value_type;
        task_action_functor(const value_type &functor) : functor_(functor) {}
#endif
        ~task_action_functor() {}
        virtual int operator()(void *priv_data) {
            return detail::task_action_functor_check::call(&value_type::operator(), functor_, priv_data);
        }

        static void placement_destroy(void *selfp) {
            if (UTIL_CONFIG_NULLPTR == selfp) {
                return;
            }

            task_action_functor<Ty> *self = reinterpret_cast<task_action_functor<Ty> *>(selfp);
            self->~task_action_functor();
        }

    private:
        value_type functor_;
    };

    // function
    template <typename Ty>
    class LIBCOPP_COTASK_API_HEAD_ONLY task_action_function : public impl::task_action_impl {
    public:
        typedef Ty (*value_type)(void *);

    public:
        task_action_function(value_type func) : func_(func) {}
        ~task_action_function() {}

        virtual int operator()(void *priv_data) {
            (*func_)(priv_data);
            return 0;
        }

        static void placement_destroy(void *selfp) {
            if (UTIL_CONFIG_NULLPTR == selfp) {
                return;
            }

            task_action_function<Ty> *self = reinterpret_cast<task_action_function<Ty> *>(selfp);
            self->~task_action_function();
        }

    private:
        value_type func_;
    };

    template <>
    class LIBCOPP_COTASK_API_HEAD_ONLY task_action_function<int> : public impl::task_action_impl {
    public:
        typedef int (*value_type)(void *);

    public:
        task_action_function(value_type func) : func_(func) {}
        ~task_action_function() {}

        virtual int operator()(void *priv_data) { return (*func_)(priv_data); }

        static void placement_destroy(void *selfp) {
            if (UTIL_CONFIG_NULLPTR == selfp) {
                return;
            }

            task_action_function<int> *self = reinterpret_cast<task_action_function<int> *>(selfp);
            self->~task_action_function();
        }

    private:
        value_type func_;
    };

    // mem function
    template <typename Ty, typename Tc>
    class LIBCOPP_COTASK_API_HEAD_ONLY task_action_mem_function : public impl::task_action_impl {
    public:
        typedef Ty Tc::*value_type;

    public:
        task_action_mem_function(value_type func, Tc *inst) : instacne_(inst), func_(func) {}
        ~task_action_mem_function() {}

        virtual int operator()(void *priv_data) {
            (instacne_->*func_)(priv_data);
            return 0;
        }

        static void placement_destroy(void *selfp) {
            if (UTIL_CONFIG_NULLPTR == selfp) {
                return;
            }

            task_action_mem_function<Ty, Tc> *self = reinterpret_cast<task_action_mem_function<Ty, Tc> *>(selfp);
            self->~task_action_mem_function();
        }

    private:
        Tc *       instacne_;
        value_type func_;
    };

    template <typename Tc>
    class LIBCOPP_COTASK_API_HEAD_ONLY task_action_mem_function<int, Tc> : public impl::task_action_impl {
    public:
        typedef int Tc::*value_type;

    public:
        task_action_mem_function(value_type func, Tc *inst) : instacne_(inst), func_(func) {}
        ~task_action_mem_function() {}

        virtual int operator()(void *priv_data) { return (instacne_->*func_)(priv_data); }

        static void placement_destroy(void *selfp) {
            if (UTIL_CONFIG_NULLPTR == selfp) {
                return;
            }

            task_action_mem_function<int, Tc> *self = reinterpret_cast<task_action_mem_function<int, Tc> *>(selfp);
            self->~task_action_mem_function();
        }

    private:
        Tc *       instacne_;
        value_type func_;
    };

    template <typename Ty>
    LIBCOPP_COTASK_API_HEAD_ONLY void placement_destroy(void *selfp) {
        if (UTIL_CONFIG_NULLPTR == selfp) {
            return;
        }

        Ty *self = reinterpret_cast<Ty *>(selfp);
        self->~Ty();
    }

    typedef void (*placement_destroy_fn_t)(void *);

    template <typename Ty>
    LIBCOPP_COTASK_API_HEAD_ONLY placement_destroy_fn_t get_placement_destroy(task_action_functor<Ty> * /*selfp*/) {
        return &task_action_functor<Ty>::placement_destroy;
    }

    template <typename Ty>
    LIBCOPP_COTASK_API_HEAD_ONLY placement_destroy_fn_t get_placement_destroy(task_action_function<Ty> * /*selfp*/) {
        return &task_action_function<Ty>::placement_destroy;
    }

    template <typename Ty, typename Tc>
    LIBCOPP_COTASK_API_HEAD_ONLY placement_destroy_fn_t get_placement_destroy(task_action_mem_function<Ty, Tc> * /*selfp*/) {
        return &task_action_mem_function<Ty, Tc>::placement_destroy;
    }

    template <typename Ty>
    LIBCOPP_COTASK_API_HEAD_ONLY placement_destroy_fn_t get_placement_destroy(Ty * /*selfp*/) {
        return &placement_destroy<Ty>;
    }
} // namespace cotask

#endif /* _COTASK_TASK_ACTIONS_H_ */

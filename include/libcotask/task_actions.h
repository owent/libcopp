/*
 * task_actions.h
 *
 *  Created on: 2014年4月1日
 *      Author: owent
 *
 *  Released under the MIT license
 */

#ifndef _COTASK_TASK_ACTIONS_H_
#define _COTASK_TASK_ACTIONS_H_

#include <libcotask/impl/task_action_impl.h>

namespace cotask {

    namespace detail {

        struct task_action_functor_check {
            // ================================================
            template <typename TR, typename TF>
            static int call(TR (TF::*)(void*), TF &fn, void* priv_data) {
                fn(priv_data);
                return 0;
            }

            template <typename TR, typename TF>
            static int call(TR (TF::*)(void*) const, const TF &fn, void* priv_data) {
                fn(priv_data);
                return 0;
            }

            // ------------------------------------------------
            template <typename TF>
            static int call(int (TF::*)(void*), TF &fn, void* priv_data) {
                return fn(priv_data);
            }

            template <typename TF>
            static int call(int (TF::*)(void*) const, const TF &fn, void* priv_data) {
                return fn(priv_data);
            }

            // ------------------------------------------------
            template <typename TR, typename TF>
            static int call(TR(TF::*)(), TF &fn, void* priv_data) {
                fn();
                return 0;
            }

            template <typename TR, typename TF>
            static int call(TR(TF::*)() const, const TF &fn, void* priv_data) {
                fn();
                return 0;
            }

            // ------------------------------------------------
            template <typename TF>
            static int call(int (TF::*)(), TF &fn, void* priv_data) {
                return fn();
            }

            template <typename TF>
            static int call(int (TF::*)() const, const TF &fn, void* priv_data) {
                return fn();
            }
        };
    }

    // functor
    template <typename Ty>
    class task_action_functor : public impl::task_action_impl {
#if defined(UTIL_CONFIG_COMPILER_CXX_RVALUE_REFERENCES) && UTIL_CONFIG_COMPILER_CXX_RVALUE_REFERENCES
    public:
        typedef typename std::remove_cv<Ty>::type value_type;

        task_action_functor(value_type &&functor) : functor_(functor) {}

        task_action_functor(task_action_functor &&other) : functor_(other.functor_) {}
        inline task_action_functor &operator=(task_action_functor &&other) { functor_ = other.functor_; }

#else
    public:
        typedef Ty value_type;
        task_action_functor(const value_type &functor) : functor_(functor) {}
#endif

        virtual int operator()(void* priv_data) { return detail::task_action_functor_check::call(&value_type::operator(), functor_, priv_data); }

    private:
        value_type functor_;
    };

    // function
    template <typename Ty>
    class task_action_function : public impl::task_action_impl {
    public:
        typedef Ty (*value_type)(void*);

    public:
        task_action_function(value_type func) : func_(func) {}

        virtual int operator()(void* priv_data) {
            (*func_)(priv_data);
            return 0;
        }

    private:
        value_type func_;
    };

    template <>
    class task_action_function<int> : public impl::task_action_impl {
    public:
        typedef int (*value_type)(void*);

    public:
        task_action_function(value_type func) : func_(func) {}

        virtual int operator()(void* priv_data) { return (*func_)(priv_data); }

    private:
        value_type func_;
    };

    // mem function
    template <typename Ty, typename Tc>
    class task_action_mem_function : public impl::task_action_impl {
    public:
        typedef Ty Tc::*value_type;

    public:
        task_action_mem_function(value_type func, Tc *inst) : instacne_(inst), func_(func) {}

        virtual int operator()(void* priv_data) {
            (instacne_->*func_)(priv_data);
            return 0;
        }

    private:
        Tc *instacne_;
        value_type func_;
    };

    template <typename Tc>
    class task_action_mem_function<int, Tc> : public impl::task_action_impl {
    public:
        typedef int Tc::*value_type;

    public:
        task_action_mem_function(value_type func, Tc *inst) : instacne_(inst), func_(func) {}

        virtual int operator()(void* priv_data) { return (instacne_->*func_)(priv_data); }

    private:
        Tc *instacne_;
        value_type func_;
    };
}

#endif /* _COTASK_TASK_ACTIONS_H_ */

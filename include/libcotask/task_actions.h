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
            template<typename TR, typename TF>
            static int call(TR (TF::*)(), TF& fn) {
                fn();
                return 0;
            }

            template<typename TR, typename TF>
            static int call(TR (TF::*)() const, const TF& fn) {
                fn();
                return 0;
            }

            // ------------------------------------------------
            template<typename TF>
            static int call(int (TF::*)(), TF& fn) {
                return fn();
            }

            template<typename TF>
            static int call(int (TF::*)() const, const TF& fn) {
                return fn();
            }
        };
    }
    // functor
    template<typename Ty>
    class task_action_functor: public impl::task_action_impl
    {
    public:
        typedef Ty value_type;

    public:
        task_action_functor(value_type functor): functor_(functor){}

        virtual int operator()() {
            return detail::task_action_functor_check::call(&value_type::operator(), functor_);
        }

    private:
        value_type functor_;
    };

    // function
    template<typename Ty>
    class task_action_function: public impl::task_action_impl
    {
    public:
        typedef Ty (*value_type)();

    public:
        task_action_function(value_type func): func_(func){}

        virtual int operator()() {
            (*func_)();
            return 0;
        }

    private:
        value_type func_;
    };

    template<>
    class task_action_function<int>: public impl::task_action_impl
    {
    public:
        typedef int (*value_type)();

    public:
        task_action_function(value_type func): func_(func){}

        virtual int operator()() {
            return (*func_)();
        }

    private:
        value_type func_;
    };

    // mem function
    template<typename Ty, typename Tc>
    class task_action_mem_function: public impl::task_action_impl
    {
    public:
        typedef Ty Tc::* value_type;

    public:
        task_action_mem_function(value_type func, Tc* inst): instacne_(inst), func_(func){}

        virtual int operator()() {
            (instacne_->*func_)();
            return 0;
        }

    private:
        Tc* instacne_;
        value_type func_;
    };

    template<typename Tc>
    class task_action_mem_function<int, Tc>: public impl::task_action_impl
    {
    public:
        typedef int Tc::* value_type;

    public:
        task_action_mem_function(value_type func, Tc* inst): instacne_(inst), func_(func){}

        virtual int operator()() {
            return (instacne_->*func_)();
        }

    private:
        Tc* instacne_;
        value_type func_;
    };
}

#endif /* _COTASK_TASK_ACTIONS_H_ */

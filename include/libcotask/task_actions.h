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

#include <libcopp/utils/features.h>
#include <libcotask/impl/task_action_impl.h>

namespace cotask {

    namespace detail {

#ifdef COPP_MACRO_TYPEOF
        template<typename TRet>
        class task_action_functor_fit {
        public:
            template<typename TFunc>
            int operator()(TFunc& fn) {
                fn();
                return 0;
            }

            template<typename TFunc>
            int operator()(const TFunc& fn) {
                fn();
                return 0;
            }
        };

        template<>
        class task_action_functor_fit<int> {
        public:
            template<typename TFunc>
            int operator()(TFunc& fn) {
                return fn();
            }

            template<typename TFunc>
            int operator()(const TFunc& fn) {
                return fn();
            }
        };

#else

        class task_action_functor_fit {
        public:
            template<typename TRet>
            int operator()(TRet ret) {
                return 0;
            }

            int operator()(int ret) {
                return ret;
            }
        };

#endif

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
#ifdef COPP_MACRO_TYPEOF
            return detail::task_action_functor_fit<COPP_MACRO_TYPEOF(functor_())>()(functor_);
#else
            return detail::task_action_functor_fit()(functor_());
#endif
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

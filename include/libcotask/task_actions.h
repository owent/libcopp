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
    template<typename Ty>
    class task_action_functor: public impl::task_action_impl
    {
    public:
        typedef Ty value_type;

    public:
        task_action_functor(value_type functor): functor_(functor){}

        virtual int operator()() {
            return functor_();
        }

    private:
        value_type functor_;
    };

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
}

#endif /* _COTASK_TASK_ACTIONS_H_ */

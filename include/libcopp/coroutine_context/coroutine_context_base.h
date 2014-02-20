#ifndef _COPP_COROUTINE_CONTEXT_COROUTINE_CONTEXT_BASE_H_
#define _COPP_COROUTINE_CONTEXT_COROUTINE_CONTEXT_BASE_H_


#if defined(_MSC_VER) && (_MSC_VER >= 1020)
# pragma once
#endif

#include <cstddef>

#include <libcopp/config/features.h>
#include <libcopp/fcontext/all.hpp>
#include <libcopp/stack_context/stack_context.h>
#include <libcopp/coroutine_context/coroutine_runnable_base.h>


namespace copp { 
    namespace detail{
        class coroutine_context_base
        {
        private:
            int runner_ret_code_;
            coroutine_runnable_base* runner_;

        protected:
            fcontext::fcontext_t caller_;
            fcontext::fcontext_t* callee_;

            bool preserve_fpu_;
            stack_context callee_stack_;
#ifdef COPP_MACRO_USE_SEGMENTED_STACKS
            stack_context caller_stack_;
#endif

        public:
            friend coroutine_runnable_base;

            coroutine_context_base();
            virtual ~coroutine_context_base();

            /**
             * create coroutine context at spefic address
             * @note stack_ptr is the start or end of stack memory address relation to platform
             * @param stack_ptr stack position address
             * @param stack_len stack len
             */
            virtual int create(coroutine_runnable_base* runner, char* stack_ptr, size_t stack_len, void(*func)(intptr_t) = &coroutine_context_base::coroutine_context_callback);

            virtual int create(coroutine_runnable_base* runner, void(*func)(intptr_t) = &coroutine_context_base::coroutine_context_callback);
            virtual int start();
            virtual int yield();
            virtual int resume();

        protected:
            virtual void run();

        public:
            int set_runner(coroutine_runnable_base* runner);
            inline int get_ret_code() const { return runner_ret_code_; }

        protected:
            static intptr_t jump_to(fcontext::fcontext_t& from_fcontext, const fcontext::fcontext_t& to_fcontext,
                stack_context& from_stack, stack_context& to_stack,
                intptr_t param, bool preserve_fpu);

            static void coroutine_context_callback(intptr_t coro_ptr);
        };
    }
}

#endif

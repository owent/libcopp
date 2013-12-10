#ifndef _COPP_COROUTINE_CONTEXT_COROUTINE_CONTEXT_BASE_H_
#define _COPP_COROUTINE_CONTEXT_COROUTINE_CONTEXT_BASE_H_


#if defined(_MSC_VER) && (_MSC_VER >= 1020)
# pragma once
#endif

#include <cstddef>

#include <libcopp/fcontext/all.hpp>
#include <libcopp/stack_context/stack_context.h>

namespace copp { 
    namespace detail{
        class coroutine_context_base
        {
        private:
            fcontext::fcontext_t caller_;
            fcontext::fcontext_t* callee_;

            stack_context env_stack_;
            bool preserve_fpu_;

        public:
            coroutine_context_base();
            virtual ~coroutine_context_base() = 0;

            /**
             * create coroutine context at spefic address
             * @note stack_ptr is the start or end of stack memory address relation to platform
             * @param stack_ptr stack position address
             * @param stack_len stack len
             */
            int create(char* stack_ptr, size_t stack_len, void(*func)(intptr_t) = &coroutine_context_base::coroutine_context_callback);

            int create(void(*func)(intptr_t) = &coroutine_context_base::coroutine_context_callback);
            int start();
            int yield();
            int resume();

        private:
            virtual void run() = 0;

        private:
            static void coroutine_context_callback(intptr_t coro_ptr);
        };
    }
}

#endif

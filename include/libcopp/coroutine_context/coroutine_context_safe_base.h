/**
 * thread safe coroutine base class
 */
#ifndef _COPP_COROUTINE_CONTEXT_COROUTINE_CONTEXT_SAFE_BASE_H_
#define _COPP_COROUTINE_CONTEXT_COROUTINE_CONTEXT_SAFE_BASE_H_


#if defined(_MSC_VER) && (_MSC_VER >= 1020)
# pragma once
#endif

#include <cstddef>

#include <libcopp/coroutine_context/coroutine_context_base.h>
#include <libcopp/utils/spin_lock.h>

namespace copp {
    namespace detail{
        enum copp_coroutine_running_status
        {
            EN_CRS_INVALID = 0,
            EN_CRS_START,
            EN_CRS_RUNNING,
            EN_CRS_FINISHED,
            EN_CRS_STOP,
        };

        class coroutine_context_safe_base : private coroutine_context_base
        {
        protected:
            using coroutine_context_base::caller_;
            using coroutine_context_base::callee_;

            using coroutine_context_base::preserve_fpu_;
            using coroutine_context_base::callee_stack_;
#ifdef COPP_MACRO_USE_SEGMENTED_STACKS
            using coroutine_context_base::stack_context caller_stack_;
#endif

            utils::spin_lock status_busy_;
            int status_running_;

        public:
            coroutine_context_safe_base();
            virtual ~coroutine_context_safe_base() = 0;

            /**
             * create coroutine context at spefic address
             * @note stack_ptr is the start or end of stack memory address relation to platform
             * @param stack_ptr stack position address
             * @param stack_len stack len
             */
            int create(char* stack_ptr, size_t stack_len, void(*func)(intptr_t) = &coroutine_context_safe_base::coroutine_context_callback);

            int create(void(*func)(intptr_t) = &coroutine_context_safe_base::coroutine_context_callback);
            int start();
            int yield();
            int resume();
            int stop();

        private:
            virtual void run() = 0;

            static void coroutine_context_callback(intptr_t coro_ptr);
        };
    }
}

#endif

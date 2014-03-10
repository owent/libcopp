#ifndef _COPP_COROUTINE_CONTEXT_COROUTINE_CONTEXT_BASE_H_
#define _COPP_COROUTINE_CONTEXT_COROUTINE_CONTEXT_BASE_H_


#if defined(_MSC_VER) && (_MSC_VER >= 1020)
# pragma once
#endif

#include <cstddef>

#include <libcopp/utils/features.h>
#include <libcopp/fcontext/all.hpp>
#include <libcopp/stack/stack_context.h>
#include <libcopp/coroutine/coroutine_runnable_base.h>

#ifdef COPP_MACRO_USE_SEGMENTED_STACKS
	#define COROUTINE_CONTEXT_BASE_USING_BASE_SEGMENTED_STACKS(base_type)\
		using base_type::caller_stack_;
#else
	#define COROUTINE_CONTEXT_BASE_USING_BASE_SEGMENTED_STACKS(base_type)
#endif

#define COROUTINE_CONTEXT_BASE_USING_BASE(base_type)	\
	protected:											\
	using base_type::caller_;							\
	using base_type::callee_;							\
	using base_type::preserve_fpu_;						\
	using base_type::callee_stack_;						\
	COROUTINE_CONTEXT_BASE_USING_BASE_SEGMENTED_STACKS(base_type)

namespace copp { 
    namespace detail{
        /**
         * base type of all coroutine context
         */
        class coroutine_context_base
        {
        private:
            int runner_ret_code_; /** coroutine return code **/
            coroutine_runnable_base* runner_; /** coroutine runner **/

        protected:
            fcontext::fcontext_t caller_; /** caller runtime context **/
            fcontext::fcontext_t* callee_; /** callee runtime context **/

            bool preserve_fpu_; /** is preserve fpu **/
            stack_context callee_stack_; /** callee stack context **/
#ifdef COPP_MACRO_USE_SEGMENTED_STACKS
            stack_context caller_stack_; /** caller stack context **/
#endif

        public:
            friend coroutine_runnable_base;

            coroutine_context_base();
            virtual ~coroutine_context_base();

            /**
             * create coroutine context at spefic address
             * @note stack_ptr is the start or end of stack memory address relation to platform
             * @param runner runner
             * @param stack_ptr stack position address
             * @param stack_len stack len
             * @param func fcontext callback
             * @return COPP_EC_SUCCESS or error code
             */
            virtual int create(coroutine_runnable_base* runner, char* stack_ptr, size_t stack_len, void(*func)(intptr_t) = &coroutine_context_base::coroutine_context_callback);

            /**
             * create coroutine context at stack context callee_
             * @param runner runner
             * @param func fcontext callback
             * @return COPP_EC_SUCCESS or error code
             */
            virtual int create(coroutine_runnable_base* runner, void(*func)(intptr_t) = &coroutine_context_base::coroutine_context_callback);

            /**
             * start coroutine
             * @return COPP_EC_SUCCESS or error code
             */
            virtual int start();

            /**
             * yield coroutine
             * @return COPP_EC_SUCCESS or error code
             */
            virtual int yield();

            /**
             * resume coroutine
             * @return COPP_EC_SUCCESS or error code
             */
            virtual int resume();

        protected:
            /**
             * coroutine entrance function
             */
            virtual void run();

        public:
            /**
             * set runner
             * @param runner
             * @return COPP_EC_SUCCESS or error code
             */
            int set_runner(coroutine_runnable_base* runner);

            /**
             * get runner return code
             * @return
             */
            inline int get_ret_code() const { return runner_ret_code_; }

        protected:
            /**
             * call platform jump to asm instruction
             * @param from_fcontext from runtime
             * @param to_fcontext to runtime
             * @param from_stack from runtime stack
             * @param to_stack to runtime stack
             * @param param coroutine intptr
             * @param preserve_fpu is preserve fpu
             * @return jump fcontext return code
             */
            static intptr_t jump_to(fcontext::fcontext_t& from_fcontext, const fcontext::fcontext_t& to_fcontext,
                stack_context& from_stack, stack_context& to_stack,
                intptr_t param, bool preserve_fpu);

            /**
             * fcontext entrance function
             * @param coro_ptr coroutine intptr
             */
            static void coroutine_context_callback(intptr_t coro_ptr);
        };
    }
}

#endif

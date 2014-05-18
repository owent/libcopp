#ifndef _COPP_COROUTINE_CONTEXT_COROUTINE_CONTEXT_BASE_H_
#define _COPP_COROUTINE_CONTEXT_COROUTINE_CONTEXT_BASE_H_


#if defined(_MSC_VER) && (_MSC_VER >= 1020)
# pragma once
#endif

#include <cstddef>

#include <libcopp/utils/features.h>
#include <libcopp/utils/non_copyable.h>
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
         * @brief base type of all coroutine context
         */
        class coroutine_context_base : utils::non_copyable
        {
        private:
            int runner_ret_code_; /** coroutine return code **/
            coroutine_runnable_base* runner_; /** coroutine runner **/

        protected:
            bool is_finished_; /** is coroutine finished **/

        protected:
            fcontext::fcontext_t caller_; /** caller runtime context **/
            fcontext::fcontext_t* callee_; /** callee runtime context **/

            bool preserve_fpu_; /** is preserve fpu **/
            stack_context callee_stack_; /** callee stack context **/
#ifdef COPP_MACRO_USE_SEGMENTED_STACKS
            stack_context caller_stack_; /** caller stack context **/
#endif

        public:
            coroutine_context_base();
            virtual ~coroutine_context_base();

        public:

            /**
             * @brief create coroutine context at stack context callee_
             * @param runner runner
             * @param func fcontext callback
             * @return COPP_EC_SUCCESS or error code
             */
            virtual int create(coroutine_runnable_base* runner, void(*func)(intptr_t) = &coroutine_context_base::coroutine_context_callback);

            /**
             * @brief start coroutine
             * @return COPP_EC_SUCCESS or error code
             */
            virtual int start();

            /**
             * @brief yield coroutine
             * @return COPP_EC_SUCCESS or error code
             */
            virtual int yield();

            /**
             * @brief resume coroutine
             * @return COPP_EC_SUCCESS or error code
             */
            virtual int resume();

        protected:
            /**
             * @brief coroutine entrance function
             */
            inline void run_and_recv_retcode() {
                if (NULL == runner_) return;

                runner_ret_code_ = (*runner_)();
            }

        protected:
            /**
             * @brief set runner
             * @param runner
             * @return COPP_EC_SUCCESS or error code
             */
            virtual int set_runner(coroutine_runnable_base* runner);

        public:

            /**
             * get runner of this coroutine context
             * @return NULL of pointer of runner
             */
            inline coroutine_runnable_base* get_runner() { return runner_; }

            /**
             * get runner of this coroutine context (const)
             * @return NULL of pointer of runner
             */
            inline const coroutine_runnable_base* get_runner() const { return runner_; }

            /**
             * @brief get runner return code
             * @return
             */
            inline int get_ret_code() const { return runner_ret_code_; }

            /**
             * @brief get runner return code
             * @return true if coroutine has run and finished
             */
            inline bool is_finished() const { return is_finished_; }

        protected:
            /**
             * @brief call platform jump to asm instruction
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
             * @brief fcontext entrance function
             * @param coro_ptr coroutine intptr
             */
            static void coroutine_context_callback(intptr_t coro_ptr);
        };
    }

    namespace this_coroutine {
        /**
         * @brief get current coroutine
         * @see detail::coroutine_context_base
         * @return pointer of current coroutine, if not in coroutine, return NULL
         */
        detail::coroutine_context_base* get_coroutine();

        /**
         * @brief get current coroutine and try to convert type
         * @see get_coroutine
         * @see detail::coroutine_context_base
         * @return pointer of current coroutine, if not in coroutine or fail to convert type, return NULL
         */
        template<typename Tc = detail::coroutine_context_base>
        Tc* get() {
            return dynamic_cast<Tc*>(get_coroutine());
        }

        /**
         * @brief yield current coroutine
         * @return 0 or error code
         */
        int yield();
    }
}

#endif

/**
 * thread safe coroutine base class
 */
#ifndef _COPP_COROUTINE_CONTEXT_COROUTINE_CONTEXT_SAFE_BASE_H_
#define _COPP_COROUTINE_CONTEXT_COROUTINE_CONTEXT_SAFE_BASE_H_


#if defined(_MSC_VER) && (_MSC_VER >= 1020)
# pragma once
#endif

#include <cstddef>

#include <libcopp/coroutine/coroutine_context_base.h>
#include <libcopp/utils/spin_lock.h>

namespace copp {
    namespace detail{
        /**
         * @brief status of safe coroutine context base
         */
        enum copp_coroutine_running_status
        {
            EN_CRS_INVALID = 0,//!< EN_CRS_INVALID
            EN_CRS_START,      //!< EN_CRS_START
            EN_CRS_RUNNING,    //!< EN_CRS_RUNNING
            EN_CRS_FINISHED,   //!< EN_CRS_FINISHED
            EN_CRS_STOP,       //!< EN_CRS_STOP
        };

        class coroutine_context_safe_base : public coroutine_context_base
        {
        public:
        	typedef coroutine_context_base base_type;
        	COROUTINE_CONTEXT_BASE_USING_BASE(base_type)

        protected:
            utils::spin_lock status_busy_; /** used in operation atom operation(not really lock) **/
            int status_running_; /** status **/

        public:
            coroutine_context_safe_base();
            virtual ~coroutine_context_safe_base();

        public:
            /**
             * @brief create coroutine context at stack context callee_
             * @param runner runner
             * @param func fcontext callback
             * @return COPP_EC_SUCCESS or error code
             *
             * @see coroutine_context_base::create
             */
            virtual int create(coroutine_runnable_base* runner, void(*func)(intptr_t) = &coroutine_context_safe_base::coroutine_context_callback);

            /**
             * @brief safety start coroutine
             * @return COPP_EC_SUCCESS or error code
             */
            virtual int start();

            /**
             * @brief safety yield coroutine
             * @return COPP_EC_SUCCESS or error code
             */
            virtual int yield();

            /**
             * @brief safety resume coroutine
             * @return COPP_EC_SUCCESS or error code
             */
            virtual int resume();

            /**
             * @brief safety stop coroutine
             * @return COPP_EC_SUCCESS or error code
             */
            virtual int stop();

        protected:
            /**
             * @brief safety set runner
             * @param runner
             * @return COPP_EC_SUCCESS or error code
             */
            virtual int set_runner(coroutine_runnable_base* runner);

        private:

            /**
             * @brief safety fcontext entrance function
             * @param coro_ptr coroutine intptr
             */
            static void coroutine_context_callback(intptr_t coro_ptr);
        };
    }
}

#endif

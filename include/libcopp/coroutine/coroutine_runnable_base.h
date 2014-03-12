#ifndef _COPP_COROUTINE_CONTEXT_RUNNABLE_BASE_H_
#define _COPP_COROUTINE_CONTEXT_RUNNABLE_BASE_H_


#if defined(_MSC_VER) && (_MSC_VER >= 1020)
# pragma once
#endif

#include <cstddef>

namespace copp {

    namespace detail {

        class coroutine_context_base;

        /**
         * @brief base type of runnable object
         * all coroutine run resource must be child of it
         */
        class coroutine_runnable_base {
        private:
            coroutine_context_base* coroutine_context_; /** binded coroutine context **/

        protected:
            coroutine_runnable_base();
            virtual ~coroutine_runnable_base() = 0;

        public:
            /**
             * @brief coroutine logic entrance
             * @note just like int main()
             * @return return code
             */
            virtual int operator()() = 0;

            /**
             * @brief get binded coroutine context
             * @return
             */
            template<typename TCOCTX>
            TCOCTX* get_coroutine_context() { return dynamic_cast<TCOCTX*>(coroutine_context_); }

            /**
             * @brief get binded coroutine context
             * @return
             */
            template<typename TCOCTX>
            const TCOCTX* get_coroutine_context() const { return dynamic_cast<const TCOCTX*>(coroutine_context_); }

            friend class coroutine_context_base;
        };
    }

    using detail::coroutine_runnable_base;
}

#endif

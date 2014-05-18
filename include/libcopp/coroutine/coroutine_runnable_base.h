#ifndef _COPP_COROUTINE_CONTEXT_RUNNABLE_BASE_H_
#define _COPP_COROUTINE_CONTEXT_RUNNABLE_BASE_H_


#if defined(_MSC_VER) && (_MSC_VER >= 1020)
# pragma once
#endif

#include <cstddef>

namespace copp {

    namespace detail {
        /**
         * @brief base type of runnable object
         * all coroutine run resource must be child of it
         */
        class coroutine_runnable_base {
        protected:
            coroutine_runnable_base();
            virtual ~coroutine_runnable_base();

        public:
            /**
             * @brief coroutine logic entrance
             * @note just like int main()
             * @return return code
             */
            virtual int operator()() = 0;
        };
    }

    using detail::coroutine_runnable_base;
}

#endif

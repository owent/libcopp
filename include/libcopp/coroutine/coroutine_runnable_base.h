#ifndef COPP_COROUTINE_CONTEXT_RUNNABLE_BASE_H
#define COPP_COROUTINE_CONTEXT_RUNNABLE_BASE_H


#pragma once

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
            virtual int operator()();

            /**
             * @brief coroutine logic entrance
             * @param priv_data private data, passed by start(priv_data)
             * @return return code
             */
            virtual int operator()(void *priv_data);
        };
    }

    using detail::coroutine_runnable_base;
}

#endif

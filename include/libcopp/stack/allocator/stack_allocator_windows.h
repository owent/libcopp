#ifndef _COPP_STACKCONTEXT_ALLOCATOR_WINDOWS_H_
#define _COPP_STACKCONTEXT_ALLOCATOR_WINDOWS_H_


#if defined(_MSC_VER) && (_MSC_VER >= 1020)
# pragma once
#endif

#include <cstddef>

#include <libcopp/utils/features.h>

#ifdef COPP_HAS_ABI_HEADERS
# include COPP_ABI_PREFIX
#endif

namespace copp {
    struct stack_context;

    namespace allocator {

        /**
         * @brief memory allocator
         * this allocator will create buffer using windows api and protect it
         */
        class stack_allocator_windows
        {
        public:
            stack_allocator_windows();
            ~stack_allocator_windows();

            /**
             * allocate memory and attach to stack context [standard function]
             * @param ctx stack context
             * @param size stack size
             */
            void allocate(stack_context &, std::size_t);

            /**
             * deallocate memory from stack context [standard function]
             * @param ctx stack context
             */
            void deallocate(stack_context &);
        };

    } 
}

#ifdef COPP_HAS_ABI_HEADERS
# include COPP_ABI_SUFFIX
#endif

#endif

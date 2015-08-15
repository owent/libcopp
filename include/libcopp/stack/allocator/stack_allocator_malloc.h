#ifndef _COPP_STACKCONTEXT_ALLOCATOR_MALLOC_H_
#define _COPP_STACKCONTEXT_ALLOCATOR_MALLOC_H_


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
         * this allocator will maintain buffer using malloc/free function
         */
        class stack_allocator_malloc
        {
        public:
            stack_allocator_malloc();
            ~stack_allocator_malloc();

            /**
             * allocate memory and attach to stack context [standard function]
             * @param ctx stack context
             * @param size stack size
             * @note size must less or equal than attached
             */
            void allocate(stack_context &ctx, std::size_t size);

            /**
             * deallocate memory from stack context [standard function]
             * @param ctx stack context
             */
            void deallocate(stack_context &ctx);
        };

    } 
}

#ifdef COPP_HAS_ABI_HEADERS
# include COPP_ABI_SUFFIX
#endif

#endif

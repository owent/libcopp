#ifndef _COPP_STACKCONTEXT_ALLOCATOR_MEMORY_H_
#define _COPP_STACKCONTEXT_ALLOCATOR_MEMORY_H_


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
         * this allocator will return address of specified memory section
         */
        class stack_allocator_memory
        {
        private:
            void* start_ptr_;
            std::size_t memory_size_;

        public:
            stack_allocator_memory();
            /**
             * construct and attach to start_ptr with size of max_size
             * @param start_ptr buffer start address
             * @param max_size buffer size
             */
            stack_allocator_memory(void* start_ptr, std::size_t max_size);
            ~stack_allocator_memory();

            /**
             * specify memory section allocated
             * @param start_ptr buffer start address
             * @param max_size buffer size
             * @note must be called before allocate operation
             */
            void attach(void* start_ptr, std::size_t max_size);

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

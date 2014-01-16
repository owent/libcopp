#ifndef _COPP_STACKCONTEXT_ALLOCATOR_MEMORY_H_
#define _COPP_STACKCONTEXT_ALLOCATOR_MEMORY_H_


#if defined(_MSC_VER) && (_MSC_VER >= 1020)
# pragma once
#endif

#include <cstddef>

#include <libcopp/config/features.h>

#ifdef COPP_HAS_ABI_HEADERS
# include COPP_ABI_PREFIX
#endif

namespace copp {
    struct stack_context;

    namespace allocator {

        class stack_allocator_memory
        {
        private:
            void* start_ptr_;
            std::size_t memory_size_;

        public:
            stack_allocator_memory();
            stack_allocator_memory(void* start_ptr, std::size_t max_size);
            ~stack_allocator_memory();

            void attach(void* start_ptr, std::size_t max_size);

            /**
             * is there stack  max size limit?
             */
            static bool is_stack_unbound();

            static std::size_t default_stacksize();

            static std::size_t minimum_stacksize();

            static std::size_t maximum_stacksize();

            void allocate(stack_context &, std::size_t);

            void deallocate(stack_context &);
        };

    } 
}

#ifdef COPP_HAS_ABI_HEADERS
# include COPP_ABI_SUFFIX
#endif

#endif

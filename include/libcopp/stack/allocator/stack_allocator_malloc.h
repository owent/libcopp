#ifndef COPP_STACKCONTEXT_ALLOCATOR_MALLOC_H
#define COPP_STACKCONTEXT_ALLOCATOR_MALLOC_H

#pragma once

#include <cstddef>

#include <libcopp/utils/features.h>

#ifdef COPP_HAS_ABI_HEADERS
#include COPP_ABI_PREFIX
#endif

namespace copp {
    struct stack_context;

    namespace allocator {

        /**
         * @brief memory allocator
         * this allocator will maintain buffer using malloc/free function
         */
        class stack_allocator_malloc {
        public:
            stack_allocator_malloc() UTIL_CONFIG_NOEXCEPT;
            ~stack_allocator_malloc();

            /**
             * allocate memory and attach to stack context [standard function]
             * @param ctx stack context
             * @param size stack size
             * @note size must less or equal than attached
             */
            void allocate(stack_context &ctx, std::size_t size) UTIL_CONFIG_NOEXCEPT;

            /**
             * deallocate memory from stack context [standard function]
             * @param ctx stack context
             */
            void deallocate(stack_context &ctx) UTIL_CONFIG_NOEXCEPT;
        };
    }
}

#ifdef COPP_HAS_ABI_HEADERS
#include COPP_ABI_SUFFIX
#endif

#endif

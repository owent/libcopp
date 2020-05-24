#ifndef COPP_STACKCONTEXT_ALLOCATOR_MALLOC_H
#define COPP_STACKCONTEXT_ALLOCATOR_MALLOC_H

#pragma once

#include <cstddef>

#include <libcopp/utils/config/compiler_features.h>
#include <libcopp/utils/config/libcopp_build_features.h>
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
        class LIBCOPP_COPP_API stack_allocator_malloc {
        public:
            stack_allocator_malloc() LIBCOPP_MACRO_NOEXCEPT;
            ~stack_allocator_malloc();
            stack_allocator_malloc(const stack_allocator_malloc &other) LIBCOPP_MACRO_NOEXCEPT;
            stack_allocator_malloc &operator=(const stack_allocator_malloc &other) LIBCOPP_MACRO_NOEXCEPT;
#if defined(UTIL_CONFIG_COMPILER_CXX_RVALUE_REFERENCES) && UTIL_CONFIG_COMPILER_CXX_RVALUE_REFERENCES
            stack_allocator_malloc(stack_allocator_malloc &&other) LIBCOPP_MACRO_NOEXCEPT;
            stack_allocator_malloc &operator=(stack_allocator_malloc &&other) LIBCOPP_MACRO_NOEXCEPT;
#endif

            /**
             * allocate memory and attach to stack context [standard function]
             * @param ctx stack context
             * @param size stack size
             * @note size must less or equal than attached
             */
            void allocate(stack_context &ctx, std::size_t size) LIBCOPP_MACRO_NOEXCEPT;

            /**
             * deallocate memory from stack context [standard function]
             * @param ctx stack context
             */
            void deallocate(stack_context &ctx) LIBCOPP_MACRO_NOEXCEPT;
        };
    } // namespace allocator
} // namespace copp

#ifdef COPP_HAS_ABI_HEADERS
#include COPP_ABI_SUFFIX
#endif

#endif

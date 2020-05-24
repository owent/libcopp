#ifndef COPP_STACKCONTEXT_ALLOCATOR_POSIX_H
#define COPP_STACKCONTEXT_ALLOCATOR_POSIX_H

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
         * this allocator will create buffer using posix api and protect it
         */
        class LIBCOPP_COPP_API stack_allocator_posix {
        public:
            stack_allocator_posix() LIBCOPP_MACRO_NOEXCEPT;
            ~stack_allocator_posix();
            stack_allocator_posix(const stack_allocator_posix &other) LIBCOPP_MACRO_NOEXCEPT;
            stack_allocator_posix &operator=(const stack_allocator_posix &other) LIBCOPP_MACRO_NOEXCEPT;
#if defined(UTIL_CONFIG_COMPILER_CXX_RVALUE_REFERENCES) && UTIL_CONFIG_COMPILER_CXX_RVALUE_REFERENCES
            stack_allocator_posix(stack_allocator_posix &&other) LIBCOPP_MACRO_NOEXCEPT;
            stack_allocator_posix &operator=(stack_allocator_posix &&other) LIBCOPP_MACRO_NOEXCEPT;
#endif

            /**
             * allocate memory and attach to stack context [standard function]
             * @param ctx stack context
             * @param size stack size
             */
            void allocate(stack_context &, std::size_t) LIBCOPP_MACRO_NOEXCEPT;

            /**
             * deallocate memory from stack context [standard function]
             * @param ctx stack context
             */
            void deallocate(stack_context &) LIBCOPP_MACRO_NOEXCEPT;
        };
    } // namespace allocator
} // namespace copp

#ifdef COPP_HAS_ABI_HEADERS
#include COPP_ABI_SUFFIX
#endif

#endif

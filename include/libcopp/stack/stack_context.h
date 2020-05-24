#ifndef COPP_STACKCONTEXT_STACKCONTEXT_H
#define COPP_STACKCONTEXT_STACKCONTEXT_H


#pragma once

#include <cstddef>

#include <libcopp/utils/config/compiler_features.h>
#include <libcopp/utils/config/libcopp_build_features.h>
#include <libcopp/utils/features.h>

namespace copp {
    struct LIBCOPP_COPP_API stack_context {
        size_t size; /** @brief stack size **/
        void * sp;   /** @brief stack end pointer **/

#ifdef LIBCOPP_MACRO_USE_SEGMENTED_STACKS
        typedef void *     segments_context_t[COPP_MACRO_SEGMENTED_STACK_NUMBER];
        segments_context_t segments_ctx; /** @brief gcc split segment stack data **/
#endif

#ifdef LIBCOPP_MACRO_USE_VALGRIND
        unsigned valgrind_stack_id;
#endif

        stack_context() LIBCOPP_MACRO_NOEXCEPT;
        ~stack_context() LIBCOPP_MACRO_NOEXCEPT;

        stack_context(const stack_context &other) LIBCOPP_MACRO_NOEXCEPT;
        stack_context &operator=(const stack_context &other) LIBCOPP_MACRO_NOEXCEPT;
#if defined(UTIL_CONFIG_COMPILER_CXX_RVALUE_REFERENCES) && UTIL_CONFIG_COMPILER_CXX_RVALUE_REFERENCES
        stack_context(stack_context &&other) LIBCOPP_MACRO_NOEXCEPT;
        stack_context &operator=(stack_context &&other) LIBCOPP_MACRO_NOEXCEPT;
#endif

        void reset() LIBCOPP_MACRO_NOEXCEPT;

        void copy_from(const stack_context &other) LIBCOPP_MACRO_NOEXCEPT;
    };
} // namespace copp

#endif

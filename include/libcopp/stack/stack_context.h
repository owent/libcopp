#ifndef COPP_STACKCONTEXT_STACKCONTEXT_H
#define COPP_STACKCONTEXT_STACKCONTEXT_H


# pragma once

#include <cstddef>

#include <libcopp/utils/features.h>

namespace copp { 
    struct stack_context
    {
        size_t size; /** @brief stack size **/
        void* sp; /** @brief stack end pointer **/

#ifdef COPP_MACRO_USE_SEGMENTED_STACKS
        typedef void* segments_context_t[COPP_MACRO_SEGMENTED_STACK_NUMBER];
        segments_context_t segments_ctx; /** @brief gcc split segment stack data **/
#endif

#ifdef COPP_MACRO_USE_VALGRIND
        unsigned                valgrind_stack_id;
#endif

        stack_context();
        ~stack_context();

        void reset();
    };
}

#endif

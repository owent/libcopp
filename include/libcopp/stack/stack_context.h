#ifndef _COPP_STACKCONTEXT_STACKCONTEXT_H_
#define _COPP_STACKCONTEXT_STACKCONTEXT_H_


#if defined(_MSC_VER) && (_MSC_VER >= 1020)
# pragma once
#endif

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

        stack_context();
        ~stack_context();

        void reset();
    };
}

#endif

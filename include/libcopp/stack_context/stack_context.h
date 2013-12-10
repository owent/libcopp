#ifndef _COPP_STACKCONTEXT_STACKCONTEXT_H_
#define _COPP_STACKCONTEXT_STACKCONTEXT_H_


#if defined(_MSC_VER) && (_MSC_VER >= 1020)
# pragma once
#endif

#include <cstddef>

namespace copp { 
    struct stack_context
    {
        size_t size;
        void* sp;

        stack_context();
        ~stack_context();
    };
}

#endif

#include <cstring>

#include "libcopp/stack/stack_context.h"

namespace copp { 

    stack_context::stack_context() :size(0), sp(NULL)
#ifdef COPP_MACRO_USE_SEGMENTED_STACKS
        ,segments_ctx()
#endif
    {
    }


    stack_context::~stack_context() {
    }

    void stack_context::reset() {
        size = 0;
        sp = NULL;
#ifdef COPP_MACRO_USE_SEGMENTED_STACKS
        memset(segments_ctx, 0, sizeof(segments_ctx));
#endif
    }
}

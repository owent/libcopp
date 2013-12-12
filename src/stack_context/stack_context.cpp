#include <cstring>

#include "libcopp/stack_context/stack_context.h"

namespace copp { 

    stack_context::stack_context() :size(0), sp(NULL)
#ifdef COPP_MACRO_USE_SEGMENTED_STACKS
        ,segments_ctx()
#endif
    {
    }


    stack_context::~stack_context()
    {
    }
}

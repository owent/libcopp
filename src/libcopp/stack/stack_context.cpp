#include <cstring>

#include "libcopp/stack/stack_context.h"

namespace copp {

    stack_context::stack_context()
        : size(0), sp(NULL)
#ifdef LIBCOPP_MACRO_USE_SEGMENTED_STACKS
          ,
          segments_ctx()
#endif
#ifdef LIBCOPP_MACRO_USE_VALGRIND
          ,
          valgrind_stack_id(0)
#endif
    {
    }


    stack_context::~stack_context() {}

    void stack_context::reset() {
        size = 0;
        sp   = NULL;
#ifdef LIBCOPP_MACRO_USE_SEGMENTED_STACKS
        memset(segments_ctx, 0, sizeof(segments_ctx));
#endif
#ifdef LIBCOPP_MACRO_USE_VALGRIND
        valgrind_stack_id = 0;
#endif
    }
} // namespace copp

#include <cstring>

#include "libcopp/stack_context/stack_context.h"

namespace copp { 

    stack_context::stack_context() :size(0), sp(NULL)
    {
    }


    stack_context::~stack_context()
    {
    }
}

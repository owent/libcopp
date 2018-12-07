#ifndef COPP_STACKCONTEXT_STACK_ALLOCATOR_H
#define COPP_STACKCONTEXT_STACK_ALLOCATOR_H


#pragma once

#include <libcopp/utils/features.h>

#include "allocator/stack_allocator_malloc.h"
#include "allocator/stack_allocator_memory.h"
#include "allocator/stack_allocator_pool.h"

#ifdef LIBCOPP_MACRO_USE_SEGMENTED_STACKS
#include "allocator/stack_allocator_split_segment.h"
#endif

#ifdef LIBCOPP_MACRO_SYS_POSIX
#include "allocator/stack_allocator_posix.h"
namespace copp {
    namespace allocator {
        typedef stack_allocator_posix default_statck_allocator;
    }
} // namespace copp

#elif defined(LIBCOPP_MACRO_SYS_WIN)

#include "allocator/stack_allocator_windows.h"
namespace copp {
    namespace allocator {
        typedef stack_allocator_windows default_statck_allocator;
    }
} // namespace copp

#else

#error unknown stack allocator

#endif

#endif /* STACK_ALLOCATOR_H_ */

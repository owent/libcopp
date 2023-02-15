// Copyright 2023 owent

#pragma once

#include <libcopp/utils/config/libcopp_build_features.h>

#include <libcopp/utils/features.h>

#include "allocator/stack_allocator_malloc.h"
#include "allocator/stack_allocator_memory.h"
#include "allocator/stack_allocator_pool.h"

#ifdef LIBCOPP_MACRO_USE_SEGMENTED_STACKS
#  include "allocator/stack_allocator_split_segment.h"
#endif

#ifdef LIBCOPP_MACRO_SYS_POSIX
#  include "allocator/stack_allocator_posix.h"
LIBCOPP_COPP_NAMESPACE_BEGIN
namespace allocator {
using default_statck_allocator = stack_allocator_posix;
}
LIBCOPP_COPP_NAMESPACE_END

#elif defined(LIBCOPP_MACRO_SYS_WIN)

#  include "allocator/stack_allocator_windows.h"
LIBCOPP_COPP_NAMESPACE_BEGIN
namespace allocator {
using default_statck_allocator = stack_allocator_windows;
}
LIBCOPP_COPP_NAMESPACE_END

#else

#  error unknown stack allocator

#endif

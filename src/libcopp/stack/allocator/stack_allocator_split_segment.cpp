// Copyright 2023 owent

#include <libcopp/utils/config/libcopp_build_features.h>

#include <libcopp/stack/allocator/stack_allocator_split_segment.h>
#include <libcopp/stack/stack_context.h>
#include <libcopp/stack/stack_traits.h>
#include <libcopp/fcontext/fcontext.hpp>

// clang-format off
#include <libcopp/utils/config/stl_include_prefix.h>  // NOLINT(build/include_order)
// clang-format on
extern "C" {
#include <signal.h>
}

#include <assert.h>
#include <algorithm>
#include <cstring>
#include <limits>
// clang-format off
#include <libcopp/utils/config/stl_include_suffix.h>  // NOLINT(build/include_order)
// clang-format on

#ifdef LIBCOPP_MACRO_USE_SEGMENTED_STACKS
extern "C" {
void *__splitstack_makecontext(std::size_t, void *[COPP_MACRO_SEGMENTED_STACK_NUMBER], std::size_t *);

void __splitstack_releasecontext(void *[COPP_MACRO_SEGMENTED_STACK_NUMBER]);

void __splitstack_resetcontext(void *[COPP_MACRO_SEGMENTED_STACK_NUMBER]);

void __splitstack_block_signals_context(void *[COPP_MACRO_SEGMENTED_STACK_NUMBER], int *new_value, int *old_value);
}
#endif

#ifdef COPP_HAS_ABI_HEADERS
#  include COPP_ABI_PREFIX
#endif

LIBCOPP_COPP_NAMESPACE_BEGIN
namespace allocator {

LIBCOPP_COPP_API stack_allocator_split_segment::stack_allocator_split_segment() LIBCOPP_MACRO_NOEXCEPT {}
LIBCOPP_COPP_API stack_allocator_split_segment::~stack_allocator_split_segment() {}
LIBCOPP_COPP_API
stack_allocator_split_segment::stack_allocator_split_segment(const stack_allocator_split_segment &)
    LIBCOPP_MACRO_NOEXCEPT {}
LIBCOPP_COPP_API stack_allocator_split_segment &stack_allocator_split_segment::operator=(
    const stack_allocator_split_segment &) LIBCOPP_MACRO_NOEXCEPT {
  return *this;
}

LIBCOPP_COPP_API
stack_allocator_split_segment::stack_allocator_split_segment(stack_allocator_split_segment &&) LIBCOPP_MACRO_NOEXCEPT {}
LIBCOPP_COPP_API stack_allocator_split_segment &stack_allocator_split_segment::operator=(
    stack_allocator_split_segment &&) LIBCOPP_MACRO_NOEXCEPT {
  return *this;
}

LIBCOPP_COPP_API void stack_allocator_split_segment::allocate(stack_context &ctx,
                                                              std::size_t size) LIBCOPP_MACRO_NOEXCEPT {
  void *start_ptr = __splitstack_makecontext(size, ctx.segments_ctx, &ctx.size);
  assert(start_ptr);
  if (!start_ptr) {
    ctx.sp = nullptr;
    return;
  }

  ctx.sp = static_cast<char *>(start_ptr) + ctx.size;  // stack down

  int off = 0;
  __splitstack_block_signals_context(ctx.segments_ctx, &off, 0);
}

LIBCOPP_COPP_API void stack_allocator_split_segment::deallocate(stack_context &ctx) LIBCOPP_MACRO_NOEXCEPT {
  __splitstack_releasecontext(ctx.segments_ctx);
}
}  // namespace allocator
LIBCOPP_COPP_NAMESPACE_END

#ifdef COPP_HAS_ABI_HEADERS
#  include COPP_ABI_SUFFIX
#endif

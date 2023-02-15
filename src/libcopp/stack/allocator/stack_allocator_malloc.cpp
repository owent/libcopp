// Copyright 2023 owent

#include <libcopp/utils/config/libcopp_build_features.h>

#include <libcopp/stack/allocator/stack_allocator_malloc.h>
#include <libcopp/stack/stack_context.h>
#include <libcopp/stack/stack_traits.h>

#if defined(LIBCOPP_MACRO_USE_VALGRIND)
#  include <valgrind/valgrind.h>
#endif

// clang-format off
#include <libcopp/utils/config/stl_include_prefix.h>  // NOLINT(build/include_order)
// clang-format on
#include <assert.h>
#include <algorithm>
#include <cstdlib>
#include <cstring>
#include <limits>
#include <memory>
#include <numeric>
// clang-format off
#include <libcopp/utils/config/stl_include_suffix.h>  // NOLINT(build/include_order)
// clang-format on

#ifdef COPP_HAS_ABI_HEADERS
#  include COPP_ABI_PREFIX
#endif

LIBCOPP_COPP_NAMESPACE_BEGIN
namespace allocator {
LIBCOPP_COPP_API stack_allocator_malloc::stack_allocator_malloc() LIBCOPP_MACRO_NOEXCEPT {}
LIBCOPP_COPP_API stack_allocator_malloc::~stack_allocator_malloc() {}
LIBCOPP_COPP_API stack_allocator_malloc::stack_allocator_malloc(const stack_allocator_malloc &) LIBCOPP_MACRO_NOEXCEPT {
}
LIBCOPP_COPP_API stack_allocator_malloc &stack_allocator_malloc::operator=(const stack_allocator_malloc &)
    LIBCOPP_MACRO_NOEXCEPT {
  return *this;
}

LIBCOPP_COPP_API stack_allocator_malloc::stack_allocator_malloc(stack_allocator_malloc &&) LIBCOPP_MACRO_NOEXCEPT {}
LIBCOPP_COPP_API stack_allocator_malloc &stack_allocator_malloc::operator=(stack_allocator_malloc &&)
    LIBCOPP_MACRO_NOEXCEPT {
  return *this;
}

LIBCOPP_COPP_API void stack_allocator_malloc::allocate(stack_context &ctx, std::size_t size) LIBCOPP_MACRO_NOEXCEPT {
  size = (std::max)(size, stack_traits::minimum_size());
  size = (std::min)(size, stack_traits::maximum_size());

  std::size_t size_ = stack_traits::round_to_page_size(size);

  void *start_ptr = malloc(size_);

  if (!start_ptr) {
    ctx.sp = nullptr;
    return;
  }

  ctx.size = size_;
  ctx.sp = static_cast<char *>(start_ptr) + ctx.size;  // stack down

#if defined(LIBCOPP_MACRO_USE_VALGRIND)
  ctx.valgrind_stack_id = VALGRIND_STACK_REGISTER(ctx.sp, start_ptr);
#endif
}

LIBCOPP_COPP_API void stack_allocator_malloc::deallocate(stack_context &ctx) LIBCOPP_MACRO_NOEXCEPT {
  assert(ctx.sp);
  assert(stack_traits::minimum_size() <= ctx.size);
  assert(stack_traits::is_unbounded() || (stack_traits::maximum_size() >= ctx.size));

#if defined(LIBCOPP_MACRO_USE_VALGRIND)
  VALGRIND_STACK_DEREGISTER(ctx.valgrind_stack_id);
#endif
  void *start_ptr = static_cast<char *>(ctx.sp) - ctx.size;
  free(start_ptr);
}
}  // namespace allocator
LIBCOPP_COPP_NAMESPACE_END

#ifdef COPP_HAS_ABI_HEADERS
#  include COPP_ABI_SUFFIX
#endif

// Copyright 2023 owent

#include <libcopp/utils/config/libcopp_build_features.h>

#include <libcopp/stack/allocator/stack_allocator_posix.h>
#include <libcopp/stack/stack_context.h>
#include <libcopp/stack/stack_traits.h>
#include <libcopp/fcontext/fcontext.hpp>

#if defined(LIBCOPP_MACRO_USE_VALGRIND)
#  include <valgrind/valgrind.h>
#endif

extern "C" {
#include <errno.h>
#include <fcntl.h>
#include <signal.h>
#include <sys/mman.h>
#include <sys/resource.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>
}

// clang-format off
#include <libcopp/utils/config/stl_include_prefix.h>  // NOLINT(build/include_order)
// clang-format on
#include <assert.h>
#include <algorithm>
#include <cstring>
#include <iostream>
#include <limits>
#include <numeric>
// clang-format off
#include <libcopp/utils/config/stl_include_suffix.h>  // NOLINT(build/include_order)
// clang-format on

#ifdef COPP_HAS_ABI_HEADERS
#  include COPP_ABI_PREFIX
#endif

LIBCOPP_COPP_NAMESPACE_BEGIN
namespace allocator {
LIBCOPP_COPP_API stack_allocator_posix::stack_allocator_posix() LIBCOPP_MACRO_NOEXCEPT {}
LIBCOPP_COPP_API stack_allocator_posix::~stack_allocator_posix() {}
LIBCOPP_COPP_API stack_allocator_posix::stack_allocator_posix(const stack_allocator_posix &) LIBCOPP_MACRO_NOEXCEPT {}
LIBCOPP_COPP_API stack_allocator_posix &stack_allocator_posix::operator=(const stack_allocator_posix &)
    LIBCOPP_MACRO_NOEXCEPT {
  return *this;
}

LIBCOPP_COPP_API stack_allocator_posix::stack_allocator_posix(stack_allocator_posix &&) LIBCOPP_MACRO_NOEXCEPT {}
LIBCOPP_COPP_API stack_allocator_posix &stack_allocator_posix::operator=(stack_allocator_posix &&)
    LIBCOPP_MACRO_NOEXCEPT {
  return *this;
}

LIBCOPP_COPP_API void stack_allocator_posix::allocate(stack_context &ctx, std::size_t size) LIBCOPP_MACRO_NOEXCEPT {
  size = (std::max)(size, stack_traits::minimum_size());
  size = (std::min)(size, stack_traits::maximum_size());

  std::size_t size_ = stack_traits::round_to_page_size(size) + stack_traits::page_size();  // add one protected page
  assert(size > 0 && size_ > 0);

  // conform to POSIX.4 (POSIX.1b-1993, _POSIX_C_SOURCE=199309L)
  void *start_ptr =
#if defined(macintosh) || defined(__APPLE__) || defined(__APPLE_CC__)
      ::mmap(0, size_, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANON, -1, 0);
#else
      ::mmap(0, size_, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
#endif

  if (!start_ptr || MAP_FAILED == start_ptr) {
    ctx.sp = nullptr;
    return;
  }

  // memset(start_ptr, 0, size_);
  ::mprotect(start_ptr, stack_traits::page_size(), PROT_NONE);

  ctx.size = size_;
  ctx.sp = static_cast<char *>(start_ptr) + ctx.size;  // stack down

#if defined(LIBCOPP_MACRO_USE_VALGRIND)
  ctx.valgrind_stack_id = VALGRIND_STACK_REGISTER(ctx.sp, start_ptr);
#endif
}

LIBCOPP_COPP_API void stack_allocator_posix::deallocate(stack_context &ctx) LIBCOPP_MACRO_NOEXCEPT {
  assert(ctx.sp);
  assert(stack_traits::minimum_size() <= ctx.size);
  assert(stack_traits::is_unbounded() || (stack_traits::maximum_size() >= ctx.size));

#if defined(LIBCOPP_MACRO_USE_VALGRIND)
  VALGRIND_STACK_DEREGISTER(ctx.valgrind_stack_id);
#endif

  void *start_ptr = static_cast<char *>(ctx.sp) - ctx.size;
  ::munmap(start_ptr, ctx.size);
}
}  // namespace allocator
LIBCOPP_COPP_NAMESPACE_END

#ifdef COPP_HAS_ABI_HEADERS
#  include COPP_ABI_SUFFIX
#endif

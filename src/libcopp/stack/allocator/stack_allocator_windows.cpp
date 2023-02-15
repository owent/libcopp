// Copyright 2023 owent

#include <libcopp/utils/config/libcopp_build_features.h>

#include <libcopp/stack/allocator/stack_allocator_windows.h>
#include <libcopp/stack/stack_context.h>
#include <libcopp/stack/stack_traits.h>

#ifndef WIN32_LEAN_AND_MEAN
#  define WIN32_LEAN_AND_MEAN
#endif

extern "C" {
#include <Windows.h>
}

// clang-format off
#include <libcopp/utils/config/stl_include_prefix.h>  // NOLINT(build/include_order)
// clang-format on
#include <assert.h>
#include <algorithm>
#include <cstring>
#include <limits>
#include <numeric>
// clang-format off
#include <libcopp/utils/config/stl_include_suffix.h>  // NOLINT(build/include_order)
// clang-format on

#if defined(COPP_MACRO_COMPILER_MSVC)
#  pragma warning(push)
#  pragma warning(disable : 4244 4267)
#endif

#ifdef COPP_HAS_ABI_HEADERS
#  include COPP_ABI_PREFIX
#endif

LIBCOPP_COPP_NAMESPACE_BEGIN
namespace allocator {

LIBCOPP_COPP_API stack_allocator_windows::stack_allocator_windows() LIBCOPP_MACRO_NOEXCEPT {}
LIBCOPP_COPP_API stack_allocator_windows::~stack_allocator_windows() {}
LIBCOPP_COPP_API stack_allocator_windows::stack_allocator_windows(const stack_allocator_windows &)
    LIBCOPP_MACRO_NOEXCEPT {}
LIBCOPP_COPP_API stack_allocator_windows &stack_allocator_windows::operator=(const stack_allocator_windows &)
    LIBCOPP_MACRO_NOEXCEPT {
  return *this;
}

LIBCOPP_COPP_API stack_allocator_windows::stack_allocator_windows(stack_allocator_windows &&) LIBCOPP_MACRO_NOEXCEPT {}
LIBCOPP_COPP_API stack_allocator_windows &stack_allocator_windows::operator=(stack_allocator_windows &&)
    LIBCOPP_MACRO_NOEXCEPT {
  return *this;
}

LIBCOPP_COPP_API void stack_allocator_windows::allocate(stack_context &ctx, std::size_t size) LIBCOPP_MACRO_NOEXCEPT {
  size = (std::max)(size, stack_traits::minimum_size());
  size = stack_traits::is_unbounded() ? size : (std::min)(size, stack_traits::maximum_size());

  std::size_t size_ = stack_traits::round_to_page_size(size) + stack_traits::page_size();
  assert(size > 0 && size_ > 0);

  void *start_ptr = ::VirtualAlloc(0, size_, MEM_COMMIT, PAGE_READWRITE);
  if (!start_ptr) {
    ctx.sp = nullptr;
    return;
  }

  // memset(start_ptr, 0, size_);
  DWORD old_options;
  ::VirtualProtect(start_ptr, stack_traits::page_size(), PAGE_READWRITE | PAGE_GUARD, &old_options);

  ctx.size = size_;
  ctx.sp = static_cast<char *>(start_ptr) + ctx.size;  // stack down
}

LIBCOPP_COPP_API void stack_allocator_windows::deallocate(stack_context &ctx) LIBCOPP_MACRO_NOEXCEPT {
  assert(ctx.sp);
  assert(stack_traits::minimum_size() <= ctx.size);
  assert(stack_traits::is_unbounded() || (stack_traits::maximum_size() >= ctx.size));

  void *start_ptr = static_cast<char *>(ctx.sp) - ctx.size;
  ::VirtualFree(start_ptr, 0, MEM_RELEASE);
}
}  // namespace allocator
LIBCOPP_COPP_NAMESPACE_END

#ifdef COPP_HAS_ABI_HEADERS
#  include COPP_ABI_SUFFIX
#endif

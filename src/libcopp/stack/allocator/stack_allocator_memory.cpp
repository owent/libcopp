// Copyright 2023 owent

#include <libcopp/utils/config/libcopp_build_features.h>

#include <libcopp/utils/std/explicit_declare.h>

#include <libcopp/stack/allocator/stack_allocator_memory.h>
#include <libcopp/stack/stack_context.h>
#include <libcopp/stack/stack_traits.h>

#if defined(LIBCOPP_MACRO_USE_VALGRIND)
#  include <valgrind/valgrind.h>
#endif

#ifdef COPP_HAS_ABI_HEADERS
#  include COPP_ABI_PREFIX
#endif

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

LIBCOPP_COPP_NAMESPACE_BEGIN
namespace allocator {

LIBCOPP_COPP_API stack_allocator_memory::stack_allocator_memory() LIBCOPP_MACRO_NOEXCEPT : start_ptr_(nullptr),
                                                                                           memory_size_(0),
                                                                                           is_used_(false) {}

LIBCOPP_COPP_API stack_allocator_memory::stack_allocator_memory(void *start_ptr,
                                                                std::size_t max_size) LIBCOPP_MACRO_NOEXCEPT
    : start_ptr_(start_ptr),
      memory_size_(max_size),
      is_used_(false) {}

LIBCOPP_COPP_API stack_allocator_memory::stack_allocator_memory(stack_allocator_memory &other) LIBCOPP_MACRO_NOEXCEPT
    : start_ptr_(nullptr),
      memory_size_(0),
      is_used_(false) {
  if (!other.is_used_) {
    swap(other);
  }
}

LIBCOPP_COPP_API stack_allocator_memory::stack_allocator_memory(stack_allocator_memory &&other) LIBCOPP_MACRO_NOEXCEPT
    : start_ptr_(nullptr),
      memory_size_(0),
      is_used_(false) {
  if (!other.is_used_) {
    swap(other);
  }
}

LIBCOPP_COPP_API stack_allocator_memory::~stack_allocator_memory() {}

LIBCOPP_COPP_API stack_allocator_memory &stack_allocator_memory::operator=(stack_allocator_memory &other)
    LIBCOPP_MACRO_NOEXCEPT {
  if (!other.is_used_) {
    swap(other);
  }
  return *this;
}

LIBCOPP_COPP_API stack_allocator_memory &stack_allocator_memory::operator=(stack_allocator_memory &&other)
    LIBCOPP_MACRO_NOEXCEPT {
  if (!other.is_used_) {
    swap(other);
  }
  return *this;
}

LIBCOPP_COPP_API void stack_allocator_memory::swap(stack_allocator_memory &other) {
  using std::swap;
  swap(start_ptr_, other.start_ptr_);
  swap(memory_size_, other.memory_size_);
  swap(is_used_, other.is_used_);
}

LIBCOPP_COPP_API void stack_allocator_memory::attach(void *start_ptr, std::size_t max_size) LIBCOPP_MACRO_NOEXCEPT {
  start_ptr_ = start_ptr;
  memory_size_ = max_size;
  is_used_ = false;
}

LIBCOPP_COPP_API void stack_allocator_memory::allocate(stack_context &ctx, std::size_t size) LIBCOPP_MACRO_NOEXCEPT {
  if (nullptr == start_ptr_ || is_used_) {
    ctx.sp = nullptr;
    return;
  }

  size = (std::max)(size, stack_traits::minimum_size());
  size = (std::min)(size, stack_traits::maximum_size());
  size = (std::min)(size, memory_size_);

  std::size_t size_ = stack_traits::round_to_page_size(size);
  assert(size > 0 && size_ > 0 && size_ <= memory_size_);

  ctx.size = size_;
  ctx.sp = static_cast<char *>(start_ptr_) + ctx.size;  // stack down

#if defined(LIBCOPP_MACRO_USE_VALGRIND)
  ctx.valgrind_stack_id = VALGRIND_STACK_REGISTER(ctx.sp, start_ptr_);
#endif
  is_used_ = true;
}

LIBCOPP_COPP_API void stack_allocator_memory::deallocate(EXPLICIT_UNUSED_ATTR stack_context &ctx)
    LIBCOPP_MACRO_NOEXCEPT {
  assert(ctx.sp);
  assert(stack_traits::minimum_size() <= ctx.size);
  assert(stack_traits::is_unbounded() || (stack_traits::maximum_size() >= ctx.size));

#if defined(LIBCOPP_MACRO_USE_VALGRIND)
  VALGRIND_STACK_DEREGISTER(ctx.valgrind_stack_id);
#endif
  is_used_ = false;
}
}  // namespace allocator
LIBCOPP_COPP_NAMESPACE_END

#ifdef COPP_HAS_ABI_HEADERS
#  include COPP_ABI_SUFFIX
#endif

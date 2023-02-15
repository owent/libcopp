// Copyright 2023 owent

#pragma once

#include <libcopp/utils/config/libcopp_build_features.h>

#include <libcopp/utils/features.h>

#include <cstddef>

#ifdef COPP_HAS_ABI_HEADERS
#  include COPP_ABI_PREFIX
#endif

LIBCOPP_COPP_NAMESPACE_BEGIN
struct stack_context;

namespace allocator {

/**
 * @brief memory allocator
 * this allocator will maintain buffer using malloc/free function
 */
class LIBCOPP_COPP_API stack_allocator_malloc {
 public:
  stack_allocator_malloc() LIBCOPP_MACRO_NOEXCEPT;
  ~stack_allocator_malloc();
  stack_allocator_malloc(const stack_allocator_malloc &other) LIBCOPP_MACRO_NOEXCEPT;
  stack_allocator_malloc &operator=(const stack_allocator_malloc &other) LIBCOPP_MACRO_NOEXCEPT;
  stack_allocator_malloc(stack_allocator_malloc &&other) LIBCOPP_MACRO_NOEXCEPT;
  stack_allocator_malloc &operator=(stack_allocator_malloc &&other) LIBCOPP_MACRO_NOEXCEPT;

  /**
   * allocate memory and attach to stack context [standard function]
   * @param ctx stack context
   * @param size stack size
   * @note size must less or equal than attached
   */
  void allocate(stack_context &ctx, std::size_t size) LIBCOPP_MACRO_NOEXCEPT;

  /**
   * deallocate memory from stack context [standard function]
   * @param ctx stack context
   */
  void deallocate(stack_context &ctx) LIBCOPP_MACRO_NOEXCEPT;
};
}  // namespace allocator
LIBCOPP_COPP_NAMESPACE_END

#ifdef COPP_HAS_ABI_HEADERS
#  include COPP_ABI_SUFFIX
#endif

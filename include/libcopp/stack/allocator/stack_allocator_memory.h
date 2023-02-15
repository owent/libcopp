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
 * this allocator will return address of specified memory section
 */
class LIBCOPP_COPP_API stack_allocator_memory {
 private:
  void *start_ptr_;
  std::size_t memory_size_;
  bool is_used_;

 public:
  stack_allocator_memory() LIBCOPP_MACRO_NOEXCEPT;
  /**
   * construct and attach to start_ptr with size of max_size
   * @param start_ptr buffer start address
   * @param max_size buffer size
   */
  stack_allocator_memory(void *start_ptr, std::size_t max_size) LIBCOPP_MACRO_NOEXCEPT;
  stack_allocator_memory(stack_allocator_memory &other) LIBCOPP_MACRO_NOEXCEPT;
  stack_allocator_memory(stack_allocator_memory &&other) LIBCOPP_MACRO_NOEXCEPT;
  ~stack_allocator_memory();

  stack_allocator_memory &operator=(stack_allocator_memory &other) LIBCOPP_MACRO_NOEXCEPT;
  stack_allocator_memory &operator=(stack_allocator_memory &&other) LIBCOPP_MACRO_NOEXCEPT;
  void swap(stack_allocator_memory &other);

  /**
   * specify memory section allocated
   * @param start_ptr buffer start address
   * @param max_size buffer size
   * @note must be called before allocate operation
   */
  void attach(void *start_ptr, std::size_t max_size) LIBCOPP_MACRO_NOEXCEPT;

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

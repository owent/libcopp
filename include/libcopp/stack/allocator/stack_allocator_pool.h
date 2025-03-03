// Copyright 2023 owent

#pragma once

#include <libcopp/utils/config/libcopp_build_features.h>
#include <libcopp/utils/features.h>
#include <libcopp/utils/memory/default_smart_ptr_trait.h>

#include <assert.h>
#include <cstddef>
#include <memory>

#ifdef LIBCOPP_HAS_ABI_HEADERS
#  include LIBCOPP_ABI_PREFIX
#endif

LIBCOPP_COPP_NAMESPACE_BEGIN
struct stack_context;

namespace allocator {

/**
 * @brief memory allocator
 * this allocator will maintain buffer using malloc/free function
 */
template <typename TPool>
class LIBCOPP_COPP_API_HEAD_ONLY stack_allocator_pool {
 public:
  using pool_type = TPool;

  // Compability with libcopp-1.x
  using pool_t = pool_type;

 public:
  stack_allocator_pool() LIBCOPP_MACRO_NOEXCEPT {}
  stack_allocator_pool(const LIBCOPP_COPP_NAMESPACE_ID::memory::default_strong_rc_ptr<pool_type> &p)
      LIBCOPP_MACRO_NOEXCEPT : pool_(p) {}
  ~stack_allocator_pool() {}

  /**
   * specify memory section allocated
   * @param start_ptr buffer start address
   * @param max_size buffer size
   * @note must be called before allocate operation
   */
  void attach(const LIBCOPP_COPP_NAMESPACE_ID::memory::default_strong_rc_ptr<pool_type> &p) LIBCOPP_MACRO_NOEXCEPT {
    pool_ = p;
  }

  /**
   * allocate memory and attach to stack context [standard function]
   * @param ctx stack context
   * @param size ignored
   * @note size must less or equal than attached
   */
  void allocate(stack_context &ctx, std::size_t) LIBCOPP_MACRO_NOEXCEPT {
    assert(pool_);
    if (pool_) {
      pool_->allocate(ctx);
    }
  }

  /**
   * deallocate memory from stack context [standard function]
   * @param ctx stack context
   */
  void deallocate(stack_context &ctx) LIBCOPP_MACRO_NOEXCEPT {
    assert(pool_);
    if (pool_) {
      pool_->deallocate(ctx);
    }
  }

 private:
  LIBCOPP_COPP_NAMESPACE_ID::memory::default_strong_rc_ptr<pool_type> pool_;
};
}  // namespace allocator
LIBCOPP_COPP_NAMESPACE_END

#ifdef LIBCOPP_HAS_ABI_HEADERS
#  include LIBCOPP_ABI_SUFFIX
#endif

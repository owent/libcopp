// Copyright 2023 owent

#pragma once

#include <libcopp/utils/config/libcopp_build_features.h>

#include <libcopp/utils/features.h>
#include <libcopp/utils/lock_holder.h>
#include <libcopp/utils/spin_lock.h>

#include <libcopp/stack/stack_context.h>
#include <libcopp/stack/stack_traits.h>

// clang-format off
#include <libcopp/utils/config/stl_include_prefix.h>  // NOLINT(build/include_order)
// clang-format on
#include <assert.h>
#include <cstring>
#include <list>
#include <memory>
// clang-format off
#include <libcopp/utils/config/stl_include_suffix.h>  // NOLINT(build/include_order)
// clang-format on

LIBCOPP_COPP_NAMESPACE_BEGIN
template <typename TAlloc>
class LIBCOPP_COPP_API_HEAD_ONLY stack_pool {
 public:
  using allocator_type = TAlloc;
  using ptr_type = std::shared_ptr<stack_pool<TAlloc> >;

  // Compability with libcopp-1.x
  using allocator_t = allocator_type;
  using ptr_t = ptr_type;

  struct limit_t {
    size_t used_stack_number;
    size_t used_stack_size;
    size_t free_stack_number;
    size_t free_stack_size;
  };

  struct configure_t {
    size_t stack_size;
    size_t stack_offset;
    size_t gc_number;
    size_t max_stack_number;
    size_t max_stack_size;
    size_t min_stack_number;
    size_t min_stack_size;
    bool auto_gc;
  };

 private:
  struct constructor_delegator {};

  stack_pool() = delete;
  stack_pool(const stack_pool &) = delete;

 public:
  static ptr_type create() { return std::make_shared<stack_pool>(constructor_delegator()); }

  stack_pool(constructor_delegator) {
    memset(&limits_, 0, sizeof(limits_));
    memset(&conf_, 0, sizeof(conf_));
    conf_.stack_size = LIBCOPP_COPP_NAMESPACE_ID::stack_traits::default_size();
    conf_.auto_gc = true;
  }
  ~stack_pool() { clear(); }

  inline const limit_t &get_limit() const { return limits_; }

  // configure
  inline allocator_type &get_origin_allocator() LIBCOPP_MACRO_NOEXCEPT { return alloc_; }
  inline const allocator_type &get_origin_allocator() const LIBCOPP_MACRO_NOEXCEPT { return alloc_; }

  size_t set_stack_size(size_t sz) {
    if (sz <= LIBCOPP_COPP_NAMESPACE_ID::stack_traits::minimum_size()) {
      sz = LIBCOPP_COPP_NAMESPACE_ID::stack_traits::minimum_size();
    } else {
      sz = LIBCOPP_COPP_NAMESPACE_ID::stack_traits::round_to_page_size(sz);
    }

    if (sz != conf_.stack_size) {
      clear();
    }

    return conf_.stack_size = sz;
  }
  size_t get_stack_size() const { return conf_.stack_size; }
  size_t get_stack_size_offset() const { return conf_.stack_offset; }

  inline void set_max_stack_size(size_t sz) LIBCOPP_MACRO_NOEXCEPT { conf_.max_stack_size = sz; }
  inline size_t get_max_stack_size() const LIBCOPP_MACRO_NOEXCEPT { return conf_.max_stack_size; }
  inline void set_max_stack_number(size_t sz) LIBCOPP_MACRO_NOEXCEPT { conf_.max_stack_number = sz; }
  inline size_t get_max_stack_number() const LIBCOPP_MACRO_NOEXCEPT { return conf_.max_stack_number; }

  inline void set_min_stack_size(size_t sz) LIBCOPP_MACRO_NOEXCEPT { conf_.min_stack_size = sz; }
  inline size_t get_min_stack_size() const LIBCOPP_MACRO_NOEXCEPT { return conf_.min_stack_size; }
  inline void set_min_stack_number(size_t sz) LIBCOPP_MACRO_NOEXCEPT { conf_.min_stack_number = sz; }
  inline size_t get_min_stack_number() const LIBCOPP_MACRO_NOEXCEPT { return conf_.min_stack_number; }

  inline void set_auto_gc(bool v) LIBCOPP_MACRO_NOEXCEPT { conf_.auto_gc = v; }
  inline bool is_auto_gc() const LIBCOPP_MACRO_NOEXCEPT { return conf_.auto_gc; }

  inline void set_gc_once_number(size_t v) LIBCOPP_MACRO_NOEXCEPT { conf_.gc_number = v; }
  inline size_t get_gc_once_number() const LIBCOPP_MACRO_NOEXCEPT { return conf_.gc_number; }

  // actions

  /**
   * allocate memory and attach to stack context [standard function]
   * @param ctx stack context
   * @param size stack size
   * @note size must less or equal than attached
   */
  void allocate(stack_context &ctx) LIBCOPP_MACRO_NOEXCEPT {
#if !defined(LIBCOPP_DISABLE_ATOMIC_LOCK) || !(LIBCOPP_DISABLE_ATOMIC_LOCK)
    LIBCOPP_COPP_NAMESPACE_ID::util::lock::lock_holder<LIBCOPP_COPP_NAMESPACE_ID::util::lock::spin_lock> lock_guard(
        action_lock_);
#endif
    // check limit
    if (0 != conf_.max_stack_number && limits_.used_stack_number >= conf_.max_stack_number) {
      ctx.sp = nullptr;
      ctx.size = 0;
      return;
    }

    if (0 != conf_.max_stack_size && limits_.used_stack_size + conf_.stack_size > conf_.max_stack_size) {
      ctx.sp = nullptr;
      ctx.size = 0;
      return;
    }

    // get from pool, in order to max reuse cache, we use FILO to allocate stack
    if (!free_list_.empty()) {
      typename std::list<stack_context>::reverse_iterator iter = free_list_.rbegin();
      assert(iter != free_list_.rend());

      // free limit
      COPP_LIKELY_IF (limits_.free_stack_number > 0) {
        --limits_.free_stack_number;
      } else {
        limits_.free_stack_number = free_list_.size() - 1;
      }

      COPP_LIKELY_IF (limits_.free_stack_size >= (*iter).size) {
        limits_.free_stack_size -= (*iter).size;
      } else {
        limits_.free_stack_size = 0;
      }

      // make sure the stack must be greater or equal than configure after reset
      COPP_LIKELY_IF (iter->size >= conf_.stack_size) {
        ctx = *iter;
        free_list_.pop_back();

        // used limit
        ++limits_.used_stack_number;
        limits_.used_stack_size += ctx.size;
        return;
      } else {
        // just pop cache
        free_list_.pop_back();
      }
    }

    // get from origin allocator
    alloc_.allocate(ctx, conf_.stack_size);
    if (nullptr != ctx.sp && ctx.size > 0) {
      // used limit
      ++limits_.used_stack_number;
      limits_.used_stack_size += ctx.size;

      conf_.stack_offset = ctx.size - conf_.stack_size;
    }
  }

  /**
   * deallocate memory from stack context [standard function]
   * @param ctx stack context
   */
  void deallocate(stack_context &ctx) LIBCOPP_MACRO_NOEXCEPT {
    assert(ctx.sp && ctx.size > 0);
    do {
#if !defined(LIBCOPP_DISABLE_ATOMIC_LOCK) || !(LIBCOPP_DISABLE_ATOMIC_LOCK)
      LIBCOPP_COPP_NAMESPACE_ID::util::lock::lock_holder<LIBCOPP_COPP_NAMESPACE_ID::util::lock::spin_lock> lock_guard(
          action_lock_);
#endif
      // check ctx
      if (ctx.sp == nullptr || 0 == ctx.size) {
        break;
      }

      // limits
      COPP_LIKELY_IF (limits_.used_stack_size >= ctx.size) {
        limits_.used_stack_size -= ctx.size;
      } else {
        limits_.used_stack_size = 0;
      }

      COPP_LIKELY_IF (limits_.used_stack_number > 0) {
        --limits_.used_stack_number;
      }

      // check size
      if (ctx.size != conf_.stack_size + conf_.stack_offset) {
        alloc_.deallocate(ctx);
        break;
      }

      // push to free list
      free_list_.push_back(ctx);

      // limits
      ++limits_.free_stack_number;
      limits_.free_stack_size += ctx.size;
    } while (false);

    // check GC
    if (conf_.auto_gc) {
      gc();
    }
  }

  size_t gc() {
    size_t ret = 0;
    // gc only if free stacks is greater than used
    if (limits_.used_stack_size >= limits_.free_stack_size && limits_.used_stack_number >= limits_.free_stack_number) {
      return ret;
    }

    // gc when stack is too large
    if (0 != conf_.min_stack_size || 0 != conf_.min_stack_number) {
      bool min_stack_size =
          conf_.min_stack_size == 0 || limits_.used_stack_size + limits_.free_stack_size <= conf_.min_stack_size;
      bool min_stack_number = conf_.min_stack_number == 0 ||
                              limits_.free_stack_number + limits_.used_stack_number <= conf_.min_stack_number;
      if (min_stack_size && min_stack_number) {
        return ret;
      }
    }

#if !defined(LIBCOPP_DISABLE_ATOMIC_LOCK) || !(LIBCOPP_DISABLE_ATOMIC_LOCK)
    LIBCOPP_COPP_NAMESPACE_ID::util::lock::lock_holder<LIBCOPP_COPP_NAMESPACE_ID::util::lock::spin_lock> lock_guard(
        action_lock_);
#endif

    size_t keep_size = limits_.free_stack_size >> 1;
    size_t keep_number = limits_.free_stack_number >> 1;
    size_t left_gc = conf_.gc_number;
    while (limits_.free_stack_size > keep_size || limits_.free_stack_number > keep_number) {
      if (free_list_.empty()) {
        limits_.free_stack_size = 0;
        limits_.free_stack_number = 0;
        break;
      }

      typename std::list<stack_context>::iterator iter = free_list_.begin();
      assert(iter != free_list_.end());

      COPP_LIKELY_IF (limits_.free_stack_number > 0) {
        --limits_.free_stack_number;
      } else {
        limits_.free_stack_number = free_list_.size() - 1;
      }

      COPP_LIKELY_IF (limits_.free_stack_size >= (*iter).size) {
        limits_.free_stack_size -= (*iter).size;
      } else {
        limits_.free_stack_size = 0;
      }

      alloc_.deallocate(*iter);
      free_list_.pop_front();
      ++ret;

      // gc max stacks once
      if (0 != left_gc) {
        --left_gc;
        if (0 == left_gc) {
          break;
        }
      }
    }

    LIBCOPP_UTIL_LOCK_ATOMIC_THREAD_FENCE(LIBCOPP_COPP_NAMESPACE_ID::util::lock::memory_order_release);

    return ret;
  }

  void clear() {
#if !defined(LIBCOPP_DISABLE_ATOMIC_LOCK) || !(LIBCOPP_DISABLE_ATOMIC_LOCK)
    LIBCOPP_COPP_NAMESPACE_ID::util::lock::lock_holder<LIBCOPP_COPP_NAMESPACE_ID::util::lock::spin_lock> lock_guard(
        action_lock_);
#endif

    limits_.free_stack_size = 0;
    limits_.free_stack_number = 0;

    for (typename std::list<stack_context>::iterator iter = free_list_.begin(); iter != free_list_.end(); ++iter) {
      alloc_.deallocate(*iter);
    }

    LIBCOPP_UTIL_LOCK_ATOMIC_THREAD_FENCE(LIBCOPP_COPP_NAMESPACE_ID::util::lock::memory_order_release);
  }

 private:
  limit_t limits_;
  configure_t conf_;
  allocator_type alloc_;
#if !defined(LIBCOPP_DISABLE_ATOMIC_LOCK) || !(LIBCOPP_DISABLE_ATOMIC_LOCK)
  LIBCOPP_COPP_NAMESPACE_ID::util::lock::spin_lock action_lock_;
#endif
  std::list<stack_context> free_list_;
};
LIBCOPP_COPP_NAMESPACE_END

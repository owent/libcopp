// Copyright 2025 owent
// Created by owent on 2025-03-03

#pragma once

#include <memory>

#include "libcopp/utils/config/compile_optimize.h"
#include "libcopp/utils/config/libcopp_build_features.h"
#include "libcopp/utils/nostd/type_traits.h"

LIBCOPP_COPP_NAMESPACE_BEGIN
namespace memory {

/// Non-standard RAII type for managing pointers obtained from allocators.
template <class Alloc>
struct LIBCOPP_UTIL_SYMBOL_VISIBLE allocated_ptr {
  using pointer = typename ::std::allocator_traits<Alloc>::pointer;
  using value_type = typename ::std::allocator_traits<Alloc>::value_type;

  /// Take ownership of p
  inline allocated_ptr(Alloc& a, pointer p) noexcept : alloc_(&a), ptr_(p) {}

  /// Convert __ptr to allocator's pointer type and take ownership of it
  template <class Ptr, class = nostd::enable_if_t<::std::is_same<Ptr, value_type*>::value>>
  inline allocated_ptr(Alloc& a, Ptr p) : alloc_(&a), ptr_(p) {}

  /// Transfer ownership of the owned pointer
  inline allocated_ptr(allocated_ptr&& gd) noexcept : alloc_(gd.alloc_), ptr_(gd.ptr_) { gd.ptr_ = nullptr; }

  /// Deallocate the owned pointer
  inline ~allocated_ptr() {
    if (ptr_ != nullptr) {
      ::std::allocator_traits<Alloc>::deallocate(*alloc_, ptr_, 1);
    }
  }

  /// Release ownership of the owned pointer
  inline allocated_ptr& operator=(std::nullptr_t) noexcept {
    ptr_ = nullptr;
    return *this;
  }

  /// Get the address that the owned pointer refers to.
  inline value_type* get() noexcept { return ptr_; }

 private:
  Alloc* alloc_;
  pointer ptr_;
};

/// Allocate space for a single object using a
template <class Alloc>
LIBCOPP_UTIL_SYMBOL_VISIBLE allocated_ptr<Alloc> allocate_guarded(Alloc& a) {
  return {a, ::std::allocator_traits<Alloc>::allocate(a, 1)};
}

}  // namespace memory
LIBCOPP_COPP_NAMESPACE_END

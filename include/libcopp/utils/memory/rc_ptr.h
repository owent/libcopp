// Copyright 2024 atframework
// Licenses under the MIT License
// @note This is a smart pointer class that is compatible with std::shared_ptr, but it is more lightweight and do not
//       use atomic operation for reference counting. It is designed for single thread usage.
// @note We support all APIs of std::shared_ptr in C++14, and partly APIs of std::shared_ptr in C++17/20/26.

#pragma once

#include <cstdint>
#include <functional>
#include <memory>
#include <ostream>
#include <type_traits>
#include <utility>

#ifdef __cpp_impl_three_way_comparison
#  include <compare>
#endif

#include "libcopp/utils/config/compile_optimize.h"
#include "libcopp/utils/config/libcopp_build_features.h"
#include "libcopp/utils/memory/allocator_ptr.h"
#include "libcopp/utils/nostd/type_traits.h"

LIBCOPP_COPP_NAMESPACE_BEGIN
namespace memory {

template <class T>
class LIBCOPP_COPP_API_HEAD_ONLY weak_rc_ptr;

template <class T>
class LIBCOPP_COPP_API_HEAD_ONLY strong_rc_ptr;

template <class T>
class LIBCOPP_COPP_API_HEAD_ONLY enable_shared_rc_from_this;

class LIBCOPP_UTIL_SYMBOL_VISIBLE __rc_ptr_counted_data_base {
 public:
  constexpr __rc_ptr_counted_data_base() noexcept : use_count_(1), weak_count_(1) {}

  LIBCOPP_COPP_API virtual ~__rc_ptr_counted_data_base() noexcept;

  // Called when use_count_ drops to zero, to release the resources
  // managed by *this.
  virtual void dispose() noexcept = 0;

  // Called when weak_count_ drops to zero.
  virtual void destroy() noexcept = 0;

  // Donohting when with -fno-exception/EHsc-
  LIBCOPP_COPP_API static void throw_bad_weak_ptr();

  // Increment the use count if it is non-zero, throw otherwise.
  LIBCOPP_UTIL_FORCEINLINE void add_ref() {
    if (!add_ref_nothrow()) {
      throw_bad_weak_ptr();
    }
  }

  // Increment the use count if it is non-zero.
  LIBCOPP_UTIL_FORCEINLINE bool add_ref_nothrow() noexcept {
    if (use_count_ == 0) {
      return false;
    }

    ++use_count_;
    return true;
  }

  // Decrement the use count.
  LIBCOPP_UTIL_FORCEINLINE void release() noexcept {
    if (--use_count_ == 0) {
      dispose();
      if (--weak_count_ == 0) {
        destroy();
      }
    }
  }

  // Increment the weak count.
  LIBCOPP_UTIL_FORCEINLINE void weak_add_ref() noexcept { ++weak_count_; }

  // Decrement the weak count.
  LIBCOPP_UTIL_FORCEINLINE void weak_release() noexcept {
    if (--weak_count_ == 0) {
      destroy();
    }
  }

  LIBCOPP_UTIL_FORCEINLINE std::size_t use_count() const noexcept { return use_count_; }

 private:
  __rc_ptr_counted_data_base(const __rc_ptr_counted_data_base&) = delete;
  __rc_ptr_counted_data_base& operator=(const __rc_ptr_counted_data_base&) = delete;

 private:
  std::size_t use_count_;
  std::size_t weak_count_;
};

/**
 * @brief Template class definition for reference-counted.
 * @note Construct object and counter with default data management and allocator.
 */
template <class T>
class LIBCOPP_COPP_API_HEAD_ONLY __rc_ptr_counted_data_default final : public __rc_ptr_counted_data_base {
 public:
  explicit __rc_ptr_counted_data_default(T* p) noexcept : ptr_(p) {}

  void dispose() noexcept override {
    using alloc_type = ::std::allocator<nostd::remove_cv_t<T>>;
    using alloc_traits = ::std::allocator_traits<alloc_type>;
    alloc_type alloc;
    if (nullptr != ptr_) {
      alloc_traits::destroy(alloc, const_cast<nostd::remove_cv_t<T>*>(ptr_));
      alloc_traits::deallocate(alloc, const_cast<nostd::remove_cv_t<T>*>(ptr_), 1);
      ptr_ = nullptr;
    }
  }

  void destroy() noexcept override {
    using alloc_type = ::std::allocator<__rc_ptr_counted_data_default<T>>;
    alloc_type alloc;
    allocated_ptr<alloc_type> guard_ptr{alloc, this};
    this->~__rc_ptr_counted_data_default();
  }

 private:
  T* ptr_;
};

/**
 * @brief Template class definition for reference-counted.(inplacement)
 * @note Construct object and counter inplace with default allocator.
 *       This will reduce memory fragmentation and slightly improve performance.
 */
template <class T>
class LIBCOPP_COPP_API_HEAD_ONLY __rc_ptr_counted_data_inplace final : public __rc_ptr_counted_data_base {
 public:
  template <class... Args>
  explicit __rc_ptr_counted_data_inplace(Args&&... args) {
    using alloc_type = ::std::allocator<nostd::remove_cv_t<T>>;
    using alloc_traits = ::std::allocator_traits<alloc_type>;
    alloc_type alloc;
    alloc_traits::construct(alloc, const_cast<nostd::remove_cv_t<T>*>(ptr()), std::forward<Args>(args)...);
  }

  void dispose() noexcept override {
    using alloc_type = ::std::allocator<nostd::remove_cv_t<T>>;
    using alloc_traits = ::std::allocator_traits<alloc_type>;
    alloc_type alloc;
    alloc_traits::destroy(alloc, const_cast<nostd::remove_cv_t<T>*>(ptr()));
  }

  void destroy() noexcept override {
    using alloc_type = ::std::allocator<__rc_ptr_counted_data_inplace<T>>;
    using alloc_traits = ::std::allocator_traits<alloc_type>;
    alloc_type alloc;
    allocated_ptr<alloc_type> guard_ptr{alloc, this};
    alloc_traits::destroy(alloc, this);
  }

  inline T* ptr() noexcept { return reinterpret_cast<T*>(addr()); }

 private:
  inline void* addr() { return reinterpret_cast<void*>(&storage_); }

  nostd::aligned_storage_t<sizeof(T), alignof(T)> storage_;
};

/**
 * @brief Template class definition for reference-counted with allocator.(inplacement)
 * @note Construct object and counter inplace with custom allocator.
 *       This will reduce memory fragmentation and slightly improve performance.
 * @note We use allocator rebind to allocate all data with custom allocator.
 */
template <class T, class Alloc>
class LIBCOPP_COPP_API_HEAD_ONLY __rc_ptr_counted_data_inplace_alloc final : public __rc_ptr_counted_data_base {
 public:
  template <class AllocInput, class... Args>
  explicit __rc_ptr_counted_data_inplace_alloc(AllocInput&& a, Args&&... args) {
    // construct allocator first
    using alloc_type_a = typename ::std::allocator_traits<Alloc>::template rebind_alloc<Alloc>;
    using alloc_traits_a = ::std::allocator_traits<alloc_type_a>;
    alloc_type_a aa;
    alloc_traits_a::construct(aa, alloc_ptr(), std::forward<AllocInput>(a));

// and then value
#if defined(LIBCOPP_MACRO_ENABLE_EXCEPTION) && LIBCOPP_MACRO_ENABLE_EXCEPTION
    try {
#endif
      using alloc_traits_v = ::std::allocator_traits<Alloc>;
      alloc_traits_v::construct(*alloc_ptr(), const_cast<nostd::remove_cv_t<T>*>(value_ptr()),
                                std::forward<Args>(args)...);
#if defined(LIBCOPP_MACRO_ENABLE_EXCEPTION) && LIBCOPP_MACRO_ENABLE_EXCEPTION
    } catch (...) {
      alloc_traits_a::destroy(aa, alloc_ptr());
      throw;
    }
#endif
  }

  void dispose() noexcept override {
    using alloc_traits_v = ::std::allocator_traits<Alloc>;
    alloc_traits_v::destroy(*alloc_ptr(), const_cast<nostd::remove_cv_t<T>*>(value_ptr()));
  }

  void destroy() noexcept override {
    using alloc_type_self =
        typename ::std::allocator_traits<Alloc>::template rebind_alloc<__rc_ptr_counted_data_inplace_alloc<T, Alloc>>;
    using alloc_traits_self = ::std::allocator_traits<alloc_type_self>;

#if defined(__GNUC__) && !defined(__clang__) && !defined(__apple_build_version__)
#  if (__GNUC__ * 100 + __GNUC_MINOR__ * 10) >= 460
#    pragma GCC diagnostic push
#  endif
#  pragma GCC diagnostic ignored "-Wuninitialized"
#elif defined(__clang__) || defined(__apple_build_version__)
#  pragma clang diagnostic push
#  pragma clang diagnostic ignored "-Wuninitialized"
#endif

    // destroy allocator first
    using alloc_type_a = typename ::std::allocator_traits<Alloc>::template rebind_alloc<Alloc>;
    using alloc_traits_a = ::std::allocator_traits<alloc_type_a>;
    LIBCOPP_UTIL_ATTRIBUTE_UNINITIALIZED alloc_type_a aa{*alloc_ptr()};
    alloc_traits_a::destroy(aa, alloc_ptr());

    // then, destroy and deallocate this
    LIBCOPP_UTIL_ATTRIBUTE_UNINITIALIZED alloc_type_self as{*alloc_ptr()};
    allocated_ptr<alloc_type_self> guard_ptr{as, this};
    alloc_traits_self::destroy(as, this);

#if defined(__GNUC__) && !defined(__clang__) && !defined(__apple_build_version__)
#  if (__GNUC__ * 100 + __GNUC_MINOR__ * 10) >= 460
#    pragma GCC diagnostic pop
#  endif
#elif defined(__clang__) || defined(__apple_build_version__)
#  pragma clang diagnostic pop
#endif
  }

  inline T* value_ptr() noexcept { return reinterpret_cast<T*>(value_addr()); }
  inline Alloc* alloc_ptr() noexcept { return reinterpret_cast<Alloc*>(alloc_addr()); }

 private:
  inline void* value_addr() { return reinterpret_cast<void*>(&storage_); }
  inline void* alloc_addr() { return reinterpret_cast<void*>(&alloc_); }

  nostd::aligned_storage_t<sizeof(T), alignof(T)> storage_;
  nostd::aligned_storage_t<sizeof(Alloc), alignof(Alloc)> alloc_;
};

/**
 * @brief Template class definition for reference-counted with deletor.
 * @note Construct object and counter with custom deletor and default allocator.
 */
template <class T, class Deleter>
class LIBCOPP_COPP_API_HEAD_ONLY __rc_ptr_counted_data_with_deleter final : public __rc_ptr_counted_data_base {
 public:
  template <class DeleterInput>
  inline __rc_ptr_counted_data_with_deleter(T* p, DeleterInput&& d) noexcept
      : ptr_(p), deleter_(std::forward<DeleterInput>(d)) {}

  void dispose() noexcept override { deleter_(ptr_); }

  void destroy() noexcept override {
    using alloc_type = ::std::allocator<__rc_ptr_counted_data_with_deleter<T, Deleter>>;
    using alloc_traits = ::std::allocator_traits<alloc_type>;
    alloc_type alloc;

    // deallocate this after function finished
    allocated_ptr<alloc_type> guard_ptr{alloc, this};
    alloc_traits::destroy(alloc, this);
  }

 private:
  T* ptr_;
  Deleter deleter_;
};

/**
 * @brief Template class definition for reference-counted with deletor and allocator.
 * @note Construct object and counter with custom deletor and custom allocator.
 */
template <class T, class Deleter, class Alloc>
class LIBCOPP_COPP_API_HEAD_ONLY __rc_ptr_counted_data_with_deleter_allocator final
    : public __rc_ptr_counted_data_base {
 public:
  template <class DeleterInput, class AllocInput>
  inline __rc_ptr_counted_data_with_deleter_allocator(T* p, DeleterInput&& d, AllocInput&& a) noexcept
      : ptr_(p), deleter_(std::forward<DeleterInput>(d)), alloc_(std::forward<AllocInput>(a)) {}

  void dispose() noexcept override { deleter_(ptr_); }

  void destroy() noexcept override {
    using alloc_type = typename ::std::allocator_traits<Alloc>::template rebind_alloc<
        __rc_ptr_counted_data_with_deleter_allocator<T, Deleter, Alloc>>;
    using alloc_traits = ::std::allocator_traits<alloc_type>;
    alloc_type alloc{alloc_};

    // deallocate this after function finished
    allocated_ptr<alloc_type> guard_ptr{alloc, this};
    alloc_traits::destroy(alloc, this);
  }

 private:
  T* ptr_;
  Deleter deleter_;
  Alloc alloc_;
};

template <class T>
struct LIBCOPP_COPP_API_HEAD_ONLY __strong_rc_default_alloc_shared_tag{};

template <class T>
struct LIBCOPP_COPP_API_HEAD_ONLY __strong_rc_with_alloc_shared_tag{};

template <class T>
class LIBCOPP_COPP_API_HEAD_ONLY __weak_rc_counter;

template <class T>
class LIBCOPP_COPP_API_HEAD_ONLY __strong_rc_counter {
  // Prevent __strong_rc_default_alloc_shared_tag and __strong_rc_with_alloc_shared_tag from matching the shared_ptr(P,
  // D) ctor.
  template <class>
  struct __not_alloc_shared_tag {
    using type = void;
  };

  template <class Y>
  struct __not_alloc_shared_tag<__strong_rc_default_alloc_shared_tag<Y>> {};

  template <class Y>
  struct __not_alloc_shared_tag<__strong_rc_with_alloc_shared_tag<Y>> {};

 public:
  constexpr __strong_rc_counter() noexcept : pi_(nullptr) {}

  template <class Y>
  explicit __strong_rc_counter(Y* p) : pi_(nullptr) {
    using alloc_type = ::std::allocator<__rc_ptr_counted_data_default<Y>>;
    using alloc_traits = ::std::allocator_traits<alloc_type>;
    alloc_type alloc;
    auto guard = allocate_guarded(alloc);
#if defined(LIBCOPP_MACRO_ENABLE_EXCEPTION) && LIBCOPP_MACRO_ENABLE_EXCEPTION
    try {
#endif
      alloc_traits::construct(alloc, guard.get(), p);
      pi_ = guard.get();
      guard = nullptr;

#if defined(LIBCOPP_MACRO_ENABLE_EXCEPTION) && LIBCOPP_MACRO_ENABLE_EXCEPTION
    } catch (...) {
      using alloc_type_y = ::std::allocator<nostd::remove_cv_t<Y>>;
      using alloc_traits_y = ::std::allocator_traits<alloc_type_y>;
      alloc_type_y alloc_y;

      if (nullptr != p) {
        alloc_traits_y::destroy(alloc_y, p);
        alloc_traits_y::deallocate(alloc_y, const_cast<nostd::remove_cv_t<Y>*>(p), 1);
      }
      throw;
    }
#endif
  }

  template <class Y, class Deleter, class = typename __not_alloc_shared_tag<nostd::remove_cvref_t<Deleter>>::type>
  __strong_rc_counter(Y* p, Deleter&& d) : pi_(nullptr) {
    using alloc_type = ::std::allocator<__rc_ptr_counted_data_with_deleter<Y, nostd::remove_cvref_t<Deleter>>>;
    using alloc_traits = ::std::allocator_traits<alloc_type>;
    alloc_type alloc;
    auto guard = allocate_guarded(alloc);
#if defined(LIBCOPP_MACRO_ENABLE_EXCEPTION) && LIBCOPP_MACRO_ENABLE_EXCEPTION
    try {
#endif
      alloc_traits::construct(alloc, guard.get(), p, std::forward<Deleter>(d));
      pi_ = guard.get();
      guard = nullptr;
#if defined(LIBCOPP_MACRO_ENABLE_EXCEPTION) && LIBCOPP_MACRO_ENABLE_EXCEPTION
    } catch (...) {
      d(p);
      throw;
    }
#endif
  }

  template <class Y, class Deleter, class Alloc,
            class = typename __not_alloc_shared_tag<nostd::remove_cvref_t<Deleter>>::type>
  __strong_rc_counter(Y* p, Deleter&& d, Alloc&& a) : pi_(nullptr) {
    using origin_alloc_traits = ::std::allocator_traits<nostd::remove_cvref_t<Alloc>>;
    using alloc_type = typename origin_alloc_traits::template rebind_alloc<
        __rc_ptr_counted_data_with_deleter_allocator<Y, nostd::remove_cvref_t<Deleter>, nostd::remove_cvref_t<Alloc>>>;
    using alloc_traits = ::std::allocator_traits<alloc_type>;
    alloc_type alloc{a};
    auto guard = allocate_guarded(alloc);
#if defined(LIBCOPP_MACRO_ENABLE_EXCEPTION) && LIBCOPP_MACRO_ENABLE_EXCEPTION
    try {
#endif
      alloc_traits::construct(alloc, guard.get(), p, std::forward<Deleter>(d), std::forward<Alloc>(a));
      pi_ = guard.get();
      guard = nullptr;
#if defined(LIBCOPP_MACRO_ENABLE_EXCEPTION) && LIBCOPP_MACRO_ENABLE_EXCEPTION
    } catch (...) {
      d(p);
      throw;
    }
#endif
  }

  template <class... Args>
  __strong_rc_counter(T*& __p, __strong_rc_default_alloc_shared_tag<T>, Args&&... args) : pi_(nullptr) {
    using alloc_type = ::std::allocator<__rc_ptr_counted_data_inplace<T>>;
    using alloc_traits = ::std::allocator_traits<alloc_type>;
    alloc_type alloc;
    auto guard = allocate_guarded(alloc);

#if defined(LIBCOPP_MACRO_ENABLE_EXCEPTION) && LIBCOPP_MACRO_ENABLE_EXCEPTION
    try {
#endif
      alloc_traits::construct(alloc, guard.get(), std::forward<Args>(args)...);
      pi_ = guard.get();
      __p = guard.get()->ptr();
      guard = nullptr;
#if defined(LIBCOPP_MACRO_ENABLE_EXCEPTION) && LIBCOPP_MACRO_ENABLE_EXCEPTION
    } catch (...) {
      throw;
    }
#endif
  }

  template <class Alloc, class... Args>
  __strong_rc_counter(T*& __p, __strong_rc_with_alloc_shared_tag<T>, Alloc&& a, Args&&... args) : pi_(nullptr) {
    using origin_alloc_traits = ::std::allocator_traits<nostd::remove_cvref_t<Alloc>>;

    using alloc_type = typename origin_alloc_traits::template rebind_alloc<
        __rc_ptr_counted_data_inplace_alloc<T, nostd::remove_cvref_t<Alloc>>>;
    using alloc_traits = ::std::allocator_traits<alloc_type>;
    alloc_type alloc{a};
    auto guard = allocate_guarded(alloc);

#if defined(LIBCOPP_MACRO_ENABLE_EXCEPTION) && LIBCOPP_MACRO_ENABLE_EXCEPTION
    try {
#endif
      alloc_traits::construct(alloc, guard.get(), std::forward<Alloc>(a), std::forward<Args>(args)...);
      pi_ = guard.get();
      __p = guard.get()->value_ptr();
      guard = nullptr;
#if defined(LIBCOPP_MACRO_ENABLE_EXCEPTION) && LIBCOPP_MACRO_ENABLE_EXCEPTION
    } catch (...) {
      throw;
    }
#endif
  }

  template <class UT, class UDeleter>
  explicit __strong_rc_counter(std::unique_ptr<UT, UDeleter>&& r) : pi_(nullptr) {
    if (r.get() == nullptr) {
      return;
    }

    using alloc_type = ::std::allocator<__rc_ptr_counted_data_with_deleter<UT, nostd::remove_cvref_t<UDeleter>>>;
    using alloc_traits = ::std::allocator_traits<alloc_type>;
    alloc_type alloc;
    auto guard = allocate_guarded(alloc);

#if defined(LIBCOPP_MACRO_ENABLE_EXCEPTION) && LIBCOPP_MACRO_ENABLE_EXCEPTION
    try {
#endif
      alloc_traits::construct(alloc, guard.get(), r.get(), std::forward<UDeleter>(r.get_deleter()));
      pi_ = guard.get();
      guard = nullptr;

      r.release();
#if defined(LIBCOPP_MACRO_ENABLE_EXCEPTION) && LIBCOPP_MACRO_ENABLE_EXCEPTION
    } catch (...) {
      throw;
    }
#endif
  }

  explicit __strong_rc_counter(const __weak_rc_counter<T>& w);

  __strong_rc_counter(const __weak_rc_counter<T>& w, std::nothrow_t) noexcept;

  ~__strong_rc_counter() {
    if (nullptr != pi_) {
      pi_->release();
    }
  }

  __strong_rc_counter(const __strong_rc_counter& r) noexcept : pi_(r.pi_) {
    if (nullptr != pi_) {
      pi_->add_ref();
    }
  }

  __strong_rc_counter(__strong_rc_counter&& r) noexcept : pi_(r.pi_) { r.pi_ = nullptr; }

  template <class Y>
  __strong_rc_counter(const __strong_rc_counter<Y>& r) noexcept : pi_(r.pi_) {
    if (nullptr != pi_) {
      pi_->add_ref();
    }
  }

  template <class Y>
  __strong_rc_counter(__strong_rc_counter<Y>&& r) noexcept : pi_(r.pi_) {
    r.pi_ = nullptr;
  }

  __strong_rc_counter& operator=(const __strong_rc_counter& r) noexcept {
    if (pi_ != r.pi_) {
      __rc_ptr_counted_data_base* origin_pi = pi_;

      pi_ = r.pi_;
      if (nullptr != pi_) {
        pi_->add_ref();
      }
      if (nullptr != origin_pi) {
        origin_pi->release();
      }
    }
    return *this;
  }

  __strong_rc_counter& operator=(__strong_rc_counter&& r) noexcept {
    if (pi_ != r.pi_) {
      __rc_ptr_counted_data_base* origin_pi = pi_;
      pi_ = r.pi_;
      r.pi_ = nullptr;

      if (nullptr != origin_pi) {
        origin_pi->release();
      }
    }
    return *this;
  }

  inline void swap(__strong_rc_counter& r) noexcept {
    __rc_ptr_counted_data_base* tmp = pi_;
    pi_ = r.pi_;
    r.pi_ = tmp;
  }

  template <class Y>
  inline void swap(__strong_rc_counter<Y>& r) noexcept {
    __rc_ptr_counted_data_base* tmp = pi_;
    pi_ = r.pi_;
    r.pi_ = tmp;
  }

  inline std::size_t use_count() const noexcept { return (nullptr != pi_) ? pi_->use_count() : 0; }

  inline __rc_ptr_counted_data_base* ref_counter() const noexcept { return pi_; }

 private:
  template <class>
  friend class LIBCOPP_COPP_API_HEAD_ONLY __strong_rc_counter;

  __rc_ptr_counted_data_base* pi_;
};

template <class T>
class LIBCOPP_COPP_API_HEAD_ONLY __weak_rc_counter {
 public:
  constexpr __weak_rc_counter() noexcept : pi_(nullptr) {}

  template <class Y>
  explicit __weak_rc_counter(const __strong_rc_counter<Y>& other) noexcept : pi_(other.ref_counter()) {
    if (nullptr != pi_) {
      pi_->weak_add_ref();
    }
  }

  ~__weak_rc_counter() {
    if (nullptr != pi_) {
      pi_->weak_release();
    }
  }

  __weak_rc_counter(const __weak_rc_counter& r) noexcept : pi_(r.pi_) {
    if (nullptr != pi_) {
      pi_->weak_add_ref();
    }
  }

  __weak_rc_counter(__weak_rc_counter&& r) noexcept : pi_(r.pi_) { r.pi_ = nullptr; }

  template <class Y>
  __weak_rc_counter(const __weak_rc_counter<Y>& r) noexcept : pi_(r.pi_) {
    if (nullptr != pi_) {
      pi_->weak_add_ref();
    }
  }

  template <class Y>
  __weak_rc_counter(__weak_rc_counter<Y>&& r) noexcept : pi_(r.pi_) {
    r.pi_ = nullptr;
  }

  __weak_rc_counter& operator=(const __weak_rc_counter& r) noexcept {
    if (pi_ != r.pi_) {
      __rc_ptr_counted_data_base* origin_pi = pi_;
      pi_ = r.pi_;
      if (nullptr != pi_) {
        pi_->weak_add_ref();
      }

      if (nullptr != origin_pi) {
        origin_pi->weak_release();
      }
    }
    return *this;
  }

  __weak_rc_counter& operator=(__weak_rc_counter&& r) noexcept {
    if (pi_ != r.pi_) {
      __rc_ptr_counted_data_base* origin_pi = pi_;
      pi_ = r.pi_;
      r.pi_ = nullptr;

      if (nullptr != origin_pi) {
        origin_pi->weak_release();
      }
    }
    return *this;
  }

  inline void swap(__weak_rc_counter& r) noexcept {
    __rc_ptr_counted_data_base* tmp = pi_;
    pi_ = r.pi_;
    r.pi_ = tmp;
  }

  template <class Y>
  inline void swap(__weak_rc_counter<Y>& r) noexcept {
    __rc_ptr_counted_data_base* tmp = pi_;
    pi_ = r.pi_;
    r.pi_ = tmp;
  }

  inline std::size_t use_count() const noexcept { return (nullptr != pi_) ? pi_->use_count() : 0; }

  inline __rc_ptr_counted_data_base* ref_counter() const noexcept { return pi_; }

 private:
  template <class>
  friend class LIBCOPP_COPP_API_HEAD_ONLY __weak_rc_counter;

  __rc_ptr_counted_data_base* pi_;
};

template <class T>
__strong_rc_counter<T>::__strong_rc_counter(const __weak_rc_counter<T>& w) : pi_(w.ref_counter()) {
  if (nullptr == pi_ || !pi_->add_ref_nothrow()) {
    pi_ = nullptr;
    __rc_ptr_counted_data_base::throw_bad_weak_ptr();
  }
}

template <class T>
__strong_rc_counter<T>::__strong_rc_counter(const __weak_rc_counter<T>& w, std::nothrow_t) noexcept
    : pi_(w.ref_counter()) {
  if (nullptr != pi_ && !pi_->add_ref_nothrow()) {
    pi_ = nullptr;
  }
}

/**
 * @brief Base class to mantain all shared APIs.
 */
template <class T, bool = std::is_array<T>::value, bool = std::is_void<T>::value>
class LIBCOPP_COPP_API_HEAD_ONLY strong_rc_ptr_access {
 public:
  using element_type = T;

  inline element_type& operator*() const noexcept {
    element_type* ret = get();
#if defined(LIBCOPP_MACRO_ENABLE_EXCEPTION) && LIBCOPP_MACRO_ENABLE_EXCEPTION
    if (nullptr == ret) {
      __rc_ptr_counted_data_base::throw_bad_weak_ptr();
    }
#endif
    return *ret;
  }

  inline element_type* operator->() const noexcept {
    element_type* ret = get();
#if defined(LIBCOPP_MACRO_ENABLE_EXCEPTION) && LIBCOPP_MACRO_ENABLE_EXCEPTION
    if (nullptr == ret) {
      __rc_ptr_counted_data_base::throw_bad_weak_ptr();
    }
#endif
    return ret;
  }

 private:
  inline element_type* get() const noexcept { return static_cast<const strong_rc_ptr<T>*>(this)->get(); }
};

/**
 * @brief A helper class that used to access internal APIs for void type.
 */
template <class T>
class LIBCOPP_COPP_API_HEAD_ONLY strong_rc_ptr_access<T, false, true> {
 public:
  using element_type = T;

  inline element_type* operator->() const noexcept {
    element_type* ret = get();
#if defined(LIBCOPP_MACRO_ENABLE_EXCEPTION) && LIBCOPP_MACRO_ENABLE_EXCEPTION
    if (nullptr == ret) {
      __rc_ptr_counted_data_base::throw_bad_weak_ptr();
    }
#endif
    return ret;
  }

 private:
  inline element_type* get() const noexcept { return static_cast<const strong_rc_ptr<T>*>(this)->get(); }
};

template <class T>
class LIBCOPP_COPP_API_HEAD_ONLY strong_rc_ptr_access<T, true, false> {
 public:
  using element_type = nostd::remove_extent_t<T>;

  element_type& operator[](std::ptrdiff_t __i) const noexcept {
    element_type* ret = get();
#if defined(LIBCOPP_MACRO_ENABLE_EXCEPTION) && LIBCOPP_MACRO_ENABLE_EXCEPTION
    if (nullptr == ret) {
      __rc_ptr_counted_data_base::throw_bad_weak_ptr();
    }
#endif
    return ret[__i];
  }

 private:
  inline element_type* get() const noexcept { return static_cast<const strong_rc_ptr<T>*>(this)->get(); }
};

/**
 * @brief A helper class that used to inject datas for classes that inherit enable_shared_rc_from_this
 * @note We use the old way to implement this feature, because some old compiler has problems for implementation of the
 *       detection idiom.
 */
template <class T1, class T2, class T3>
LIBCOPP_COPP_API_HEAD_ONLY inline void __enable_shared_from_this_with(const __strong_rc_counter<T1>* __n,
                                                                      const T2* __py,
                                                                      const enable_shared_rc_from_this<T3>* __p) {
  if (nullptr != __p) {
    __p->__internal_weak_assign(const_cast<T2*>(__py), *__n);
  }
}

// All ptr shares the same lifetime.
template <class T1, class T2, class T3, size_t T3SIZE>
LIBCOPP_COPP_API_HEAD_ONLY inline void __enable_shared_from_this_with(
    const __strong_rc_counter<T1>* __n, const T2* __py, const enable_shared_rc_from_this<T3[T3SIZE]>* __p) {
  if (nullptr != __p) {
    for (auto& p : *__p) {
      p->__internal_weak_assign(const_cast<T2*>(__py), *__n);
    }
  }
}

#ifdef _MANAGED

// Avoid C4793, ... causes native code generation

struct __sp_any_pointer {
  template <class T>
  __sp_any_pointer(T*) {}  // NOLINT: runtime/explicit
};

LIBCOPP_COPP_API_HEAD_ONLY inline void __enable_shared_from_this_with(__sp_any_pointer, __sp_any_pointer,
                                                                      __sp_any_pointer) {}

#else  // _MANAGED

LIBCOPP_COPP_API_HEAD_ONLY inline void __enable_shared_from_this_with(...) {}

#endif  // _MANAGED

/**
 * @brief A std::shared_ptr replacement that is more lightweight and do not use atomic operation for reference counting.
 * @note This class is designed for single thread usage.
 */
template <class T>
class LIBCOPP_COPP_API_HEAD_ONLY strong_rc_ptr : public strong_rc_ptr_access<T> {
 public:
  using element_type = nostd::remove_extent_t<T>;
  using weak_type = weak_rc_ptr<T>;

  // Allow nostd::nullable<T> to be used as a strong_rc_ptr<T>
  using nullability_compatible_type = void;
  using absl_nullability_compatible = void;

 public:
  constexpr strong_rc_ptr() noexcept : ptr_(nullptr), ref_counter_() {}

  constexpr strong_rc_ptr(std::nullptr_t) noexcept : ptr_(nullptr), ref_counter_() {}

  template <class Y>
  strong_rc_ptr(Y* ptr) noexcept  // NOLINT: runtime/explicit
      : ptr_(ptr), ref_counter_(ptr) {
    static_assert(!std::is_void<Y>::value, "incomplete type");
    static_assert(sizeof(Y) > 0, "incomplete type");
    __enable_shared_from_this_with(&ref_counter_, ptr, ptr);
  }

  template <class Y, class Deleter>
  strong_rc_ptr(Y* ptr, Deleter d) : ptr_(ptr), ref_counter_(ptr, std::move(d)) {
    __enable_shared_from_this_with(&ref_counter_, ptr, ptr);
  }

  template <class Y, class Deleter, class Alloc>
  strong_rc_ptr(Y* ptr, Deleter d, Alloc a) : ptr_(ptr), ref_counter_(ptr, std::move(d), std::move(a)) {
    __enable_shared_from_this_with(&ref_counter_, ptr, ptr);
  }

  template <class Deleter>
  strong_rc_ptr(std::nullptr_t ptr, Deleter d) : ptr_(ptr), ref_counter_(ptr, std::move(d)) {}

  template <class Y, class Deleter, class Alloc>
  strong_rc_ptr(std::nullptr_t ptr, Deleter d, Alloc a) : ptr_(ptr), ref_counter_(ptr, std::move(d), std::move(a)) {}

  template <class Y>
  strong_rc_ptr(const strong_rc_ptr<Y>& other) noexcept : ptr_(other.ptr_), ref_counter_(other.ref_counter_) {}

  template <class Y>
  strong_rc_ptr(strong_rc_ptr<Y>&& other) noexcept : ptr_(other.ptr_), ref_counter_() {
    ref_counter_.swap(other.ref_counter_);
    other.ptr_ = nullptr;
  }

  template <class Y>
  strong_rc_ptr(const strong_rc_ptr<Y>& other, element_type* ptr) noexcept
      : ptr_(ptr), ref_counter_(other.ref_counter_) {}

  template <class Y>
  strong_rc_ptr(strong_rc_ptr<Y>&& other, element_type* ptr) noexcept : ptr_(ptr), ref_counter_() {
    ref_counter_.swap(other.ref_counter_);
    other.ptr_ = nullptr;
  }

  strong_rc_ptr(const weak_rc_ptr<T>& other)  // NOLINT: runtime/explicit
      : ptr_(nullptr), ref_counter_(other.ref_counter_) {
    ptr_ = other.ptr_;
  }

  strong_rc_ptr(const weak_rc_ptr<T>& other, std::nothrow_t) noexcept
      : ptr_(nullptr), ref_counter_(other.ref_counter_, std::nothrow) {
    ptr_ = other.use_count() > 0 ? other.ptr_ : nullptr;
  }

  template <class Y>
  strong_rc_ptr(const weak_rc_ptr<Y>& other)  // NOLINT: runtime/explicit
      : ptr_(nullptr), ref_counter_(other.ref_counter_) {
    ptr_ = other.ptr_;
  }

  template <class Y, class Deleter>
  strong_rc_ptr(std::unique_ptr<Y, Deleter>&& other)  // NOLINT: runtime/explicit
      : ptr_(other.get()), ref_counter_() {
    ref_counter_ = __strong_rc_counter<element_type>{std::move(other)};
    __enable_shared_from_this_with(&ref_counter_, ptr_, ptr_);
  }

  /**
   * @brief This is a special constructor for make_strong_rc/allocate_strong_rc.
   * @param __tag Tag for type decetion
   * @param args Arguments to construct the object.
   */
  template <class... Args>
  strong_rc_ptr(__strong_rc_default_alloc_shared_tag<T> __tag, Args&&... args)  // NOLINT: runtime/explicit
      : ptr_(nullptr), ref_counter_(ptr_, __tag, std::forward<Args>(args)...) {
    __enable_shared_from_this_with(&ref_counter_, ptr_, ptr_);
  }

  /**
   * @brief This is a special constructor for make_strong_rc/allocate_strong_rc.
   * @param __tag Tag for type decetion
   * @param args Arguments to construct the object.
   */
  template <class... Args>
  strong_rc_ptr(__strong_rc_with_alloc_shared_tag<T> __tag, Args&&... args)  // NOLINT: runtime/explicit
      : ptr_(nullptr), ref_counter_(ptr_, __tag, std::forward<Args>(args)...) {
    __enable_shared_from_this_with(&ref_counter_, ptr_, ptr_);
  }

  ~strong_rc_ptr() noexcept = default;
  explicit strong_rc_ptr(const strong_rc_ptr&) noexcept = default;
  strong_rc_ptr& operator=(const strong_rc_ptr&) noexcept = default;

  explicit strong_rc_ptr(strong_rc_ptr&& other) noexcept : ptr_(other.ptr_), ref_counter_() {
    ref_counter_.swap(other.ref_counter_);
    other.ptr_ = nullptr;
  }

  inline strong_rc_ptr& operator=(strong_rc_ptr&& other) noexcept {
    strong_rc_ptr{std::move(other)}.swap(*this);
    return *this;
  }

  template <class Y>
  inline strong_rc_ptr& operator=(const strong_rc_ptr<Y>& other) noexcept {
    ptr_ = other.ptr_;
    ref_counter_ = other.ref_counter_;
    return *this;
  }

  template <class Y>
  inline strong_rc_ptr& operator=(strong_rc_ptr<Y>&& other) noexcept {
    strong_rc_ptr{std::move(other)}.swap(*this);
    return *this;
  }

  template <class Y, class Deleter>
  inline strong_rc_ptr& operator=(std::unique_ptr<Y, Deleter>&& other) {
    strong_rc_ptr{std::move(other)}.swap(*this);
    return *this;
  }

  inline void reset() noexcept { strong_rc_ptr().swap(*this); }

  template <class Y>
  inline void reset(Y* ptr) noexcept {
    if (ptr_ == ptr && ptr != nullptr) {
      return;
    }

    strong_rc_ptr{ptr}.swap(*this);
  }

  template <class Y, class Deleter>
  inline void reset(Y* ptr, Deleter d) noexcept {
    strong_rc_ptr{ptr, std::move(d)}.swap(*this);
  }

  template <class Y, class Deleter, class Alloc>
  inline void reset(Y* ptr, Deleter d, Alloc a) noexcept {
    strong_rc_ptr{ptr, std::move(d), std::move(a)}.swap(*this);
  }

  inline void swap(strong_rc_ptr& other) noexcept {
    std::swap(ptr_, other.ptr_);
    ref_counter_.swap(other.ref_counter_);
  }

  inline element_type* get() const noexcept { return ptr_; }

  inline std::size_t use_count() const noexcept { return ref_counter_.use_count(); }

  inline bool unique() const noexcept { return ref_counter_.use_count() == 1; }

  inline explicit operator bool() const noexcept { return nullptr != get(); }

  template <class Y>
  inline bool owner_before(strong_rc_ptr<Y> const& r) const noexcept {
    return std::less<__rc_ptr_counted_data_base*>()(ref_counter_.ref_counter(), r.ref_counter_.ref_counter());
  }

  template <class Y>
  inline bool owner_before(weak_rc_ptr<Y> const& r) const noexcept {
    return std::less<__rc_ptr_counted_data_base*>()(ref_counter_.ref_counter(), r.ref_counter_.ref_counter());
  }

  template <class Y>
  inline bool owner_equal(strong_rc_ptr<Y> const& r) const noexcept {
    return ref_counter_.ref_counter() == r.ref_counter_.ref_counter();
  }

  template <class Y>
  inline bool owner_equal(weak_rc_ptr<Y> const& r) const noexcept {
    return ref_counter_.ref_counter() == r.ref_counter_.ref_counter();
  }

  std::size_t owner_hash() const noexcept {
    return std::hash<__rc_ptr_counted_data_base*>()(ref_counter_.ref_counter());
  }

 private:
  template <class>
  friend class LIBCOPP_COPP_API_HEAD_ONLY weak_rc_ptr;

  template <class>
  friend class LIBCOPP_COPP_API_HEAD_ONLY strong_rc_ptr;

 private:
  element_type* ptr_;
  __strong_rc_counter<element_type> ref_counter_;
};

template <class T1, class T2>
LIBCOPP_COPP_API_HEAD_ONLY inline bool operator==(const strong_rc_ptr<T1>& l, const strong_rc_ptr<T2>& r) noexcept {
  return l.get() == r.get();
}

template <class T1>
LIBCOPP_COPP_API_HEAD_ONLY inline bool operator==(const strong_rc_ptr<T1>& l, ::std::nullptr_t) noexcept {
  return !l;
}

template <class T1, class T2, bool = std::is_convertible<T1*, T2*>::value || std::is_convertible<T2*, T1*>::value>
struct __strong_rc_ptr_compare_common_type;

template <class T1, class T2>
struct __strong_rc_ptr_compare_common_type<T1, T2, true> {
  using left_type = T1*;
  using right_type = T2*;

  using common_type = nostd::common_type_t<left_type, right_type>;
};

template <class T1, class T2>
struct __strong_rc_ptr_compare_common_type<T1, T2, false> {
  using left_type = const void*;
  using right_type = const void*;

  using common_type = const void*;
};

// Use three way comparison if available
#ifdef __cpp_impl_three_way_comparison
template <class T1, class T2>
LIBCOPP_COPP_API_HEAD_ONLY inline std::strong_ordering operator<=>(const strong_rc_ptr<T1>& l,
                                                                   const strong_rc_ptr<T2>& r) noexcept {
  return reinterpret_cast<typename __strong_rc_ptr_compare_common_type<T1, T2>::left_type>(l.get()) <=>
         reinterpret_cast<typename __strong_rc_ptr_compare_common_type<T1, T2>::right_type>(r.get());
}

template <class T1>
LIBCOPP_COPP_API_HEAD_ONLY inline std::strong_ordering operator<=>(const strong_rc_ptr<T1>& l,
                                                                   ::std::nullptr_t) noexcept {
  return l.get() <=> static_cast<T1*>(nullptr);
}
#else

template <class T1>
LIBCOPP_COPP_API_HEAD_ONLY inline bool operator==(::std::nullptr_t, const strong_rc_ptr<T1>& r) noexcept {
  return !r;
}

template <class T1, class T2>
LIBCOPP_COPP_API_HEAD_ONLY inline bool operator!=(const strong_rc_ptr<T1>& l, const strong_rc_ptr<T2>& r) noexcept {
  return l.get() != r.get();
}

template <class T1>
LIBCOPP_COPP_API_HEAD_ONLY inline bool operator!=(const strong_rc_ptr<T1>& l, ::std::nullptr_t) noexcept {
  return l.get() != nullptr;
}

template <class T1>
LIBCOPP_COPP_API_HEAD_ONLY inline bool operator!=(::std::nullptr_t, const strong_rc_ptr<T1>& r) noexcept {
  return r.get() != nullptr;
}

template <class T1, class T2>
LIBCOPP_COPP_API_HEAD_ONLY inline bool operator<(const strong_rc_ptr<T1>& l, const strong_rc_ptr<T2>& r) noexcept {
  return std::less<typename __strong_rc_ptr_compare_common_type<
      typename strong_rc_ptr<T1>::element_type, typename strong_rc_ptr<T2>::element_type>::common_type>()(l.get(),
                                                                                                          r.get());
}

template <class T1>
LIBCOPP_COPP_API_HEAD_ONLY inline bool operator<(const strong_rc_ptr<T1>& l, ::std::nullptr_t) noexcept {
  return std::less<T1>()(l.get(), nullptr);
}

template <class T1>
LIBCOPP_COPP_API_HEAD_ONLY inline bool operator<(::std::nullptr_t, const strong_rc_ptr<T1>& r) noexcept {
  return std::less<T1>()(nullptr, r.get());
}

template <class T1, class T2>
LIBCOPP_COPP_API_HEAD_ONLY inline bool operator>(const strong_rc_ptr<T1>& l, const strong_rc_ptr<T2>& r) noexcept {
  return std::greater<typename __strong_rc_ptr_compare_common_type<
      typename strong_rc_ptr<T1>::element_type, typename strong_rc_ptr<T2>::element_type>::common_type>()(l.get(),
                                                                                                          r.get());
}

template <class T1>
LIBCOPP_COPP_API_HEAD_ONLY inline bool operator>(const strong_rc_ptr<T1>& l, ::std::nullptr_t) noexcept {
  return std::greater<T1>()(l.get(), nullptr);
}

template <class T1>
LIBCOPP_COPP_API_HEAD_ONLY inline bool operator>(::std::nullptr_t, const strong_rc_ptr<T1>& r) noexcept {
  return std::greater<T1>()(nullptr, r.get());
}

template <class T1, class T2>
LIBCOPP_COPP_API_HEAD_ONLY inline bool operator<=(const strong_rc_ptr<T1>& l, const strong_rc_ptr<T2>& r) noexcept {
  return !(r < l);
}

template <class T1>
LIBCOPP_COPP_API_HEAD_ONLY inline bool operator<=(const strong_rc_ptr<T1>& l, ::std::nullptr_t) noexcept {
  return !(nullptr < l);
}

template <class T1>
LIBCOPP_COPP_API_HEAD_ONLY inline bool operator<=(::std::nullptr_t, const strong_rc_ptr<T1>& r) noexcept {
  return !(r < nullptr);
}

template <class T1, class T2>
LIBCOPP_COPP_API_HEAD_ONLY inline bool operator>=(const strong_rc_ptr<T1>& l, const strong_rc_ptr<T2>& r) noexcept {
  return !(r > l);
}

template <class T1>
LIBCOPP_COPP_API_HEAD_ONLY inline bool operator>=(const strong_rc_ptr<T1>& l, ::std::nullptr_t) noexcept {
  return !(nullptr > l);
}

template <class T1>
LIBCOPP_COPP_API_HEAD_ONLY inline bool operator>=(::std::nullptr_t, const strong_rc_ptr<T1>& r) noexcept {
  return !(r > nullptr);
}
#endif

/**
 * @brief A std::weak_ptr replacement that is more lightweight and do not use atomic operation for reference counting.
 * @note This class is designed for single thread usage.
 */
template <typename T>
class LIBCOPP_COPP_API_HEAD_ONLY weak_rc_ptr {
 public:
  using element_type = nostd::remove_extent_t<T>;

 public:
  constexpr weak_rc_ptr() noexcept : ptr_(nullptr), ref_counter_() {}

  weak_rc_ptr(const weak_rc_ptr&) noexcept = default;  // NOLINT: runtime/explicit
  weak_rc_ptr& operator=(const weak_rc_ptr&) noexcept = default;
  ~weak_rc_ptr() = default;

  weak_rc_ptr(weak_rc_ptr&& other) noexcept  // NOLINT: runtime/explicit
      : ptr_(other.ptr_), ref_counter_(std::move(other.ref_counter_)) {
    other.ptr_ = nullptr;
  }

  weak_rc_ptr& operator=(weak_rc_ptr&& other) noexcept {
    weak_rc_ptr{std::move(other)}.swap(*this);
    return *this;
  }

  template <class Y>
  weak_rc_ptr(const strong_rc_ptr<Y>& other) noexcept  // NOLINT: runtime/explicit
      : ptr_(other.ptr_), ref_counter_(other.ref_counter_) {}

  template <class Y>
  weak_rc_ptr(const weak_rc_ptr<Y>& other) noexcept : ref_counter_(other.ref_counter_) {
    ptr_ = other.lock().get();
  }

  template <class Y>
  weak_rc_ptr(weak_rc_ptr<Y>&& other) noexcept : ptr_(other.lock().get()), ref_counter_(std::move(other.ref_counter_)) {
    other.ptr_ = nullptr;
  }

  template <class Y>
  weak_rc_ptr& operator=(const weak_rc_ptr<Y>& other) noexcept {
    ptr_ = other.lock().get();
    ref_counter_ = other.ref_counter_;
    return *this;
  }

  template <class Y>
  weak_rc_ptr& operator=(weak_rc_ptr<Y>&& other) noexcept {
    weak_rc_ptr{std::move(other)}.swap(*this);
    return *this;
  }

  template <class Y>
  weak_rc_ptr& operator=(const strong_rc_ptr<Y>& other) noexcept {
    ptr_ = other.ptr_;
    __weak_rc_counter<T>{other.ref_counter_}.swap(ref_counter_);
    return *this;
  }

  inline void reset() noexcept { weak_rc_ptr{}.swap(*this); }

  inline void swap(weak_rc_ptr& r) noexcept {
    std::swap(ptr_, r.ptr_);
    ref_counter_.swap(r.ref_counter_);
  }

  inline std::size_t use_count() const noexcept { return ref_counter_.use_count(); }

  inline bool expired() const noexcept { return ref_counter_.use_count() == 0; }

  inline strong_rc_ptr<T> lock() const noexcept { return strong_rc_ptr<T>(*this, std::nothrow); }

  template <class Y>
  inline bool owner_before(strong_rc_ptr<Y> const& r) const noexcept {
    return std::less<__rc_ptr_counted_data_base*>()(ref_counter_.ref_counter(), r.ref_counter_.ref_counter());
  }

  template <class Y>
  inline bool owner_before(weak_rc_ptr<Y> const& r) const noexcept {
    return std::less<__rc_ptr_counted_data_base*>()(ref_counter_.ref_counter(), r.ref_counter_.ref_counter());
  }

  template <class Y>
  inline bool owner_equal(strong_rc_ptr<Y> const& r) const noexcept {
    return ref_counter_.ref_counter() == r.ref_counter_.ref_counter();
  }

  template <class Y>
  inline bool owner_equal(weak_rc_ptr<Y> const& r) const noexcept {
    return ref_counter_.ref_counter() == r.ref_counter_.ref_counter();
  }

  std::size_t owner_hash() const noexcept {
    return std::hash<__rc_ptr_counted_data_base*>()(ref_counter_.ref_counter());
  }

 private:
  template <class>
  friend class LIBCOPP_COPP_API_HEAD_ONLY enable_shared_rc_from_this;

  // Used by enable_shared_rc_from_this.
  void assign(element_type* __ptr, const __strong_rc_counter<T>& __refcount) noexcept {
    if (use_count() == 0) {
      ptr_ = __ptr;
      ref_counter_ = __weak_rc_counter<T>{__refcount};
    }
  }

  template <class>
  friend class LIBCOPP_COPP_API_HEAD_ONLY weak_rc_ptr;

  template <class>
  friend class LIBCOPP_COPP_API_HEAD_ONLY strong_rc_ptr;

 private:
  element_type* ptr_;
  __weak_rc_counter<T> ref_counter_;
};

template <class T>
class enable_shared_rc_from_this {
 public:
  strong_rc_ptr<T> shared_from_this() {
    strong_rc_ptr<T> result = weak_this_.lock();
#if defined(LIBCOPP_MACRO_ENABLE_EXCEPTION) && LIBCOPP_MACRO_ENABLE_EXCEPTION
    if (this != result.get()) {
      __rc_ptr_counted_data_base::throw_bad_weak_ptr();
    }
#endif
    // NRVO
    return result;
  }

  strong_rc_ptr<const T> shared_from_this() const {
    strong_rc_ptr<T> result = weak_this_.lock();
#if defined(LIBCOPP_MACRO_ENABLE_EXCEPTION) && LIBCOPP_MACRO_ENABLE_EXCEPTION
    if (this != result.get()) {
      __rc_ptr_counted_data_base::throw_bad_weak_ptr();
    }
#endif
    // NRVO
    return strong_rc_ptr<T>{weak_this_};
  }

  weak_rc_ptr<T> weak_from_this() { return weak_this_; }

  weak_rc_ptr<const T> weak_from_this() const { return weak_this_; }

 protected:
  enable_shared_rc_from_this() = default;
  enable_shared_rc_from_this(const enable_shared_rc_from_this&) = default;
  enable_shared_rc_from_this& operator=(const enable_shared_rc_from_this&) = default;
  ~enable_shared_rc_from_this() = default;

 private:
  template <class>
  friend class LIBCOPP_COPP_API_HEAD_ONLY strong_rc_ptr;

 public:
  template <class Y>
  void __internal_weak_assign(T* __p, const __strong_rc_counter<Y>& __n) const noexcept {
    weak_this_.assign(__p, __n);
  }

 private:
  mutable weak_rc_ptr<T> weak_this_;
};

/**
 * @brief A std::make_shared replacement for strong_rc_ptr(non-array).
 * @param args Arguments to construct the object.
 */
template <class T, class... TArgs>
nostd::enable_if_t<!::std::is_array<T>::value, strong_rc_ptr<T>> make_strong_rc(TArgs&&... args) {
  return strong_rc_ptr<T>(__strong_rc_default_alloc_shared_tag<T>{}, std::forward<TArgs>(args)...);
}

/**
 * @brief A std::make_shared replacement for strong_rc_ptr(bounded array).
 * @param args Arguments to construct the object.
 */
template <class T, class... TArgs>
nostd::enable_if_t<nostd::is_bounded_array<T>::value, strong_rc_ptr<T>> make_strong_rc(TArgs&&... args) {
  return strong_rc_ptr<T>(__strong_rc_default_alloc_shared_tag<T>{}, std::forward<TArgs>(args)...);
}

/**
 * @brief A std::allocate_shared (C++20) replacement for strong_rc_ptr(non-array).
 * @param alloc The custom allocator.
 * @param args Arguments to construct the object.
 */
template <class T, class Alloc, class... TArgs>
nostd::enable_if_t<!::std::is_array<T>::value, strong_rc_ptr<T>> allocate_strong_rc(const Alloc& alloc,
                                                                                    TArgs&&... args) {
  return strong_rc_ptr<T>(__strong_rc_with_alloc_shared_tag<T>{}, alloc, std::forward<TArgs>(args)...);
}

/**
 * @brief A std::allocate_shared (C++20) replacement for strong_rc_ptr(bounded array).
 * @param alloc The custom allocator.
 * @param args Arguments to construct the object.
 */
template <class T, class Alloc, class... TArgs>
nostd::enable_if_t<nostd::is_bounded_array<T>::value, strong_rc_ptr<T>> allocate_strong_rc(const Alloc& alloc,
                                                                                           TArgs&&... args) {
  return strong_rc_ptr<T>(__strong_rc_with_alloc_shared_tag<T>{}, alloc, std::forward<TArgs>(args)...);
}

/**
 * @brief A std::static_pointer_cast replacement for strong_rc_ptr
 * @param r A strong_rc_ptr instance.
 * @return Converted strong_rc_ptr instance.
 */
template <class T, class Y>
strong_rc_ptr<T> static_pointer_cast(const strong_rc_ptr<Y>& r) noexcept {
  return strong_rc_ptr<T>(r, static_cast<typename strong_rc_ptr<T>::element_type*>(r.get()));
}

/**
 * @brief A std::const_pointer_cast replacement for strong_rc_ptr
 * @param r A strong_rc_ptr instance.
 * @return Converted strong_rc_ptr instance.
 */
template <class T, class Y>
strong_rc_ptr<T> const_pointer_cast(const strong_rc_ptr<Y>& r) noexcept {
  return strong_rc_ptr<T>(r, const_cast<typename strong_rc_ptr<T>::element_type*>(r.get()));
}

#if defined(LIBCOPP_MACRO_ENABLE_RTTI) && LIBCOPP_MACRO_ENABLE_RTTI
/**
 * @brief A std::dynamic_pointer_cast replacement for strong_rc_ptr
 * @param r A strong_rc_ptr instance.
 * @return Converted strong_rc_ptr instance.
 */
template <class T, class Y>
strong_rc_ptr<T> dynamic_pointer_cast(const strong_rc_ptr<Y>& r) noexcept {
  return strong_rc_ptr<T>(r, dynamic_cast<typename strong_rc_ptr<T>::element_type*>(r.get()));
}
#endif

}  // namespace memory
LIBCOPP_COPP_NAMESPACE_END

namespace std {
/**
 * @brief Support std::swap for strong_rc_ptr.
 * @param a A strong_rc_ptr instance.
 * @param b Another strong_rc_ptr instance.
 */
template <class T>
LIBCOPP_COPP_API_HEAD_ONLY void swap(LIBCOPP_COPP_NAMESPACE_ID::memory::strong_rc_ptr<T>& a,
                                     LIBCOPP_COPP_NAMESPACE_ID::memory::strong_rc_ptr<T>& b) noexcept {
  a.swap(b);
}

/**
 * @brief Support std::hash for strong_rc_ptr.
 * @param a A strong_rc_ptr instance.
 * @param b Another strong_rc_ptr instance.
 */
template <class T>
struct LIBCOPP_COPP_API_HEAD_ONLY hash<LIBCOPP_COPP_NAMESPACE_ID::memory::strong_rc_ptr<T>> {
  std::size_t operator()(const LIBCOPP_COPP_NAMESPACE_ID::memory::strong_rc_ptr<T>& s) const noexcept {
    return std::hash<T*>()(s.get());
  }
};

/**
 * @brief Support std::basic_ostream for strong_rc_ptr.
 * @param a A strong_rc_ptr instance.
 * @param b Another strong_rc_ptr instance.
 */
template <class CharT, class TraitT, class T>
LIBCOPP_COPP_API_HEAD_ONLY inline std::basic_ostream<CharT, TraitT>& operator<<(
    std::basic_ostream<CharT, TraitT>& __os, const LIBCOPP_COPP_NAMESPACE_ID::memory::strong_rc_ptr<T>& __p) {
  __os << __p.get();
  return __os;
}
}  // namespace std

LIBCOPP_COPP_NAMESPACE_BEGIN
namespace memory {

/**
 * @brief Helper class to switch between strong_rc_ptr and std::shared_ptr.
 */
enum class compat_strong_ptr_mode : int8_t {
  kStrongRc = 0,  // Use strong_rc_ptr
  kStl = 1,       // Use shared_ptr
};

/**
 * @brief Helper trait class to switch between strong_rc_ptr and std::shared_ptr.
 */
template <compat_strong_ptr_mode>
struct LIBCOPP_COPP_API_HEAD_ONLY compat_strong_ptr_function_trait;

template <>
struct LIBCOPP_COPP_API_HEAD_ONLY compat_strong_ptr_function_trait<compat_strong_ptr_mode::kStrongRc> {
  template <class Y>
  using shared_ptr = memory::strong_rc_ptr<Y>;

  template <class Y>
  using weak_ptr = memory::weak_rc_ptr<Y>;

  template <class Y>
  using enable_shared_from_this = memory::enable_shared_rc_from_this<Y>;

  template <class Y, class... ArgsT>
  static inline memory::strong_rc_ptr<Y> make_shared(ArgsT&&... args) {
    return memory::make_strong_rc<Y>(std::forward<ArgsT>(args)...);
  }

  template <class Y, class Alloc, class... TArgs>
  static inline memory::strong_rc_ptr<Y> allocate_shared(const Alloc& alloc, TArgs&&... args) {
    return memory::allocate_strong_rc<Y>(alloc, std::forward<TArgs>(args)...);
  }

  template <class Y, class F>
  static inline memory::strong_rc_ptr<Y> static_pointer_cast(F&& f) {
    return memory::static_pointer_cast<Y>(std::forward<F>(f));
  }

  template <class Y, class F>
  static inline memory::strong_rc_ptr<Y> const_pointer_cast(F&& f) {
    return memory::const_pointer_cast<Y>(std::forward<F>(f));
  }

#if defined(LIBCOPP_MACRO_ENABLE_RTTI) && LIBCOPP_MACRO_ENABLE_RTTI
  template <class Y, class F>
  static inline memory::strong_rc_ptr<Y> dynamic_pointer_cast(F&& f) {
    return memory::dynamic_pointer_cast<Y>(std::forward<F>(f));
  }
#endif
};

template <>
struct LIBCOPP_COPP_API_HEAD_ONLY compat_strong_ptr_function_trait<compat_strong_ptr_mode::kStl> {
  template <class Y>
  using shared_ptr = std::shared_ptr<Y>;

  template <class Y>
  using weak_ptr = std::weak_ptr<Y>;

  template <class Y>
  using enable_shared_from_this = std::enable_shared_from_this<Y>;

  template <class Y, class... ArgsT>
  static inline std::shared_ptr<Y> make_shared(ArgsT&&... args) {
    // Some versions os STL will cause warning by mistake, which may trigger -Werror/-WX to fail the build.
    // Use include guard to ignore them.
    // NOLINT: build/include
#include "libcopp/utils/config/stl_include_prefix.h"  // IWYU pragma: keep
    return std::make_shared<Y>(std::forward<ArgsT>(args)...);
// NOLINT: build/include
#include "libcopp/utils/config/stl_include_suffix.h"  // IWYU pragma: keep
  }

  template <class Y, class Alloc, class... TArgs>
  static inline std::shared_ptr<Y> allocate_shared(const Alloc& alloc, TArgs&&... args) {
// Some versions os STL will cause warning by mistake, which may trigger -Werror/-WX to fail the build.
// Use include guard to ignore them.
// NOLINT: build/include
#include "libcopp/utils/config/stl_include_prefix.h"  // IWYU pragma: keep
    return std::allocate_shared<Y>(alloc, std::forward<TArgs>(args)...);
// NOLINT: build/include
#include "libcopp/utils/config/stl_include_suffix.h"  // IWYU pragma: keep
  }

  template <class Y, class F>
  static inline std::shared_ptr<Y> static_pointer_cast(F&& f) {
    return std::static_pointer_cast<Y>(std::forward<F>(f));
  }

  template <class Y, class F>
  static inline std::shared_ptr<Y> const_pointer_cast(F&& f) {
    return std::const_pointer_cast<Y>(std::forward<F>(f));
  }

#if defined(LIBCOPP_MACRO_ENABLE_RTTI) && LIBCOPP_MACRO_ENABLE_RTTI
  template <class Y, class F>
  static inline std::shared_ptr<Y> dynamic_pointer_cast(F&& f) {
    return std::dynamic_pointer_cast<Y>(std::forward<F>(f));
  }
#endif
};

/**
 * @brief A simple helper trait class to switch between strong_rc_ptr and std::shared_ptr.
 * @note This is for type declaration only.
 */
template <class T, compat_strong_ptr_mode PtrMode>
struct LIBCOPP_COPP_API_HEAD_ONLY compat_strong_ptr_type_trait {
  using shared_ptr = typename compat_strong_ptr_function_trait<PtrMode>::template shared_ptr<T>;
  using weak_ptr = typename compat_strong_ptr_function_trait<PtrMode>::template weak_ptr<T>;
};

}  // namespace memory
LIBCOPP_COPP_NAMESPACE_END

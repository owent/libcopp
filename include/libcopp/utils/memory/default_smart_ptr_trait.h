// Copyright 2025 owent
// Licenses under the MIT License
// @note This is a smart pointer class that is compatible with std::shared_ptr, but it is more lightweight and do not
//       use atomic operation for reference counting. It is designed for single thread usage.
// @note We support all APIs of std::shared_ptr in C++14, and partly APIs of std::shared_ptr in C++17/20/26.

#pragma once

#include "libcopp/utils/memory/rc_ptr.h"
#include "libcopp/utils/nostd/type_traits.h"

LIBCOPP_COPP_NAMESPACE_BEGIN
namespace memory {

#if LIBCOPP_MACRO_ENABLE_MULTI_THREAD
using default_rc_ptr_trait = compat_strong_ptr_function_trait<compat_strong_ptr_mode::kStl>;
#else
using default_rc_ptr_trait = compat_strong_ptr_function_trait<compat_strong_ptr_mode::kStrongRc>;
#endif

template <class T>
using default_strong_rc_ptr = typename default_rc_ptr_trait::template shared_ptr<T>;

template <class T>
using default_weak_rc_ptr = typename default_rc_ptr_trait::template weak_ptr<T>;

template <class T>
using default_enable_shared_from_this = typename default_rc_ptr_trait::template enable_shared_from_this<T>;

template <class T, class... ArgsT>
LIBCOPP_UTIL_FORCEINLINE default_strong_rc_ptr<T> default_make_strong(ArgsT&&... args) {
  return default_rc_ptr_trait::template make_shared<T>(std::forward<ArgsT>(args)...);
}

template <class T, class Alloc, class... TArgs>
LIBCOPP_UTIL_FORCEINLINE default_strong_rc_ptr<T> default_allocate_strong(const Alloc& alloc, TArgs&&... args) {
  return default_rc_ptr_trait::template allocate_shared<T>(alloc, std::forward<TArgs>(args)...);
}

template <class T, class F>
LIBCOPP_UTIL_FORCEINLINE default_strong_rc_ptr<T> default_static_pointer_cast(F&& f) {
  return default_rc_ptr_trait::template static_pointer_cast<T>(std::forward<F>(f));
}

template <class T, class F>
LIBCOPP_UTIL_FORCEINLINE default_strong_rc_ptr<T> default_const_pointer_cast(F&& f) {
  return default_rc_ptr_trait::template const_pointer_cast<T>(std::forward<F>(f));
}

#if defined(LIBCOPP_MACRO_ENABLE_RTTI) && LIBCOPP_MACRO_ENABLE_RTTI
template <class T, class F>
LIBCOPP_UTIL_FORCEINLINE default_strong_rc_ptr<T> default_dynamic_pointer_cast(F&& f) {
  return default_rc_ptr_trait::template dynamic_pointer_cast<T>(std::forward<F>(f));
}
#endif

}  // namespace memory
LIBCOPP_COPP_NAMESPACE_END

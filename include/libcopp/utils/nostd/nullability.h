// Copyright 2025 owent
// Created by owent on 2025-03-03

#pragma once

#include <memory>
#include <type_traits>

#include "libcopp/utils/config/compile_optimize.h"
#include "libcopp/utils/config/libcopp_build_features.h"
#include "libcopp/utils/nostd/type_traits.h"

LIBCOPP_COPP_NAMESPACE_BEGIN
namespace nostd {
template <class, class = void>
struct __is_nullability_compatible : ::std::false_type {};

// Allow custom to support nullability by define nullability_compatible_type as void
template <class T>
struct __is_nullability_compatible<T, void_t<typename T::nullability_compatible_type>> : ::std::true_type {};

template <class T>
struct __is_nullability_support {
  LIBCOPP_UTIL_MACRO_INLINE_VARIABLE static constexpr const bool value = __is_nullability_compatible<T>::value;
};

template <class T>
struct __is_nullability_support<T*> {
  LIBCOPP_UTIL_MACRO_INLINE_VARIABLE static constexpr const bool value = true;
};

template <class T, class U>
struct __is_nullability_support<T U::*> {
  LIBCOPP_UTIL_MACRO_INLINE_VARIABLE static constexpr const bool value = true;
};

template <class T, class... Deleter>
struct __is_nullability_support<std::unique_ptr<T, Deleter...>> {
  LIBCOPP_UTIL_MACRO_INLINE_VARIABLE static constexpr const bool value = true;
};

template <class T>
struct __is_nullability_support<std::shared_ptr<T>> {
  LIBCOPP_UTIL_MACRO_INLINE_VARIABLE static constexpr const bool value = true;
};

template <class T>
struct __enable_nullable {
  static_assert(__is_nullability_support<remove_cv_t<T>>::value,
                "Template argument must be a raw or supported smart pointer "
                "type. See nostd/nullability.h.");
  using type = T;
};

template <class T>
struct __enable_nonnull {
  static_assert(__is_nullability_support<remove_cv_t<T>>::value,
                "Template argument must be a raw or supported smart pointer "
                "type. See nostd/nullability.h.");
  using type = T;
};

template <class T>
struct __enable_nullability_unknown {
  static_assert(__is_nullability_support<remove_cv_t<T>>::value,
                "Template argument must be a raw or supported smart pointer "
                "type. See nostd/nullability.h.");
  using type = T;
};

template <class T, class = typename __enable_nullable<T>::type>
using nullable
#if LIBCOPP_UTIL_HAVE_CPP_ATTRIBUTE(clang::annotate)
    [[clang::annotate("Nullable")]]
#endif
    = T;

template <class T, class = typename __enable_nonnull<T>::type>
using nonnull
#if LIBCOPP_UTIL_HAVE_CPP_ATTRIBUTE(clang::annotate)
    [[clang::annotate("Nonnull")]]
#endif
    = T;

template <class T, class = typename __enable_nullability_unknown<T>::type>
using nullability_unknown
#if LIBCOPP_UTIL_HAVE_CPP_ATTRIBUTE(clang::annotate)
    [[clang::annotate("Nullability_Unspecified")]]
#endif
    = T;

}  // namespace nostd
LIBCOPP_COPP_NAMESPACE_END

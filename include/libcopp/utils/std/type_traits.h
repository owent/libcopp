// Copyright 2023 owent

#pragma once

#include <libcopp/utils/config/libcopp_build_features.h>

// clang-format off
#include <libcopp/utils/config/stl_include_prefix.h>  // NOLINT(build/include_order)
// clang-format on
#include <functional>
#include <memory>
#include <type_traits>
// clang-format off
#include <libcopp/utils/config/stl_include_suffix.h>  // NOLINT(build/include_order)
// clang-format on

LIBCOPP_COPP_NAMESPACE_BEGIN
namespace type_traits {
#if (defined(__cplusplus) && __cplusplus >= 201703L) || ((defined(_MSVC_LANG) && _MSVC_LANG >= 201703L))
#  define COPP_RETURN_VALUE_DECAY(F, ARG) \
    typename std::decay<decltype(std::invoke(std::declval<F>(), std::declval<ARG>()))>::type
#elif (defined(__cplusplus) &&                                                                             \
       (__cplusplus >= 201103L || (defined(__GXX_EXPERIMENTAL_CXX0X__) && __GXX_EXPERIMENTAL_CXX0X__))) || \
    ((defined(_MSVC_LANG) && _MSVC_LANG >= 201103L))
#  define COPP_RETURN_VALUE_DECAY(F, ARG) typename std::decay<typename std::result_of<F(ARG)>::type>::type
#endif

template <class T>
struct is_shared_ptr : public std::false_type {};
template <class T>
struct is_shared_ptr<std::shared_ptr<T> > : public std::true_type {};
}  // namespace type_traits
LIBCOPP_COPP_NAMESPACE_END

#if (defined(__cplusplus) && __cplusplus >= 201402L) || ((defined(_MSVC_LANG) && _MSVC_LANG >= 201402L))
#  define COPP_IS_TIRVIALLY_COPYABLE(X) std::is_trivially_copyable<X>
#  define COPP_IS_TIRVIALLY_COPYABLE_V(X) std::is_trivially_copyable<X>::value
#elif (defined(__cplusplus) && __cplusplus >= 201103L) || ((defined(_MSVC_LANG) && _MSVC_LANG >= 201103L))
#  define COPP_IS_TIRVIALLY_COPYABLE(X) std::is_trivial<X>
#  define COPP_IS_TIRVIALLY_COPYABLE_V(X) std::is_trivial<X>::value
#else
#  define COPP_IS_TIRVIALLY_COPYABLE(X) std::is_pod<X>
#  define COPP_IS_TIRVIALLY_COPYABLE_V(X) std::is_pod<X>::value
#endif

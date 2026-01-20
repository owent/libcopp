// Copyright 2026 owent
// Created by owent on 2025-03-03

#pragma once

#include "libcopp/utils/config/compile_optimize.h"
#include "libcopp/utils/config/libcopp_build_features.h"

#include <string>

LIBCOPP_COPP_NAMESPACE_BEGIN
namespace nostd {

#if ((defined(__cplusplus) && __cplusplus >= 201703L) || (defined(_MSVC_LANG) && _MSVC_LANG >= 201703L))

using ::std::data;
using ::std::size;

#else

template <class TCONTAINER>
LIBCOPP_COPP_API_HEAD_ONLY constexpr inline auto size(const TCONTAINER& container) -> decltype(container.size()) {
  return container.size();
}

template <class T, size_t SIZE>
LIBCOPP_COPP_API_HEAD_ONLY constexpr inline size_t size(const T (&)[SIZE]) noexcept {
  return SIZE;
}

template <class TCONTAINER>
LIBCOPP_COPP_API_HEAD_ONLY constexpr inline auto data(TCONTAINER& container) -> decltype(container.data()) {
  return container.data();
}

template <class TCONTAINER>
LIBCOPP_COPP_API_HEAD_ONLY constexpr inline auto data(const TCONTAINER& container) -> decltype(container.data()) {
  return container.data();
}

template <class T, size_t SIZE>
LIBCOPP_COPP_API_HEAD_ONLY constexpr inline T* data(T (&array_value)[SIZE]) noexcept {
  return array_value;
}

template <class TELEMENT>
LIBCOPP_COPP_API_HEAD_ONLY constexpr inline const TELEMENT* data(std::initializer_list<TELEMENT> l) noexcept {
  return l.begin();
}

#endif
}  // namespace nostd
LIBCOPP_COPP_NAMESPACE_END

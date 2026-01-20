// Copyright 2026 owent
// Created by owent on 2017-05-18

#pragma once

#include <iterator>

#include "libcopp/utils/config/compile_optimize.h"
#include "libcopp/utils/config/libcopp_build_features.h"

LIBCOPP_COPP_NAMESPACE_BEGIN
namespace util {

template <class ContainerType, bool IsConst = false>
class LIBCOPP_COPP_API_HEAD_ONLY iterator_guard {
 public:
  using iterator_type = typename std::conditional<IsConst, typename ContainerType::const_iterator,
                                                  typename ContainerType::iterator>::type;

  using reference = typename std::iterator_traits<iterator_type>::reference;
  using pointer = typename std::iterator_traits<iterator_type>::pointer;
  using value_type = typename std::iterator_traits<iterator_type>::value_type;

  LIBCOPP_UTIL_FORCEINLINE void set(const ContainerType& owner, iterator_type v) LIBCOPP_MACRO_NOEXCEPT {
    owner_ = &owner;
    iter_ = v;
  }

  LIBCOPP_UTIL_FORCEINLINE void reset(const ContainerType& owner) LIBCOPP_MACRO_NOEXCEPT {
    owner_ = nullptr;
    iter_ = owner.end();
  }

  LIBCOPP_UTIL_FORCEINLINE iterator_type release(const ContainerType& owner) {
    iterator_type ret = iter_;
    owner_ = nullptr;
    iter_ = owner.end();

    return ret;
  }

  LIBCOPP_UTIL_FORCEINLINE bool valid(const ContainerType& owner) const LIBCOPP_MACRO_NOEXCEPT {
    if (owner_ != &owner) {
      return false;
    }

    return iter_ != owner.end();
  }

  LIBCOPP_UTIL_FORCEINLINE reference operator*() const LIBCOPP_MACRO_NOEXCEPT { return *iter_; }

  LIBCOPP_UTIL_FORCEINLINE pointer operator->() const LIBCOPP_MACRO_NOEXCEPT { return &(*iter_); }

  LIBCOPP_UTIL_FORCEINLINE iterator_guard() LIBCOPP_MACRO_NOEXCEPT : owner_(nullptr) {}

 private:
  const ContainerType* owner_;
  iterator_type iter_;
};

}  // namespace util
LIBCOPP_COPP_NAMESPACE_END

// Copyright 2022 owent

#pragma once

#include <libcopp/utils/config/libcopp_build_features.h>

#include <libcopp/utils/features.h>

#include <memory>

LIBCOPP_COTASK_NAMESPACE_BEGIN
namespace core {
class LIBCOPP_COTASK_API_HEAD_ONLY standard_new_allocator {
 public:
  /**
   * @brief allocate a object
   * @param args construct parameters
   * @return pointer of new object
   */
  template <class Ty, class... TARGS>
  static std::shared_ptr<Ty> allocate(Ty *, TARGS &&...args) {
    return std::make_shared<Ty>(std::forward<TARGS>(args)...);
  }

  template <class Ty>
  static void deallocate(std::shared_ptr<Ty> &) {}
};
}  // namespace core
LIBCOPP_COTASK_NAMESPACE_END

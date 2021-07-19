/*
 * uint64_id_allocator.h
 *
 *  Created on: 2020-05-18
 *      Author: owent
 *
 *  Released under the MIT license
 */

#ifndef COPP_UTILS_UINT64_ID_ALLOCATOR_H
#define COPP_UTILS_UINT64_ID_ALLOCATOR_H

#pragma once

#include <stdint.h>

#include "config/libcopp_build_features.h"

namespace copp {
namespace util {
class LIBCOPP_COPP_API uint64_id_allocator {
 public:
  using value_type = uint64_t;

  static constexpr const value_type npos = 0; /** invalid key **/

  static value_type allocate() LIBCOPP_MACRO_NOEXCEPT;
  static void deallocate(value_type) LIBCOPP_MACRO_NOEXCEPT;
};
}  // namespace util
}  // namespace copp

#endif

// Copyright 2023 owent

#pragma once

#include <libcopp/utils/config/libcopp_build_features.h>

// clang-format off
#include <libcopp/utils/config/stl_include_prefix.h>  // NOLINT(build/include_order)
// clang-format on
#include <stdint.h>
// clang-format off
#include <libcopp/utils/config/stl_include_suffix.h>  // NOLINT(build/include_order)
// clang-format on

LIBCOPP_COPP_NAMESPACE_BEGIN
namespace util {
class LIBCOPP_COPP_API uint64_id_allocator {
 public:
  using value_type = uint64_t;

  static constexpr const value_type npos = 0; /** invalid key **/

  static value_type allocate() LIBCOPP_MACRO_NOEXCEPT;
  static void deallocate(value_type) LIBCOPP_MACRO_NOEXCEPT;
};
}  // namespace util
LIBCOPP_COPP_NAMESPACE_END

// Copyright 2023 owent

#pragma once

#include <libcopp/utils/config/libcopp_build_features.h>

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
#  pragma once
#endif

// clang-format off
#include <libcopp/utils/config/stl_include_prefix.h>  // NOLINT(build/include_order)
// clang-format on
#include <stdint.h>
#include <cstddef>
// clang-format off
#include <libcopp/utils/config/stl_include_suffix.h>  // NOLINT(build/include_order)
// clang-format on

#include "libcopp/fcontext/detail/config.hpp"
#include "libcopp/utils/features.h"

#ifdef COPP_HAS_ABI_HEADERS
#  include COPP_ABI_PREFIX
#endif
LIBCOPP_COPP_NAMESPACE_BEGIN
namespace fcontext {

extern "C" {

#define COPP_BOOST_CONTEXT_CALLDECL __attribute__((cdecl))

struct stack_t {
  void *sp;
  std::size_t size;

  stack_t() : sp(0), size(0) {}
};

struct fcontext_t {
  uint32_t fc_greg[6];
  stack_t fc_stack;
  uint32_t fc_freg[2];

  fcontext_t() : fc_greg(), fc_stack(), fc_freg() {}
};
}
}  // namespace fcontext
LIBCOPP_COPP_NAMESPACE_END

#ifdef COPP_HAS_ABI_HEADERS
#  include COPP_ABI_SUFFIX
#endif

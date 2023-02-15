// Copyright 2023 owent

#pragma once

#include <libcopp/utils/config/libcopp_build_features.h>

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

#define COPP_BOOST_CONTEXT_CALLDECL

// on MIPS we assume 64bit regsiters - even for 32bit ABIs

struct stack_t {
  void *sp;
  std::size_t size;

  stack_t() : sp(0), size(0) {}
};

struct fp_t {
  uint64_t fc_freg[6];

  fp_t() : fc_freg() {}
};

struct fcontext_t {
  uint32_t fc_greg[12];
  stack_t fc_stack;
  fp_t fc_fp;

  fcontext_t() : fc_greg(), fc_stack(), fc_fp() {}
};
}
}  // namespace fcontext
LIBCOPP_COPP_NAMESPACE_END

#ifdef COPP_HAS_ABI_HEADERS
#  include COPP_ABI_SUFFIX
#endif

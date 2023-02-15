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

// if defined(_LP64) we are compiling for sparc64, otherwise it is 32 bit
// sparc.

struct stack_t {
  void *sp;
  std::size_t size;

  stack_t() : sp(0), size(0) {}
};

struct fp_t {
#ifdef _LP64
  uint64_t fp_freg[32];
  uint64_t fp_fprs, fp_fsr;
#else
  uint64_t fp_freg[16];
  uint32_t fp_fsr;
#endif

  fp_t()
      : fp_freg(),
#ifdef _LP64
        fp_fprs(),
#endif
        fp_fsr() {
  }
}
#ifdef _LP64
__attribute__((__aligned__(64)))  // allow VIS instructions to be used
#endif
;

struct fcontext_t {
  fp_t fc_fp;  // fpu stuff first, for easier alignement
#ifdef _LP64
  uint64_t
#else
  uint32_t
#endif
      fc_greg[8];
  stack_t fc_stack;

  fcontext_t() : fc_fp(), fc_greg(), fc_stack() {}
};
}
}  // namespace fcontext
LIBCOPP_COPP_NAMESPACE_END

#ifdef COPP_HAS_ABI_HEADERS
#  include COPP_ABI_SUFFIX
#endif

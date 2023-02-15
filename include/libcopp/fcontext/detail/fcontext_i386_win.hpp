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

#if defined(COPP_MACRO_COMPILER_MSVC)
#  pragma warning(push)
#  pragma warning(disable : 4351)
#endif

#ifdef COPP_HAS_ABI_HEADERS
#  include COPP_ABI_PREFIX
#endif

LIBCOPP_COPP_NAMESPACE_BEGIN
namespace fcontext {

extern "C" {

#define COPP_BOOST_CONTEXT_CALLDECL __cdecl

struct stack_t {
  void *sp;
  std::size_t size;
  void *limit;

  stack_t() : sp(0), size(0), limit(0) {}
};

struct fp_t {
  uint32_t fc_freg[2];

  fp_t() : fc_freg() {}
};

struct fcontext_t {
  uint32_t fc_greg[6];
  stack_t fc_stack;
  void *fc_excpt_lst;
  void *fc_local_storage;
  fp_t fc_fp;
  uint32_t fc_dealloc;

  fcontext_t() : fc_greg(), fc_stack(), fc_excpt_lst(0), fc_local_storage(0), fc_fp(), fc_dealloc(0) {}
};
}
}  // namespace fcontext
LIBCOPP_COPP_NAMESPACE_END

#ifdef COPP_HAS_ABI_HEADERS
#  include COPP_ABI_SUFFIX
#endif

#if defined(COPP_MACRO_COMPILER_MSVC)
#  pragma warning(pop)
#endif

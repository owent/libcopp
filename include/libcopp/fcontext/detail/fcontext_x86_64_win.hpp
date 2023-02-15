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

#define COPP_BOOST_CONTEXT_CALLDECL

struct stack_t {
  void *sp;
  std::size_t size;
  void *limit;

  stack_t() : sp(0), size(0), limit(0) {}
};

struct fcontext_t {
  uint64_t fc_greg[10];
  stack_t fc_stack;
  void *fc_local_storage;
  uint64_t fc_fp[24];
  uint64_t fc_dealloc;

  fcontext_t() : fc_greg(), fc_stack(), fc_local_storage(0), fc_fp(), fc_dealloc() {}
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

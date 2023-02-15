// Copyright 2023 owent

#pragma once

#include <libcopp/utils/config/libcopp_build_features.h>

#if defined(__PGI)
#  include <stdint.h>
#endif

#if defined(_WIN32_WCE)
using intptr_t = int;
#endif

#include <stdint.h>
#include <cstddef>

#include "libcopp/fcontext/detail/config.hpp"
#include "libcopp/utils/features.h"

#ifdef COPP_HAS_ABI_HEADERS
#  include COPP_ABI_PREFIX
#endif

LIBCOPP_COPP_NAMESPACE_BEGIN
namespace fcontext {
using fcontext_t = void*;

struct transfer_t {
  fcontext_t fctx;
  void* data;
};

extern "C" COPP_BOOST_CONTEXT_DECL transfer_t COPP_BOOST_CONTEXT_CALLDECL copp_jump_fcontext_v2(fcontext_t const to,
                                                                                                void* vp);

extern "C" COPP_BOOST_CONTEXT_DECL fcontext_t COPP_BOOST_CONTEXT_CALLDECL copp_make_fcontext_v2(void* sp,
                                                                                                std::size_t size,
                                                                                                void (*fn)(transfer_t));

extern "C" COPP_BOOST_CONTEXT_DECL transfer_t COPP_BOOST_CONTEXT_CALLDECL
copp_ontop_fcontext_v2(fcontext_t const to, void* vp, transfer_t (*fn)(transfer_t));
}  // namespace fcontext
LIBCOPP_COPP_NAMESPACE_END

#ifdef COPP_HAS_ABI_HEADERS
#  include COPP_ABI_SUFFIX
#endif

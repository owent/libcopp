
//          Copyright Oliver Kowalke 2009.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#ifndef _COPP_BOOST_CONTEXT_DETAIL_FCONTEXT_MIPS_H
#define _COPP_BOOST_CONTEXT_DETAIL_FCONTEXT_MIPS_H

#include <cstddef>

#include <cstddef>
#include <stdint.h>

#include "libcopp/fcontext/detail/config.hpp"
#include "libcopp/utils/features.h"

#ifdef COPP_HAS_ABI_HEADERS
#include COPP_ABI_PREFIX
#endif

namespace copp {
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
    }
}

#ifdef COPP_HAS_ABI_HEADERS
#include COPP_ABI_SUFFIX
#endif

#endif // BOOST_CONTEXT_DETAIL_FCONTEXT_MIPS_H

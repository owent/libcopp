
//          Copyright Oliver Kowalke 2009.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#ifndef _COPP_BOOST_CONTEXT_FCONTEXT_H
#define _COPP_BOOST_CONTEXT_FCONTEXT_H

#if defined(__PGI)
#include <stdint.h>
#endif

#if defined(_WIN32_WCE)
typedef int intptr_t;
#endif

#include <cstddef>
#include <stdint.h>

#include "libcopp/utils/features.h"
#include "libcopp/fcontext/detail/config.hpp"

#ifdef COPP_HAS_ABI_HEADERS
# include COPP_ABI_PREFIX
#endif

namespace copp {
    namespace fcontext {
        typedef void*   fcontext_t;

        struct transfer_t {
            fcontext_t  fctx;
            void    *   data;
        };

        extern "C" COPP_BOOST_CONTEXT_DECL
        transfer_t COPP_BOOST_CONTEXT_CALLDECL copp_jump_fcontext(fcontext_t const to, void * vp);

        extern "C" COPP_BOOST_CONTEXT_DECL
        fcontext_t COPP_BOOST_CONTEXT_CALLDECL copp_make_fcontext(void * sp, std::size_t size, void (* fn)( transfer_t));

        extern "C" COPP_BOOST_CONTEXT_DECL
        transfer_t COPP_BOOST_CONTEXT_CALLDECL copp_ontop_fcontext( fcontext_t const to, void * vp, transfer_t (* fn)( transfer_t) );
    }
}

#ifdef COPP_HAS_ABI_HEADERS
# include COPP_ABI_SUFFIX
#endif

#endif // _COPP_BOOST_CONTEXT_FCONTEXT_H


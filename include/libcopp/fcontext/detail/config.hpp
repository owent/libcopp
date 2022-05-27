
//          Copyright Oliver Kowalke 2009.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#ifndef _COPP_BOOST_CONTEXT_DETAIL_CONFIG_H
#define _COPP_BOOST_CONTEXT_DETAIL_CONFIG_H

// clang-format off
#include <libcopp/utils/config/stl_include_prefix.h>  // NOLINT(build/include_order)
// clang-format on
// required for SD-6 compile-time integer sequences
#include <utility>
// clang-format off
#include <libcopp/utils/config/stl_include_suffix.h>  // NOLINT(build/include_order)
// clang-format on

#ifdef _COPP_BOOST_CONTEXT_DECL
#  undef _COPP_BOOST_CONTEXT_DECL
#endif

#if !defined(COPP_BOOST_CONTEXT_DECL)
#  define COPP_BOOST_CONTEXT_DECL
#endif
#undef COPP_BOOST_CONTEXT_CALLDECL
#if (defined(i386) || defined(__i386__) || defined(__i386) || defined(__i486__) || defined(__i586__) ||       \
     defined(__i686__) || defined(__X86__) || defined(_X86_) || defined(__THW_INTEL__) || defined(__I86__) || \
     defined(__INTEL__) || defined(__IA32__) || defined(_M_IX86) || defined(_I86_)) &&                        \
    defined(_WIN32)
#  define COPP_BOOST_CONTEXT_CALLDECL __cdecl
#else
#  define COPP_BOOST_CONTEXT_CALLDECL
#endif

// BOOST_CONTEXT_SEGMENTS defined at utils/features.h

#endif  // _COPP_BOOST_CONTEXT_DETAIL_CONFIG_H

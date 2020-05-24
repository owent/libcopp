#ifndef _COPP_STACKCONTEXT_STACK_TRAITS_H
#define _COPP_STACKCONTEXT_STACK_TRAITS_H

#pragma once

#include <cstddef>
#include <libcopp/utils/config/libcopp_build_features.h>
#include <libcopp/utils/features.h>

#ifdef COPP_HAS_ABI_HEADERS
#include COPP_ABI_PREFIX
#endif

namespace copp {
    struct stack_traits {
        static LIBCOPP_COPP_API bool is_unbounded() LIBCOPP_MACRO_NOEXCEPT;

        static LIBCOPP_COPP_API std::size_t page_size() LIBCOPP_MACRO_NOEXCEPT;

        static LIBCOPP_COPP_API std::size_t default_size() LIBCOPP_MACRO_NOEXCEPT;

        static LIBCOPP_COPP_API std::size_t minimum_size() LIBCOPP_MACRO_NOEXCEPT;

        static LIBCOPP_COPP_API std::size_t maximum_size() LIBCOPP_MACRO_NOEXCEPT;

        static LIBCOPP_COPP_API std::size_t round_to_page_size(std::size_t stacksize) LIBCOPP_MACRO_NOEXCEPT;
    };

} // namespace copp

#ifdef COPP_HAS_ABI_HEADERS
#include COPP_ABI_SUFFIX
#endif

#endif // BOOST_CONTEXT_STACK_TRAITS_H

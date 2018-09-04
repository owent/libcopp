#ifndef _COPP_STACKCONTEXT_STACK_TRAITS_H
#define _COPP_STACKCONTEXT_STACK_TRAITS_H

# pragma once

#include <cstddef>
#include <libcopp/utils/features.h>

#ifdef COPP_HAS_ABI_HEADERS
# include COPP_ABI_PREFIX
#endif

namespace copp { 
    struct stack_traits
    {
        static bool is_unbounded() COPP_MACRO_NOEXCEPT;

        static std::size_t page_size() COPP_MACRO_NOEXCEPT;

        static std::size_t default_size() COPP_MACRO_NOEXCEPT;

        static std::size_t minimum_size() COPP_MACRO_NOEXCEPT;

        static std::size_t maximum_size() COPP_MACRO_NOEXCEPT;

        static std::size_t round_to_page_size(std::size_t stacksize) COPP_MACRO_NOEXCEPT;
    };

}

#ifdef COPP_HAS_ABI_HEADERS
# include COPP_ABI_SUFFIX
#endif

#endif // BOOST_CONTEXT_STACK_TRAITS_H

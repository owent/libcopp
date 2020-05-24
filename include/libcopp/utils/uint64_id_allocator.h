/*
 * uint64_id_allocator.h
 *
 *  Created on: 2020-05-18
 *      Author: owent
 *
 *  Released under the MIT license
 */

#ifndef COPP_UTILS_UINT64_ID_ALLOCATOR_H
#define COPP_UTILS_UINT64_ID_ALLOCATOR_H

#pragma once

#include <stdint.h>

#include "config/libcopp_build_features.h"
#include "config/compiler_features.h"

namespace copp {
    namespace util {
        class LIBCOPP_COPP_API uint64_id_allocator {
        public:
            typedef uint64_t value_type;

#if defined(UTIL_CONFIG_COMPILER_CXX_CONSTEXPR) && UTIL_CONFIG_COMPILER_CXX_CONSTEXPR
            static constexpr value_type npos = 0; /** invalid key **/
#else
            static const value_type npos = 0; /** invalid key **/
#endif

            static value_type allocate() LIBCOPP_MACRO_NOEXCEPT;
            static void       deallocate(value_type) LIBCOPP_MACRO_NOEXCEPT;
        };
    }
}

#endif

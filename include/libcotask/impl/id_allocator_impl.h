/*
 * id_allocator_impl.h
 *
 *  Created on: 2014年4月1日
 *      Author: owent
 *
 *  Released under the MIT license
 */

#ifndef COTASK_IMPL_ID_ALLOCATOR_IMPL_H
#define COTASK_IMPL_ID_ALLOCATOR_IMPL_H

#pragma once

#include <libcopp/utils/config/compiler_features.h>
#include <libcopp/utils/config/libcopp_build_features.h>

namespace cotask {
    namespace impl {
        template <typename Ty>
        class LIBCOPP_COTASK_API_HEAD_ONLY id_allocator {
        public:
            typedef Ty value_type;
            ~id_allocator(){};
            value_type allocate() UTIL_CONFIG_NOEXCEPT;
            void       deallocate(value_type) UTIL_CONFIG_NOEXCEPT;
        };
    } // namespace impl
} // namespace cotask


#endif /* _COTASK_IMPL_ID_ALLOCATOR_IMPL_H_ */

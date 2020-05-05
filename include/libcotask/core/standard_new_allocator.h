/**
 * standard_new_allocator.h
 *
 *  Created on: 2014年4月1日
 *      Author: owent
 *  @date 2017-05-10
 *  Released under the MIT license
 */

#ifndef COTASK_CORE_STANDARD_NEW_ALLOCATOR_H
#define COTASK_CORE_STANDARD_NEW_ALLOCATOR_H

#pragma once

#include <libcopp/utils/config/libcopp_build_features.h>
#include <libcopp/utils/features.h>
#include <libcopp/utils/std/smart_ptr.h>

namespace cotask {
    namespace core {
        class LIBCOPP_COTASK_API_HEAD_ONLY standard_new_allocator {
        public:
            /**
             * @brief allocate a object
             * @param args construct parameters
             * @return pointer of new object
             */
            template <class Ty, class... TARGS>
            static std::shared_ptr<Ty> allocate(Ty *, TARGS &&... args) {
                return std::make_shared<Ty>(std::forward<TARGS>(args)...);
            }

            template <class Ty>
            static void deallocate(std::shared_ptr<Ty> &) {}
        };
    } // namespace core
} // namespace cotask


#endif /* STANDARD_NEW_ALLOCATOR_H_ */

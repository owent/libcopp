/**
 * @file remove_cv.h
 * @brief
 * Licensed under the MIT licenses.
 *
 * @version 1.0
 * @author OWenT, owt5008137@live.com
 * @date 2014年3月27日
 *
 * @history
 *
 */

#ifndef UTIL_TYPE_TRAITS_REMOVE_CV_H
#define UTIL_TYPE_TRAITS_REMOVE_CV_H

#pragma once

#include "const_common.h"

namespace util {
    namespace type_traits {
        /**
         * type traits - remove const
         */
        template <typename Ty>
        struct remove_const {
            typedef Ty type;
        };

        template <typename Ty>
        struct remove_const<const Ty> {
            typedef Ty type;
        };

#ifdef _MSC_VER
        template <typename Ty>
        struct remove_const<const Ty[]> {
            typedef Ty type[];
        };

        template <typename Ty, size_t COUNT>
        struct remove_const<const Ty[COUNT]> {
            typedef Ty type[COUNT];
        };
#endif

        /**
         * type traits - remove const
         */
        template <typename Ty>
        struct remove_volatile {
            typedef Ty type;
        };

        template <typename Ty>
        struct remove_volatile<volatile Ty> {
            typedef Ty type;
        };
#ifdef _MSC_VER
        template <typename Ty>
        struct remove_volatile<volatile Ty[]> {
            typedef Ty type[];
        };

        template <typename Ty, size_t COUNT>
        struct remove_volatile<volatile Ty[COUNT]> {
            typedef Ty type[COUNT];
        };
#endif

        /**
         * type traits - remove cv
         */
        template <typename Ty>
        struct remove_cv {
            typedef typename remove_const<typename remove_volatile<Ty>::type>::type type;
        };
    } // namespace type_traits
} // namespace util


#endif /* REMOVE_CV_H_ */

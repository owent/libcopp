/**
 * Copyright (c) 2014, Tencent
 * All rights reserved.
 *
 * @file remove_cv.h
 * @brief 
 *
 *
 * @version 1.0
 * @author owentou, owentou@tencent.com
 * @date 2014年3月27日
 *
 * @history
 *
 */

#ifndef _UTIL_TYPE_TRAITS_REMOVE_CV_H_
#define _UTIL_TYPE_TRAITS_REMOVE_CV_H_

#include "const_common.h"

namespace util
{
    namespace type_traits
    {
        /**
         * type traits - remove const
         */
        template<typename Ty>
        struct remove_const
        {
            typedef Ty type;
        };

        template<typename Ty>
        struct remove_const<const Ty>
        {
            typedef Ty type;
        };

    #ifdef _MSC_VER
        template<typename Ty>
        struct remove_const<const Ty[]>
        {
            typedef Ty type[];
        };

        template<typename Ty, size_t COUNT>
        struct remove_const<const Ty[COUNT]>
        {
            typedef Ty type[COUNT];
        };
    #endif

        /**
         * type traits - remove const
         */
        template<typename Ty>
        struct remove_volatile
        {
            typedef Ty type;
        };

        template<typename Ty>
        struct remove_volatile<volatile Ty>
        {
            typedef Ty type;
        };
    #ifdef _MSC_VER
        template<typename Ty>
        struct remove_volatile<volatile Ty[]>
        {
            typedef Ty type[];
        };

        template<typename Ty, size_t COUNT>
        struct remove_volatile<volatile Ty[COUNT]>
        {
            typedef Ty type[COUNT];
        };
    #endif

        /**
         * type traits - remove cv
         */
        template<typename Ty>
        struct remove_cv
        {
            typedef typename remove_const<typename remove_volatile<Ty>::type>::type     type;
        };
    }
}


#endif /* REMOVE_CV_H_ */

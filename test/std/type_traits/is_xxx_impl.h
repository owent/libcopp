/**
 * Copyright (c) 2014, Tencent
 * All rights reserved.
 *
 * @file is_xxx_impl.h
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

#ifndef _UTIL_TYPE_TRAITS_IS_XXX_IMPL_H_
#define _UTIL_TYPE_TRAITS_IS_XXX_IMPL_H_

#include "remove_cv.h"
#include "is_integral_impl.h"
#include "is_floating_point_impl.h"

namespace util
{
    namespace type_traits
    {
        /**
         * type traits - is void
         */
        namespace detail
        {
            template<class Ty>
            struct _is_void: public false_type
            {
            };

            template<>
            struct _is_void<void>: public true_type
            {
            };
        }
        template<class Ty>
        struct is_void: public detail::_is_void< typename remove_cv<Ty>::type >
        {
        };

        /**
         * type traits - is array
         */
        template<typename Ty>
        struct is_array : public false_type { };

        template<typename Ty, size_t SIZE>
        struct is_array<Ty[SIZE]> : public true_type { };

        template<typename Ty>
        struct is_array<Ty[]> : public true_type { };

        /**
         * type traits - is pointer
         */
        namespace detail
        {
            template<typename Ty>
            struct _is_pointer : public false_type { };

            template<typename Ty>
            struct _is_pointer<Ty*> : public true_type { };
        }

        template<typename Ty>
        struct is_pointer : public integral_constant<
            bool,
            (detail::_is_pointer<typename remove_cv<Ty>::type >::value)
        > { };


        /**
         * type traits - is same
         */
        template<class TL, class TR>
        struct is_same: public false_type { };

        template<class TL>
        struct is_same<TL, TL>: public true_type { };

        /**
         * type traits - is arithmetic
         */
        template<class Ty>
        struct is_arithmetic: public integral_constant<bool,
            (is_integral<Ty>::value || is_floating_point<Ty>::value)
        > { };

        /**
         * type traits - is fundamental
         */
        template<class Ty>
        struct is_fundamental: public integral_constant<bool,
            (is_arithmetic<Ty>::value || is_void<Ty>::value)
        > { };

        /**
         * type traits - is const
         */
        template<typename>
        struct is_const : public false_type { };

        template<typename Ty>
        struct is_const<const Ty> : public true_type { };

#ifdef _MSC_VER
        template<class Ty, size_t SIZE>
        struct is_const<Ty[SIZE]> : public false_type { };

        template<class Ty, size_t SIZE>
        struct is_const<const Ty[SIZE]> : public true_type { };
#endif

        /**
         * type traits - is volatile
         */
        template<typename>
        struct is_volatile : public false_type { };

        template<typename _Tp>
        struct is_volatile<volatile _Tp> : public true_type { };

    }
}

#endif /* _UTIL_TYPE_TRAITS_IS_XXX_IMPL_H_ */

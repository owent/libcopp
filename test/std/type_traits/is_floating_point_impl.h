/**
 * Copyright (c) 2014, Tencent
 * All rights reserved.
 *
 * @file is_floating_point_impl.h
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

#ifndef _UTIL_TYPE_TRAITS_IS_FLOATING_POINT_IMPL_H_
#define _UTIL_TYPE_TRAITS_IS_FLOATING_POINT_IMPL_H_

#include "remove_cv.h"

namespace util
{
    namespace type_traits
    {
        namespace detail
        {
            template<typename Ty>
            struct _is_floating_point : public util::type_traits::false_type
            {
            };

#define _DEFINE_FLOATING_POINT_SPEC(TYPE) template<> struct _is_floating_point<TYPE> : public util::type_traits::true_type {}

            _DEFINE_FLOATING_POINT_SPEC(float);
            _DEFINE_FLOATING_POINT_SPEC(double);
            _DEFINE_FLOATING_POINT_SPEC(long double);

#undef _DEFINE_FLOATING_POINT_SPEC
        }

        template<typename Ty>
        struct is_floating_point : public detail::_is_floating_point< typename remove_cv<Ty>::type > {};
    }
}

#endif /* _UTIL_TYPE_TRAITS_IS_FLOATING_POINT_IMPL_H_ */

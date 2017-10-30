/**
 * @file is_floating_point_impl.h
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

#ifndef UTIL_TYPE_TRAITS_IS_FLOATING_POINT_IMPL_H
#define UTIL_TYPE_TRAITS_IS_FLOATING_POINT_IMPL_H

#pragma once

#include "remove_cv.h"

namespace util {
    namespace type_traits {
        namespace detail {
            template <typename Ty>
            struct _is_floating_point : public util::type_traits::false_type {};

#define _DEFINE_FLOATING_POINT_SPEC(TYPE) \
    template <>                           \
    struct _is_floating_point<TYPE> : public util::type_traits::true_type {}

            _DEFINE_FLOATING_POINT_SPEC(float);
            _DEFINE_FLOATING_POINT_SPEC(double);
            _DEFINE_FLOATING_POINT_SPEC(long double);

#undef _DEFINE_FLOATING_POINT_SPEC
        } // namespace detail

        template <typename Ty>
        struct is_floating_point : public detail::_is_floating_point<typename remove_cv<Ty>::type> {};
    } // namespace type_traits
} // namespace util

#endif /* _UTIL_TYPE_TRAITS_IS_FLOATING_POINT_IMPL_H_ */

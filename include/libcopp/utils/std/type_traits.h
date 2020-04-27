/**
 * @file type_traits.h
 * @brief type_traits compatiable
 * Licensed under the MIT licenses.
 *
 * @version 1.0
 * @author OWenT, owt5008137@live.com
 * @date 2020-04-26
 *
 * @history
 *
 */
#ifndef LIBCOPP_UTILS_STD_TYPE_TRAITS_H
#define LIBCOPP_UTILS_STD_TYPE_TRAITS_H

#pragma once

#include <type_traits>

namespace copp {
    namespace type_traits {
        template <class T>
        struct remove_cvref {
            typedef typename std::remove_cv<typename std::remove_reference<T>::type>::type type;
        };

#if (defined(__cplusplus) && __cplusplus >= 201703L) || ((defined(_MSVC_LANG) && _MSVC_LANG >= 201703L))
#define COPP_RETURN_VALUE_CVREF(F, ARG) typename ::copp::type_traits::remove_cvref<std::invoke_result_t<F, ARG> >::type
#elif (defined(__cplusplus) && __cplusplus >= 201103L) || ((defined(_MSVC_LANG) && _MSVC_LANG >= 201103L))
#define COPP_RETURN_VALUE_CVREF(F, ARG) typename ::copp::type_traits::remove_cvref<std::result_of<F, ARG> >::type
#endif

        template <class T>
        struct is_shared_ptr : public std::false_type {};
        template <class T>
        struct is_shared_ptr<std::shared_ptr<T> > : public std::true_type {};
    } // namespace type_traits
} // namespace copp

#endif

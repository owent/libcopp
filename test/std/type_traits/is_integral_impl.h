/**
 * Copyright (c) 2014, Tencent
 * All rights reserved.
 *
 * @file is_integral_impl.h
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

#ifndef _UTIL_TYPE_TRAITS_IS_INTEGRAL_IMPL_H_
#define _UTIL_TYPE_TRAITS_IS_INTEGRAL_IMPL_H_

#include "remove_cv.h"

namespace util
{
    namespace type_traits
    {
        namespace detail
        {
            template<typename Ty>
            struct _is_integral : public util::type_traits::false_type
            {
            };

#define _DEFINE_INTEGRAL_SPEC(TYPE) template<> struct _is_integral<TYPE> : public util::type_traits::true_type {}

            _DEFINE_INTEGRAL_SPEC(bool);
            _DEFINE_INTEGRAL_SPEC(char);
            _DEFINE_INTEGRAL_SPEC(unsigned char);
            _DEFINE_INTEGRAL_SPEC(signed char);
            _DEFINE_INTEGRAL_SPEC(short);
            _DEFINE_INTEGRAL_SPEC(unsigned short);
            _DEFINE_INTEGRAL_SPEC(int);
            _DEFINE_INTEGRAL_SPEC(unsigned int);
            _DEFINE_INTEGRAL_SPEC(long);
            _DEFINE_INTEGRAL_SPEC(unsigned long);

#ifdef _LONGLONG
            _DEFINE_INTEGRAL_SPEC(_LONGLONG);
            _DEFINE_INTEGRAL_SPEC(_ULONGLONG);
#endif

#if !defined(_MSC_VER)
            _DEFINE_INTEGRAL_SPEC(long long);
            _DEFINE_INTEGRAL_SPEC(unsigned long long);
#endif


            // MSVC or GCC char16_t, char32_t
#if (defined(_HAS_CHAR16_T_LANGUAGE_SUPPORT) && _HAS_CHAR16_T_LANGUAGE_SUPPORT) || defined(_GLIBCXX_INCLUDE_AS_CXX0X)
            _DEFINE_INTEGRAL_SPEC(char16_t);
            _DEFINE_INTEGRAL_SPEC(char32_t);
#endif

            // MSVC or GCC wchar_t
#if defined(_GLIBCXX_USE_WCHAR_T) || (defined(_NATIVE_WCHAR_T_DEFINED) && _NATIVE_WCHAR_T_DEFINED)
            _DEFINE_INTEGRAL_SPEC(wchar_t);
#endif


#undef _DEFINE_INTEGRAL_SPEC
        }

        template<typename Ty>
        struct is_integral : public detail::_is_integral< typename remove_cv<Ty>::type > {};
    }
}

#endif /* IS_INTEGRAL_IMPL_H_ */

/**
 * @file decltype.h
 * @brief decltype兼容层
 * Licensed under the MIT licenses.
 *
 * @version 1.0
 * @author OWenT, owt5008137@live.com
 * @date 2014.07.14
 *
 * @history
 *
 */

#ifndef _STD_DECLTYPE_H_
#define _STD_DECLTYPE_H_


#if defined(_MSC_VER) && (_MSC_VER >= 1020)
# pragma once
#endif

#if (defined(__cplusplus) && __cplusplus >= 201103L) || (defined(_MSC_VER) && (_MSC_VER >= 1600))
    #define STD_DECLTYPE decltype

#elif defined(__clang__) && defined(__has_feature)
    #if __has_feature(cxx_decltype) || __has_extension(cxx_decltype)
        #define STD_DECLTYPE decltype
    #endif
#endif

#if !defined(STD_DECLTYPE)
    #if defined(__COMO__) && defined(__GNUG__)
        #define STD_DECLTYPE typeof

    #elif !defined(__COMO__) && defined(__GNUC__)
        #define STD_DECLTYPE __typeof__

    #elif !defined(__COMO__) && defined(__MWERKS__) && __MWERKS__ > 0x3003
        #define STD_DECLTYPE __typeof__

    #elif !defined(__COMO__) && defined(__SUNPRO_CC) && __SUNPRO_CC > 0x590
        #define STD_DECLTYPE __typeof__

    #endif

#endif

#endif


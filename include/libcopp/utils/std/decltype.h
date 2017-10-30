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

#ifndef STD_DECLTYPE_H
#define STD_DECLTYPE_H


#pragma once

#include <libcopp/utils/config/compiler_features.h>

#if UTIL_CONFIG_COMPILER_CXX_DECLTYPE && UTIL_CONFIG_COMPILER_CXX_DECLTYPE
#define STD_DECLTYPE decltype
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

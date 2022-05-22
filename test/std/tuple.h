/**
 * @file tuple.h
 * @brief 导入多元组库
 * Licensed under the MIT licenses.
 *
 * @version 1.0
 * @author OWenT, owt5008137@live.com
 * @date 2012.08.02
 *
 * @history
 *
 */

#ifndef STD_TUPLE_H
#define STD_TUPLE_H

#pragma once

// ============================================================
// 公共包含部分
// 自动导入TR1库
// ============================================================

/**
 * 导入多维元组（tuple）
 * 相当于std::pair的增强版，最多支持10维
 * 如果是G++且支持c++0x草案1（tr1版本）的tuple[GCC版本高于4.0]
 * 则会启用GNU-C++的多维元组
 *
 * 如果是VC++且支持c++0x草案1（tr1版本）的tuple[VC++版本高于9.0 SP1]
 * 则会启用VC++的多维元组
 *
 * 否则启用boost中的tuple库（如果是这种情况需要加入boost库）
 */

#include "utility.h"

// VC9.0 SP1以上分支判断
#if defined(_MSC_VER) && ((_MSC_VER == 1500 && defined(_HAS_TR1)) || _MSC_VER > 1500)
// 采用VC std::tr1库
#  include <tuple>
#elif defined(__clang__) && __clang_major__ >= 3
// 采用Clang c++11库
#  include <tuple>
#elif defined(__GNUC__) && __GNUC__ >= 4
// 采用G++ std::tr1库
#  if __cplusplus >= 201103L || defined(__GXX_EXPERIMENTAL_CXX0X__)
#    include <tuple>
#  else
#    include <tr1/tuple>
namespace std {
using tr1::get;
using tr1::ignore;
using tr1::make_tuple;
using tr1::tie;
using tr1::tuple;
using tr1::tuple_element;
using tr1::tuple_size;
}  // namespace std
#  endif
#else
// 采用boost库
#  include <boost/tr1/tuple.hpp>
namespace std {
using tr1::get;
using tr1::ignore;
using tr1::make_tuple;
using tr1::tie;
using tr1::tuple;
using tr1::tuple_element;
using tr1::tuple_size;
}  // namespace std
#endif

#endif

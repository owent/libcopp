/**
 * @file utility.h
 * @brief 导入公共高级库
 * Licensed under the MIT licenses.
 *
 * @version 1.0
 * @author OWenT, owt5008137@live.com
 * @date 2012.08.02
 *
 * @history
 *
 */

#ifndef STD_UTILITY_H
#define STD_UTILITY_H

#pragma once

// ============================================================
// 公共包含部分
// 自动导入TR1库
// ============================================================

/**
 * 导入utility文件（utility）
 * 如果是G++且支持c++0x草案1（tr1版本）的utility[GCC版本高于4.0]
 * 则会启用GNU-C++的utility
 *
 * 如果是VC++且支持c++0x草案1（tr1版本）的utility[VC++版本高于9.0 SP1]
 * 则会启用VC++的utility
 *
 * 否则启用boost中的utility库（如果是这种情况需要加入boost库）
 */

// VC9.0 SP1以上分支判断
#if defined(_MSC_VER) && ((_MSC_VER == 1500 && defined(_HAS_TR1)) || _MSC_VER > 1500)
// 采用VC std::tr1库
#  include <utility>
#elif defined(__clang__) && __clang_major__ >= 3
// 采用Clang c++11库
#  include <utility>
#elif defined(__GNUC__) && __GNUC__ >= 4
// 采用G++ std::tr1库
#  if __cplusplus >= 201103L || defined(__GXX_EXPERIMENTAL_CXX0X__)
#    include <utility>
#  else
#    include <tr1/utility>
namespace std {
using tr1::get;
using tr1::tuple_element;
using tr1::tuple_size;
}  // namespace std
#  endif
#else
// 采用boost库
#  include <boost/tr1/utility.hpp>
namespace std {
using tr1::get;
using tr1::tuple_element;
using tr1::tuple_size;
}  // namespace std
#endif

#endif

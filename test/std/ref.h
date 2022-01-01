/**
 * @file ref.h
 * @brief 导入高级库，hash、引用包装等
 * Licensed under the MIT licenses.
 *
 * @version 1.0
 * @author OWenT, owt5008137@live.com
 * @date 2012.08.02
 *
 * @history
 *
 */

#ifndef STD_REF_H
#define STD_REF_H

#pragma once

// ============================================================
// 公共包含部分
// 自动导入引用包装库
// ============================================================

/**
 * 导入引用包装（std::ref, std::cref, std::reference_wrapper）
 * 如果是G++且支持c++0x草案1（tr1版本）的smart_ptr[GCC版本高于4.0]
 * 则会启用GNU-C++的引用包装
 *
 * 如果是VC++且支持c++0x草案1（tr1版本）的smart_ptr[VC++版本高于9.0 SP1]
 * 则会启用VC++的引用包装
 *
 * 否则启用boost中的引用包装库（如果是这种情况需要加入boost库）
 */

// VC9.0 SP1以上分支判断
#if defined(_MSC_VER) && ((_MSC_VER == 1500 && defined(_HAS_TR1)) || _MSC_VER > 1500)
// 采用VC std::tr1库
#  include <functional>
#elif defined(__clang__) && __clang_major__ >= 3
// 采用Clang c++11库
#  include <functional>
#elif defined(__GNUC__) && __GNUC__ >= 4
// 采用G++ std::tr1库
#  if __cplusplus >= 201103L || defined(__GXX_EXPERIMENTAL_CXX0X__)
#    include <functional>
#  else
#    include <tr1/functional>
namespace std {
using tr1::cref;
using tr1::ref;
using tr1::reference_wrapper;
}  // namespace std
#  endif
#else
// 采用boost tr1库
#  include <boost/tr1/functional.hpp>
namespace std {
using tr1::cref;
using tr1::ref;
using tr1::reference_wrapper;
}  // namespace std
#endif

#endif

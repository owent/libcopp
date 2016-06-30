/**
 *
 * @file smart_ptr.h
 * @brief 导入智能指针库
 * Licensed under the MIT licenses.
 *
 * @version 1.0
 * @author OWenT, owt5008137@live.com
 * @date 2012.08.02
 *     2014.05.17 vc add include <memory>
 * @history
 *
 */

#ifndef _STD_SMARTPTR_H_
#define _STD_SMARTPTR_H_


#if defined(_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif

// ============================================================
// 公共包含部分
// 自动导入TR1库
// ============================================================

/**
* 导入智能指针（smart_ptr）
* 如果是G++且支持c++0x草案1（tr1版本）的smart_ptr[GCC版本高于4.0]
* 则会启用GNU-C++的智能指针
*
* 如果是VC++且支持c++0x草案1（tr1版本）的smart_ptr[VC++版本高于9.0 SP1]
* 则会启用VC++的智能指针
*
* 否则启用boost中的smart_ptr库（如果是这种情况需要加入boost库）
*/

#if defined(_MSC_VER) && _MSC_VER >= 1500
#include <memory>
#endif

// VC9.0 SP1以上分支判断
#if defined(_MSC_VER) && ((_MSC_VER == 1500 && defined(_HAS_TR1)) || _MSC_VER > 1500)
// 采用VC std::tr1库
#include <memory>
#elif defined(__clang__) && __clang_major__ >= 3
// 采用Clang c++11库
#include <memory>
#elif defined(__GNUC__) && __GNUC__ >= 4
// 采用G++ std::tr1库
#if __cplusplus >= 201103L || defined(__GXX_EXPERIMENTAL_CXX0X__)
#include <memory>
#else
#include <tr1/memory>
namespace std {
    using tr1::bad_weak_ptr;
    using tr1::const_pointer_cast;
    using tr1::dynamic_pointer_cast;
    using tr1::enable_shared_from_this;
    using tr1::get_deleter;
    using tr1::shared_ptr;
    using tr1::make_shared;
    using tr1::static_pointer_cast;
    using tr1::swap;
    using tr1::weak_ptr;
}
#endif
#else
// 采用boost tr1库
#include <boost/tr1/memory.hpp>
namespace std {
    using tr1::bad_weak_ptr;
    using tr1::const_pointer_cast;
    using tr1::dynamic_pointer_cast;
    using tr1::enable_shared_from_this;
    using tr1::get_deleter;
    using tr1::shared_ptr;
    using tr1::make_shared;
    using tr1::static_pointer_cast;
    using tr1::swap;
    using tr1::weak_ptr;
}
#endif

#endif

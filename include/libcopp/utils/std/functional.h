/**
 * @file functional.h
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

#ifndef STD_FUNCTIONAL_H
#define STD_FUNCTIONAL_H

#pragma once

// ============================================================
// 公共包含部分
// 自动导入TR1库
// ============================================================

/**
* 导入可调用对象（functional）
* 如果是G++且支持c++0x草案1（tr1版本）的functional[GCC版本高于4.3]
* 则会启用GNU-C++的functional
*
* 如果是VC++且支持c++0x草案1（tr1版本）的functional[VC++版本高于9.0 SP1]
* 则会启用VC++的functional
*
* 否则启用boost中的functional库（如果是这种情况需要加入boost库）
*/

#include <cstdlib>

// VC9.0 SP1以上分支判断
#if defined(_MSC_VER) && ((_MSC_VER == 1500 && defined(_HAS_TR1)) || _MSC_VER > 1500)
// 采用VC std::tr1库
#include <functional>
#elif defined(__clang__) && __clang_major__ >= 3 && (__cplusplus >= 201103L || !defined(_LIBCPP_HAS_NO_VARIADICS))
// 采用Clang c++11库
#include <functional>
#elif defined(__GNUC__) && ((__GNUC__ == 4 && __GNUC_MINOR__ >= 3) || __GNUC__ > 4)
// 采用GCC c++11库
#if __cplusplus >= 201103L || defined(__GXX_EXPERIMENTAL_CXX0X__)
#include <functional>
#else
#include <tr1/functional>
namespace std {
    using tr1::ref;
    using tr1::cref;
    using tr1::hash;
    using tr1::reference_wrapper;
    using tr1::result_of;
    using tr1::swap;

    using tr1::bad_function_call;
    using tr1::bind;
    using tr1::function;
    using tr1::is_bind_expression;
    using tr1::is_placeholder;
    using tr1::mem_fn;
    using tr1::swap;

    namespace placeholders {
        using namespace tr1::placeholders;
    }
}
#endif
#else
// 采用boost tr1库
#include <boost/tr1/functional>
namespace std {
    using tr1::ref;
    using tr1::cref;
    using tr1::hash;
    using tr1::reference_wrapper;
    using tr1::result_of;
    using tr1::swap;

    using tr1::bad_function_call;
    using tr1::bind;
    using tr1::function;
    using tr1::is_bind_expression;
    using tr1::is_placeholder;
    using tr1::mem_fn;
    using tr1::swap;

    namespace placeholders {
        using namespace tr1::placeholders;
    }
}
#endif


#endif

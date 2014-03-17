/**
* @file thread.h
* @brief 导入多线程特性支持兼容层
* Licensed under the MIT licenses.
*
* @version 1.0
* @author OWenT, owt5008137@live.com
* @date 2014.03.13
*
* @history
*
*/

#ifndef _STD_THREAD_H_
#define _STD_THREAD_H_


#if defined(_MSC_VER) && (_MSC_VER >= 1020)
# pragma once
#endif

/**
 * 导入线程特性支持 (thread)
 * 规则如下
 * @see http://en.wikipedia.org/wiki/Thread-local_storage#C.2B.2B
 * @note 不支持 C++ Builder 编译器
 */

#if defined(__cplusplus) && __cplusplus >= 201103L
    #define THREAD_TLS thread_local
// VC 2003
#elif defined(_MSC_VER) && (_MSC_VER >= 1300)
    #define THREAD_TLS __declspec( thread )
#else
    #define THREAD_TLS __thread
#endif

#endif

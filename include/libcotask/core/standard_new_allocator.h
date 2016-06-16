/**
 * standard_new_allocator.h
 *
 *  Created on: 2014年4月1日
 *      Author: owent
 *  @date 2014-04-02
 *  Released under the MIT license
 */

#ifndef _COTASK_CORE_STANDARD_NEW_ALLOCATOR_H_
#define _COTASK_CORE_STANDARD_NEW_ALLOCATOR_H_

#include <new>
#include <libcopp/utils/features.h>

namespace cotask {
    namespace core {
        class standard_new_allocator
        {
        public:

#if defined(COPP_MACRO_ENABLE_VARIADIC_TEMPLATE) && COPP_MACRO_ENABLE_VARIADIC_TEMPLATE
            /**
             * @brief allocate a object
             * @param args construct parameters
             * @return pointer of new object
             */
            template<typename Ty, typename... TARGS>

#if defined(UTIL_CONFIG_COMPILER_CXX_RVALUE_REFERENCES) && UTIL_CONFIG_COMPILER_CXX_RVALUE_REFERENCES
            static Ty* allocate(Ty*, TARGS&&... args) UTIL_CONFIG_NOEXCEPT {
#else
            static Ty* allocate(Ty*, TARGS... args) UTIL_CONFIG_NOEXCEPT {
#endif
            
                Ty* ret = new (std::nothrow) Ty(std::forward<TARGS>(args)...);
                return ret;
            }
#else
            /**
             * @brief allocate a object with 0 parameter(s).
             * @return pointer of new object
             */
            template< typename Ty >
            static Ty* allocate(Ty*) UTIL_CONFIG_NOEXCEPT {
                Ty* ret = new (std::nothrow) Ty();
                return ret;
            }

            /**
             * @brief allocate a object with 1 parameter(s).
             * @param arg0  parameter 0
             * @return pointer of new object
             */
            template< typename Ty,typename TARG0 >
            static Ty* allocate(Ty*, TARG0 arg0) UTIL_CONFIG_NOEXCEPT {
                Ty* ret = new (std::nothrow) Ty(arg0);
                return ret;
            }

            /**
             * @brief allocate a object with 2 parameter(s).
             * @param arg0  parameter 0
             * @param arg1  parameter 1
             * @return pointer of new object
             */
            template< typename Ty,typename TARG0, typename TARG1 >
            static Ty* allocate(Ty*, TARG0 arg0, TARG1 arg1) UTIL_CONFIG_NOEXCEPT {
                Ty* ret = new (std::nothrow) Ty(arg0, arg1);
                return ret;
            }

            /**
             * @brief allocate a object with 3 parameter(s).
             * @param arg0  parameter 0
             * @param arg1  parameter 1
             * @param arg2  parameter 2
             * @return pointer of new object
             */
            template< typename Ty,typename TARG0, typename TARG1, typename TARG2 >
            static Ty* allocate(Ty*, TARG0 arg0, TARG1 arg1, TARG2 arg2) UTIL_CONFIG_NOEXCEPT {
                Ty* ret = new (std::nothrow) Ty(arg0, arg1, arg2);
                return ret;
            }

            /**
             * @brief allocate a object with 4 parameter(s).
             * @param arg0  parameter 0
             * @param arg1  parameter 1
             * @param arg2  parameter 2
             * @param arg3  parameter 3
             * @return pointer of new object
             */
            template< typename Ty,typename TARG0, typename TARG1, typename TARG2, typename TARG3 >
            static Ty* allocate(Ty*, TARG0 arg0, TARG1 arg1, TARG2 arg2, TARG3 arg3) UTIL_CONFIG_NOEXCEPT {
                Ty* ret = new (std::nothrow) Ty(arg0, arg1, arg2, arg3);
                return ret;
            }

            /**
             * @brief allocate a object with 5 parameter(s).
             * @param arg0  parameter 0
             * @param arg1  parameter 1
             * @param arg2  parameter 2
             * @param arg3  parameter 3
             * @param arg4  parameter 4
             * @return pointer of new object
             */
            template< typename Ty,typename TARG0, typename TARG1, typename TARG2, typename TARG3, typename TARG4 >
            static Ty* allocate(Ty*, TARG0 arg0, TARG1 arg1, TARG2 arg2, TARG3 arg3, TARG4 arg4) UTIL_CONFIG_NOEXCEPT {
                Ty* ret = new (std::nothrow) Ty(arg0, arg1, arg2, arg3, arg4);
                return ret;
            }

            /**
             * @brief allocate a object with 6 parameter(s).
             * @param arg0  parameter 0
             * @param arg1  parameter 1
             * @param arg2  parameter 2
             * @param arg3  parameter 3
             * @param arg4  parameter 4
             * @param arg5  parameter 5
             * @return pointer of new object
             */
            template< typename Ty,typename TARG0, typename TARG1, typename TARG2, typename TARG3, typename TARG4, typename TARG5 >
            static Ty* allocate(Ty*, TARG0 arg0, TARG1 arg1, TARG2 arg2, TARG3 arg3, TARG4 arg4, TARG5 arg5) UTIL_CONFIG_NOEXCEPT {
                Ty* ret = new (std::nothrow) Ty(arg0, arg1, arg2, arg3, arg4, arg5);
                return ret;
            }

            /**
             * @brief allocate a object with 7 parameter(s).
             * @param arg0  parameter 0
             * @param arg1  parameter 1
             * @param arg2  parameter 2
             * @param arg3  parameter 3
             * @param arg4  parameter 4
             * @param arg5  parameter 5
             * @param arg6  parameter 6
             * @return pointer of new object
             */
            template< typename Ty,typename TARG0, typename TARG1, typename TARG2, typename TARG3, typename TARG4, typename TARG5, typename TARG6 >
            static Ty* allocate(Ty*, TARG0 arg0, TARG1 arg1, TARG2 arg2, TARG3 arg3, TARG4 arg4, TARG5 arg5, TARG6 arg6) UTIL_CONFIG_NOEXCEPT {
                Ty* ret = new (std::nothrow) Ty(arg0, arg1, arg2, arg3, arg4, arg5, arg6);
                return ret;
            }

            /**
             * @brief allocate a object with 8 parameter(s).
             * @param arg0  parameter 0
             * @param arg1  parameter 1
             * @param arg2  parameter 2
             * @param arg3  parameter 3
             * @param arg4  parameter 4
             * @param arg5  parameter 5
             * @param arg6  parameter 6
             * @param arg7  parameter 7
             * @return pointer of new object
             */
            template< typename Ty,typename TARG0, typename TARG1, typename TARG2, typename TARG3, typename TARG4, typename TARG5, typename TARG6, typename TARG7 >
            static Ty* allocate(Ty*, TARG0 arg0, TARG1 arg1, TARG2 arg2, TARG3 arg3, TARG4 arg4, TARG5 arg5, TARG6 arg6, TARG7 arg7) UTIL_CONFIG_NOEXCEPT {
                Ty* ret = new (std::nothrow) Ty(arg0, arg1, arg2, arg3, arg4, arg5, arg6, arg7);
                return ret;
            }

            /**
             * @brief allocate a object with 9 parameter(s).
             * @param arg0  parameter 0
             * @param arg1  parameter 1
             * @param arg2  parameter 2
             * @param arg3  parameter 3
             * @param arg4  parameter 4
             * @param arg5  parameter 5
             * @param arg6  parameter 6
             * @param arg7  parameter 7
             * @param arg8  parameter 8
             * @return pointer of new object
             */
            template< typename Ty,typename TARG0, typename TARG1, typename TARG2, typename TARG3, typename TARG4, typename TARG5, typename TARG6, typename TARG7, typename TARG8 >
            static Ty* allocate(Ty*, TARG0 arg0, TARG1 arg1, TARG2 arg2, TARG3 arg3, TARG4 arg4, TARG5 arg5, TARG6 arg6, TARG7 arg7, TARG8 arg8) UTIL_CONFIG_NOEXCEPT {
                Ty* ret = new (std::nothrow) Ty(arg0, arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8);
                return ret;
            }

#endif
            template<typename Ty>
            static void deallocate(Ty* pt){
                delete pt;
            }
        };
    }
}


#endif /* STANDARD_NEW_ALLOCATOR_H_ */

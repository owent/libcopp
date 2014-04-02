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
            Ty* allocate(Ty*, TARGS... args) {
                Ty* ret = new Ty(args...);
                return ret;
            }
#else
            /**
             * @brief allocate a object with 0 parameter(s).
             * @return pointer of new object
             */
            template< typename Ty >
            Ty* allocate(Ty*) {
                Ty* ret = new Ty();
                return ret;
            }

            /**
             * @brief allocate a object with 1 parameter(s).
             * @param arg0  parameter 0
             * @return pointer of new object
             */
            template< typename Ty,typename TARG0 >
            Ty* allocate(Ty*, TARG0 arg0) {
                Ty* ret = new Ty(arg0);
                return ret;
            }

            /**
             * @brief allocate a object with 2 parameter(s).
             * @param arg0  parameter 0
             * @param arg1  parameter 1
             * @return pointer of new object
             */
            template< typename Ty,typename TARG0, typename TARG1 >
            Ty* allocate(Ty*, TARG0 arg0, TARG1 arg1) {
                Ty* ret = new Ty(arg0, arg1);
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
            Ty* allocate(Ty*, TARG0 arg0, TARG1 arg1, TARG2 arg2) {
                Ty* ret = new Ty(arg0, arg1, arg2);
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
            Ty* allocate(Ty*, TARG0 arg0, TARG1 arg1, TARG2 arg2, TARG3 arg3) {
                Ty* ret = new Ty(arg0, arg1, arg2, arg3);
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
            Ty* allocate(Ty*, TARG0 arg0, TARG1 arg1, TARG2 arg2, TARG3 arg3, TARG4 arg4) {
                Ty* ret = new Ty(arg0, arg1, arg2, arg3, arg4);
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
            Ty* allocate(Ty*, TARG0 arg0, TARG1 arg1, TARG2 arg2, TARG3 arg3, TARG4 arg4, TARG5 arg5) {
                Ty* ret = new Ty(arg0, arg1, arg2, arg3, arg4, arg5);
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
            Ty* allocate(Ty*, TARG0 arg0, TARG1 arg1, TARG2 arg2, TARG3 arg3, TARG4 arg4, TARG5 arg5, TARG6 arg6) {
                Ty* ret = new Ty(arg0, arg1, arg2, arg3, arg4, arg5, arg6);
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
            Ty* allocate(Ty*, TARG0 arg0, TARG1 arg1, TARG2 arg2, TARG3 arg3, TARG4 arg4, TARG5 arg5, TARG6 arg6, TARG7 arg7) {
                Ty* ret = new Ty(arg0, arg1, arg2, arg3, arg4, arg5, arg6, arg7);
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
            Ty* allocate(Ty*, TARG0 arg0, TARG1 arg1, TARG2 arg2, TARG3 arg3, TARG4 arg4, TARG5 arg5, TARG6 arg6, TARG7 arg7, TARG8 arg8) {
                Ty* ret = new Ty(arg0, arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8);
                return ret;
            }

#endif
            template<typename Ty>
            void deallocate(Ty* pt){
                delete pt;
            }
        };
    }
}


#endif /* STANDARD_NEW_ALLOCATOR_H_ */

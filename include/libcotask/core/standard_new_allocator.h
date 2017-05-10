/**
 * standard_new_allocator.h
 *
 *  Created on: 2014年4月1日
 *      Author: owent
 *  @date 2017-05-10
 *  Released under the MIT license
 */

#ifndef _COTASK_CORE_STANDARD_NEW_ALLOCATOR_H_
#define _COTASK_CORE_STANDARD_NEW_ALLOCATOR_H_

#include <libcopp/utils/features.h>
#include <libcopp/utils/std/smart_ptr.h>

namespace cotask {
    namespace core {
        class standard_new_allocator {
        public:
#if defined(COPP_MACRO_ENABLE_VARIADIC_TEMPLATE) && COPP_MACRO_ENABLE_VARIADIC_TEMPLATE
            /**
             * @brief allocate a object
             * @param args construct parameters
             * @return pointer of new object
             */
            template <typename Ty, typename... TARGS>
            static std::shared_ptr<Ty> allocate(Ty *, TARGS COPP_MACRO_RV_REF... args) {
                return COPP_MACRO_STD_MOVE(std::make_shared<Ty>(COPP_MACRO_STD_FORWARD(TARGS, args)...));
            }

#else
            /**
             * @brief allocate a object with 0 parameter(s).
             * @return pointer of new object
             */
            template <typename Ty>
            static std::shared_ptr<Ty> allocate(Ty *) {
                return COPP_MACRO_STD_MOVE(std::make_shared<Ty>());
            }

            /**
             * @brief allocate a object with 1 parameter(s).
             * @param arg0  parameter 0
             * @return pointer of new object
             */
            template <typename Ty, typename TARG0>
            static std::shared_ptr<Ty> allocate(Ty *, TARG0 COPP_MACRO_RV_REF arg0) {
                return COPP_MACRO_STD_MOVE(std::make_shared<Ty>(COPP_MACRO_STD_FORWARD(TARG0, arg0)));
            }

            /**
             * @brief allocate a object with 2 parameter(s).
             * @param arg0  parameter 0
             * @param arg1  parameter 1
             * @return pointer of new object
             */
            template <typename Ty, typename TARG0, typename TARG1>
            static std::shared_ptr<Ty> allocate(Ty *, TARG0 COPP_MACRO_RV_REF arg0, TARG1 COPP_MACRO_RV_REF arg1) {
                return COPP_MACRO_STD_MOVE(std::make_shared<Ty>(COPP_MACRO_STD_FORWARD(TARG0, arg0), COPP_MACRO_STD_FORWARD(TARG1, arg1)));
            }

            /**
             * @brief allocate a object with 3 parameter(s).
             * @param arg0  parameter 0
             * @param arg1  parameter 1
             * @param arg2  parameter 2
             * @return pointer of new object
             */
            template <typename Ty, typename TARG0, typename TARG1, typename TARG2>
            static std::shared_ptr<Ty> allocate(Ty *, TARG0 COPP_MACRO_RV_REF arg0, TARG1 COPP_MACRO_RV_REF arg1,
                                                TARG2 COPP_MACRO_RV_REF arg2) {
                return COPP_MACRO_STD_MOVE(std::make_shared<Ty>(COPP_MACRO_STD_FORWARD(TARG0, arg0), COPP_MACRO_STD_FORWARD(TARG1, arg1),
                                                                COPP_MACRO_STD_FORWARD(TARG2, arg2)));
            }

            /**
             * @brief allocate a object with 4 parameter(s).
             * @param arg0  parameter 0
             * @param arg1  parameter 1
             * @param arg2  parameter 2
             * @param arg3  parameter 3
             * @return pointer of new object
             */
            template <typename Ty, typename TARG0, typename TARG1, typename TARG2, typename TARG3>
            static std::shared_ptr<Ty> allocate(Ty *, TARG0 COPP_MACRO_RV_REF arg0, TARG1 COPP_MACRO_RV_REF arg1,
                                                TARG2 COPP_MACRO_RV_REF arg2, TARG3 COPP_MACRO_RV_REF arg3) {
                return COPP_MACRO_STD_MOVE(std::make_shared<Ty>(COPP_MACRO_STD_FORWARD(TARG0, arg0), COPP_MACRO_STD_FORWARD(TARG1, arg1),
                                                                COPP_MACRO_STD_FORWARD(TARG2, arg2), COPP_MACRO_STD_FORWARD(TARG3, arg3)));
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
            template <typename Ty, typename TARG0, typename TARG1, typename TARG2, typename TARG3, typename TARG4>
            static std::shared_ptr<Ty> allocate(Ty *, TARG0 COPP_MACRO_RV_REF arg0, TARG1 COPP_MACRO_RV_REF arg1,
                                                TARG2 COPP_MACRO_RV_REF arg2, TARG3 COPP_MACRO_RV_REF arg3, TARG4 COPP_MACRO_RV_REF arg4) {
                return COPP_MACRO_STD_MOVE(std::make_shared<Ty>(COPP_MACRO_STD_FORWARD(TARG0, arg0), COPP_MACRO_STD_FORWARD(TARG1, arg1),
                                                                COPP_MACRO_STD_FORWARD(TARG2, arg2), COPP_MACRO_STD_FORWARD(TARG3, arg3),
                                                                COPP_MACRO_STD_FORWARD(TARG4, arg4)));
            }

#endif
            template <typename Ty>
            static void deallocate(std::shared_ptr<Ty> &pt) {}
        };
    }
}


#endif /* STANDARD_NEW_ALLOCATOR_H_ */

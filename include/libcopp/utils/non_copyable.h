/*
 * non_copyable.h
 *
 *  Created on: 2014年4月2日
 *      Author: owent
 *
 *  Released under the MIT license
 */

#ifndef _COPP_UTILS_NON_COPYABLE_H_
#define _COPP_UTILS_NON_COPYABLE_H_

#include "libcopp/utils/config/compiler_features.h"

namespace copp {
    namespace utils {
        class non_copyable
        {
        protected:
            non_copyable(){}
            ~non_copyable(){}

        private:
            non_copyable(const non_copyable&) UTIL_CONFIG_DELETED_FUNCTION;
            non_copyable& operator=(const non_copyable&) UTIL_CONFIG_DELETED_FUNCTION;

#if defined(UTIL_CONFIG_COMPILER_CXX_RVALUE_REFERENCES) && UTIL_CONFIG_COMPILER_CXX_RVALUE_REFERENCES
            non_copyable(const non_copyable&&) UTIL_CONFIG_DELETED_FUNCTION;
            non_copyable& operator=(const non_copyable&&) UTIL_CONFIG_DELETED_FUNCTION;
#endif
        };
    }
}


#endif /* NON_COPYABLE_H_ */

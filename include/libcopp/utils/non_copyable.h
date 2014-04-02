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


namespace copp {
    namespace utils {
        class non_copyable
        {
        protected:
            non_copyable(){}
            ~non_copyable(){}

        private:
            non_copyable(const non_copyable&);
            non_copyable& operator=(const non_copyable&);
        };
    }
}


#endif /* NON_COPYABLE_H_ */

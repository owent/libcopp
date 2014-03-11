/*
 * standard_int_key_allocator.h
 *
 *  Created on: 2014年3月10日
 *      Author: owent
 *
 *  Released under the MIT license
 */

#ifndef STANDARD_INT_KEY_ALLOCATOR_H_
#define STANDARD_INT_KEY_ALLOCATOR_H_

#include <stdint.h>

namespace copp {
    namespace detail {

        template<typename TKey = uint32_t>
        class standard_int_key_allocator
        {
        public:
            typedef TKey value_type;

        public:
            static const value_type npos = 0;

            value_type allocate(){
                static value_type id_s = npos;
                ++ id_s;
                while (npos == id_s)
                    ++ id_s;
                return id_s;
            }

            void deallocate(value_type key){
            }
        };
    }
}

#endif /* STANDARD_INT32_KEY_ALLOCATOR_H_ */

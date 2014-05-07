/*
 * standard_int_id_allocator.h
 *
 *  Created on: 2014年4月1日
 *      Author: owent
 *
 *  Released under the MIT license
 */

#ifndef _COTASK_CORE_STANDARD_INT_ID_ALLOCATOR_H_
#define _COTASK_CORE_STANDARD_INT_ID_ALLOCATOR_H_

#include <stdint.h>
#include <ctime>
#include <libcotask/impl/id_allocator_impl.h>

namespace cotask {
    namespace core {
        template<typename TKey = uint64_t>
        class standard_int_id_allocator
        {
        public:
            typedef cotask::impl::id_allocator<TKey> base_type;
            typedef typename base_type::value_type value_type;

            static const value_type npos = 0; /** invalid key **/
        public:
            value_type allocate() {
                static value_type start = 0;
                static const value_type end = static_cast<value_type>(1) << (sizeof(value_type) * 4);
                static value_type time_stamp = static_cast<value_type>(time(NULL));

                value_type ret = npos;
                while(npos == ret) {
                    // spin lock & wait next id segment
                    while (start >= end) {
                        value_type now_time = static_cast<value_type>(time(NULL));
                        if (time_stamp != now_time) {
                            time_stamp = now_time;
                            start = 0;
                        }
                    }

                    ret = gen_id(time_stamp, start ++);
                }

                return ret;
            }

            void deallocate(value_type) {
            }

        private:
            value_type gen_id(value_type time_stamp, value_type index) {
                return (time_stamp << (sizeof(value_type) * 4)) | index;
            }
        };
    }
}

#endif /* STANDARD_INT_ID_ALLOCATOR_H_ */

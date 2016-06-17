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

#include <libcopp/utils/atomic_int_type.h>
#include <libcotask/impl/id_allocator_impl.h>

namespace cotask {
    namespace core {

        /**
         * @biref allocate a id of specify type
         * @note we guarantee id will not repeated for a short time. 
         *       the time depennd the length of TKey.
         */
        template<typename TKey = uint64_t>
        class standard_int_id_allocator
        {
        public:
            typedef cotask::impl::id_allocator<TKey> base_type;
            typedef typename base_type::value_type value_type;

            static const value_type npos = 0; /** invalid key **/
        public:
            value_type allocate() UTIL_CONFIG_NOEXCEPT {
                static util::lock::atomic_int_type<value_type> seq_alloc(255);

                static const size_t seq_bits = sizeof(value_type) * 4;
                static const value_type time_mask = (static_cast<value_type>(1) << (sizeof(value_type) * 8 - seq_bits)) - 1;

                // always do not allocate 0 as a valid ID
                value_type ret = npos;
                while(npos == ret) {
                    value_type res = seq_alloc.load();
                    value_type time_part = res >> seq_bits;

                    value_type next_ret = res + 1;
                    value_type next_time_part = next_ret >> seq_bits;
                    if (0 == time_part || time_part != next_time_part) {
                        value_type now_time = time_part;
                        while (time_part == now_time) {
                            now_time = static_cast<value_type>(time(NULL)) & time_mask;
                        }

                        // if failed, maybe another thread do it
                        if (seq_alloc.compare_exchange_strong(res, now_time << seq_bits)) {
                            ret = now_time << seq_bits;
                        }
                    } else {
                        if (seq_alloc.compare_exchange_weak(res, next_ret)) {
                            ret = next_ret;
                        }
                    }
                }

                return ret;
            }

            void deallocate(value_type) UTIL_CONFIG_NOEXCEPT {}
        };
    }
}

#endif /* STANDARD_INT_ID_ALLOCATOR_H_ */

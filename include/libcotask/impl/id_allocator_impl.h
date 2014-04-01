/*
 * id_allocator_impl.h
 *
 *  Created on: 2014年4月1日
 *      Author: owent
 *
 *  Released under the MIT license
 */

#ifndef _COTASK_IMPL_ID_ALLOCATOR_IMPL_H_
#define _COTASK_IMPL_ID_ALLOCATOR_IMPL_H_


namespace cotask {
    namespace impl {
        template<typename Ty>
        class id_allocator
        {
        public:
            typedef Ty value_type;
            virtual ~id_allocator() = 0;
            virtual value_type allocate() = 0;
            virtual void deallocate(value_type) = 0;
        };
    }
}


#endif /* _COTASK_IMPL_ID_ALLOCATOR_IMPL_H_ */

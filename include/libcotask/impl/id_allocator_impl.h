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
            ~id_allocator() {};
            value_type allocate();
            void deallocate(value_type);
        };
    }
}


#endif /* _COTASK_IMPL_ID_ALLOCATOR_IMPL_H_ */

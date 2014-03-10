/*
 * pool_manager.h
 *
 *  Created on: 2014年3月10日
 *      Author: owent
 *
 *  Released under the MIT license
 */

#ifndef _COPP_COROUTINE_CORE_POOL_MANAGER_H_
#define _COPP_COROUTINE_CORE_POOL_MANAGER_H_

#include <map>

#include <libcopp/utils/features.h>

namespace copp {
    namespace utils {
        namespace detail{
            template<typename TCO, typename TIDALLOC>
            class pool_manager_node: public TCO {
            public:
                typedef TIDALLOC id_allocator_type;
                typedef typename id_allocator_type::value_type id_type;
                typedef TCO coroutine_type;
                typedef coroutine_type base_type;

            public:

                #if define(COPP_MACRO_ENABLE_VARIADIC_TEMPLATE) && COPP_MACRO_ENABLE_VARIADIC_TEMPLATE
                template<typename... TARGS>
                pool_manager_node(id_type id, TARGS... args): base_type(args...), id_(id) {}
                #else

                #endif
            private:
                id_type id_;
            };

            template<typename TCO, typename TIDALLOC,
                typename TDATA = std::map<typename TIDALLOC::value_type,
                pool_manager_node<TCO, TIDALLOC> > >
            class pool_mamanger {
            public:
                typedef TIDALLOC id_allocator_type;
                typedef typename id_allocator_type::value_type id_type;
                typedef TCO coroutine_type;
                typedef detail::pool_manager_node<TCO, TIDALLOC> node_type;
                typedef TDATA container_type;

            private:
                container_type container_;
            };
        }
    }
}


#endif /* POOL_MANAGER_H_ */

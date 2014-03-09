#ifndef _COPP_COROUTINE_CONTEXT_MANAGER_H_
#define _COPP_COROUTINE_CONTEXT_MANAGER_H_


#if defined(_MSC_VER) && (_MSC_VER >= 1020)
# pragma once
#endif

#include <cstddef>
#include <map>

namespace copp {

    namespace detail
    {
        template<typename TCO, typename TIDALLOC>
        class coroutine_mgr_node: public TCO
	{
        public:
            typename TIDALLOC id_allocator_type;
            typename typename id_allocator_type::value_type id_type;
            typename TCO coroutine_type;
            typename coroutine_type base_type;

        public:
#if define(COPP_MACRO_ENABLE_VARIADIC_TEMPLATE) && COPP_MACRO_ENABLE_VARIADIC_TEMPLATE
            template<typename... TARGS>
            coroutine_mgr_node(id_type id, TARGS... args): base_type(args...), id_(id){}

#else

#endif


        ptivate:
            id_type id_;
	};
    }

    template<typename TCO, typename TIDALLOC, 
        typename TDATA = std::map<typename TIDALLOC::value_type, detail::coroutine_mgr_node<TCO, TIDALLOC> > >
    class coroutine_manager
    {
    public:
        typename TIDALLOC id_allocator_type;
	typename typename id_allocator_type::value_type id_type;
	typename TCO coroutine_type;
	typename detail::coroutine_mgr_node<TCO, TIDALLOC> node_type;
	typename TDATA container_type;

    private:
        container_type container_;

    };
    

}

#endif

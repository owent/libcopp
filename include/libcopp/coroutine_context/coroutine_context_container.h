/**
 * coroutine context container
 */
#ifndef _COPP_COROUTINE_CONTEXT_COROUTINE_CONTEXT_CONTAINER_H_
#define _COPP_COROUTINE_CONTEXT_COROUTINE_CONTEXT_CONTAINER_H_


#if defined(_MSC_VER) && (_MSC_VER >= 1020)
# pragma once
#endif

#include <cstddef>

#include <libcopp/coroutine_context/coroutine_context_safe_base.h>
#include <libcopp/stack_context/stack_allocator.h>

namespace copp {
    namespace detail{
        template<typename TCOC, typename TALLOC>
        class coroutine_context_container : public TCOC {
        public:
            typedef TCOC coroutine_context_type;
            typedef TCOC base_type;
            typedef TALLOC allocator_type;

            coroutine_context_container() : base_type(), alloc_(){}
            coroutine_context_container(const allocator_type& alloc) : alloc_(alloc){}

            inline const allocator_type& get_allocator() const {
                return alloc_;
            }

            inline allocator_type& get_allocator() {
                return alloc_;
            }

        public:
            int create(coroutine_runnable_base* runner, std::size_t stack_size_, void(*func)(intptr_t) = NULL){
                alloc_.allocate(callee_stack_, stack_size_);
                return base_type::create(runner, func);
            }

            int create(coroutine_runnable_base* runner, void(*func)(intptr_t) = NULL){
                alloc_.allocate(callee_stack_, alloc_.default_stacksize());
                return base_type::create(runner, func);
            }

        private:
            allocator_type alloc_;
        };
    }

    typedef detail::coroutine_context_container<
        detail::coroutine_context_safe_base, 
        allocator::default_statck_allocator
    > coroutine_context_default;
}

#endif

/**
 * coroutine context container
 */
#ifndef COPP_COROUTINE_CONTEXT_COROUTINE_CONTEXT_CONTAINER_H
#define COPP_COROUTINE_CONTEXT_COROUTINE_CONTEXT_CONTAINER_H


#pragma once

#include <cstddef>

#include <libcopp/coroutine/coroutine_context_base.h>
#include <libcopp/stack/stack_allocator.h>
#include <libcopp/stack/stack_traits.h>
#include <libcopp/utils/errno.h>

namespace copp {
    namespace detail {

        /**
         * @brief coroutine container
         * contain stack context, stack allocator and runtime fcontext
         */
        template <typename TCOC, typename TALLOC>
        class coroutine_context_container : public TCOC {
        public:
            typedef TCOC coroutine_context_type;
            typedef TCOC base_type;
            typedef TALLOC allocator_type;

            COROUTINE_CONTEXT_BASE_USING_BASE(base_type)
        public:
            coroutine_context_container() UTIL_CONFIG_NOEXCEPT : base_type(), alloc_() {}
            coroutine_context_container(const allocator_type &alloc) UTIL_CONFIG_NOEXCEPT : alloc_(alloc) {}

            ~coroutine_context_container() { _reset_stack(); }

            /**
             * @brief get stack allocator
             * @return stack allocator
             */
            inline const allocator_type &get_allocator() const UTIL_CONFIG_NOEXCEPT { return alloc_; }

            /**
             * @brief get stack allocator
             * @return stack allocator
             */
            inline allocator_type &get_allocator() UTIL_CONFIG_NOEXCEPT { return alloc_; }


        protected:
            /**
             * @brief deallocate stack context
             */
            void _reset_stack() UTIL_CONFIG_NOEXCEPT {
                if (NULL == callee_stack_.sp || 0 == callee_stack_.size) {
                    return;
                }

                alloc_.deallocate(callee_stack_);
                callee_stack_.reset();
            }

        public:
            /**
             * @brief create and init coroutine with specify runner and specify stack size
             * @param runner runner
             * @param stack_size_ stack size
             * @param func fcontext callback(set NULL to use default callback)
             * @return COPP_EC_SUCCESS or error code
             */
            int create(coroutine_runnable_base *runner, std::size_t stack_size_,
                       void (*func)(::copp::fcontext::transfer_t) = NULL) UTIL_CONFIG_NOEXCEPT {
                if (NULL != callee_stack_.sp) {
                    return COPP_EC_ALREADY_INITED;
                }

                alloc_.allocate(callee_stack_, stack_size_);

                if (NULL == callee_stack_.sp) {
                    return COPP_EC_ALLOC_STACK_FAILED;
                }

                return base_type::create(runner, func);
            }

            /**
             * @brief create and init coroutine with specify runner and default stack size
             * @param runner runner
             * @param func fcontext callback(set NULL to use default callback)
             * @return COPP_EC_SUCCESS or error code
             */
            int create(coroutine_runnable_base *runner, void (*func)(::copp::fcontext::transfer_t) = NULL) UTIL_CONFIG_NOEXCEPT {
                if (NULL != callee_stack_.sp) {
                    return COPP_EC_ALREADY_INITED;
                }

                alloc_.allocate(callee_stack_, stack_traits::default_size());

                if (NULL == callee_stack_.sp) {
                    return COPP_EC_ALLOC_STACK_FAILED;
                }

                return base_type::create(runner, func);
            }

        private:
            coroutine_context_container(const coroutine_context_container &) UTIL_CONFIG_DELETED_FUNCTION;

        private:
            allocator_type alloc_; /** stack allocator **/
        };
    }

    typedef detail::coroutine_context_container<detail::coroutine_context_base, allocator::default_statck_allocator>
        coroutine_context_default;
}

#endif

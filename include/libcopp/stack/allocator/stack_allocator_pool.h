#ifndef COPP_STACKCONTEXT_ALLOCATOR_POOL_H
#define COPP_STACKCONTEXT_ALLOCATOR_POOL_H

#pragma once

#include <assert.h>
#include <cstddef>

#include <libcopp/utils/config/compiler_features.h>
#include <libcopp/utils/config/libcopp_build_features.h>
#include <libcopp/utils/features.h>
#include <libcopp/utils/std/smart_ptr.h>

#ifdef COPP_HAS_ABI_HEADERS
#include COPP_ABI_PREFIX
#endif

namespace copp {
    struct stack_context;

    namespace allocator {

        /**
         * @brief memory allocator
         * this allocator will maintain buffer using malloc/free function
         */
        template <typename TPool>
        class LIBCOPP_COPP_API_HEAD_ONLY stack_allocator_pool {
        public:
            typedef TPool pool_t;

        public:
            stack_allocator_pool() LIBCOPP_MACRO_NOEXCEPT {}
            stack_allocator_pool(const std::shared_ptr<pool_t> &p) LIBCOPP_MACRO_NOEXCEPT : pool_(p) {}
            ~stack_allocator_pool() {}

            /**
             * specify memory section allocated
             * @param start_ptr buffer start address
             * @param max_size buffer size
             * @note must be called before allocate operation
             */
            void attach(const std::shared_ptr<pool_t> &p) LIBCOPP_MACRO_NOEXCEPT { pool_ = p; }

            /**
             * allocate memory and attach to stack context [standard function]
             * @param ctx stack context
             * @param size ignored
             * @note size must less or equal than attached
             */
            void allocate(stack_context &ctx, std::size_t) LIBCOPP_MACRO_NOEXCEPT {
                assert(pool_);
                if (pool_) {
                    pool_->allocate(ctx);
                }
            }

            /**
             * deallocate memory from stack context [standard function]
             * @param ctx stack context
             */
            void deallocate(stack_context &ctx) LIBCOPP_MACRO_NOEXCEPT {
                assert(pool_);
                if (pool_) {
                    pool_->deallocate(ctx);
                }
            }

        private:
            std::shared_ptr<pool_t> pool_;
        };
    } // namespace allocator
} // namespace copp

#ifdef COPP_HAS_ABI_HEADERS
#include COPP_ABI_SUFFIX
#endif

#endif

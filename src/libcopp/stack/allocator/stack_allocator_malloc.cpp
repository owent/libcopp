#include <algorithm>
#include <assert.h>
#include <cstring>
#include <limits>
#include <memory>
#include <numeric>


#include "libcopp/stack/allocator/stack_allocator_malloc.h"
#include "libcopp/stack/stack_context.h"
#include "libcopp/stack/stack_traits.h"


#if defined(COPP_MACRO_USE_VALGRIND)
#include <valgrind/valgrind.h>
#endif

#ifdef COPP_HAS_ABI_HEADERS
#include COPP_ABI_PREFIX
#endif

namespace copp {
    namespace allocator {
        stack_allocator_malloc::stack_allocator_malloc() UTIL_CONFIG_NOEXCEPT {}
        stack_allocator_malloc::~stack_allocator_malloc() {}

        void stack_allocator_malloc::allocate(stack_context &ctx, std::size_t size) UTIL_CONFIG_NOEXCEPT {
            size = (std::max)(size, stack_traits::minimum_size());
            size = (std::min)(size, stack_traits::maximum_size());

            std::size_t size_ = stack_traits::round_to_page_size(size);

            void *start_ptr = malloc(size_);

            if (!start_ptr) {
                ctx.sp = NULL;
                return;
            }

            ctx.size = size_;
            ctx.sp = static_cast<char *>(start_ptr) + ctx.size; // stack down

#if defined(COPP_MACRO_USE_VALGRIND)
            ctx.valgrind_stack_id = VALGRIND_STACK_REGISTER(ctx.sp, start_ptr);
#endif
        }

        void stack_allocator_malloc::deallocate(stack_context &ctx) UTIL_CONFIG_NOEXCEPT {
            assert(ctx.sp);
            assert(stack_traits::minimum_size() <= ctx.size);
            assert(stack_traits::is_unbounded() || (stack_traits::maximum_size() >= ctx.size));

#if defined(COPP_MACRO_USE_VALGRIND)
            VALGRIND_STACK_DEREGISTER(ctx.valgrind_stack_id);
#endif
            void *start_ptr = static_cast<char *>(ctx.sp) - ctx.size;
            free(start_ptr);
        }
    }
}

#ifdef COPP_HAS_ABI_HEADERS
#include COPP_ABI_SUFFIX
#endif

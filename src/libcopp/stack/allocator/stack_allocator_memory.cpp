#include <cstring>
#include <algorithm>
#include <numeric>
#include <assert.h>
#include <limits>

#include "libcopp/stack/stack_context.h"
#include "libcopp/stack/stack_traits.h"
#include "libcopp/stack/allocator/stack_allocator_memory.h"

#if defined(COPP_MACRO_USE_VALGRIND)
#include <valgrind/valgrind.h>
#endif


#ifdef COPP_HAS_ABI_HEADERS
# include COPP_ABI_PREFIX
#endif

namespace copp { 
    namespace allocator {

        stack_allocator_memory::stack_allocator_memory() : start_ptr_(NULL), memory_size_(0){}

        stack_allocator_memory::stack_allocator_memory(void* start_ptr, std::size_t max_size) : start_ptr_(start_ptr), memory_size_(max_size){}

        stack_allocator_memory::~stack_allocator_memory() { }

        void stack_allocator_memory::attach(void* start_ptr, std::size_t max_size){
            start_ptr_ = start_ptr;
            memory_size_ = max_size;
        }

        void stack_allocator_memory::allocate(stack_context & ctx, std::size_t size)
        {
            if(NULL == start_ptr_) {
                ctx.sp = NULL;
                return;
            }

            size = (std::max)(size, stack_traits::minimum_size());
            size = (std::min)(size, stack_traits::maximum_size());
            size = (std::min)(size, memory_size_);

            std::size_t size_ = stack_traits::round_to_page_size(size);
            assert(size > 0 && size_ > 0 && size_ <= memory_size_);

            ctx.size = size_;
            ctx.sp = static_cast<char *>(start_ptr_) + ctx.size; // stack down
            
#if defined(COPP_MACRO_USE_VALGRIND)
            ctx.valgrind_stack_id = VALGRIND_STACK_REGISTER( ctx.sp, start_ptr);
#endif
        }

        void stack_allocator_memory::deallocate(stack_context & ctx)
        {
            assert(ctx.sp);
            assert(stack_traits::minimum_size() <= ctx.size);
            assert(stack_traits::is_unbounded() || (stack_traits::maximum_size() >= ctx.size));
            
#if defined(COPP_MACRO_USE_VALGRIND)
            VALGRIND_STACK_DEREGISTER( ctx.valgrind_stack_id);
#endif
        }

    } 
}

#ifdef COPP_HAS_ABI_HEADERS
# include COPP_ABI_SUFFIX
#endif

#include <cstring>
#include <algorithm>
#include <numeric>
#include <assert.h>
#include <limits>

#include "libcopp/stack_context/stack_context.h"
#include "libcopp/stack_context/allocator/stack_allocator_memory.h"

// x86_64
// test x86_64 before i386 because icc might
// define __i686__ for x86_64 too
#if defined(__x86_64__) || defined(__x86_64) \
    || defined(__amd64__) || defined(__amd64) \
    || defined(_M_X64) || defined(_M_AMD64)

// Windows seams not to provide a constant or function
// telling the minimal stacksize
# define MIN_STACKSIZE  8 * 1024
#else
# define MIN_STACKSIZE  4 * 1024
#endif

#ifdef COPP_HAS_ABI_HEADERS
# include COPP_ABI_PREFIX
#endif

namespace copp { 
    namespace allocator {
        namespace sys
        {
            static std::size_t pagesize()
            {
                return MIN_STACKSIZE;
            }

            static std::size_t page_count(std::size_t stacksize)
            {
                return static_cast<std::size_t>((stacksize - 1) / pagesize() + 1);
            }
        }

        stack_allocator_memory::stack_allocator_memory(void* start_ptr, std::size_t max_size) : start_ptr_(start_ptr), memory_size_(max_size){}

        stack_allocator_memory::~stack_allocator_memory() { }

        bool stack_allocator_memory::is_stack_unbound() { return true; }

        std::size_t stack_allocator_memory::default_stacksize() {
            std::size_t size = 64 * 1024; // 64 KB
            if (is_stack_unbound())
                return (std::max)(size, minimum_stacksize() );

            assert(maximum_stacksize() >= minimum_stacksize());
            return maximum_stacksize() == minimum_stacksize()
                ? minimum_stacksize()
                : (std::min)(size, maximum_stacksize());
        }

        std::size_t stack_allocator_memory::minimum_stacksize() { return MIN_STACKSIZE; }

        std::size_t stack_allocator_memory::maximum_stacksize() {
            assert(!is_stack_unbound());
            return std::numeric_limits<std::size_t>::max();
        }

        void stack_allocator_memory::allocate(stack_context & ctx, std::size_t size)
        {
            assert(NULL != start_ptr_);

            size = (std::max)(size, minimum_stacksize());
            size = (std::min)(size, maximum_stacksize());
            size = (std::min)(size, memory_size_);

            std::size_t page_number(sys::page_count(size));
            std::size_t size_ = page_number * sys::pagesize();
            assert(size > 0 && size_ > 0 && size_ < memory_size_);

            ctx.size = size_;
            ctx.sp = static_cast<char *>(start_ptr_) +ctx.size; // stack down
        }

        void stack_allocator_memory::deallocate(stack_context & ctx)
        {
            assert(ctx.sp);
            assert(minimum_stacksize() <= ctx.size);
            assert(is_stack_unbound() || (maximum_stacksize() >= ctx.size));
        }

    } 
}

#ifdef COPP_HAS_ABI_HEADERS
# include COPP_ABI_SUFFIX
#endif

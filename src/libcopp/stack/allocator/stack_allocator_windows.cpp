#include <cstring>
#include <algorithm>
#include <numeric>
#include <assert.h>
#include <limits>

extern "C" {
#include <windows.h>
}

#include "libcopp/stack/stack_context.h"
#include "libcopp/stack/allocator/stack_allocator_windows.h"

# if defined(COPP_MACRO_COMPILER_MSVC)
# pragma warning(push)
# pragma warning(disable:4244 4267)
# endif

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
            static SYSTEM_INFO system_info_()
            {
                SYSTEM_INFO si;
                ::GetSystemInfo(&si);
                return si;
            }

            static SYSTEM_INFO system_info()
            {
                static SYSTEM_INFO si = system_info_();
                return si;
            }

            static std::size_t pagesize()
            {
                return static_cast<std::size_t>(system_info().dwPageSize);
            }

            static std::size_t round_to_page_size(std::size_t stacksize)
            {
                // page size must be 2^N
                return static_cast<std::size_t>((stacksize + pagesize() - 1) & (~(pagesize() - 1)));
            }
        }

        stack_allocator_windows::stack_allocator_windows() { }

        stack_allocator_windows::~stack_allocator_windows() { }

        bool stack_allocator_windows::is_stack_unbound() { return true; }

        std::size_t stack_allocator_windows::default_stacksize() {
            std::size_t size = 64 * 1024; // 64 KB
            if (is_stack_unbound())
                return (std::max)(size, minimum_stacksize() );

            assert(is_stack_unbound() || maximum_stacksize() >= minimum_stacksize());
            return is_stack_unbound() ? 
                size
                : (std::min)(size, maximum_stacksize());
        }

        std::size_t stack_allocator_windows::minimum_stacksize() { return MIN_STACKSIZE; }

        std::size_t stack_allocator_windows::maximum_stacksize() {
            assert(is_stack_unbound());
            return SIZE_MAX;
        }

        void stack_allocator_windows::allocate(stack_context & ctx, std::size_t size)
        {
            size = (std::max)(size, minimum_stacksize());
            size = is_stack_unbound() ? size: (std::min)(size, maximum_stacksize());

            std::size_t size_ = sys::round_to_page_size(size) + sys::pagesize();
            assert(size > 0 && size_ > 0);

            void* start_ptr = ::VirtualAlloc(0, size_, MEM_COMMIT, PAGE_READWRITE);
            if (!start_ptr) {
                ctx.sp = NULL;
                return;
            }

            // memset(start_ptr, 0, size_);
            DWORD old_options;
            ::VirtualProtect(start_ptr, sys::pagesize(), PAGE_READWRITE | PAGE_GUARD, &old_options);

            ctx.size = size_;
            ctx.sp = static_cast<char *>(start_ptr) + ctx.size; // stack down
        }

        void stack_allocator_windows::deallocate(stack_context & ctx)
        {
            assert(ctx.sp);
            assert(minimum_stacksize() <= ctx.size);
            assert(is_stack_unbound() || (maximum_stacksize() >= ctx.size));

            void* start_ptr = static_cast< char * >(ctx.sp) - ctx.size;
            ::VirtualFree(start_ptr, 0, MEM_RELEASE);
        }

    } 
}

#ifdef COPP_HAS_ABI_HEADERS
# include COPP_ABI_SUFFIX
#endif

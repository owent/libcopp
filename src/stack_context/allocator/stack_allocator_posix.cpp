extern "C" {
#include <fcntl.h>
#include <signal.h>
#include <sys/mman.h>
#include <sys/resource.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>
}

#include <cstring>
#include <algorithm>
#include <numeric>
#include <assert.h>
#include <limits>


#include "libcopp/stack_context/stack_context.h"
#include "libcopp/fcontext/fcontext.hpp"
#include "libcopp/stack_context/allocator/stack_allocator_posix.h"

#if !defined (SIGSTKSZ)
# define SIGSTKSZ (8 * 1024)
# define UDEF_SIGSTKSZ
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
                std::size_t size = ::sysconf(_SC_PAGESIZE);
                return size;
            }

            static std::size_t round_to_page_size(std::size_t stacksize)
            {
                // page size must be 2^N
                return static_cast<std::size_t>((stacksize + pagesize() - 1) & (~(pagesize() - 1)));
            }

            rlimit stacksize_limit_()
            {
                rlimit limit;
                // conforming to POSIX.1-2001
                ::getrlimit( RLIMIT_STACK, & limit);
                return limit;
            }

            rlimit stacksize_limit()
            {
                static rlimit limit = stacksize_limit_();
                return limit;
            }
        }

        stack_allocator_posix::stack_allocator_posix() { }

        stack_allocator_posix::~stack_allocator_posix() { }

        bool stack_allocator_posix::is_stack_unbound() {
            return RLIM_INFINITY == sys::stacksize_limit().rlim_max;
        }

        std::size_t stack_allocator_posix::default_stacksize() {
            std::size_t size = 8 * minimum_stacksize(); // 64 KB
            if (is_stack_unbound())
                return size;

            assert(maximum_stacksize() >= minimum_stacksize());
            return maximum_stacksize() == size
                ? size
                : (std::min)(size, maximum_stacksize());
        }

        std::size_t stack_allocator_posix::minimum_stacksize() {
            return SIGSTKSZ + sizeof(fcontext::fcontext_t) + 15;
        }

        std::size_t stack_allocator_posix::maximum_stacksize() {
            if(is_stack_unbound())
                return std::numeric_limits<std::size_t>::max();
            return sys::stacksize_limit().rlim_max;
        }

        void stack_allocator_posix::allocate(stack_context & ctx, std::size_t size)
        {
            size = (std::max)(size, minimum_stacksize());
            size = (std::min)(size, maximum_stacksize());

            std::size_t size_ = sys::round_to_page_size(size) + sys::pagesize(); // add one protected page
            assert(size > 0 && size_ > 0);

            const int fd(::open("/dev/zero", O_RDONLY));
            assert(-1 != fd);

            // conform to POSIX.4 (POSIX.1b-1993, _POSIX_C_SOURCE=199309L)
            void* start_ptr = ::mmap( 0, size_, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANON, -1, 0);
            ::close(fd);

            if (!start_ptr) throw std::bad_alloc();

            memset(start_ptr, 0, size_);
            ::mprotect( start_ptr, sys::pagesize(), PROT_NONE);

            ctx.size = size_;
            ctx.sp = static_cast<char *>(start_ptr) +ctx.size; // stack down
        }

        void stack_allocator_posix::deallocate(stack_context & ctx)
        {
            assert(ctx.sp);
            assert(minimum_stacksize() <= ctx.size);
            assert(is_stack_unbound() || (maximum_stacksize() >= ctx.size));

            void* start_ptr = static_cast< char * >(ctx.sp) - ctx.size;
            ::munmap(start_ptr, ctx.size);
        }

    } 
}

#ifdef COPP_HAS_ABI_HEADERS
# include COPP_ABI_SUFFIX
#endif

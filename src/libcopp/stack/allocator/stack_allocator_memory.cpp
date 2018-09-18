#include <algorithm>
#include <assert.h>
#include <cstring>
#include <limits>
#include <numeric>

#include "libcopp/stack/allocator/stack_allocator_memory.h"
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

        stack_allocator_memory::stack_allocator_memory() UTIL_CONFIG_NOEXCEPT : start_ptr_(NULL), memory_size_(0), is_used_(false) {}

        stack_allocator_memory::stack_allocator_memory(void *start_ptr, std::size_t max_size) UTIL_CONFIG_NOEXCEPT : start_ptr_(start_ptr),
                                                                                                                     memory_size_(max_size),
                                                                                                                     is_used_(false) {}

        stack_allocator_memory::stack_allocator_memory(stack_allocator_memory &other) UTIL_CONFIG_NOEXCEPT : start_ptr_(NULL),
                                                                                                             memory_size_(0),
                                                                                                             is_used_(false) {
            if (!other.is_used_) {
                swap(other);
            }
        }

#if defined(UTIL_CONFIG_COMPILER_CXX_RVALUE_REFERENCES) && UTIL_CONFIG_COMPILER_CXX_RVALUE_REFERENCES
        stack_allocator_memory::stack_allocator_memory(stack_allocator_memory &&other) UTIL_CONFIG_NOEXCEPT : start_ptr_(NULL),
                                                                                                              memory_size_(0),
                                                                                                              is_used_(false) {
            if (!other.is_used_) {
                swap(other);
            }
        }
#endif

        stack_allocator_memory::~stack_allocator_memory() {}

        stack_allocator_memory &stack_allocator_memory::operator=(stack_allocator_memory &other) UTIL_CONFIG_NOEXCEPT {
            if (!other.is_used_) {
                swap(other);
            }
            return *this;
        }
#if defined(UTIL_CONFIG_COMPILER_CXX_RVALUE_REFERENCES) && UTIL_CONFIG_COMPILER_CXX_RVALUE_REFERENCES
        stack_allocator_memory &stack_allocator_memory::operator=(stack_allocator_memory &&other) UTIL_CONFIG_NOEXCEPT {
            if (!other.is_used_) {
                swap(other);
            }
            return *this;
        }
#endif

        void stack_allocator_memory::swap(stack_allocator_memory &other) {
            using std::swap;
            swap(start_ptr_, other.start_ptr_);
            swap(memory_size_, other.memory_size_);
            swap(is_used_, other.is_used_);
        }

        void stack_allocator_memory::attach(void *start_ptr, std::size_t max_size) UTIL_CONFIG_NOEXCEPT {
            start_ptr_   = start_ptr;
            memory_size_ = max_size;
            is_used_     = false;
        }

        void stack_allocator_memory::allocate(stack_context &ctx, std::size_t size) UTIL_CONFIG_NOEXCEPT {
            if (NULL == start_ptr_ || is_used_) {
                ctx.sp = NULL;
                return;
            }

            size = (std::max)(size, stack_traits::minimum_size());
            size = (std::min)(size, stack_traits::maximum_size());
            size = (std::min)(size, memory_size_);

            std::size_t size_ = stack_traits::round_to_page_size(size);
            assert(size > 0 && size_ > 0 && size_ <= memory_size_);

            ctx.size = size_;
            ctx.sp   = static_cast<char *>(start_ptr_) + ctx.size; // stack down

#if defined(COPP_MACRO_USE_VALGRIND)
            ctx.valgrind_stack_id = VALGRIND_STACK_REGISTER(ctx.sp, start_ptr_);
#endif
            is_used_ = true;
        }

        void stack_allocator_memory::deallocate(stack_context &ctx) UTIL_CONFIG_NOEXCEPT {
            assert(ctx.sp);
            assert(stack_traits::minimum_size() <= ctx.size);
            assert(stack_traits::is_unbounded() || (stack_traits::maximum_size() >= ctx.size));

#if defined(COPP_MACRO_USE_VALGRIND)
            VALGRIND_STACK_DEREGISTER(ctx.valgrind_stack_id);
#endif
            is_used_ = false;
        }
    } // namespace allocator
} // namespace copp

#ifdef COPP_HAS_ABI_HEADERS
#include COPP_ABI_SUFFIX
#endif

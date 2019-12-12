/**
 * coroutine context container
 */
#ifndef COPP_COROUTINE_CONTEXT_COROUTINE_CONTEXT_CONTAINER_H
#define COPP_COROUTINE_CONTEXT_COROUTINE_CONTEXT_CONTAINER_H


#pragma once

#include <cstddef>

#include <libcopp/coroutine/coroutine_context.h>
#include <libcopp/stack/stack_allocator.h>
#include <libcopp/stack/stack_traits.h>
#include <libcopp/utils/errno.h>

namespace copp {
    /**
     * @brief coroutine container
     * contain stack context, stack allocator and runtime fcontext
     */
    template <typename TALLOC>
    class coroutine_context_container : public coroutine_context {
    public:
        typedef coroutine_context                           coroutine_context_type;
        typedef coroutine_context                           base_type;
        typedef TALLOC                                      allocator_type;
        typedef coroutine_context_container<allocator_type> this_type;
        typedef libcopp::util::intrusive_ptr<this_type>     ptr_t;
        typedef coroutine_context::callback_t               callback_t;

        COROUTINE_CONTEXT_BASE_USING_BASE(base_type)

    private:
        coroutine_context_container(const allocator_type &alloc) UTIL_CONFIG_NOEXCEPT : alloc_(alloc), ref_count_(0) {}

#if defined(UTIL_CONFIG_COMPILER_CXX_RVALUE_REFERENCES) && UTIL_CONFIG_COMPILER_CXX_RVALUE_REFERENCES
        coroutine_context_container(allocator_type &&alloc) UTIL_CONFIG_NOEXCEPT : alloc_(COPP_MACRO_STD_MOVE(alloc)), ref_count_(0) {}
#endif

    public:
        ~coroutine_context_container() {}

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

    public:
        /**
         * @brief create and init coroutine with specify runner and specify stack size
         * @param runner runner
         * @param stack_sz stack size
         * @param private_buffer_size private buffer size
         * @param coroutine_size extend buffer before coroutine
         * @return COPP_EC_SUCCESS or error code
         */
        static ptr_t create(
#if defined(UTIL_CONFIG_COMPILER_CXX_RVALUE_REFERENCES) && UTIL_CONFIG_COMPILER_CXX_RVALUE_REFERENCES
            callback_t &&runner,
#else
            const callback_t &runner,
#endif
            allocator_type &alloc, size_t stack_sz = 0, size_t private_buffer_size = 0, size_t coroutine_size = 0) UTIL_CONFIG_NOEXCEPT {
            ptr_t ret;
            if (0 == stack_sz) {
                stack_sz = stack_traits::default_size();
            }

            // padding to sizeof size_t
            coroutine_size               = align_address_size(coroutine_size);
            const size_t this_align_size = align_address_size(sizeof(this_type));
            coroutine_size += this_align_size;
            private_buffer_size = coroutine_context::align_private_data_size(private_buffer_size);

            if (stack_sz <= coroutine_size + private_buffer_size) {
                return ret;
            }

            stack_context callee_stack;
            alloc.allocate(callee_stack, stack_sz);

            if (NULL == callee_stack.sp) {
                return ret;
            }

            // placement new
            unsigned char *this_addr = reinterpret_cast<unsigned char *>(callee_stack.sp);
            // stack down
            this_addr -= private_buffer_size + this_align_size;
            ret.reset(new ((void *)this_addr) this_type(COPP_MACRO_STD_MOVE(alloc)));

            // callee_stack and alloc unavailable any more.
            if (ret) {
                ret->alloc_        = COPP_MACRO_STD_MOVE(alloc);
                ret->callee_stack_ = COPP_MACRO_STD_MOVE(callee_stack);
            } else {
                alloc.deallocate(callee_stack);
                return ret;
            }

            // after this call runner will be unavailable
            callback_t callback(COPP_MACRO_STD_MOVE(runner));
            if (coroutine_context::create(ret.get(), callback, ret->callee_stack_, coroutine_size, private_buffer_size) < 0) {
                ret.reset();
            }

            return ret;
        }

        template <class TRunner>
        static inline ptr_t create(TRunner *runner, allocator_type &alloc, size_t stack_size = 0, size_t private_buffer_size = 0,
                                   size_t coroutine_size = 0) UTIL_CONFIG_NOEXCEPT {
            if (UTIL_CONFIG_NULLPTR == runner) {
                return create(callback_t(), alloc, stack_size, private_buffer_size, coroutine_size);
            }

            typedef int (TRunner::*runner_fn_t)(void *);
            runner_fn_t fn = &TRunner::operator();
            return create(std::bind(fn, runner, std::placeholders::_1), alloc, stack_size, private_buffer_size, coroutine_size);
        }

        static inline ptr_t create(int (*fn)(void *), allocator_type &alloc, size_t stack_size = 0, size_t private_buffer_size = 0,
                                   size_t coroutine_size = 0) UTIL_CONFIG_NOEXCEPT {
            if (UTIL_CONFIG_NULLPTR == fn) {
                return create(callback_t(), alloc, stack_size, private_buffer_size, coroutine_size);
            }

            return create(callback_t(fn), alloc, stack_size, private_buffer_size, coroutine_size);
        }

        static ptr_t create(
#if defined(UTIL_CONFIG_COMPILER_CXX_RVALUE_REFERENCES) && UTIL_CONFIG_COMPILER_CXX_RVALUE_REFERENCES
            callback_t &&runner,
#else
            const callback_t &runner,
#endif
            size_t stack_size = 0, size_t private_buffer_size = 0, size_t coroutine_size = 0) UTIL_CONFIG_NOEXCEPT {
            allocator_type alloc;
            return create(COPP_MACRO_STD_MOVE(runner), alloc, stack_size, private_buffer_size, coroutine_size);
        }

        template <class TRunner>
        static inline ptr_t create(TRunner *runner, size_t stack_size = 0, size_t private_buffer_size = 0,
                                   size_t coroutine_size = 0) UTIL_CONFIG_NOEXCEPT {
            typedef int (TRunner::*runner_fn_t)(void *);
            runner_fn_t fn = &TRunner::operator();
            return create(std::bind(fn, runner, std::placeholders::_1), stack_size, private_buffer_size, coroutine_size);
        }

        static inline ptr_t create(int (*fn)(void *), size_t stack_size = 0, size_t private_buffer_size = 0,
                                   size_t coroutine_size = 0) UTIL_CONFIG_NOEXCEPT {
            return create(callback_t(fn), stack_size, private_buffer_size, coroutine_size);
        }

        inline size_t use_count() const UTIL_CONFIG_NOEXCEPT { return ref_count_.load(); }

    private:
        coroutine_context_container(const coroutine_context_container &) UTIL_CONFIG_DELETED_FUNCTION;

    private:
        friend void intrusive_ptr_add_ref(this_type *p) {
            if (p == UTIL_CONFIG_NULLPTR) {
                return;
            }

            ++p->ref_count_;
        }

        friend void intrusive_ptr_release(this_type *p) {
            if (p == UTIL_CONFIG_NULLPTR) {
                return;
            }

            size_t left = --p->ref_count_;
            if (0 == left) {
                allocator_type copy_alloc(COPP_MACRO_STD_MOVE(p->alloc_));
                stack_context  copy_stack(COPP_MACRO_STD_MOVE(p->callee_stack_));

                // then destruct object and reset data
                p->~coroutine_context_container();

                // final, recycle stack buffer
                copy_alloc.deallocate(copy_stack);
            }
        }

    private:
        allocator_type alloc_; /** stack allocator **/
#if defined(LIBCOPP_DISABLE_ATOMIC_LOCK) && LIBCOPP_DISABLE_ATOMIC_LOCK
        libcopp::util::lock::atomic_int_type<libcopp::util::lock::unsafe_int_type<size_t> > ref_count_; /** status **/
#else
        libcopp::util::lock::atomic_int_type<size_t> ref_count_; /** status **/
#endif
    };

    typedef coroutine_context_container<allocator::default_statck_allocator> coroutine_context_default;
} // namespace copp

#endif

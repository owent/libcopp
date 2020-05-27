#ifndef COPP_COROUTINE_CONTEXT_COROUTINE_CONTEXT_FIBER_BASE_H
#define COPP_COROUTINE_CONTEXT_COROUTINE_CONTEXT_FIBER_BASE_H

#pragma once

#include <cstddef>

#include "coroutine_context.h"

#if defined(LIBCOPP_MACRO_ENABLE_WIN_FIBER) && LIBCOPP_MACRO_ENABLE_WIN_FIBER

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif

#include <Windows.h>

namespace copp {
    /**
     * @brief base type of all coroutine context of windows fiber
     */
    class coroutine_context_fiber : public coroutine_context_base {
    public:
        typedef libcopp::util::intrusive_ptr<coroutine_context_fiber> ptr_t;

#if defined(UTIL_CONFIG_COMPILER_CXX_ALIAS_TEMPLATES) && UTIL_CONFIG_COMPILER_CXX_ALIAS_TEMPLATES
        using callback_t = coroutine_context_base::callback_t;
        using status_t   = coroutine_context_base::status_t;
        using flag_t     = coroutine_context_base::flag_t;
#else
        typedef coroutine_context_base::callback_t callback_t;
        typedef coroutine_context_base::status_t   status_t;
        typedef coroutine_context_base::flag_t     flag_t;
#endif

    private:
        using coroutine_context_base::runner_ret_code_;
        using coroutine_context_base::flags_;
        using coroutine_context_base::runner_;
        using coroutine_context_base::priv_data_;
        using coroutine_context_base::private_buffer_size_;
        using coroutine_context_base::status_;
#if defined(LIBCOPP_MACRO_ENABLE_STD_EXCEPTION_PTR) && LIBCOPP_MACRO_ENABLE_STD_EXCEPTION_PTR
        using coroutine_context_base::unhandle_exception_;
#endif

        struct jump_src_data_t {
            coroutine_context_fiber *from_co;
            LPVOID                   from_fiber;
            coroutine_context_fiber *to_co;
            LPVOID                   to_fiber;
            void                    *priv_data;
        };

        friend struct LIBCOPP_COPP_API_HEAD_ONLY libcopp_fiber_inner_api_helper;
        friend struct LIBCOPP_COPP_API_HEAD_ONLY fiber_context_tls_data_t;

    protected:
        LPVOID caller_; /** caller runtime context **/
        LPVOID callee_; /** callee runtime context **/

        stack_context callee_stack_; /** callee stack context **/

    protected:
        LIBCOPP_COPP_API coroutine_context_fiber() LIBCOPP_MACRO_NOEXCEPT;

    public:
        LIBCOPP_COPP_API ~coroutine_context_fiber();

    private:
        coroutine_context_fiber(const coroutine_context_fiber &) UTIL_CONFIG_DELETED_FUNCTION;
        coroutine_context_fiber &operator=(const coroutine_context_fiber &) UTIL_CONFIG_DELETED_FUNCTION;

#if defined(UTIL_CONFIG_COMPILER_CXX_RVALUE_REFERENCES) && UTIL_CONFIG_COMPILER_CXX_RVALUE_REFERENCES
        coroutine_context_fiber(const coroutine_context_fiber &&) UTIL_CONFIG_DELETED_FUNCTION;
        coroutine_context_fiber &operator=(const coroutine_context_fiber &&) UTIL_CONFIG_DELETED_FUNCTION;
#endif

    public:
        /**
         * @brief create coroutine context at stack context callee_
         * @param runner runner
         * @param callee_stack stack context
         * @param coroutine_size size of coroutine object
         * @param private_buffer_size size of private buffer
         * @return COPP_EC_SUCCESS or error code
         */
        static LIBCOPP_COPP_API int create(coroutine_context_fiber *p, callback_t &runner, const stack_context &callee_stack,
                                           size_t coroutine_size, size_t private_buffer_size,
                                           size_t stack_reserve_size_of_fiber = 0) LIBCOPP_MACRO_NOEXCEPT;

        template <typename TRunner>
        static LIBCOPP_COPP_API_HEAD_ONLY int create(coroutine_context_fiber *p, TRunner *runner, const stack_context &callee_stack,
                                                     size_t coroutine_size, size_t private_buffer_size,
                                                     size_t stack_reserve_size_of_fiber = 0) LIBCOPP_MACRO_NOEXCEPT {
            return create(p, std::bind(&TRunner::operator(), runner, std::placeholders::_1), callee_stack, coroutine_size,
                          private_buffer_size, stack_reserve_size_of_fiber);
        }

        /**
         * @brief start coroutine
         * @param priv_data private data, will be passed to runner operator() or return to yield
         * @exception if exception is enabled, it will throw all unhandled exception after resumed
         * @return COPP_EC_SUCCESS or error code
         */
        LIBCOPP_COPP_API int start(void *priv_data = UTIL_CONFIG_NULLPTR);

#if defined(LIBCOPP_MACRO_ENABLE_STD_EXCEPTION_PTR) && LIBCOPP_MACRO_ENABLE_STD_EXCEPTION_PTR
        /**
         * @brief start coroutine
         * @param unhandled set exception_ptr of unhandled exception if it's exists
         * @param priv_data private data, will be passed to runner operator() or return to yield
         * @return COPP_EC_SUCCESS or error code
         */
        LIBCOPP_COPP_API int start(std::exception_ptr& unhandled, void *priv_data = UTIL_CONFIG_NULLPTR) LIBCOPP_MACRO_NOEXCEPT;
#endif

        /**
         * @brief resume coroutine
         * @param priv_data private data, will be passed to runner operator() or return to yield
         * @exception if exception is enabled, it will throw all unhandled exception after resumed
         * @return COPP_EC_SUCCESS or error code
         */
        LIBCOPP_COPP_API int resume(void *priv_data = UTIL_CONFIG_NULLPTR);

#if defined(LIBCOPP_MACRO_ENABLE_STD_EXCEPTION_PTR) && LIBCOPP_MACRO_ENABLE_STD_EXCEPTION_PTR
        /**
         * @brief resume coroutine
         * @param unhandled set exception_ptr of unhandled exception if it's exists
         * @param priv_data private data, will be passed to runner operator() or return to yield
         * @return COPP_EC_SUCCESS or error code
         */
        LIBCOPP_COPP_API int resume(std::exception_ptr& unhandled, void *priv_data = UTIL_CONFIG_NULLPTR) LIBCOPP_MACRO_NOEXCEPT;
#endif


        /**
         * @brief yield coroutine
         * @param priv_data private data, if not NULL, will get the value from start(priv_data) or resume(priv_data)
         * @return COPP_EC_SUCCESS or error code
         */
        LIBCOPP_COPP_API int yield(void **priv_data = UTIL_CONFIG_NULLPTR) LIBCOPP_MACRO_NOEXCEPT;
    };

    namespace this_fiber {
        /**
         * @brief get current coroutine
         * @see detail::coroutine_context_fiber
         * @return pointer of current coroutine, if not in coroutine, return NULL
         */
        LIBCOPP_COPP_API coroutine_context_fiber *get_coroutine() LIBCOPP_MACRO_NOEXCEPT;

        /**
         * @brief get current coroutine and try to convert type
         * @see get_coroutine
         * @see detail::coroutine_context_fiber
         * @return pointer of current coroutine, if not in coroutine or fail to convert type, return NULL
         */
        template <typename Tc>
        LIBCOPP_COPP_API_HEAD_ONLY Tc *get() {
            return static_cast<Tc *>(get_coroutine());
        }

        /**
         * @brief yield current coroutine
         * @param priv_data private data, if not NULL, will get the value from start(priv_data) or resume(priv_data)
         * @return 0 or error code
         */
        LIBCOPP_COPP_API int yield(void **priv_data = UTIL_CONFIG_NULLPTR) LIBCOPP_MACRO_NOEXCEPT;
    } // namespace this_coroutine
} // namespace copp

#endif

#endif

#ifndef _COPP_COROUTINE_CONTEXT_COROUTINE_CONTEXT_BASE_H_
#define _COPP_COROUTINE_CONTEXT_COROUTINE_CONTEXT_BASE_H_

#pragma once

#include <cstddef>

#include <libcopp/fcontext/all.hpp>
#include <libcopp/stack/stack_context.h>
#include <libcopp/utils/atomic_int_type.h>
#include <libcopp/utils/features.h>
#include <libcopp/utils/non_copyable.h>
#include <libcopp/utils/std/functional.h>
#include <libcopp/utils/std/smart_ptr.h>

#ifdef COPP_MACRO_USE_SEGMENTED_STACKS
#define COROUTINE_CONTEXT_BASE_USING_BASE_SEGMENTED_STACKS(base_type) using base_type::caller_stack_;
#else
#define COROUTINE_CONTEXT_BASE_USING_BASE_SEGMENTED_STACKS(base_type)
#endif

#define COROUTINE_CONTEXT_BASE_USING_BASE(base_type) \
protected:                                           \
    using base_type::caller_;                        \
    using base_type::callee_;                        \
    using base_type::callee_stack_;                  \
    COROUTINE_CONTEXT_BASE_USING_BASE_SEGMENTED_STACKS(base_type)

namespace copp {
    /**
     * @brief base type of all coroutine context
     */
    class coroutine_context : utils::non_copyable {
    public:
        typedef std::intrusive_ptr<coroutine_context> ptr_t;
        typedef std::function<int(void *)> callback_t;

        /**
        * @brief status of safe coroutine context base
        */
        struct status_t {
            enum type {
                EN_CRS_INVALID = 0, //!< EN_CRS_INVALID
                EN_CRS_READY,       //!< EN_CRS_READY
                EN_CRS_RUNNING,     //!< EN_CRS_RUNNING
                EN_CRS_FINISHED,    //!< EN_CRS_FINISHED
                EN_CRS_EXITED,      //!< EN_CRS_EXITED
            };
        };

        struct flag_t {
            enum type {
                EN_CFT_UNKNOWN = 0,
                EN_CFT_FINISHED = 0x01,
            };
        };

    private:
        int runner_ret_code_; /** coroutine return code **/
        int flags_;           /** flags **/
        callback_t runner_;   /** coroutine runner **/
        void *priv_data_;
        size_t private_buffer_size_;

        struct jump_src_data_t {
            coroutine_context *from_co;
            coroutine_context *to_co;
            void *priv_data;
        };

    protected:
        fcontext::fcontext_t caller_; /** caller runtime context **/
        fcontext::fcontext_t callee_; /** callee runtime context **/

        stack_context callee_stack_; /** callee stack context **/
#ifdef COPP_MACRO_USE_SEGMENTED_STACKS
        stack_context caller_stack_; /** caller stack context **/
#endif

    private:
#if defined(PROJECT_DISABLE_MT) && PROJECT_DISABLE_MT
        util::lock::atomic_int_type<util::lock::unsafe_int_type<int> > status_; /** status **/
#else
        util::lock::atomic_int_type<int> status_; /** status **/
#endif

    protected:
        coroutine_context() UTIL_CONFIG_NOEXCEPT;

    public:
        ~coroutine_context();

    public:
        /**
            * @brief create coroutine context at stack context callee_
            * @param runner runner
            * @param callee_stack stack context
            * @param coroutine_size size of coroutine object
            * @param private_buffer_size size of private buffer
            * @return COPP_EC_SUCCESS or error code
            */
        static int create(coroutine_context *p, callback_t &runner, const stack_context &callee_stack, size_t coroutine_size,
                          size_t private_buffer_size) UTIL_CONFIG_NOEXCEPT;

        template <typename TRunner>
        static int create(coroutine_context *p, TRunner *runner, const stack_context &callee_stack, size_t coroutine_size,
                          size_t private_buffer_size) UTIL_CONFIG_NOEXCEPT {
            return create(p, std::bind(&TRunner::operator(), runner, std::placeholders::_1), callee_stack, coroutine_size,
                          private_buffer_size);
        }

        /**
            * @brief start coroutine
            * @param priv_data private data, will be passed to runner operator() or return to yield
            * @return COPP_EC_SUCCESS or error code
            */
        int start(void *priv_data = UTIL_CONFIG_NULLPTR);

        /**
            * @brief resume coroutine
            * @param priv_data private data, will be passed to runner operator() or return to yield
            * @return COPP_EC_SUCCESS or error code
            */
        int resume(void *priv_data = UTIL_CONFIG_NULLPTR);


        /**
            * @brief yield coroutine
            * @param priv_data private data, if not NULL, will get the value from start(priv_data) or resume(priv_data)
            * @return COPP_EC_SUCCESS or error code
            */
        int yield(void **priv_data = UTIL_CONFIG_NULLPTR);


    protected:
        /**
            * @brief coroutine entrance function
            */
        inline void run_and_recv_retcode(void *priv_data) {
            if (!runner_) return;

            runner_ret_code_ = runner_(priv_data);
        }

    public:
        /**
            * @brief set runner
            * @param runner
            * @return COPP_EC_SUCCESS or error code
            */
        int set_runner(const callback_t &runner);

#if defined(UTIL_CONFIG_COMPILER_CXX_RVALUE_REFERENCES) && UTIL_CONFIG_COMPILER_CXX_RVALUE_REFERENCES
        int set_runner(callback_t &&runner);
#endif

        /**
            * get runner of this coroutine context (const)
            * @return NULL of pointer of runner
            */
        inline const std::function<int(void *)> &get_runner() const UTIL_CONFIG_NOEXCEPT { return runner_; }

        /**
            * @brief get runner return code
            * @return
            */
        inline int get_ret_code() const UTIL_CONFIG_NOEXCEPT { return runner_ret_code_; }

        /**
            * @brief get runner return code
            * @return true if coroutine has run and finished
            */
        bool is_finished() const UTIL_CONFIG_NOEXCEPT;

        /**
            * @brief get private buffer(raw pointer)
            */
        inline void *get_private_buffer() const UTIL_CONFIG_NOEXCEPT { return priv_data_; }

        /**
            * @brief get private buffer size
            */
        inline size_t get_private_buffer_size() const UTIL_CONFIG_NOEXCEPT { return private_buffer_size_; }

    protected:
        /**
            * @brief call platform jump to asm instruction
            * @param to_fctx jump to function context
            * @param from_sctx jump from stack context(only used for save segment stack)
            * @param to_sctx jump to stack context(only used for set segment stack)
            * @param jump_transfer jump data
            */
        static void jump_to(fcontext::fcontext_t &to_fctx, stack_context &from_sctx, stack_context &to_sctx,
                            jump_src_data_t &jump_transfer) UTIL_CONFIG_NOEXCEPT;

        /**
            * @brief fcontext entrance function
            * @param src_ctx where jump from
            */
        static void coroutine_context_callback(::copp::fcontext::transfer_t src_ctx);

    public:
        static inline size_t align_private_data_size(size_t sz) {
// static size_t random_index = 0;
// UTIL_CONFIG_CONSTEXPR size_t random_mask = 63;
#if (defined(__cplusplus) && __cplusplus >= 201103L) || (defined(__STDC_VERSION__) && __STDC_VERSION__ >= 201112L) || \
    (defined(_MSC_VER) && _MSC_VER >= 1700)
            UTIL_CONFIG_CONSTEXPR size_t align_mask = sizeof(max_align_t) - 1;
#else
            UTIL_CONFIG_CONSTEXPR size_t align_mask = 2 * sizeof(long double) - 1;
#endif

            // align
            sz += align_mask;
            sz &= ~align_mask;

            // random
            // ++random_index;
            // random_index &= random_mask;
            // sz += random_index * (align_mask + 1);
            return sz;
        }

        static inline size_t align_address_size(size_t sz) {
#if (defined(__cplusplus) && __cplusplus >= 201103L) || (defined(__STDC_VERSION__) && __STDC_VERSION__ >= 201112L) || \
    (defined(_MSC_VER) && _MSC_VER >= 1700)
            UTIL_CONFIG_CONSTEXPR size_t align_mask = sizeof(max_align_t) - 1;
#else
            UTIL_CONFIG_CONSTEXPR size_t align_mask = 2 * sizeof(long double) - 1;
#endif

            sz += align_mask;
            sz &= ~align_mask;
            return sz;
        }
    };

    namespace this_coroutine {
        /**
         * @brief get current coroutine
         * @see detail::coroutine_context
         * @return pointer of current coroutine, if not in coroutine, return NULL
         */
        coroutine_context *get_coroutine() UTIL_CONFIG_NOEXCEPT;

        /**
         * @brief get current coroutine and try to convert type
         * @see get_coroutine
         * @see detail::coroutine_context
         * @return pointer of current coroutine, if not in coroutine or fail to convert type, return NULL
         */
        template <typename Tc>
        Tc *get() {
            return static_cast<Tc *>(get_coroutine());
        }

        /**
         * @brief yield current coroutine
         * @param priv_data private data, if not NULL, will get the value from start(priv_data) or resume(priv_data)
         * @return 0 or error code
         */
        int yield(void **priv_data = UTIL_CONFIG_NULLPTR);
    }
}

#endif

#ifndef COPP_COROUTINE_CONTEXT_COROUTINE_CONTEXT_BASE_H
#define COPP_COROUTINE_CONTEXT_COROUTINE_CONTEXT_BASE_H

#pragma once

#include <cstddef>

#include <libcopp/fcontext/all.hpp>
#include <libcopp/stack/stack_context.h>
#include <libcopp/utils/atomic_int_type.h>
#include <libcopp/utils/config/compiler_features.h>
#include <libcopp/utils/config/libcopp_build_features.h>
#include <libcopp/utils/features.h>
#include <libcopp/utils/intrusive_ptr.h>
#include <libcopp/utils/std/functional.h>
#include <libcopp/utils/std/smart_ptr.h>

#ifdef LIBCOPP_MACRO_USE_SEGMENTED_STACKS
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

    namespace details {
        template <size_t N, bool BIGGER_THAN_16>
        struct LIBCOPP_COPP_API_HEAD_ONLY align_helper_inner;

        template <size_t N>
        struct LIBCOPP_COPP_API_HEAD_ONLY align_helper_inner<N, true> {
#if defined(UTIL_CONFIG_COMPILER_CXX_CONSTEXPR) && UTIL_CONFIG_COMPILER_CXX_CONSTEXPR
            static constexpr size_t value = N;
#else
            static const size_t value = N;
#endif
        };

        template <size_t N>
        struct LIBCOPP_COPP_API_HEAD_ONLY align_helper_inner<N, false> {
#if defined(UTIL_CONFIG_COMPILER_CXX_CONSTEXPR) && UTIL_CONFIG_COMPILER_CXX_CONSTEXPR
            static constexpr size_t value = 16;
#else
            static const size_t value = 16;
#endif
        };

        template <size_t N>
        struct LIBCOPP_COPP_API_HEAD_ONLY align_helper {
#if defined(UTIL_CONFIG_COMPILER_CXX_CONSTEXPR) && UTIL_CONFIG_COMPILER_CXX_CONSTEXPR
            static constexpr size_t value = align_helper_inner<N, N >= 16>::value;
#else
            static const size_t value = align_helper_inner<N, N >= 16>::value;
#endif
        };

        // We should align to at least 16 bytes, @see https://wiki.osdev.org/System_V_ABI for more details
#if (defined(__cplusplus) && __cplusplus >= 201103L) || (defined(__STDC_VERSION__) && __STDC_VERSION__ >= 201112L) || \
    (defined(_MSC_VER) && _MSC_VER >= 1900)
#define COROUTINE_CONTEXT_BASE_ALIGN_UNIT_SIZE ::copp::details::align_helper<sizeof(max_align_t)>::value
#else
#define COROUTINE_CONTEXT_BASE_ALIGN_UNIT_SIZE ::copp::details::align_helper<2 * sizeof(size_t)>::value
#endif

        UTIL_CONFIG_STATIC_ASSERT(COROUTINE_CONTEXT_BASE_ALIGN_UNIT_SIZE >= 16 && 0 == COROUTINE_CONTEXT_BASE_ALIGN_UNIT_SIZE % 16);
    } // namespace details

    /**
     * @brief base type of all coroutine context
     */
    class coroutine_context {
    public:
        typedef libcopp::util::intrusive_ptr<coroutine_context> ptr_t;
        typedef std::function<int(void *)>                      callback_t;

        /**
         * @brief status of safe coroutine context base
         */
        struct LIBCOPP_COPP_API status_t {
            enum type {
                EN_CRS_INVALID = 0, //!< EN_CRS_INVALID
                EN_CRS_READY,       //!< EN_CRS_READY
                EN_CRS_RUNNING,     //!< EN_CRS_RUNNING
                EN_CRS_FINISHED,    //!< EN_CRS_FINISHED
                EN_CRS_EXITED,      //!< EN_CRS_EXITED
            };
        };

        struct LIBCOPP_COPP_API flag_t {
            enum type {
                EN_CFT_UNKNOWN  = 0,
                EN_CFT_FINISHED = 0x01,
                EN_CFT_MASK     = 0xFF,
            };
        };

    private:
        int        runner_ret_code_; /** coroutine return code **/
        int        flags_;           /** flags **/
        callback_t runner_;          /** coroutine runner **/
        void *     priv_data_;
        size_t     private_buffer_size_;

        struct jump_src_data_t {
            coroutine_context *from_co;
            coroutine_context *to_co;
            void *             priv_data;
        };

        friend struct LIBCOPP_COPP_API_HEAD_ONLY libcopp_inner_api_helper;

    protected:
        fcontext::fcontext_t caller_; /** caller runtime context **/
        fcontext::fcontext_t callee_; /** callee runtime context **/

        stack_context callee_stack_; /** callee stack context **/
#ifdef LIBCOPP_MACRO_USE_SEGMENTED_STACKS
        stack_context caller_stack_; /** caller stack context **/
#endif

    private:
#if defined(LIBCOPP_DISABLE_ATOMIC_LOCK) && LIBCOPP_DISABLE_ATOMIC_LOCK
        libcopp::util::lock::atomic_int_type<libcopp::util::lock::unsafe_int_type<int> > status_; /** status **/
#else
        libcopp::util::lock::atomic_int_type<int> status_; /** status **/
#endif

    protected:
        LIBCOPP_COPP_API coroutine_context() UTIL_CONFIG_NOEXCEPT;

    public:
        LIBCOPP_COPP_API ~coroutine_context();

    private:
        coroutine_context(const coroutine_context &) UTIL_CONFIG_DELETED_FUNCTION;
        coroutine_context &operator=(const coroutine_context &) UTIL_CONFIG_DELETED_FUNCTION;

#if defined(UTIL_CONFIG_COMPILER_CXX_RVALUE_REFERENCES) && UTIL_CONFIG_COMPILER_CXX_RVALUE_REFERENCES
        coroutine_context(const coroutine_context &&) UTIL_CONFIG_DELETED_FUNCTION;
        coroutine_context &operator=(const coroutine_context &&) UTIL_CONFIG_DELETED_FUNCTION;
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
        static LIBCOPP_COPP_API int create(coroutine_context *p, callback_t &runner, const stack_context &callee_stack,
                                           size_t coroutine_size, size_t private_buffer_size) UTIL_CONFIG_NOEXCEPT;

        template <typename TRunner>
        static LIBCOPP_COPP_API_HEAD_ONLY int create(coroutine_context *p, TRunner *runner, const stack_context &callee_stack,
                                                     size_t coroutine_size, size_t private_buffer_size) UTIL_CONFIG_NOEXCEPT {
            return create(p, std::bind(&TRunner::operator(), runner, std::placeholders::_1), callee_stack, coroutine_size,
                          private_buffer_size);
        }

        /**
         * @brief start coroutine
         * @param priv_data private data, will be passed to runner operator() or return to yield
         * @return COPP_EC_SUCCESS or error code
         */
        LIBCOPP_COPP_API int start(void *priv_data = UTIL_CONFIG_NULLPTR);

        /**
         * @brief resume coroutine
         * @param priv_data private data, will be passed to runner operator() or return to yield
         * @return COPP_EC_SUCCESS or error code
         */
        LIBCOPP_COPP_API int resume(void *priv_data = UTIL_CONFIG_NULLPTR);


        /**
         * @brief yield coroutine
         * @param priv_data private data, if not NULL, will get the value from start(priv_data) or resume(priv_data)
         * @return COPP_EC_SUCCESS or error code
         */
        LIBCOPP_COPP_API int yield(void **priv_data = UTIL_CONFIG_NULLPTR);

        /**
         * @brief set all flags to true
         * @param flags (flags & EN_CFT_MASK) must be 0
         * @return true if flags is available, or return false
         */
        LIBCOPP_COPP_API bool set_flags(int flags);

        /**
         * @brief set all flags to false
         * @param flags (flags & EN_CFT_MASK) must be 0
         * @return true if flags is available, or return false
         */
        LIBCOPP_COPP_API bool unset_flags(int flags);

        /**
         * @brief check flags
         * @param flags flags to be checked
         * @return true if flags any flags is true
         */
        LIBCOPP_COPP_API bool check_flags(int flags) const;

    protected:
        /**
         * @brief coroutine entrance function
         */
        UTIL_FORCEINLINE void run_and_recv_retcode(void *priv_data) {
            if (!runner_) return;

            runner_ret_code_ = runner_(priv_data);
        }

    public:
        /**
         * @brief set runner
         * @param runner
         * @return COPP_EC_SUCCESS or error code
         */
#if defined(UTIL_CONFIG_COMPILER_CXX_RVALUE_REFERENCES) && UTIL_CONFIG_COMPILER_CXX_RVALUE_REFERENCES
        LIBCOPP_COPP_API int set_runner(callback_t &&runner);
#else
        LIBCOPP_COPP_API int                      set_runner(const callback_t &runner);
#endif

        /**
         * get runner of this coroutine context (const)
         * @return NULL of pointer of runner
         */
        UTIL_FORCEINLINE const std::function<int(void *)> &get_runner() const UTIL_CONFIG_NOEXCEPT { return runner_; }

        /**
         * @brief get runner return code
         * @return
         */
        UTIL_FORCEINLINE int get_ret_code() const UTIL_CONFIG_NOEXCEPT { return runner_ret_code_; }

        /**
         * @brief get runner return code
         * @return true if coroutine has run and finished
         */
        LIBCOPP_COPP_API bool is_finished() const UTIL_CONFIG_NOEXCEPT;

        /**
         * @brief get private buffer(raw pointer)
         */
        UTIL_FORCEINLINE void *get_private_buffer() const UTIL_CONFIG_NOEXCEPT { return priv_data_; }

        /**
         * @brief get private buffer size
         */
        UTIL_FORCEINLINE size_t get_private_buffer_size() const UTIL_CONFIG_NOEXCEPT { return private_buffer_size_; }

    public:
        static UTIL_FORCEINLINE size_t align_private_data_size(size_t sz) {
            // static size_t random_index = 0;
            // UTIL_CONFIG_CONSTEXPR size_t random_mask = 63;
            UTIL_CONFIG_CONSTEXPR size_t align_mask = COROUTINE_CONTEXT_BASE_ALIGN_UNIT_SIZE - 1;

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
            UTIL_CONFIG_CONSTEXPR size_t align_mask = COROUTINE_CONTEXT_BASE_ALIGN_UNIT_SIZE - 1;

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
        LIBCOPP_COPP_API coroutine_context *get_coroutine() UTIL_CONFIG_NOEXCEPT;

        /**
         * @brief get current coroutine and try to convert type
         * @see get_coroutine
         * @see detail::coroutine_context
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
        LIBCOPP_COPP_API int yield(void **priv_data = UTIL_CONFIG_NULLPTR);
    } // namespace this_coroutine
} // namespace copp

#endif

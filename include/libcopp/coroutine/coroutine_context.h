// Copyright 2023 owent

#pragma once

#include <libcopp/utils/config/libcopp_build_features.h>

#include "coroutine_context_base.h"

#ifdef LIBCOPP_MACRO_USE_SEGMENTED_STACKS
#  define COROUTINE_CONTEXT_BASE_USING_BASE_SEGMENTED_STACKS(base_type) using base_type::caller_stack_;
#else
#  define COROUTINE_CONTEXT_BASE_USING_BASE_SEGMENTED_STACKS(base_type)
#endif

#define COROUTINE_CONTEXT_BASE_USING_BASE(base_type) \
 protected:                                          \
  using base_type::caller_;                          \
  using base_type::callee_;                          \
  using base_type::callee_stack_;                    \
  COROUTINE_CONTEXT_BASE_USING_BASE_SEGMENTED_STACKS(base_type)

LIBCOPP_COPP_NAMESPACE_BEGIN
/**
 * @brief base type of all stackful coroutine context
 */
class coroutine_context : public coroutine_context_base {
 public:
  using ptr_type = LIBCOPP_COPP_NAMESPACE_ID::util::intrusive_ptr<coroutine_context>;
  using callback_type = coroutine_context_base::callback_type;
  using status_type = coroutine_context_base::status_type;
  using flag_type = coroutine_context_base::flag_type;

  // Compability with libcopp-1.x
  using ptr_t = ptr_type;
  using callback_t = callback_type;
  using status_t = status_type;
  using flag_t = flag_type;

 private:
  using coroutine_context_base::flags_;
  using coroutine_context_base::priv_data_;
  using coroutine_context_base::private_buffer_size_;
  using coroutine_context_base::runner_;
  using coroutine_context_base::runner_ret_code_;
  using coroutine_context_base::status_;
#if defined(LIBCOPP_MACRO_ENABLE_STD_EXCEPTION_PTR) && LIBCOPP_MACRO_ENABLE_STD_EXCEPTION_PTR
  using coroutine_context_base::unhandle_exception_;
#endif

  struct jump_src_data_t {
    coroutine_context *from_co;
    coroutine_context *to_co;
    void *priv_data;
  };

  friend struct libcopp_internal_api_set;

 protected:
  fcontext::fcontext_t caller_; /** caller runtime context **/
  fcontext::fcontext_t callee_; /** callee runtime context **/

  stack_context callee_stack_; /** callee stack context **/
#ifdef LIBCOPP_MACRO_USE_SEGMENTED_STACKS
  stack_context caller_stack_; /** caller stack context **/
#endif

 protected:
  LIBCOPP_COPP_API coroutine_context() LIBCOPP_MACRO_NOEXCEPT;

 public:
  LIBCOPP_COPP_API ~coroutine_context();

 private:
  coroutine_context(const coroutine_context &) = delete;
  coroutine_context &operator=(const coroutine_context &) = delete;
  coroutine_context(const coroutine_context &&) = delete;
  coroutine_context &operator=(const coroutine_context &&) = delete;

 public:
  /**
   * @brief create coroutine context at stack context callee_
   * @param runner runner
   * @param callee_stack stack context
   * @param coroutine_size size of coroutine object
   * @param private_buffer_size size of private buffer
   * @return COPP_EC_SUCCESS or error code
   */
  static LIBCOPP_COPP_API int create(coroutine_context *p, callback_type &&runner, const stack_context &callee_stack,
                                     size_t coroutine_size, size_t private_buffer_size) LIBCOPP_MACRO_NOEXCEPT;

  template <typename TRunner>
  static LIBCOPP_COPP_API_HEAD_ONLY int create(coroutine_context *p, TRunner *runner, const stack_context &callee_stack,
                                               size_t coroutine_size,
                                               size_t private_buffer_size) LIBCOPP_MACRO_NOEXCEPT {
    return create(
        p, [runner](void *private_data) { return (*runner)(private_data); }, callee_stack, coroutine_size,
        private_buffer_size);
  }

  /**
   * @brief start coroutine
   * @param priv_data private data, will be passed to runner operator() or return to yield
   * @exception if exception is enabled, it will throw all unhandled exception after resumed
   * @return COPP_EC_SUCCESS or error code
   */
  LIBCOPP_COPP_API int start(void *priv_data = nullptr);

#if defined(LIBCOPP_MACRO_ENABLE_STD_EXCEPTION_PTR) && LIBCOPP_MACRO_ENABLE_STD_EXCEPTION_PTR
  /**
   * @brief start coroutine
   * @param unhandled set exception_ptr of unhandled exception if it's exists
   * @param priv_data private data, will be passed to runner operator() or return to yield
   * @return COPP_EC_SUCCESS or error code
   */
  LIBCOPP_COPP_API int start(std::exception_ptr &unhandled, void *priv_data = nullptr) LIBCOPP_MACRO_NOEXCEPT;
#endif

  /**
   * @brief resume coroutine
   * @param priv_data private data, will be passed to runner operator() or return to yield
   * @exception if exception is enabled, it will throw all unhandled exception after resumed
   * @return COPP_EC_SUCCESS or error code
   */
  LIBCOPP_COPP_API int resume(void *priv_data = nullptr);

#if defined(LIBCOPP_MACRO_ENABLE_STD_EXCEPTION_PTR) && LIBCOPP_MACRO_ENABLE_STD_EXCEPTION_PTR
  /**
   * @brief resume coroutine
   * @param unhandled set exception_ptr of unhandled exception if it's exists
   * @param priv_data private data, will be passed to runner operator() or return to yield
   * @return COPP_EC_SUCCESS or error code
   */
  LIBCOPP_COPP_API int resume(std::exception_ptr &unhandled, void *priv_data = nullptr) LIBCOPP_MACRO_NOEXCEPT;
#endif

  /**
   * @brief yield coroutine
   * @param priv_data private data, if not nullptr, will get the value from start(priv_data) or resume(priv_data)
   * @return COPP_EC_SUCCESS or error code
   */
  LIBCOPP_COPP_API int yield(void **priv_data = nullptr) LIBCOPP_MACRO_NOEXCEPT;
};

namespace this_coroutine {
/**
 * @brief get current coroutine
 * @see detail::coroutine_context
 * @return pointer of current coroutine, if not in coroutine, return nullptr
 */
LIBCOPP_COPP_API coroutine_context *get_coroutine() LIBCOPP_MACRO_NOEXCEPT;

/**
 * @brief get current coroutine and try to convert type
 * @see get_coroutine
 * @see detail::coroutine_context
 * @return pointer of current coroutine, if not in coroutine or fail to convert type, return nullptr
 */
template <typename Tc>
LIBCOPP_COPP_API_HEAD_ONLY Tc *get() {
  return static_cast<Tc *>(get_coroutine());
}

/**
 * @brief yield current coroutine
 * @param priv_data private data, if not nullptr, will get the value from start(priv_data) or resume(priv_data)
 * @return 0 or error code
 */
LIBCOPP_COPP_API int yield(void **priv_data = nullptr) LIBCOPP_MACRO_NOEXCEPT;
}  // namespace this_coroutine
LIBCOPP_COPP_NAMESPACE_END

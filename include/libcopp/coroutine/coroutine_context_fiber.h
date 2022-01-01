// Copyright 2022 owent

#pragma once

#include <libcopp/utils/config/libcopp_build_features.h>

#include <cstddef>

#include "coroutine_context.h"

#if defined(LIBCOPP_MACRO_ENABLE_WIN_FIBER) && LIBCOPP_MACRO_ENABLE_WIN_FIBER

#  ifndef WIN32_LEAN_AND_MEAN
#    define WIN32_LEAN_AND_MEAN
#  endif

#  include <Windows.h>

LIBCOPP_COPP_NAMESPACE_BEGIN
/**
 * @brief base type of all coroutine context of windows fiber
 */
class coroutine_context_fiber : public coroutine_context_base {
 public:
  using ptr_type = libcopp::util::intrusive_ptr<coroutine_context_fiber>;

  using callback_type = coroutine_context_base::callback_type;
  using status_t = coroutine_context_base::status_t;
  using flag_t = coroutine_context_base::flag_t;

  // Compability with libcopp-1.x
  using ptr_t = ptr_type;
  using callback_t = callback_type;

 private:
  using coroutine_context_base::flags_;
  using coroutine_context_base::priv_data_;
  using coroutine_context_base::private_buffer_size_;
  using coroutine_context_base::runner_;
  using coroutine_context_base::runner_ret_code_;
  using coroutine_context_base::status_;
#  if defined(LIBCOPP_MACRO_ENABLE_STD_EXCEPTION_PTR) && LIBCOPP_MACRO_ENABLE_STD_EXCEPTION_PTR
  using coroutine_context_base::unhandle_exception_;
#  endif

  struct jump_src_data_t {
    coroutine_context_fiber *from_co;
    LPVOID from_fiber;
    coroutine_context_fiber *to_co;
    LPVOID to_fiber;
    void *priv_data;
  };

  friend struct LIBCOPP_COPP_API_HEAD_ONLY libcopp_fiber_internal_api_set;
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
  coroutine_context_fiber(const coroutine_context_fiber &) = delete;
  coroutine_context_fiber &operator=(const coroutine_context_fiber &) = delete;
  coroutine_context_fiber(const coroutine_context_fiber &&) = delete;
  coroutine_context_fiber &operator=(const coroutine_context_fiber &&) = delete;

 public:
  /**
   * @brief create coroutine context at stack context callee_
   * @param runner runner
   * @param callee_stack stack context
   * @param coroutine_size size of coroutine object
   * @param private_buffer_size size of private buffer
   * @return COPP_EC_SUCCESS or error code
   */
  static LIBCOPP_COPP_API int create(coroutine_context_fiber *p, callback_type &&runner,
                                     const stack_context &callee_stack, size_t coroutine_size,
                                     size_t private_buffer_size,
                                     size_t stack_reserve_size_of_fiber = 0) LIBCOPP_MACRO_NOEXCEPT;

  template <typename TRunner>
  static LIBCOPP_COPP_API_HEAD_ONLY int create(coroutine_context_fiber *p, TRunner *runner,
                                               const stack_context &callee_stack, size_t coroutine_size,
                                               size_t private_buffer_size,
                                               size_t stack_reserve_size_of_fiber = 0) LIBCOPP_MACRO_NOEXCEPT {
    return create(
        p, [runner](void *private_data) { return (*runner)(private_data); }, callee_stack, coroutine_size,
        private_buffer_size, stack_reserve_size_of_fiber);
  }

  /**
   * @brief start coroutine
   * @param priv_data private data, will be passed to runner operator() or return to yield
   * @exception if exception is enabled, it will throw all unhandled exception after resumed
   * @return COPP_EC_SUCCESS or error code
   */
  LIBCOPP_COPP_API int start(void *priv_data = nullptr);

#  if defined(LIBCOPP_MACRO_ENABLE_STD_EXCEPTION_PTR) && LIBCOPP_MACRO_ENABLE_STD_EXCEPTION_PTR
  /**
   * @brief start coroutine
   * @param unhandled set exception_ptr of unhandled exception if it's exists
   * @param priv_data private data, will be passed to runner operator() or return to yield
   * @return COPP_EC_SUCCESS or error code
   */
  LIBCOPP_COPP_API int start(std::exception_ptr &unhandled, void *priv_data = nullptr) LIBCOPP_MACRO_NOEXCEPT;
#  endif

  /**
   * @brief resume coroutine
   * @param priv_data private data, will be passed to runner operator() or return to yield
   * @exception if exception is enabled, it will throw all unhandled exception after resumed
   * @return COPP_EC_SUCCESS or error code
   */
  LIBCOPP_COPP_API int resume(void *priv_data = nullptr);

#  if defined(LIBCOPP_MACRO_ENABLE_STD_EXCEPTION_PTR) && LIBCOPP_MACRO_ENABLE_STD_EXCEPTION_PTR
  /**
   * @brief resume coroutine
   * @param unhandled set exception_ptr of unhandled exception if it's exists
   * @param priv_data private data, will be passed to runner operator() or return to yield
   * @return COPP_EC_SUCCESS or error code
   */
  LIBCOPP_COPP_API int resume(std::exception_ptr &unhandled, void *priv_data = nullptr) LIBCOPP_MACRO_NOEXCEPT;
#  endif

  /**
   * @brief yield coroutine
   * @param priv_data private data, if not nullptr, will get the value from start(priv_data) or resume(priv_data)
   * @return COPP_EC_SUCCESS or error code
   */
  LIBCOPP_COPP_API int yield(void **priv_data = nullptr) LIBCOPP_MACRO_NOEXCEPT;
};

namespace this_fiber {
/**
 * @brief get current coroutine
 * @see detail::coroutine_context_fiber
 * @return pointer of current coroutine, if not in coroutine, return nullptr
 */
LIBCOPP_COPP_API coroutine_context_fiber *get_coroutine() LIBCOPP_MACRO_NOEXCEPT;

/**
 * @brief get current coroutine and try to convert type
 * @see get_coroutine
 * @see detail::coroutine_context_fiber
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
}  // namespace this_fiber
LIBCOPP_COPP_NAMESPACE_END

#endif

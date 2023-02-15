// Copyright 2023 owent

#pragma once

#include <libcopp/utils/config/libcopp_build_features.h>

#include <libcopp/stack/stack_context.h>
#include <libcopp/utils/atomic_int_type.h>
#include <libcopp/utils/features.h>
#include <libcopp/utils/intrusive_ptr.h>
#include <libcopp/fcontext/all.hpp>

// clang-format off
#include <libcopp/utils/config/stl_include_prefix.h>  // NOLINT(build/include_order)
// clang-format on
#include <cstddef>
#include <functional>
#include <memory>
#if defined(LIBCOPP_MACRO_ENABLE_STD_EXCEPTION_PTR) && LIBCOPP_MACRO_ENABLE_STD_EXCEPTION_PTR
#  include <exception>
#endif
// clang-format off
#include <libcopp/utils/config/stl_include_suffix.h>  // NOLINT(build/include_order)
// clang-format on

LIBCOPP_COPP_NAMESPACE_BEGIN

namespace details {
template <size_t N1, size_t N2, bool BIGGER_THAN_16>
struct LIBCOPP_COPP_API_HEAD_ONLY align_helper_inner;

template <size_t N1, size_t N2>
struct LIBCOPP_COPP_API_HEAD_ONLY align_helper_inner<N1, N2, true> {
  static constexpr const size_t value = N1;
};

template <size_t N1, size_t N2>
struct LIBCOPP_COPP_API_HEAD_ONLY align_helper_inner<N1, N2, false> {
  static constexpr const size_t value = N2;
};

template <size_t N, size_t COMPARE_TO>
struct LIBCOPP_COPP_API_HEAD_ONLY align_helper {
  static constexpr const size_t value = align_helper_inner<N, COMPARE_TO, N >= COMPARE_TO>::value;
};

// We should align to at least 16 bytes, @see https://wiki.osdev.org/System_V_ABI for more details
#if (defined(__cplusplus) && __cplusplus >= 201103L) || (defined(__STDC_VERSION__) && __STDC_VERSION__ >= 201112L) || \
    (defined(_MSC_VER) && _MSC_VER >= 1900)
#  define COROUTINE_CONTEXT_BASE_ALIGN_UNIT_SIZE \
    LIBCOPP_COPP_NAMESPACE_ID::details::align_helper<sizeof(max_align_t), 16>::value
#else
#  define COROUTINE_CONTEXT_BASE_ALIGN_UNIT_SIZE \
    LIBCOPP_COPP_NAMESPACE_ID::details::align_helper<2 * sizeof(size_t), 16>::value
#endif

// Some architecture may require stack to be aligned to 64
// @see "3.2.2 The Stack Frame" of https://github.com/hjl-tools/x86-psABI/wiki/x86-64-psABI-1.0.pdf
// More documents about x86/x86_64 canbe found at https://stackoverflow.com/tags/x86/info
#define COROUTINE_CONTEXT_STACK_ALIGN_UNIT_SIZE \
  LIBCOPP_COPP_NAMESPACE_ID::details::align_helper<COROUTINE_CONTEXT_BASE_ALIGN_UNIT_SIZE, 64>::value

static_assert(COROUTINE_CONTEXT_BASE_ALIGN_UNIT_SIZE >= 16 && 0 == COROUTINE_CONTEXT_BASE_ALIGN_UNIT_SIZE % 16,
              "COROUTINE_CONTEXT_BASE_ALIGN_UNIT_SIZE");
static_assert(COROUTINE_CONTEXT_STACK_ALIGN_UNIT_SIZE >= 16 && 0 == COROUTINE_CONTEXT_STACK_ALIGN_UNIT_SIZE % 16,
              "COROUTINE_CONTEXT_STACK_ALIGN_UNIT_SIZE");
}  // namespace details

/**
 * @brief base type of all coroutine context
 */
class coroutine_context_base {
 public:
  using callback_type = std::function<int(void *)>;

  /**
   * @brief status of safe coroutine context base
   */
  struct LIBCOPP_COPP_API status_type {
    enum type {
      EN_CRS_INVALID = 0,  //!< EN_CRS_INVALID
      EN_CRS_READY,        //!< EN_CRS_READY
      EN_CRS_RUNNING,      //!< EN_CRS_RUNNING
      EN_CRS_FINISHED,     //!< EN_CRS_FINISHED
      EN_CRS_EXITED,       //!< EN_CRS_EXITED
    };
  };

  struct LIBCOPP_COPP_API flag_type {
    enum type {
      EN_CFT_UNKNOWN = 0,
      EN_CFT_FINISHED = 0x01,
      EN_CFT_IS_FIBER = 0x02,
      EN_CFT_MASK = 0xFF,
    };
  };

  // Compability with libcopp-1.x
  using callback_t = callback_type;
  using status_t = status_type;
  using flag_t = flag_type;

 protected:
  int runner_ret_code_;  /** coroutine return code **/
  int flags_;            /** flags **/
  callback_type runner_; /** coroutine runner **/
  void *priv_data_;
  size_t private_buffer_size_;

#if defined(LIBCOPP_DISABLE_ATOMIC_LOCK) && LIBCOPP_DISABLE_ATOMIC_LOCK
  LIBCOPP_COPP_NAMESPACE_ID::util::lock::atomic_int_type<LIBCOPP_COPP_NAMESPACE_ID::util::lock::unsafe_int_type<int> >
      status_; /** status **/
#else
  LIBCOPP_COPP_NAMESPACE_ID::util::lock::atomic_int_type<int> status_; /** status **/
#endif

#if defined(LIBCOPP_MACRO_ENABLE_STD_EXCEPTION_PTR) && LIBCOPP_MACRO_ENABLE_STD_EXCEPTION_PTR
  std::exception_ptr unhandle_exception_;
#endif

 protected:
  LIBCOPP_COPP_API coroutine_context_base() LIBCOPP_MACRO_NOEXCEPT;

 public:
  LIBCOPP_COPP_API ~coroutine_context_base();

 private:
  coroutine_context_base(const coroutine_context_base &) = delete;
  coroutine_context_base &operator=(const coroutine_context_base &) = delete;

  coroutine_context_base(const coroutine_context_base &&) = delete;
  coroutine_context_base &operator=(const coroutine_context_base &&) = delete;

 public:
  /**
   * @brief set all flags to true
   * @param flags (flags & EN_CFT_MASK) must be 0
   * @return true if flags is available, or return false
   */
  LIBCOPP_COPP_API bool set_flags(int flags) LIBCOPP_MACRO_NOEXCEPT;

  /**
   * @brief set all flags to false
   * @param flags (flags & EN_CFT_MASK) must be 0
   * @return true if flags is available, or return false
   */
  LIBCOPP_COPP_API bool unset_flags(int flags) LIBCOPP_MACRO_NOEXCEPT;

  /**
   * @brief check flags
   * @param flags flags to be checked
   * @return true if flags any flags is true
   */
  LIBCOPP_COPP_API bool check_flags(int flags) const LIBCOPP_MACRO_NOEXCEPT;

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
  LIBCOPP_COPP_API int set_runner(callback_type &&runner);

  /**
   * get runner of this coroutine context (const)
   * @return nullptr of pointer of runner
   */
  UTIL_FORCEINLINE const std::function<int(void *)> &get_runner() const LIBCOPP_MACRO_NOEXCEPT { return runner_; }

  /**
   * @brief get runner return code
   * @return
   */
  UTIL_FORCEINLINE int get_ret_code() const LIBCOPP_MACRO_NOEXCEPT { return runner_ret_code_; }

  /**
   * @brief get runner return code
   * @return true if coroutine has run and finished
   */
  LIBCOPP_COPP_API bool is_finished() const LIBCOPP_MACRO_NOEXCEPT;

  /**
   * @brief get private buffer(raw pointer)
   */
  UTIL_FORCEINLINE void *get_private_buffer() const LIBCOPP_MACRO_NOEXCEPT { return priv_data_; }

  /**
   * @brief get private buffer size
   */
  UTIL_FORCEINLINE size_t get_private_buffer_size() const LIBCOPP_MACRO_NOEXCEPT { return private_buffer_size_; }

 public:
  UTIL_FORCEINLINE static size_t align_private_data_size(size_t sz) {
    // static size_t random_index = 0;
    // constexpr const size_t random_mask = 63;
    constexpr const size_t align_mask = COROUTINE_CONTEXT_BASE_ALIGN_UNIT_SIZE - 1;

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
    constexpr const size_t align_mask = COROUTINE_CONTEXT_BASE_ALIGN_UNIT_SIZE - 1;

    sz += align_mask;
    sz &= ~align_mask;
    return sz;
  }

  static inline size_t align_stack_size(size_t sz) {
    constexpr const size_t align_mask = COROUTINE_CONTEXT_STACK_ALIGN_UNIT_SIZE - 1;

    sz += align_mask;
    sz &= ~align_mask;
    return sz;
  }

#if defined(LIBCOPP_MACRO_ENABLE_STD_EXCEPTION_PTR) && LIBCOPP_MACRO_ENABLE_STD_EXCEPTION_PTR
  static inline void maybe_rethrow(std::exception_ptr &inout) {
    COPP_UNLIKELY_IF (inout) {
      std::exception_ptr eptr;
      std::swap(eptr, inout);
      std::rethrow_exception(eptr);
    }
  }

  UTIL_FORCEINLINE void maybe_rethrow() { maybe_rethrow(unhandle_exception_); }
#endif

  /**
   * @brief get current coroutine
   * @see detail::coroutine_context_base
   * @return pointer of current coroutine, if not in coroutine, return nullptr
   */
  static LIBCOPP_COPP_API coroutine_context_base *get_this_coroutine_base() LIBCOPP_MACRO_NOEXCEPT;

  /**
   * @brief set current coroutine
   * @see detail::coroutine_context_base
   * @param ctx pointer of current coroutine, if not in coroutine, set nullptr
   */
  static LIBCOPP_COPP_API void set_this_coroutine_base(coroutine_context_base *ctx) LIBCOPP_MACRO_NOEXCEPT;
};
LIBCOPP_COPP_NAMESPACE_END

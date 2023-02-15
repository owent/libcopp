// Copyright 2023 owent

#include <libcopp/utils/config/libcopp_build_features.h>

#include <libcopp/utils/errno.h>
#include <libcopp/utils/std/explicit_declare.h>

#include <libcopp/coroutine/coroutine_context_fiber.h>

// clang-format off
#include <libcopp/utils/config/stl_include_prefix.h>  // NOLINT(build/include_order)
// clang-format on
#include <assert.h>
#include <algorithm>
#include <cstdlib>
#include <cstring>
// clang-format off
#include <libcopp/utils/config/stl_include_suffix.h>  // NOLINT(build/include_order)
// clang-format on

#if defined(LIBCOPP_MACRO_ENABLE_WIN_FIBER) && LIBCOPP_MACRO_ENABLE_WIN_FIBER
LIBCOPP_COPP_NAMESPACE_BEGIN
struct fiber_context_tls_data_t {
  using jump_src_data_t = coroutine_context_fiber::jump_src_data_t;

  LPVOID thread_fiber;
  jump_src_data_t jump_data;
  fiber_context_tls_data_t() : thread_fiber(nullptr) {
    jump_data.from_co = nullptr;
    jump_data.from_fiber = nullptr;
    jump_data.to_co = nullptr;
    jump_data.to_fiber = nullptr;
    jump_data.priv_data = nullptr;
  }
  ~fiber_context_tls_data_t() {
    if (thread_fiber) {
      ConvertFiberToThread();
      thread_fiber = nullptr;
    }
  }
};

#  if defined(LIBCOPP_LOCK_DISABLE_THIS_MT) && LIBCOPP_LOCK_DISABLE_THIS_MT
static fiber_context_tls_data_t gt_current_fiber;
#  else
static COPP_MACRO_THREAD_LOCAL fiber_context_tls_data_t gt_current_fiber;
#  endif

static inline LPVOID get_this_fiber_address() {
  if (!gt_current_fiber.thread_fiber) {
    gt_current_fiber.thread_fiber = ConvertThreadToFiber(nullptr);
  }

  return gt_current_fiber.thread_fiber;
}

static inline fiber_context_tls_data_t::jump_src_data_t &get_this_fiber_jump_src() {
  return gt_current_fiber.jump_data;
}

struct libcopp_fiber_internal_api_set {
  using jump_src_data_t = coroutine_context_fiber::jump_src_data_t;

  static inline fiber_context_tls_data_t::jump_src_data_t &build_this_fiber_jump_src(coroutine_context_fiber &to_ctx,
                                                                                     void *data) {
    fiber_context_tls_data_t::jump_src_data_t &jump_src = get_this_fiber_jump_src();

    jump_src.from_co = ::LIBCOPP_COPP_NAMESPACE_ID::this_fiber::get_coroutine();
    if (nullptr == jump_src.from_co) {
      jump_src.from_fiber = get_this_fiber_address();
    } else {
      jump_src.from_fiber = jump_src.from_co->callee_;
    }

    jump_src.to_co = &to_ctx;
    jump_src.to_fiber = to_ctx.callee_;
    jump_src.priv_data = data;
    return jump_src;
  }

  UTIL_FORCEINLINE static void set_caller(coroutine_context_fiber *src, LPVOID fctx) {
    if (nullptr != src) {
      src->caller_ = fctx;
    }
  }

  // UTIL_FORCEINLINE static void set_callee(coroutine_context_fiber *src, LPVOID fctx) {
  //     if (nullptr != src) {
  //         src->callee_ = fctx;
  //     }
  // }

  static void __stdcall coroutine_fiber_context_callback(LPVOID lpParameter) {
    coroutine_context_fiber *ctx = reinterpret_cast<coroutine_context_fiber *>(lpParameter);
    assert(ctx);
    if (nullptr == ctx) {
      abort();
      // return; // clang-analyzer will report "Unreachable code"
    }

    // copy jump_src_data_t in case it's destroyed later
    jump_src_data_t jump_src = get_this_fiber_jump_src();

    // this must in a coroutine
    coroutine_context_fiber *ins_ptr = jump_src.to_co;
    assert(ins_ptr == ctx);
    if (ctx != ins_ptr) {
      abort();
      // return; // clang-analyzer will report "Unreachable code"
    }

    // update caller of to_co
    ins_ptr->caller_ = jump_src.from_fiber;

    // There is no need to update fiber's callee_ of from_co
    // if (nullptr != jump_src.from_co) {
    //     jump_src.from_co->callee_ = jump_src.from_fiber;
    // }

    // this_fiber
    coroutine_context_base::set_this_coroutine_base(ins_ptr);

    // run logic code
#  if defined(LIBCOPP_MACRO_ENABLE_STD_EXCEPTION_PTR) && LIBCOPP_MACRO_ENABLE_STD_EXCEPTION_PTR
    try {
#  endif
      ins_ptr->run_and_recv_retcode(jump_src.priv_data);
#  if defined(LIBCOPP_MACRO_ENABLE_STD_EXCEPTION_PTR) && LIBCOPP_MACRO_ENABLE_STD_EXCEPTION_PTR
    } catch (...) {
      ins_ptr->unhandle_exception_ = std::current_exception();
    }
#  endif

    ins_ptr->flags_ |= coroutine_context_fiber::flag_type::EN_CFT_FINISHED;
    // add memory fence to flush flags_(used in is_finished())
    // LIBCOPP_UTIL_LOCK_ATOMIC_THREAD_FENCE(LIBCOPP_COPP_NAMESPACE_ID::util::lock::memory_order_release);
    ins_ptr->yield();
  }
};
/**
 * @brief call SwitchToFiber
 * @param to_fiber jump to function context
 * @param jump_src jump data
 */
static inline void jump_to(LPVOID to_fiber,
                           libcopp_fiber_internal_api_set::jump_src_data_t &jump_src) LIBCOPP_MACRO_NOEXCEPT {
  coroutine_context_fiber *restore_co = jump_src.from_co;

  SwitchToFiber(to_fiber);

  // Fiber may be yield by another thread, we can not use jump_src_data_t from parameter here
  jump_src = get_this_fiber_jump_src();

  /**
   * save from_co's context and switch status
   *
   * if we jump sequence is A->B->C->A.resume(), and if this call is A->B, then
   * jump_src->from_co = C, jump_src->to_co = A
   * and now we should save the callee of C and set the caller of A = C
   *
   * if we jump sequence is A->B.yield()->A, and if this call is A->B, then
   * jump_src->from_co = B, jump_src->to_co = nullptr
   * and now we should save the callee of B and should change the caller of A
   *
   */

  // update caller of to_co if not jump from yield
  libcopp_fiber_internal_api_set::set_caller(jump_src.to_co, jump_src.from_fiber);

  // There is no need to update fiber's callee_ of from_co
  // libcopp_fiber_internal_api_set::set_callee(jump_src->from_co, res.fctx);

  // this_fiber
  coroutine_context_base::set_this_coroutine_base(restore_co);
}

LIBCOPP_COPP_API coroutine_context_fiber::coroutine_context_fiber() LIBCOPP_MACRO_NOEXCEPT : coroutine_context_base(),
                                                                                             caller_(nullptr),
                                                                                             callee_(nullptr),
                                                                                             callee_stack_() {
  flags_ |= flag_type::EN_CFT_IS_FIBER;
  // set_flags(flag_type::EN_CFT_IS_FIBER); // can not use set_flags to set a coroutine context's flag here
}

LIBCOPP_COPP_API coroutine_context_fiber::~coroutine_context_fiber() {
  if (nullptr != callee_) {
    DeleteFiber(callee_);
  }
}

LIBCOPP_COPP_API int coroutine_context_fiber::create(coroutine_context_fiber *p, callback_type &&runner,
                                                     const stack_context &callee_stack, size_t coroutine_size,
                                                     size_t private_buffer_size,
                                                     size_t stack_reserve_size_of_fiber) LIBCOPP_MACRO_NOEXCEPT {
  if (nullptr == p) {
    return COPP_EC_ARGS_ERROR;
  }

  // must aligned to sizeof(size_t)
  if (0 != (private_buffer_size & (sizeof(size_t) - 1))) {
    return COPP_EC_ARGS_ERROR;
  }

  if (0 != (coroutine_size & (sizeof(size_t) - 1))) {
    return COPP_EC_ARGS_ERROR;
  }

  size_t stack_offset = private_buffer_size + coroutine_size;
  if (nullptr == callee_stack.sp || callee_stack.size <= stack_offset) {
    return COPP_EC_ARGS_ERROR;
  }

  // stack down
  // |STACK BUFFER........COROUTINE..this..padding..PRIVATE DATA.....callee_stack.sp |
  // |------------------------------callee_stack.size -------------------------------|
  if (callee_stack.sp <= p || coroutine_size < sizeof(coroutine_context_fiber)) {
    return COPP_EC_ARGS_ERROR;
  }

  size_t this_offset = reinterpret_cast<unsigned char *>(callee_stack.sp) - reinterpret_cast<unsigned char *>(p);
  if (this_offset < sizeof(coroutine_context_fiber) + private_buffer_size || this_offset > stack_offset) {
    return COPP_EC_ARGS_ERROR;
  }

  // if runner is empty, we can set it later
  p->set_runner(std::move(runner));

  if (&p->callee_stack_ != &callee_stack) {
    p->callee_stack_ = callee_stack;
  }
  p->private_buffer_size_ = private_buffer_size;

  // stack down, left enough private data
  p->priv_data_ = reinterpret_cast<unsigned char *>(p->callee_stack_.sp) - p->private_buffer_size_;
  p->callee_ =
      CreateFiberEx(0, stack_reserve_size_of_fiber,
                    0,  // We don't use FIBER_FLAG_FLOAT_SWITCH because fcontext version also don't save XMM0-XMM7
                    &libcopp_fiber_internal_api_set::coroutine_fiber_context_callback, p);
  if (nullptr == p->callee_) {
    return COPP_EC_FCONTEXT_MAKE_FAILED;
  }

  return COPP_EC_SUCCESS;
}

#  if defined(LIBCOPP_MACRO_ENABLE_STD_EXCEPTION_PTR) && LIBCOPP_MACRO_ENABLE_STD_EXCEPTION_PTR
LIBCOPP_COPP_API int coroutine_context_fiber::start(void *priv_data) {
  std::exception_ptr eptr;
  int ret = start(eptr, priv_data);
  maybe_rethrow(eptr);
  return ret;
}

LIBCOPP_COPP_API int coroutine_context_fiber::start(std::exception_ptr &unhandled,
                                                    void *priv_data) LIBCOPP_MACRO_NOEXCEPT {
#  else
LIBCOPP_COPP_API int coroutine_context_fiber::start(void *priv_data) {
#  endif
  if (nullptr == callee_) {
    return COPP_EC_NOT_INITED;
  }

  coroutine_context_base *this_ctx = coroutine_context_base::get_this_coroutine_base();
  if (this_ctx && !this_ctx->check_flags(flag_type::EN_CFT_IS_FIBER)) {
    return LIBCOPP_COPP_NAMESPACE_ID::COPP_EC_CAN_NOT_USE_CROSS_FCONTEXT_AND_FIBER;
  }

  int from_status = status_type::EN_CRS_READY;
  do {
    if (from_status < status_type::EN_CRS_READY) {
      return COPP_EC_NOT_INITED;
    }

    if (status_.compare_exchange_strong(from_status, status_type::EN_CRS_RUNNING,
                                        LIBCOPP_COPP_NAMESPACE_ID::util::lock::memory_order_acq_rel,
                                        LIBCOPP_COPP_NAMESPACE_ID::util::lock::memory_order_acquire)) {
      break;
    } else {
      // finished or stoped
      if (from_status > status_type::EN_CRS_RUNNING) {
        return COPP_EC_NOT_READY;
      }

      // already running
      if (status_type::EN_CRS_RUNNING == from_status) {
        return COPP_EC_IS_RUNNING;
      }
    }
  } while (true);

  libcopp_fiber_internal_api_set::jump_src_data_t jump_src =
      libcopp_fiber_internal_api_set::build_this_fiber_jump_src(*this, priv_data);
  jump_to(callee_, jump_src);

  // Move changing status to EN_CRS_EXITED is finished
  if (check_flags(flag_type::EN_CFT_FINISHED)) {
    // if in finished status, change it to exited
    status_.store(status_type::EN_CRS_EXITED, LIBCOPP_COPP_NAMESPACE_ID::util::lock::memory_order_release);
  }

#  if defined(LIBCOPP_MACRO_ENABLE_STD_EXCEPTION_PTR) && LIBCOPP_MACRO_ENABLE_STD_EXCEPTION_PTR
  COPP_UNLIKELY_IF (unhandle_exception_) {
    std::swap(unhandled, unhandle_exception_);
  }
#  endif

  return COPP_EC_SUCCESS;
}

LIBCOPP_COPP_API int coroutine_context_fiber::resume(void *priv_data) { return start(priv_data); }
#  if defined(LIBCOPP_MACRO_ENABLE_STD_EXCEPTION_PTR) && LIBCOPP_MACRO_ENABLE_STD_EXCEPTION_PTR
LIBCOPP_COPP_API int coroutine_context_fiber::resume(std::exception_ptr &unhandled,
                                                     void *priv_data) LIBCOPP_MACRO_NOEXCEPT {
  return start(unhandled, priv_data);
}
#  endif

LIBCOPP_COPP_API int coroutine_context_fiber::yield(void **priv_data) LIBCOPP_MACRO_NOEXCEPT {
  if (nullptr == callee_) {
    return COPP_EC_NOT_INITED;
  }

  int from_status = status_type::EN_CRS_RUNNING;
  int to_status = status_type::EN_CRS_READY;
  if (check_flags(flag_type::EN_CFT_FINISHED)) {
    to_status = status_type::EN_CRS_FINISHED;
  }
  if (false == status_.compare_exchange_strong(from_status, to_status,
                                               LIBCOPP_COPP_NAMESPACE_ID::util::lock::memory_order_acq_rel,
                                               LIBCOPP_COPP_NAMESPACE_ID::util::lock::memory_order_acquire)) {
    switch (from_status) {
      case status_type::EN_CRS_INVALID:
        return COPP_EC_NOT_INITED;
      case status_type::EN_CRS_READY:
        return COPP_EC_NOT_RUNNING;
      case status_type::EN_CRS_FINISHED:
      case status_type::EN_CRS_EXITED:
        return COPP_EC_ALREADY_EXIST;
      default:
        return COPP_EC_UNKNOWN;
    }
  }

  // success or finished will continue
  jump_src_data_t jump_data;
  jump_data.from_co = this;
  jump_data.from_fiber = callee_;
  jump_data.to_co = nullptr;
  jump_data.to_fiber = caller_;
  jump_data.priv_data = nullptr;

  jump_to(caller_, jump_data);

  if (nullptr != priv_data) {
    *priv_data = jump_data.priv_data;
  }

  return COPP_EC_SUCCESS;
}

namespace this_fiber {
LIBCOPP_COPP_API coroutine_context_fiber *get_coroutine() LIBCOPP_MACRO_NOEXCEPT {
  coroutine_context_base *ret = coroutine_context_base::get_this_coroutine_base();
  if (ret && !ret->check_flags(coroutine_context_base::flag_type::EN_CFT_IS_FIBER)) {
    ret = nullptr;
  }
  return static_cast<coroutine_context_fiber *>(ret);
}

LIBCOPP_COPP_API int yield(void **priv_data) LIBCOPP_MACRO_NOEXCEPT {
  coroutine_context_fiber *pco = get_coroutine();
  if (nullptr != pco) {
    return pco->yield(priv_data);
  }

  return COPP_EC_NOT_RUNNING;
}
}  // namespace this_fiber
LIBCOPP_COPP_NAMESPACE_END

#endif

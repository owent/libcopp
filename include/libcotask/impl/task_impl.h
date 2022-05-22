// Copyright 2022 owent

#pragma once

#include <libcopp/utils/config/libcopp_build_features.h>

#include <libcopp/utils/atomic_int_type.h>

#include <libcopp/utils/uint64_id_allocator.h>

#include <libcopp/utils/lock_holder.h>
#include <libcopp/utils/spin_lock.h>

#include <libcotask/task_actions.h>

#include <stdint.h>
#include <list>
#include <memory>

LIBCOPP_COTASK_NAMESPACE_BEGIN
enum EN_TASK_STATUS {
  EN_TS_INVALID = 0,
  EN_TS_CREATED,
  EN_TS_RUNNING,
  EN_TS_WAITING,
  EN_TS_DONE,
  EN_TS_CANCELED,
  EN_TS_KILLED,
  EN_TS_TIMEOUT,
};

namespace impl {

class UTIL_SYMBOL_VISIBLE task_impl {
 public:
  using id_type = LIBCOPP_COPP_NAMESPACE_ID::util::uint64_id_allocator::value_type;
  using id_allocator_type = LIBCOPP_COPP_NAMESPACE_ID::util::uint64_id_allocator;

  // Compability with libcopp-1.x
  using id_t = id_type;
  using id_allocator_t = id_allocator_type;

 protected:
  using action_ptr_type = task_action_impl *;

  // Compability with libcopp-1.x
  using action_ptr_t = action_ptr_type;

  struct LIBCOPP_COTASK_API ext_coroutine_flag_t {
    enum type {
      EN_ECFT_UNKNOWN = 0,
      EN_ECFT_COTASK = 0x0100,
      EN_ECFT_MASK = 0xFF00,
    };
  };

 private:
  task_impl(const task_impl &) = delete;
  task_impl &operator=(const task_impl &) = delete;

  task_impl(const task_impl &&) = delete;
  task_impl &operator=(const task_impl &&) = delete;

 public:
  LIBCOPP_COTASK_API task_impl();
  LIBCOPP_COTASK_API virtual ~task_impl() = 0;

  UTIL_FORCEINLINE id_type get_id() const LIBCOPP_MACRO_NOEXCEPT { return id_; }

  /**
   * get task status
   * @return task status
   */
  UTIL_FORCEINLINE EN_TASK_STATUS get_status() const LIBCOPP_MACRO_NOEXCEPT {
    return static_cast<EN_TASK_STATUS>(status_.load(LIBCOPP_COPP_NAMESPACE_ID::util::lock::memory_order_acquire));
  }

  LIBCOPP_COTASK_API virtual bool is_canceled() const LIBCOPP_MACRO_NOEXCEPT;
  LIBCOPP_COTASK_API virtual bool is_completed() const LIBCOPP_MACRO_NOEXCEPT;
  LIBCOPP_COTASK_API virtual bool is_faulted() const LIBCOPP_MACRO_NOEXCEPT;
  LIBCOPP_COTASK_API virtual bool is_timeout() const LIBCOPP_MACRO_NOEXCEPT;
  /**
   * @brief check if a cotask is exiting
   * @note cotask is exiting means the cotask is is_completed() or is killed.
   *       if a cotask is killed and is running, then is_completed() == false but is_exiting() == true,
   *       and after the cotask finished, is_completed() == true
   * @return return true if a cotask is exiting.
   */
  LIBCOPP_COTASK_API bool is_exiting() const LIBCOPP_MACRO_NOEXCEPT;

 public:
  LIBCOPP_COTASK_API virtual int get_ret_code() const = 0;

  virtual int start(void *priv_data, EN_TASK_STATUS expected_status = EN_TS_CREATED) = 0;
  virtual int resume(void *priv_data, EN_TASK_STATUS expected_status = EN_TS_WAITING) = 0;
  virtual int yield(void **priv_data) = 0;
  virtual int cancel(void *priv_data) = 0;
  virtual int kill(enum EN_TASK_STATUS status, void *priv_data) = 0;
  UTIL_FORCEINLINE int kill(void *priv_data) { return kill(EN_TS_KILLED, priv_data); }

  UTIL_FORCEINLINE int start() { return start(nullptr); }
  UTIL_FORCEINLINE int resume() { return resume(nullptr); }
  UTIL_FORCEINLINE int yield() { return yield(nullptr); }
  UTIL_FORCEINLINE int cancel() { return cancel(nullptr); }
  UTIL_FORCEINLINE int kill(enum EN_TASK_STATUS status) { return kill(status, nullptr); }
  UTIL_FORCEINLINE int kill() { return kill(nullptr); }

  LIBCOPP_COTASK_API virtual int on_finished();

#if defined(LIBCOPP_MACRO_ENABLE_WIN_FIBER) && LIBCOPP_MACRO_ENABLE_WIN_FIBER
  virtual bool is_fiber() const LIBCOPP_MACRO_NOEXCEPT = 0;
#endif

  /**
   * get current running task
   * @return current running task or empty pointer
   */
  static LIBCOPP_COTASK_API task_impl *this_task();

  /**
   * @brief get raw action pointer
   * @note this function is provided just for debug or show some information, it may return the inner type created by
   * cotask
   * @return pointer to task_action instance
   */
  UTIL_FORCEINLINE action_ptr_type get_raw_action() const LIBCOPP_MACRO_NOEXCEPT { return action_; }

 protected:
  LIBCOPP_COTASK_API void _set_action(action_ptr_type action);
  LIBCOPP_COTASK_API action_ptr_type _get_action();

  LIBCOPP_COTASK_API bool _cas_status(EN_TASK_STATUS &expected, EN_TASK_STATUS desired);

#if defined(LIBCOPP_MACRO_ENABLE_STD_EXCEPTION_PTR) && LIBCOPP_MACRO_ENABLE_STD_EXCEPTION_PTR
  LIBCOPP_COTASK_API int _notify_finished(std::list<std::exception_ptr> &unhandled, void *priv_data);
#else
  LIBCOPP_COTASK_API int _notify_finished(void *priv_data);
#endif

 private:
  action_ptr_type action_;
  id_type id_;

 protected:
  void *finish_priv_data_;

 private:
#if !defined(LIBCOPP_DISABLE_ATOMIC_LOCK) || !(LIBCOPP_DISABLE_ATOMIC_LOCK)
  LIBCOPP_COPP_NAMESPACE_ID::util::lock::atomic_int_type<uint32_t> status_;
#else
  LIBCOPP_COPP_NAMESPACE_ID::util::lock::atomic_int_type<
      LIBCOPP_COPP_NAMESPACE_ID::util::lock::unsafe_int_type<uint32_t> >
      status_;
#endif
};
}  // namespace impl
LIBCOPP_COTASK_NAMESPACE_END

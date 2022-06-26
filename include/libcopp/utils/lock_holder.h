/**
 * @file lock_holder.h
 * @brief 锁管理器
 * Licensed under the MIT licenses.
 *
 * @version 1.0
 * @author owent
 * @date 2015-06-29
 *
 * @note 实现锁的自管理操作,类似std::lock_guard，增加了针对特定场合使用的try_unlock函数
 *
 * @history
 */

#pragma once

// clang-format off
#include <libcopp/utils/config/stl_include_prefix.h>  // NOLINT(build/include_order)
// clang-format on
#include <cstring>
// clang-format off
#include <libcopp/utils/config/stl_include_suffix.h>  // NOLINT(build/include_order)
// clang-format on

#include <libcopp/utils/config/libcopp_build_features.h>

LIBCOPP_COPP_NAMESPACE_BEGIN
namespace util {
namespace lock {
namespace detail {
template <typename TLock>
struct LIBCOPP_COPP_API_HEAD_ONLY default_lock_action {
  inline bool operator()(TLock &lock) const noexcept {
    lock.lock();
    return true;
  }
};

template <typename TLock>
struct LIBCOPP_COPP_API_HEAD_ONLY default_try_lock_action {
  inline bool operator()(TLock &lock) const noexcept { return lock.try_lock(); }
};

template <typename TLock>
struct LIBCOPP_COPP_API_HEAD_ONLY default_unlock_action {
  inline void operator()(TLock &lock) const noexcept { lock.unlock(); }
};

template <typename TLock>
struct LIBCOPP_COPP_API_HEAD_ONLY default_try_unlock_action {
  inline bool operator()(TLock &lock) const noexcept { return lock.try_unlock(); }
};

template <typename TLock>
struct LIBCOPP_COPP_API_HEAD_ONLY default_read_lock_action {
  inline bool operator()(TLock &lock) const noexcept {
    lock.read_lock();
    return true;
  }
};

template <typename TLock>
struct LIBCOPP_COPP_API_HEAD_ONLY default_read_unlock_action {
  inline void operator()(TLock &lock) const noexcept { lock.read_unlock(); }
};

template <typename TLock>
struct LIBCOPP_COPP_API_HEAD_ONLY default_write_lock_action {
  inline bool operator()(TLock &lock) const noexcept {
    lock.write_lock();
    return true;
  }
};

template <typename TLock>
struct LIBCOPP_COPP_API_HEAD_ONLY default_write_unlock_action {
  inline void operator()(TLock &lock) const noexcept { lock.write_unlock(); }
};
}  // namespace detail

template <typename TLock, typename TLockAct = detail::default_lock_action<TLock>,
          typename TUnlockAct = detail::default_unlock_action<TLock> >
class LIBCOPP_COPP_API_HEAD_ONLY lock_holder {
 public:
  using value_type = TLock;

  lock_holder(lock_holder &&other) : lock_flag_(other.lock_flag_) { other.lock_flag_ = nullptr; }

  inline lock_holder &operator=(lock_holder &&other) noexcept {
    if (&other == this) {
      return *this;
    }

    if (lock_flag_ != other.lock_flag_) {
      reset();
    }

    lock_flag_ = other.lock_flag_;
    return *this;
  }

  inline lock_holder(TLock &lock) : lock_flag_(&lock) {
    if (false == TLockAct()(lock)) {
      lock_flag_ = nullptr;
    }
  }

  ~lock_holder() {
    if (nullptr != lock_flag_) {
      TUnlockAct()(*lock_flag_);
    }
  }

  inline bool is_available() const noexcept { return nullptr != lock_flag_; }

  inline void reset() noexcept {
    if (nullptr != lock_flag_) {
      value_type *value = lock_flag_;
      lock_flag_ = nullptr;
      TUnlockAct()(*value);
    }
  }

 private:
  lock_holder(const lock_holder &) = delete;
  lock_holder &operator=(const lock_holder &) = delete;

 private:
  value_type *lock_flag_;
};

template <typename TLock>
class LIBCOPP_COPP_API_HEAD_ONLY read_lock_holder
    : public lock_holder<TLock, detail::default_read_lock_action<TLock>, detail::default_read_unlock_action<TLock> > {
 public:
  inline read_lock_holder(TLock &lock)
      : lock_holder<TLock, detail::default_read_lock_action<TLock>, detail::default_read_unlock_action<TLock> >(lock) {}
};

template <typename TLock>
class LIBCOPP_COPP_API_HEAD_ONLY write_lock_holder
    : public lock_holder<TLock, detail::default_write_lock_action<TLock>, detail::default_write_unlock_action<TLock> > {
 public:
  inline write_lock_holder(TLock &lock)
      : lock_holder<TLock, detail::default_write_lock_action<TLock>, detail::default_write_unlock_action<TLock> >(
            lock) {}
};
}  // namespace lock
}  // namespace util
LIBCOPP_COPP_NAMESPACE_END

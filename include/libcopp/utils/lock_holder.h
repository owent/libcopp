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

#ifndef UTIL_LOCK_LOCK_HOLDER_H
#define UTIL_LOCK_LOCK_HOLDER_H

#pragma once

#include <cstring>

namespace util {
    namespace lock {
        namespace detail {
            template <typename TLock>
            struct default_lock_action {
                bool operator()(TLock &lock) const {
                    lock.lock();
                    return true;
                }
            };

            template <typename TLock>
            struct default_try_lock_action {
                bool operator()(TLock &lock) const { return lock.try_lock(); }
            };

            template <typename TLock>
            struct default_unlock_action {
                void operator()(TLock &lock) const { lock.unlock(); }
            };

            template <typename TLock>
            struct default_try_unlock_action {
                bool operator()(TLock &lock) const { return lock.try_unlock(); }
            };
        }

        template <typename TLock, typename TLockAct = detail::default_lock_action<TLock>,
                  typename TUnlockAct = detail::default_unlock_action<TLock> >
        class lock_holder {
        public:
            typedef TLock value_type;

            lock_holder(TLock &lock) : lock_flag_(&lock) {
                if (false == TLockAct()(lock)) {
                    lock_flag_ = NULL;
                }
            }

            ~lock_holder() {
                if (NULL != lock_flag_) {
                    TUnlockAct()(*lock_flag_);
                }
            }

            bool is_available() const { return NULL != lock_flag_; }

        private:
            lock_holder(const lock_holder &);
            lock_holder &operator=(const lock_holder &);

        private:
            value_type *lock_flag_;
        };
    }
}

#endif /* _UTIL_LOCK_LOCK_HOLDER_H_ */

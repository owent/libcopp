﻿/**
 * @file atomic_int_type_msvc_impl.h
 * @brief 整数类型的原子操作-MSVC统一接口
 * Licensed under the MIT licenses.
 *
 * @version 1.0
 * @author owent
 * @date 2016-06-14
 *
 * @note 低版本 VC使用InterlockedExchange等实现原子操作
 * @see https://msdn.microsoft.com/en-us/library/windows/desktop/ms683560(v=vs.85).aspx
 *
 * @history
 *     2016-06-14
 */

#ifndef _UTIL_LOCK_ATOMIC_INT_TYPE_MSVC_IMPL_H_
#define _UTIL_LOCK_ATOMIC_INT_TYPE_MSVC_IMPL_H_

#pragma once

namespace util {
    namespace lock {
        namespace detail {
            template<int INT_SIZE>
            struct atomic_msvc_oprs;

            template<>
            struct atomic_msvc_oprs<1> {}

            template<int INT_SIZE>
            struct atomic_msvc_oprs {
                typedef LONG opr_t;

                static opr_t exchange(volatile opr_t * target, opr_t value, ::util::lock::memory_order order) {
                    switch(order) {
                        case ::util::lock::memory_order_relaxed:
                            return InterlockedExchangeNoFence(target, value);
                        case ::util::lock::memory_order_acquire:
                            return InterlockedExchangeAcquire(target, value);
                        case ::util::lock::memory_order_release:
                            return InterlockedExchangeRelease(target, value);
                        default:
                            return InterlockedExchange(target, value);
                    }
                }

                static opr_t cas(volatile opr_t * target, opr_t value, opr_t expected, ::util::lock::memory_order order) {
                    switch(order) {
                        case ::util::lock::memory_order_relaxed:
                            return InterlockedCompareExchangeNoFence(target, value, expected);
                        case ::util::lock::memory_order_acquire:
                            return InterlockedCompareExchangeAcquire(target, value, expected);
                        case ::util::lock::memory_order_release:
                            return InterlockedCompareExchangeRelease(target, value, expected);
                        default:
                            return InterlockedCompareExchange(target, value, expected);
                    }
                }

                static opr_t add(volatile opr_t * target, opr_t value, ::util::lock::memory_order order) {
                    switch(order) {
                        case ::util::lock::memory_order_relaxed:
                            return InterlockedAddNoFence(target, value);
                        case ::util::lock::memory_order_acquire:
                            return InterlockedAddAcquire(target, value);
                        case ::util::lock::memory_order_release:
                            return InterlockedAddRelease(target, value);
                        default:
                            return InterlockedAdd(target, value);
                    }
                }

                static opr_t sub(volatile opr_t * target, opr_t value, ::util::lock::memory_order order) {
                    switch(order) {
                        case ::util::lock::memory_order_relaxed:
                            return InterlockedAddNoFence(target, ~(value - 1);
                        case ::util::lock::memory_order_acquire:
                            return InterlockedAddAcquire(target, ~(value - 1));
                        case ::util::lock::memory_order_release:
                            return InterlockedAddRelease(target, ~(value - 1));
                        default:
                            return InterlockedAdd(target, ~(value - 1));
                    }
                }

                static opr_t and(volatile opr_t * target, opr_t value, ::util::lock::memory_order order) {
                    switch(order) {
                        case ::util::lock::memory_order_relaxed:
                            return InterlockedAndNoFence(target, value);
                        case ::util::lock::memory_order_acquire:
                            return InterlockedAndAcquire(target, value);
                        case ::util::lock::memory_order_release:
                            return InterlockedAndRelease(target, value);
                        default:
                            return InterlockedAnd(target, value);
                    }
                }

                static opr_t or(volatile opr_t * target, opr_t value, ::util::lock::memory_order order) {
                    switch(order) {
                        case ::util::lock::memory_order_relaxed:
                            return InterlockedOrNoFence(target, value);
                        case ::util::lock::memory_order_acquire:
                            return InterlockedOrAcquire(target, value);
                        case ::util::lock::memory_order_release:
                            return InterlockedOrRelease(target, value);
                        default:
                            return InterlockedOr(target, value);
                    }
                }

                static opr_t xor(volatile opr_t * target, opr_t value, ::util::lock::memory_order order) {
                    switch(order) {
                        case ::util::lock::memory_order_relaxed:
                            return InterlockedXorNoFence(target, value);
                        case ::util::lock::memory_order_acquire:
                            return InterlockedXorAcquire(target, value);
                        case ::util::lock::memory_order_release:
                            return InterlockedXorRelease(target, value);
                        default:
                            return InterlockedXor(target, value);
                    }
                }
            };
        }
    }
}

#endif /* _UTIL_LOCK_ATOMIC_INT_TYPE_MSVC_IMPL_H_ */

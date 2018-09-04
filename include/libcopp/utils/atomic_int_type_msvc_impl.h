/**
 * @file atomic_int_type_msvc_impl.h
 * @brief 整数类型的原子操作-MSVC统一接口
 * Licensed under the MIT licenses.
 *
 * @version 1.0
 * @author owent
 * @date 2016-06-14
 *
 * @note 低版本 VC使用InterlockedExchange等实现原子操作
 * @see https://msdn.microsoft.com/en-us/library/windows/desktop/ms686360(v=vs.85).aspx
 *
 * @history
 *     2016-06-14
 */
#ifndef UTIL_LOCK_ATOMIC_INT_TYPE_MSVC_IMPL_H
#define UTIL_LOCK_ATOMIC_INT_TYPE_MSVC_IMPL_H

#pragma once

#include <Windows.h>

namespace util {
    namespace lock {

        enum memory_order {
            memory_order_relaxed = 0,
            memory_order_consume,
            memory_order_acquire,
            memory_order_release,
            memory_order_acq_rel,
            memory_order_seq_cst
        };

        namespace detail {

            template<int INT_SIZE>
            struct atomic_msvc_oprs;

            template<>
            struct atomic_msvc_oprs<1> {
                typedef char opr_t;

                static opr_t exchange(volatile opr_t * target, opr_t value, ::util::lock::memory_order order) {
                    return InterlockedExchange8(target, value);
                }

                static opr_t cas(volatile opr_t * target, opr_t value, opr_t expected, ::util::lock::memory_order order) {
                    volatile short* star = reinterpret_cast<volatile short*>(target);
                    short real_expect = expected;
                    real_expect |= 0xFF00 & *star;

                    switch (order) {
                    case ::util::lock::memory_order_relaxed:
                        return static_cast<opr_t>(InterlockedCompareExchangeNoFence16(star, value, real_expect));
                    case ::util::lock::memory_order_acquire:
                        return static_cast<opr_t>(InterlockedCompareExchangeAcquire16(star, value, real_expect));
                    case ::util::lock::memory_order_release:
                        return static_cast<opr_t>(InterlockedCompareExchangeRelease16(star, value, real_expect));
                    default:
                        return static_cast<opr_t>(InterlockedCompareExchange16(star, value, real_expect));
                    }
                }

                static opr_t inc(volatile opr_t * target, ::util::lock::memory_order order) {
                    return add(target, 1, order);
                }

                static opr_t dec(volatile opr_t * target, ::util::lock::memory_order order) {
                    return sub(target, 1, order);
                }

                static opr_t add(volatile opr_t * target, opr_t value, ::util::lock::memory_order order) {
                    // no api just like InterlockedAdd16, use cas to simulate it
                    if (NULL == target || 0 == value) {
                        return 0;
                    }

                    opr_t ret = *target;
                    while (true) {
                        if (ret == cas(target, ret + value, ret, order)) {
                            ret += value;
                            break;
                        }
                        ret = *target;
                    }

                    return ret;
                }

                static opr_t sub(volatile opr_t * target, opr_t value, ::util::lock::memory_order order) {
                    // no api just like InterlockedAdd16, use cas to simulate it
                    if (NULL == target || 0 == value) {
                        return 0;
                    }

                    opr_t ret = *target;
                    while (true) {
                        if (ret == cas(target, ret - value, ret, order)) {
                            ret -= value;
                            break;
                        }
                        ret = *target;
                    }

                    return ret;
                }

                static opr_t and(volatile opr_t * target, opr_t value, ::util::lock::memory_order order) {
                    return InterlockedAnd8(target, value);
                }

                static opr_t or(volatile opr_t * target, opr_t value, ::util::lock::memory_order order) {
                    return InterlockedOr8(target, value);
                }

                static opr_t xor(volatile opr_t * target, opr_t value, ::util::lock::memory_order order) {
                    return InterlockedXor8(target, value);
                }
            };

            template<>
            struct atomic_msvc_oprs<2> {
                typedef SHORT opr_t;

                static opr_t exchange(volatile opr_t * target, opr_t value, ::util::lock::memory_order order) {
                    return InterlockedExchange16(target, value);
                }

                static opr_t cas(volatile opr_t * target, opr_t value, opr_t expected, ::util::lock::memory_order order) {
                    switch(order) {
                        case ::util::lock::memory_order_relaxed:
                            return InterlockedCompareExchangeNoFence16(target, value, expected);
                        case ::util::lock::memory_order_acquire:
                            return InterlockedCompareExchangeAcquire16(target, value, expected);
                        case ::util::lock::memory_order_release:
                            return InterlockedCompareExchangeRelease16(target, value, expected);
                        default:
                            return InterlockedCompareExchange16(target, value, expected);
                    }
                }

                static opr_t inc(volatile opr_t * target, ::util::lock::memory_order order) {
                    switch(order) {
                        case ::util::lock::memory_order_relaxed:
                            return InterlockedIncrementNoFence16(target);
                        case ::util::lock::memory_order_acquire:
                            return InterlockedIncrementAcquire16(target);
                        case ::util::lock::memory_order_release:
                            return InterlockedIncrementRelease16(target);
                        default:
                            return InterlockedIncrement16(target);
                    }
                }

                static opr_t dec(volatile opr_t * target, ::util::lock::memory_order order) {
                    switch(order) {
                        case ::util::lock::memory_order_relaxed:
                            return InterlockedDecrementNoFence16(target);
                        case ::util::lock::memory_order_acquire:
                            return InterlockedDecrementAcquire16(target);
                        case ::util::lock::memory_order_release:
                            return InterlockedDecrementRelease16(target);
                        default:
                            return InterlockedDecrement16(target);
                    }
                }

                static opr_t add(volatile opr_t * target, opr_t value, ::util::lock::memory_order order) {
                    // no api just like InterlockedAdd16, use cas to simulate it
                    if (NULL == target || 0 == value) {
                        return 0;
                    }

                    opr_t ret = *target;
                    while (true) {
                        if (ret == cas(target, ret + value, ret, order)) {
                            ret += value;
                            break;
                        }
                        ret = *target;
                    }

                    return ret;
                }

                static opr_t sub(volatile opr_t * target, opr_t value, ::util::lock::memory_order order) {
                    // no api just like InterlockedAdd16, use cas to simulate it
                    if (NULL == target || 0 == value) {
                        return 0;
                    }

                    opr_t ret = *target;
                    while (true) {
                        if (ret == cas(target, ret - value, ret, order)) {
                            ret -= value;
                            break;
                        }
                        ret = *target;
                    }

                    return ret;
                }

                static opr_t and(volatile opr_t * target, opr_t value, ::util::lock::memory_order order) {
                    return InterlockedAnd16(target, value);
                }

                static opr_t or(volatile opr_t * target, opr_t value, ::util::lock::memory_order order) {
                    return InterlockedOr16(target, value);
                }

                static opr_t xor(volatile opr_t * target, opr_t value, ::util::lock::memory_order order) {
                    return InterlockedXor16(target, value);
                }
            };

            template<>
            struct atomic_msvc_oprs<8> {
                typedef LONGLONG opr_t;

                static opr_t exchange(volatile opr_t * target, opr_t value, ::util::lock::memory_order order) {
                    switch(order) {
                        case ::util::lock::memory_order_relaxed:
                            return InterlockedExchangeNoFence64(target, value);
                        case ::util::lock::memory_order_acquire:
                            return InterlockedExchangeAcquire64(target, value);
                        default:
                            return InterlockedExchange64(target, value);
                    }
                }

                static opr_t cas(volatile opr_t * target, opr_t value, opr_t expected, ::util::lock::memory_order order) {
                    switch(order) {
                        case ::util::lock::memory_order_relaxed:
                            return InterlockedCompareExchangeNoFence64(target, value, expected);
                        case ::util::lock::memory_order_acquire:
                            return InterlockedCompareExchangeAcquire64(target, value, expected);
                        case ::util::lock::memory_order_release:
                            return InterlockedCompareExchangeRelease64(target, value, expected);
                        default:
                            return InterlockedCompareExchange64(target, value, expected);
                    }
                }

                static opr_t inc(volatile opr_t * target, ::util::lock::memory_order order) {
                    switch(order) {
                        case ::util::lock::memory_order_relaxed:
                            return InterlockedIncrementNoFence64(target);
                        case ::util::lock::memory_order_acquire:
                            return InterlockedIncrementAcquire64(target);
                        case ::util::lock::memory_order_release:
                            return InterlockedIncrementRelease64(target);
                        default:
                            return InterlockedIncrement64(target);
                    }
                }

                static opr_t dec(volatile opr_t * target, ::util::lock::memory_order order) {
                    switch(order) {
                        case ::util::lock::memory_order_relaxed:
                            return InterlockedDecrementNoFence64(target);
                        case ::util::lock::memory_order_acquire:
                            return InterlockedDecrementAcquire64(target);
                        case ::util::lock::memory_order_release:
                            return InterlockedDecrementRelease64(target);
                        default:
                            return InterlockedDecrement64(target);
                    }
                }

                static opr_t add(volatile opr_t * target, opr_t value, ::util::lock::memory_order order) {
                    switch(order) {
                        case ::util::lock::memory_order_relaxed:
                            return InterlockedAddNoFence64(target, value);
                        case ::util::lock::memory_order_acquire:
                            return InterlockedAddAcquire64(target, value);
                        case ::util::lock::memory_order_release:
                            return InterlockedAddRelease64(target, value);
                        default:
                            return InterlockedAdd64(target, value);
                    }
                }

                static opr_t sub(volatile opr_t * target, opr_t value, ::util::lock::memory_order order) {
                    switch(order) {
                        case ::util::lock::memory_order_relaxed:
                            return InterlockedAddNoFence64(target, ~(value - 1));
                        case ::util::lock::memory_order_acquire:
                            return InterlockedAddAcquire64(target, ~(value - 1));
                        case ::util::lock::memory_order_release:
                            return InterlockedAddRelease64(target, ~(value - 1));
                        default:
                            return InterlockedAdd64(target, ~(value - 1));
                    }
                }

                static opr_t and(volatile opr_t * target, opr_t value, ::util::lock::memory_order order) {
                    switch(order) {
                        case ::util::lock::memory_order_relaxed:
                            return InterlockedAnd64NoFence(target, value);
                        case ::util::lock::memory_order_acquire:
                            return InterlockedAnd64Acquire(target, value);
                        case ::util::lock::memory_order_release:
                            return InterlockedAnd64Release(target, value);
                        default:
                            return InterlockedAnd64(target, value);
                    }
                }

                static opr_t or(volatile opr_t * target, opr_t value, ::util::lock::memory_order order) {
                    switch(order) {
                        case ::util::lock::memory_order_relaxed:
                            return InterlockedOr64NoFence(target, value);
                        case ::util::lock::memory_order_acquire:
                            return InterlockedOr64Acquire(target, value);
                        case ::util::lock::memory_order_release:
                            return InterlockedOr64Release(target, value);
                        default:
                            return InterlockedOr64(target, value);
                    }
                }

                static opr_t xor(volatile opr_t * target, opr_t value, ::util::lock::memory_order order) {
                    switch(order) {
                        case ::util::lock::memory_order_relaxed:
                            return InterlockedXor64NoFence(target, value);
                        case ::util::lock::memory_order_acquire:
                            return InterlockedXor64Acquire(target, value);
                        case ::util::lock::memory_order_release:
                            return InterlockedXor64Release(target, value);
                        default:
                            return InterlockedXor64(target, value);
                    }
                }
            };

            template<int INT_SIZE>
            struct atomic_msvc_oprs {
                typedef LONG opr_t;

                static opr_t exchange(volatile opr_t * target, opr_t value, ::util::lock::memory_order order) {
                    switch(order) {
                        case ::util::lock::memory_order_relaxed:
                            return InterlockedExchangeNoFence(target, value);
                        case ::util::lock::memory_order_acquire:
                            return InterlockedExchangeAcquire(target, value);
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

                static opr_t inc(volatile opr_t * target, ::util::lock::memory_order order) {
                    switch(order) {
                        case ::util::lock::memory_order_relaxed:
                            return InterlockedIncrementNoFence(target);
                        case ::util::lock::memory_order_acquire:
                            return InterlockedIncrementAcquire(target);
                        case ::util::lock::memory_order_release:
                            return InterlockedIncrementRelease(target);
                        default:
                            return InterlockedIncrement(target);
                    }
                }

                static opr_t dec(volatile opr_t * target, ::util::lock::memory_order order) {
                    switch(order) {
                        case ::util::lock::memory_order_relaxed:
                            return InterlockedDecrementNoFence(target);
                        case ::util::lock::memory_order_acquire:
                            return InterlockedDecrementAcquire(target);
                        case ::util::lock::memory_order_release:
                            return InterlockedDecrementRelease(target);
                        default:
                            return InterlockedDecrement(target);
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
                            return InterlockedAddNoFence(target, ~(value - 1));
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

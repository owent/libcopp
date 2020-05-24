/**
 *
 * @file intrusive_ptr.h
 * @brief 侵入式智能指针
 * @note 这不是std标准中的一部分，但是这是对smart_ptr.h的补充
 * Licensed under the MIT licenses.
 *
 * @version 1.0
 * @author OWenT, owt5008137@live.com
 * @date 2017.05.18
 * @history
 *
 */

#ifndef LIBCOPP_UTIL_INTRUSIVE_PTR_H
#define LIBCOPP_UTIL_INTRUSIVE_PTR_H

#pragma once

#include <assert.h>
#include <cstddef>
#include <memory>
#include <ostream>

#ifdef __cpp_impl_three_way_comparison
#include <compare>
#endif

#include <libcopp/utils/config/compiler_features.h>
#include <libcopp/utils/config/libcopp_build_features.h>

namespace libcopp {
    namespace util {
        //
        //  intrusive_ptr
        //
        //  A smart pointer that uses intrusive reference counting.
        //
        //  Relies on unqualified calls to
        //
        //      void intrusive_ptr_add_ref(T * p);
        //      void intrusive_ptr_release(T * p);
        //
        //          (p != NULL)
        //
        //  The object is responsible for destroying itself.
        //

        template <typename T>
        class intrusive_ptr {
        public:
            typedef intrusive_ptr<T> self_type;
            typedef T                element_type;

            UTIL_CONFIG_CONSTEXPR intrusive_ptr() LIBCOPP_MACRO_NOEXCEPT : px(NULL) {}

            intrusive_ptr(T *p, bool add_ref = true) : px(p) {
                if (px != NULL && add_ref) {
                    intrusive_ptr_add_ref(px);
                }
            }

            template <typename U>
            intrusive_ptr(intrusive_ptr<U> const &rhs, typename std::enable_if<std::is_convertible<U *, T *>::value>::type * = NULL)
                : px(rhs.get()) {
                if (px != NULL) {
                    intrusive_ptr_add_ref(px);
                }
            }

            intrusive_ptr(self_type const &rhs) : px(rhs.px) {
                if (px != NULL) {
                    intrusive_ptr_add_ref(px);
                }
            }

            ~intrusive_ptr() {
                if (px != NULL) {
                    intrusive_ptr_release(px);
                }
            }

            template <typename U>
            friend class intrusive_ptr;

            template <typename U>
            intrusive_ptr &operator=(intrusive_ptr<U> const &rhs) {
                self_type(rhs).swap(*this);
                return *this;
            }

            // Move support

    #if defined(UTIL_CONFIG_COMPILER_CXX_RVALUE_REFERENCES) && UTIL_CONFIG_COMPILER_CXX_RVALUE_REFERENCES

            intrusive_ptr(self_type &&rhs) LIBCOPP_MACRO_NOEXCEPT : px(rhs.px) { rhs.px = NULL; }

            self_type &operator=(self_type &&rhs) LIBCOPP_MACRO_NOEXCEPT {
                self_type(static_cast<self_type &&>(rhs)).swap(*this);
                return *this;
            }

            template <typename U>
            intrusive_ptr(intrusive_ptr<U> &&rhs,
                        typename std::enable_if<std::is_convertible<U *, T *>::value>::type * = NULL) LIBCOPP_MACRO_NOEXCEPT : px(rhs.px) {
                rhs.px = NULL;
            }

            template <typename U, typename Deleter>
            self_type &operator=(std::unique_ptr<U, Deleter> &&rhs) {
                self_type(rhs.release()).swap(*this);
                return *this;
            }
    #endif

            self_type &operator=(self_type const &rhs) {
                self_type(rhs).swap(*this);
                return *this;
            }

            inline void reset() LIBCOPP_MACRO_NOEXCEPT { self_type().swap(*this); }

            inline void reset(element_type *rhs) { self_type(rhs).swap(*this); }

            inline void reset(element_type *rhs, bool add_ref) { self_type(rhs, add_ref).swap(*this); }

            inline element_type *get() const LIBCOPP_MACRO_NOEXCEPT { return px; }

            inline element_type *detach() LIBCOPP_MACRO_NOEXCEPT {
                element_type *ret = px;
                px                = NULL;
                return ret;
            }

            inline element_type &operator*() const {
                assert(px != 0);
                return *px;
            }

            inline element_type *operator->() const {
                assert(px != 0);
                return px;
            }

            // implicit conversion to "bool"
            inline operator bool() const LIBCOPP_MACRO_NOEXCEPT { return px != NULL; }
            // operator! is redundant, but some compilers need it
            inline bool operator!() const LIBCOPP_MACRO_NOEXCEPT { return px == NULL; }

            inline void swap(intrusive_ptr &rhs) LIBCOPP_MACRO_NOEXCEPT {
                element_type *tmp = px;
                px                = rhs.px;
                rhs.px            = tmp;
            }

        private:
            element_type *px;
        };

        template <typename T, typename U>
        inline bool operator==(intrusive_ptr<T> const &a, intrusive_ptr<U> const &b) LIBCOPP_MACRO_NOEXCEPT {
            return a.get() == b.get();
        }

        template <typename T, typename U>
        inline bool operator==(intrusive_ptr<T> const &a, U *b) LIBCOPP_MACRO_NOEXCEPT {
            return a.get() == b;
        }

        template <typename T, typename U>
        inline bool operator==(T *a, intrusive_ptr<U> const &b) LIBCOPP_MACRO_NOEXCEPT {
            return a == b.get();
        }

    #ifdef __cpp_impl_three_way_comparison
        template <typename T, typename U>
        inline std::strong_ordering operator<=>(intrusive_ptr<T> const &a, intrusive_ptr<U> const &b) LIBCOPP_MACRO_NOEXCEPT {
            return a.get() <=> b.get();
        }

        template <typename T, typename U>
        inline std::strong_ordering operator<=>(intrusive_ptr<T> const &a, U *b) LIBCOPP_MACRO_NOEXCEPT {
            return a.get() <=> b;
        }

        template <typename T, typename U>
        inline std::strong_ordering operator<=>(T *a, intrusive_ptr<U> const &b) LIBCOPP_MACRO_NOEXCEPT {
            return a <=> b.get();
        }
    #else
        template <typename T, typename U>
        inline bool operator!=(intrusive_ptr<T> const &a, intrusive_ptr<U> const &b) LIBCOPP_MACRO_NOEXCEPT {
            return a.get() != b.get();
        }

        template <typename T, typename U>
        inline bool operator!=(intrusive_ptr<T> const &a, U *b) LIBCOPP_MACRO_NOEXCEPT {
            return a.get() != b;
        }

        template <typename T, typename U>
        inline bool operator!=(T *a, intrusive_ptr<U> const &b) LIBCOPP_MACRO_NOEXCEPT {
            return a != b.get();
        }

        template <typename T, typename U>
        inline bool operator<(intrusive_ptr<T> const &a, intrusive_ptr<U> const &b) LIBCOPP_MACRO_NOEXCEPT {
            return a.get() < b.get();
        }

        template <typename T, typename U>
        inline bool operator<(intrusive_ptr<T> const &a, U *b) LIBCOPP_MACRO_NOEXCEPT {
            return a.get() < b;
        }

        template <typename T, typename U>
        inline bool operator<(T *a, intrusive_ptr<U> const &b) LIBCOPP_MACRO_NOEXCEPT {
            return a < b.get();
        }

        template <typename T, typename U>
        inline bool operator<=(intrusive_ptr<T> const &a, intrusive_ptr<U> const &b) LIBCOPP_MACRO_NOEXCEPT {
            return a.get() <= b.get();
        }

        template <typename T, typename U>
        inline bool operator<=(intrusive_ptr<T> const &a, U *b) LIBCOPP_MACRO_NOEXCEPT {
            return a.get() <= b;
        }

        template <typename T, typename U>
        inline bool operator<=(T *a, intrusive_ptr<U> const &b) LIBCOPP_MACRO_NOEXCEPT {
            return a <= b.get();
        }

        template <typename T, typename U>
        inline bool operator>(intrusive_ptr<T> const &a, intrusive_ptr<U> const &b) LIBCOPP_MACRO_NOEXCEPT {
            return a.get() > b.get();
        }

        template <typename T, typename U>
        inline bool operator>(intrusive_ptr<T> const &a, U *b) LIBCOPP_MACRO_NOEXCEPT {
            return a.get() > b;
        }

        template <typename T, typename U>
        inline bool operator>(T *a, intrusive_ptr<U> const &b) LIBCOPP_MACRO_NOEXCEPT {
            return a > b.get();
        }

        template <typename T, typename U>
        inline bool operator>=(intrusive_ptr<T> const &a, intrusive_ptr<U> const &b) LIBCOPP_MACRO_NOEXCEPT {
            return a.get() >= b.get();
        }

        template <typename T, typename U>
        inline bool operator>=(intrusive_ptr<T> const &a, U *b) LIBCOPP_MACRO_NOEXCEPT {
            return a.get() >= b;
        }

        template <typename T, typename U>
        inline bool operator>=(T *a, intrusive_ptr<U> const &b) LIBCOPP_MACRO_NOEXCEPT {
            return a >= b.get();
        }

    #endif

    #if defined(UTIL_CONFIG_COMPILER_CXX_NULLPTR) && UTIL_CONFIG_COMPILER_CXX_NULLPTR

        template <typename T>
        inline bool operator==(intrusive_ptr<T> const &p, std::nullptr_t) LIBCOPP_MACRO_NOEXCEPT {
            return p.get() == nullptr;
        }

        template <typename T>
        inline bool operator==(std::nullptr_t, intrusive_ptr<T> const &p) LIBCOPP_MACRO_NOEXCEPT {
            return p.get() == nullptr;
        }

    #ifdef __cpp_impl_three_way_comparison
        template <typename T>
        inline std::strong_ordering operator<=>(intrusive_ptr<T> const &p, std::nullptr_t) LIBCOPP_MACRO_NOEXCEPT {
            return p.get() <=> nullptr;
        }

        template <typename T>
        inline std::strong_ordering operator<=>(std::nullptr_t, intrusive_ptr<T> const &p) LIBCOPP_MACRO_NOEXCEPT {
            return p.get() <=> nullptr;
        }
    #else
        template <typename T>
        inline bool operator!=(intrusive_ptr<T> const &p, std::nullptr_t) LIBCOPP_MACRO_NOEXCEPT {
            return p.get() != nullptr;
        }

        template <typename T>
        inline bool operator!=(std::nullptr_t, intrusive_ptr<T> const &p) LIBCOPP_MACRO_NOEXCEPT {
            return p.get() != nullptr;
        }

        template <typename T>
        inline bool operator<(intrusive_ptr<T> const &p, std::nullptr_t) LIBCOPP_MACRO_NOEXCEPT {
            return p.get() < nullptr;
        }

        template <typename T>
        inline bool operator<(std::nullptr_t, intrusive_ptr<T> const &p) LIBCOPP_MACRO_NOEXCEPT {
            return p.get() < nullptr;
        }

        template <typename T>
        inline bool operator<=(intrusive_ptr<T> const &p, std::nullptr_t) LIBCOPP_MACRO_NOEXCEPT {
            return p.get() <= nullptr;
        }

        template <typename T>
        inline bool operator<=(std::nullptr_t, intrusive_ptr<T> const &p) LIBCOPP_MACRO_NOEXCEPT {
            return p.get() <= nullptr;
        }

        template <typename T>
        inline bool operator>(intrusive_ptr<T> const &p, std::nullptr_t) LIBCOPP_MACRO_NOEXCEPT {
            return p.get() > nullptr;
        }

        template <typename T>
        inline bool operator>(std::nullptr_t, intrusive_ptr<T> const &p) LIBCOPP_MACRO_NOEXCEPT {
            return p.get() > nullptr;
        }

        template <typename T>
        inline bool operator>=(intrusive_ptr<T> const &p, std::nullptr_t) LIBCOPP_MACRO_NOEXCEPT {
            return p.get() >= nullptr;
        }

        template <typename T>
        inline bool operator>=(std::nullptr_t, intrusive_ptr<T> const &p) LIBCOPP_MACRO_NOEXCEPT {
            return p.get() >= nullptr;
        }
    #endif

    #endif

        template <typename T>
        void swap(intrusive_ptr<T> &lhs, intrusive_ptr<T> &rhs) {
            lhs.swap(rhs);
        }

        // mem_fn support

        template <typename T>
        T *get_pointer(intrusive_ptr<T> const &p) {
            return p.get();
        }

        template <typename T, typename U>
        intrusive_ptr<T> static_pointer_cast(intrusive_ptr<U> const &p) {
            return static_cast<T *>(p.get());
        }

        template <typename T, typename U>
        intrusive_ptr<T> const_pointer_cast(intrusive_ptr<U> const &p) {
            return const_cast<T *>(p.get());
        }

    #if defined(LIBATFRAME_UTILS_ENABLE_RTTI) && LIBATFRAME_UTILS_ENABLE_RTTI
        template <typename T, typename U>
        intrusive_ptr<T> dynamic_pointer_cast(intrusive_ptr<U> const &p) {
            return dynamic_cast<T *>(p.get());
        }
    #endif

        // operator<<
        template <typename E, typename T, typename Y>
        std::basic_ostream<E, T> &operator<<(std::basic_ostream<E, T> &os, intrusive_ptr<Y> const &p) {
            os << p.get();
            return os;
        }
    } // namespace util
} // namespace libcopp


#if defined(LOCK_DISABLE_MT) && LOCK_DISABLE_MT
#define LIBCOPP_UTIL_INTRUSIVE_PTR_ATOMIC_TYPE ::libcopp::util::lock::atomic_int_type<libcopp::util::lock::unsafe_int_type<size_t> >
#else
#define LIBCOPP_UTIL_INTRUSIVE_PTR_ATOMIC_TYPE ::libcopp::util::lock::atomic_int_type<size_t>
#endif

#define LIBCOPP_UTIL_INTRUSIVE_PTR_REF_MEMBER_DECL(T)                   \
                                                                        \
private:                                                                \
    LIBCOPP_UTIL_INTRUSIVE_PTR_ATOMIC_TYPE intrusive_ref_counter_;      \
    friend void                            intrusive_ptr_add_ref(T *p); \
    friend void                            intrusive_ptr_release(T *p); \
                                                                        \
public:                                                                 \
    const size_t use_count() const { return intrusive_ref_counter_.load(); }

#define LIBCOPP_UTIL_INTRUSIVE_PTR_REF_FN_DECL(T) \
    void intrusive_ptr_add_ref(T *p);             \
    void intrusive_ptr_release(T *p);

#define LIBCOPP_UTIL_INTRUSIVE_PTR_REF_MEMBER_INIT() this->intrusive_ref_counter_.store(0)

#define LIBCOPP_UTIL_INTRUSIVE_PTR_REF_FN_DEFI(T)     \
    void intrusive_ptr_add_ref(T *p) {                \
        if (nullptr != p) {                           \
            ++p->intrusive_ref_counter_;              \
        }                                             \
    }                                                 \
    void intrusive_ptr_release(T *p) {                \
        if (nullptr == p) {                           \
            return;                                   \
        }                                             \
        assert(p->intrusive_ref_counter_.load() > 0); \
        size_t ref = --p->intrusive_ref_counter_;     \
        if (0 == ref) {                               \
            delete p;                                 \
        }                                             \
    }


#endif

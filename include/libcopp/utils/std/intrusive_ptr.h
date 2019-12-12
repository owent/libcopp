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

#ifndef STD_INTRUSIVE_PTR_H
#define STD_INTRUSIVE_PTR_H

#pragma once

#include <assert.h>
#include <cstddef>
#include <ostream>

#include <libcopp/utils/config/compiler_features.h>
#include <libcopp/utils/config/libcopp_build_features.h>

namespace std {
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

        UTIL_CONFIG_CONSTEXPR intrusive_ptr() UTIL_CONFIG_NOEXCEPT : px(NULL) {}

        intrusive_ptr(T *p, bool add_ref = true) : px(p) {
            if (px != NULL && add_ref) {
                intrusive_ptr_add_ref(px);
            }
        }

        template <typename U>
        intrusive_ptr(intrusive_ptr<U> const &rhs) : px(rhs.get()) {
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

        intrusive_ptr(self_type &&rhs) UTIL_CONFIG_NOEXCEPT : px(rhs.px) { rhs.px = NULL; }

        self_type &operator=(self_type &&rhs) UTIL_CONFIG_NOEXCEPT {
            self_type(static_cast<self_type &&>(rhs)).swap(*this);
            return *this;
        }

        template <typename U>
        intrusive_ptr(intrusive_ptr<U> &&rhs) UTIL_CONFIG_NOEXCEPT : px(rhs.px) {
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

        inline void reset() UTIL_CONFIG_NOEXCEPT { self_type().swap(*this); }

        inline void reset(element_type *rhs) { self_type(rhs).swap(*this); }

        inline void reset(element_type *rhs, bool add_ref) { self_type(rhs, add_ref).swap(*this); }

        inline element_type *get() const UTIL_CONFIG_NOEXCEPT { return px; }

        inline element_type *detach() UTIL_CONFIG_NOEXCEPT {
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
        inline operator bool() const UTIL_CONFIG_NOEXCEPT { return px != NULL; }
        // operator! is redundant, but some compilers need it
        inline bool operator!() const UTIL_CONFIG_NOEXCEPT { return px == NULL; }

        inline void swap(intrusive_ptr &rhs) UTIL_CONFIG_NOEXCEPT {
            element_type *tmp = px;
            px                = rhs.px;
            rhs.px            = tmp;
        }

    private:
        element_type *px;
    };

    template <typename T, typename U>
    inline bool operator==(intrusive_ptr<T> const &a, intrusive_ptr<U> const &b) {
        return a.get() == b.get();
    }

    template <typename T, typename U>
    inline bool operator!=(intrusive_ptr<T> const &a, intrusive_ptr<U> const &b) {
        return a.get() != b.get();
    }

    template <typename T, typename U>
    inline bool operator==(intrusive_ptr<T> const &a, U *b) {
        return a.get() == b;
    }

    template <typename T, typename U>
    inline bool operator!=(intrusive_ptr<T> const &a, U *b) {
        return a.get() != b;
    }

    template <typename T, typename U>
    inline bool operator==(T *a, intrusive_ptr<U> const &b) {
        return a == b.get();
    }

    template <typename T, typename U>
    inline bool operator!=(T *a, intrusive_ptr<U> const &b) {
        return a != b.get();
    }

#if defined(UTIL_CONFIG_COMPILER_CXX_NULLPTR) && UTIL_CONFIG_COMPILER_CXX_NULLPTR

    template <typename T>
    inline bool operator==(intrusive_ptr<T> const &p, std::nullptr_t) UTIL_CONFIG_NOEXCEPT {
        return p.get() == nullptr;
    }

    template <typename T>
    inline bool operator==(std::nullptr_t, intrusive_ptr<T> const &p) UTIL_CONFIG_NOEXCEPT {
        return p.get() == nullptr;
    }

    template <typename T>
    inline bool operator!=(intrusive_ptr<T> const &p, std::nullptr_t) UTIL_CONFIG_NOEXCEPT {
        return p.get() != nullptr;
    }

    template <typename T>
    inline bool operator!=(std::nullptr_t, intrusive_ptr<T> const &p) UTIL_CONFIG_NOEXCEPT {
        return p.get() != nullptr;
    }

#endif

    template <typename T>
    inline bool operator<(intrusive_ptr<T> const &a, intrusive_ptr<T> const &b) {
        return std::less<T *>()(a.get(), b.get());
    }

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

    template <typename T, typename U>
    intrusive_ptr<T> dynamic_pointer_cast(intrusive_ptr<U> const &p) {
        return dynamic_cast<T *>(p.get());
    }

    // operator<<
    template <typename E, typename T, typename Y>
    std::basic_ostream<E, T> &operator<<(std::basic_ostream<E, T> &os, intrusive_ptr<Y> const &p) {
        os << p.get();
        return os;
    }
} // namespace std


#if defined(LOCK_DISABLE_MT) && LOCK_DISABLE_MT
#define UTIL_INTRUSIVE_PTR_ATOMIC_TYPE ::util::lock::atomic_int_type<util::lock::unsafe_int_type<size_t> >
#else
#define UTIL_INTRUSIVE_PTR_ATOMIC_TYPE ::util::lock::atomic_int_type<size_t>
#endif

#define UTIL_INTRUSIVE_PTR_REF_MEMBER_DECL(T)                   \
                                                                \
private:                                                        \
    UTIL_INTRUSIVE_PTR_ATOMIC_TYPE intrusive_ref_counter_;      \
    friend void                    intrusive_ptr_add_ref(T *p); \
    friend void                    intrusive_ptr_release(T *p); \
                                                                \
public:                                                         \
    const size_t use_count() const { return intrusive_ref_counter_.load(); }

#define UTIL_INTRUSIVE_PTR_REF_FN_DECL(T) \
    void intrusive_ptr_add_ref(T *p);     \
    void intrusive_ptr_release(T *p);

#define UTIL_INTRUSIVE_PTR_REF_MEMBER_INIT() this->intrusive_ref_counter_.store(0)

#define UTIL_INTRUSIVE_PTR_REF_FN_DEFI(T)             \
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

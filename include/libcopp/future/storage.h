#ifndef COPP_FUTURE_STORAGE_H
#define COPP_FUTURE_STORAGE_H

#pragma once

#include <cstring>

#include <libcopp/utils/config/compiler_features.h>

#include <libcopp/utils/std/coroutine.h>
#include <libcopp/utils/std/explicit_declare.h>
#include <libcopp/utils/std/functional.h>
#include <libcopp/utils/std/smart_ptr.h>
#include <libcopp/utils/std/type_traits.h>

#include <libcopp/utils/features.h>


namespace copp {
    namespace future {
        // FUNCTION TEMPLATE make_unique
        template <class T, class... TARGS, typename std::enable_if<!std::is_array<T>::value, int>::type = 0>
        EXPLICIT_NODISCARD_ATTR std::unique_ptr<T> make_unique(TARGS &&... args) { // make a unique_ptr
            return std::unique_ptr<T>(new T(std::forward<TARGS>(args)...));
        }

        template <class T, typename std::enable_if<std::is_array<T>::value && std::extent<T>::value == 0, int>::type = 0>
        EXPLICIT_NODISCARD_ATTR std::unique_ptr<T> make_unique(size_t sz) { // make a unique_ptr
#if defined(UTIL_CONFIG_COMPILER_CXX_ALIAS_TEMPLATES) && UTIL_CONFIG_COMPILER_CXX_ALIAS_TEMPLATES
            using TELEM = typename std::remove_extent<T>::type;
#else
            typedef typename std::remove_extent<T>::type TELEM;
#endif
            return std::unique_ptr<T>(new TELEM[sz]());
        }

        template <class T, class... TARGS, typename std::enable_if<std::extent<T>::value != 0, int>::type = 0>
        void make_unique(TARGS &&...) UTIL_CONFIG_DELETED_FUNCTION;

        template <class T>
        struct LIBCOPP_COPP_API_HEAD_ONLY small_object_optimize_storage_delete_t {
            inline void operator()(T *) const LIBCOPP_MACRO_NOEXCEPT {
                // Do nothing
            }
            template <class U>
            inline void operator()(U *) const LIBCOPP_MACRO_NOEXCEPT {
                // Do nothing
            }
        };

        template <class T>
        struct LIBCOPP_COPP_API_HEAD_ONLY poll_storage_select_ptr_t;

        template <>
        struct LIBCOPP_COPP_API_HEAD_ONLY poll_storage_select_ptr_t<void> {
            typedef std::unique_ptr<void, small_object_optimize_storage_delete_t<void> > type;
        };

        template <class T>
        struct LIBCOPP_COPP_API_HEAD_ONLY poll_storage_select_ptr_t {
            typedef typename std::conditional<COPP_IS_TIRVIALLY_COPYABLE_V(T) && sizeof(T) < (sizeof(size_t) << 2),
                                              std::unique_ptr<T, small_object_optimize_storage_delete_t<T> >,
                                              std::unique_ptr<T, std::default_delete<T> > >::type type;
        };

        template <class T>
        struct LIBCOPP_COPP_API_HEAD_ONLY compact_storage_select_t;

        template <>
        struct LIBCOPP_COPP_API_HEAD_ONLY compact_storage_select_t<void> {
            typedef std::unique_ptr<void, small_object_optimize_storage_delete_t<void> > type;
        };


        template <class T>
        struct LIBCOPP_COPP_API_HEAD_ONLY compact_storage_select_t {
            typedef
                typename std::conditional<COPP_IS_TIRVIALLY_COPYABLE_V(T) && sizeof(T) <= (sizeof(size_t) << 2),
                                          std::unique_ptr<T, small_object_optimize_storage_delete_t<T> >, std::shared_ptr<T> >::type type;
        };

        template <class T, class TPTR>
        struct LIBCOPP_COPP_API_HEAD_ONLY poll_storage_base_t;

        template <>
        struct LIBCOPP_COPP_API_HEAD_ONLY poll_storage_base_t<void, std::unique_ptr<void, small_object_optimize_storage_delete_t<void> > >
            : public std::true_type {
            typedef void                                                                 value_type;
            typedef std::unique_ptr<void, small_object_optimize_storage_delete_t<void> > ptr_type;
            typedef ptr_type                                                             storage_type;

            static UTIL_FORCEINLINE void construct_default_storage(storage_type &out) LIBCOPP_MACRO_NOEXCEPT { out.reset(); }

            template <class U, class UDELETOR,
                      typename std::enable_if<std::is_convertible<typename std::decay<U>::type, bool>::value, bool>::type = false>
            static UTIL_FORCEINLINE void construct_storage(storage_type &out, std::unique_ptr<U, UDELETOR> &&in) LIBCOPP_MACRO_NOEXCEPT {
                if (in) {
                    out.reset(reinterpret_cast<void *>(&out));
                } else {
                    out.reset();
                }
            }

            template <class U, typename std::enable_if<std::is_convertible<typename std::decay<U>::type, bool>::value, bool>::type = false>
            static UTIL_FORCEINLINE void construct_storage(storage_type &out, U &&in) LIBCOPP_MACRO_NOEXCEPT {
                if (in) {
                    out.reset(reinterpret_cast<void *>(&out));
                } else {
                    out.reset();
                }
            }

            static UTIL_FORCEINLINE void move_storage(storage_type &out, storage_type &&in) LIBCOPP_MACRO_NOEXCEPT {
                if (in) {
                    out.reset(reinterpret_cast<void *>(&out));
                } else {
                    out.reset();
                }

                in.reset();
            }

            static UTIL_FORCEINLINE void reset(storage_type &storage) LIBCOPP_MACRO_NOEXCEPT { storage.reset(); }
            static UTIL_FORCEINLINE void swap(storage_type &l, storage_type &r) LIBCOPP_MACRO_NOEXCEPT {
                if (!!l == !!r) {
                    return;
                }

                if (l) {
                    l.reset();
                    r.reset(reinterpret_cast<void *>(&r));
                } else {
                    l.reset(reinterpret_cast<void *>(&l));
                    r.reset();
                }
            }

            static UTIL_FORCEINLINE const ptr_type &unwrap(const storage_type &storage) LIBCOPP_MACRO_NOEXCEPT { return storage; }
            static UTIL_FORCEINLINE ptr_type &      unwrap(storage_type &storage) LIBCOPP_MACRO_NOEXCEPT { return storage; }
        };

        template <class T>
        struct LIBCOPP_COPP_API_HEAD_ONLY poll_storage_base_t<T, std::unique_ptr<T, small_object_optimize_storage_delete_t<T> > >
            : public std::true_type {
            typedef T                                                              value_type;
            typedef std::unique_ptr<T, small_object_optimize_storage_delete_t<T> > ptr_type;
            typedef std::pair<T, ptr_type>                                         storage_type;

            static UTIL_FORCEINLINE void construct_default_storage(storage_type &out) LIBCOPP_MACRO_NOEXCEPT {
                memset(&out.first, 0, sizeof(out.first));
                out.second.reset();
            }

            template <class U, class UDELETOR,
                      typename std::enable_if<std::is_base_of<T, typename std::decay<U>::type>::value ||
                                                  std::is_convertible<typename std::decay<U>::type, T>::value,
                                              bool>::type = false>
            static UTIL_FORCEINLINE void construct_storage(storage_type &out, std::unique_ptr<U, UDELETOR> &&in) LIBCOPP_MACRO_NOEXCEPT {
                if (in) {
                    out.first = *in;
                    out.second.reset(&out.first);
                    in.reset();
                } else {
                    memset(&out.first, 0, sizeof(out.first));
                    out.second.reset();
                }
            }

            template <class U, typename std::enable_if<std::is_base_of<T, typename std::decay<U>::type>::value ||
                                                           std::is_convertible<typename std::decay<U>::type, T>::value,
                                                       bool>::type = false>
            static UTIL_FORCEINLINE void construct_storage(storage_type &out, U &&in) LIBCOPP_MACRO_NOEXCEPT {
                out.first = in;
                out.second.reset(&out.first);
            }

            static UTIL_FORCEINLINE void move_storage(storage_type &out, storage_type &&in) LIBCOPP_MACRO_NOEXCEPT {
                if (in.second) {
                    out.first = in.first;
                    out.second.reset(&out.first);
                    in.second.reset();
                } else {
                    memset(&out.first, 0, sizeof(out.first));
                    out.second.reset();
                }
            }

            static UTIL_FORCEINLINE void reset(storage_type &storage) LIBCOPP_MACRO_NOEXCEPT { storage.second.reset(); }
            static UTIL_FORCEINLINE void swap(storage_type &l, storage_type &r) LIBCOPP_MACRO_NOEXCEPT {
                value_type lv = l.first;
                l.first = r.first;
                r.first = lv;
                if (!!l.second == !!r.second) {
                    return;
                }

                if (l.second) {
                    l.second.reset();
                    r.second.reset(&r.first);
                } else {
                    l.second.reset(&l.first);
                    r.second.reset();
                }
            }

            static UTIL_FORCEINLINE const ptr_type &unwrap(const storage_type &storage) LIBCOPP_MACRO_NOEXCEPT { return storage.second; }
            static UTIL_FORCEINLINE ptr_type &      unwrap(storage_type &storage) LIBCOPP_MACRO_NOEXCEPT { return storage.second; }
        };

        template <class T, class TPTR>
        struct LIBCOPP_COPP_API_HEAD_ONLY poll_storage_base_t : public std::false_type {
            typedef T        value_type;
            typedef TPTR     ptr_type;
            typedef ptr_type storage_type;

            static UTIL_FORCEINLINE void construct_default_storage(storage_type &out) LIBCOPP_MACRO_NOEXCEPT { out.reset(); }

            template <class U, class UDELETOR,
                      typename std::enable_if<std::is_base_of<T, typename std::decay<U>::type>::value, bool>::type = false>
            static UTIL_FORCEINLINE void construct_storage(storage_type &out, std::unique_ptr<U, UDELETOR> &&in) LIBCOPP_MACRO_NOEXCEPT {
                out = std::move(in);
            }

            template <class U, typename std::enable_if<std::is_base_of<T, typename std::decay<U>::type>::value &&
                                                           type_traits::is_shared_ptr<ptr_type>::value,
                                                       bool>::type = false>
            static UTIL_FORCEINLINE void construct_storage(storage_type &out, std::shared_ptr<U> &&in) LIBCOPP_MACRO_NOEXCEPT {
                out = std::move(std::static_pointer_cast<typename ptr_type::element_type>(in));
            }

            static UTIL_FORCEINLINE void move_storage(storage_type &out, storage_type &&in) LIBCOPP_MACRO_NOEXCEPT { out = std::move(in); }

            static UTIL_FORCEINLINE void reset(storage_type &storage) LIBCOPP_MACRO_NOEXCEPT { storage.reset(); }
            static UTIL_FORCEINLINE void swap(storage_type &l, storage_type &r) LIBCOPP_MACRO_NOEXCEPT { std::swap(l, r); }

            static UTIL_FORCEINLINE const ptr_type &unwrap(const storage_type &storage) LIBCOPP_MACRO_NOEXCEPT { return storage; }
            static UTIL_FORCEINLINE ptr_type &      unwrap(storage_type &storage) LIBCOPP_MACRO_NOEXCEPT { return storage; }
        };

        template <class T, class TPTR>
        struct LIBCOPP_COPP_API_HEAD_ONLY compact_storage_t;

        template <class T>
        struct LIBCOPP_COPP_API_HEAD_ONLY compact_storage_t<T, std::unique_ptr<T, small_object_optimize_storage_delete_t<T> > >
            : public std::true_type {
            typedef T                                                              value_type;
            typedef std::unique_ptr<T, small_object_optimize_storage_delete_t<T> > ptr_type;
            typedef T                                                              storage_type;

            static UTIL_FORCEINLINE bool is_shared_storage() LIBCOPP_MACRO_NOEXCEPT { return false; }
            static UTIL_FORCEINLINE void destroy_storage(storage_type &) {
                // do nothing for trival copyable object
            }
            static UTIL_FORCEINLINE void construct_default_storage(storage_type &out) LIBCOPP_MACRO_NOEXCEPT { memset(&out, 0, sizeof(out)); }

            static UTIL_FORCEINLINE void construct_storage(storage_type &out) LIBCOPP_MACRO_NOEXCEPT { construct_default_storage(out); }

            template <class U, class UDELETOR,
                      typename std::enable_if<std::is_base_of<T, typename std::decay<U>::type>::value ||
                                                  std::is_convertible<typename std::decay<U>::type, T>::value,
                                              bool>::type = false>
            static UTIL_FORCEINLINE void construct_storage(storage_type &out, std::unique_ptr<U, UDELETOR> &&in) LIBCOPP_MACRO_NOEXCEPT {
                if (in) {
                    out = *in;
                } else {
                    memset(&out, 0, sizeof(out));
                }
            }

            template <class U, typename std::enable_if<std::is_base_of<T, typename std::decay<U>::type>::value ||
                                                           std::is_convertible<typename std::decay<U>::type, T>::value,
                                                       bool>::type = false>
            static UTIL_FORCEINLINE void construct_storage(storage_type &out, U &&in) LIBCOPP_MACRO_NOEXCEPT {
                out = in;
            }

            static UTIL_FORCEINLINE void clone_storage(storage_type &out, const storage_type &in) LIBCOPP_MACRO_NOEXCEPT {
                memcpy(&out, &in, sizeof(out));
            }

            static UTIL_FORCEINLINE void move_storage(storage_type &out, storage_type &&in) LIBCOPP_MACRO_NOEXCEPT {
                memcpy(&out, &in, sizeof(out));
                memset(&in, 0, sizeof(in));
            }

            static UTIL_FORCEINLINE void swap(storage_type &l, storage_type &r) LIBCOPP_MACRO_NOEXCEPT {
                storage_type lv = l;
                l = r;
                r = lv;
            }

            static UTIL_FORCEINLINE value_type *      unwrap(storage_type &storage) LIBCOPP_MACRO_NOEXCEPT { return &storage; }
            static UTIL_FORCEINLINE const value_type *unwrap(const storage_type &storage) LIBCOPP_MACRO_NOEXCEPT { return &storage; }
            static UTIL_FORCEINLINE ptr_type          clone_ptr(storage_type &storage) LIBCOPP_MACRO_NOEXCEPT { return ptr_type(&storage); }
        };

        template <class T>
        struct LIBCOPP_COPP_API_HEAD_ONLY compact_storage_t<T, std::shared_ptr<T> > : public std::false_type {
            typedef T                  value_type;
            typedef std::shared_ptr<T> ptr_type;
            typedef ptr_type           storage_type;

            static UTIL_FORCEINLINE bool is_shared_storage() LIBCOPP_MACRO_NOEXCEPT { return true; }
            static UTIL_FORCEINLINE void destroy_storage(storage_type &out) { out.reset(); }
            static UTIL_FORCEINLINE void construct_default_storage(storage_type &out) { out.reset(); }

            template <class U, class UDELETOR,
                      typename std::enable_if<std::is_base_of<T, typename std::decay<U>::type>::value, bool>::type = false>
            static UTIL_FORCEINLINE void construct_storage(storage_type &out, std::unique_ptr<U, UDELETOR> &&in) {
                if (in) {
                    out = std::move(in);
                } else {
                    out.reset();
                }
            }

            template <class U, typename std::enable_if<std::is_base_of<T, typename std::decay<U>::type>::value, bool>::type = false>
            static UTIL_FORCEINLINE void construct_storage(storage_type &out, std::shared_ptr<U> &&in) {
                if (in) {
                    out = std::static_pointer_cast<T>(in);
                } else {
                    out.reset();
                }
            }

            template <class... TARGS>
            static UTIL_FORCEINLINE void construct_storage(storage_type &out, TARGS &&... in) {
                out = std::make_shared<T>(std::forward<TARGS>(in)...);
            }

            static UTIL_FORCEINLINE void clone_storage(storage_type &out, const storage_type &in) { out = in; }
            static UTIL_FORCEINLINE void move_storage(storage_type &out, storage_type &&in) LIBCOPP_MACRO_NOEXCEPT {
                out.swap(in);
                in.reset();
            }

            static UTIL_FORCEINLINE void swap(storage_type &l, storage_type &r) LIBCOPP_MACRO_NOEXCEPT { l.swap(r); }

            static UTIL_FORCEINLINE value_type *      unwrap(storage_type &storage) LIBCOPP_MACRO_NOEXCEPT { return storage.get(); }
            static UTIL_FORCEINLINE const value_type *unwrap(const storage_type &storage) LIBCOPP_MACRO_NOEXCEPT { return storage.get(); }
            static UTIL_FORCEINLINE ptr_type          clone_ptr(storage_type &storage) LIBCOPP_MACRO_NOEXCEPT { return storage; }
        };

        template <class T>
        struct default_compact_storage_t : public compact_storage_t<T, typename compact_storage_select_t<T>::type> {
            typedef compact_storage_t<T, typename compact_storage_select_t<T>::type> type;
        };

        template <class TOK, class TERR, bool is_all_trivial>
        class LIBCOPP_COPP_API_HEAD_ONLY result_base_t;

        template <class TOK, class TERR>
        class LIBCOPP_COPP_API_HEAD_ONLY result_base_t<TOK, TERR, true> {
        public:
            typedef TOK  success_type;
            typedef TERR error_type;
            enum mode_t {
                EN_RESULT_SUCCESS = 0,
                EN_RESULT_ERROR   = 1,
            };

            UTIL_FORCEINLINE bool is_success() const LIBCOPP_MACRO_NOEXCEPT { return mode_ == EN_RESULT_SUCCESS; }
            UTIL_FORCEINLINE bool is_error() const LIBCOPP_MACRO_NOEXCEPT { return mode_ == EN_RESULT_ERROR; }

            UTIL_FORCEINLINE const success_type *get_success() const LIBCOPP_MACRO_NOEXCEPT { return is_success() ? &success_data_ : NULL; }
            UTIL_FORCEINLINE success_type *      get_success() LIBCOPP_MACRO_NOEXCEPT { return is_success() ? &success_data_ : NULL; }
            UTIL_FORCEINLINE const error_type *  get_error() const LIBCOPP_MACRO_NOEXCEPT { return is_error() ? &error_data_ : NULL; }
            UTIL_FORCEINLINE error_type *        get_error() LIBCOPP_MACRO_NOEXCEPT { return is_error() ? &error_data_ : NULL; }

        private:
            template <class UOK, class UERR>
            friend class result_t;
            template <class TRESULT, bool>
            friend struct _make_result_instance_helper;

            template <class TARGS>
            inline void construct_success(TARGS &&args) LIBCOPP_MACRO_NOEXCEPT {
                make_success_base(std::forward<TARGS>(args));
            }

            template <class TARGS>
            inline void construct_error(TARGS &&args) LIBCOPP_MACRO_NOEXCEPT {
                make_error_base(std::forward<TARGS>(args));
            }

            template <class TARGS>
            inline void make_success_base(TARGS &&args) LIBCOPP_MACRO_NOEXCEPT {
                success_data_ = args;
                mode_         = EN_RESULT_SUCCESS;
            }

            template <class TARGS>
            inline void make_error_base(TARGS &&args) LIBCOPP_MACRO_NOEXCEPT {
                error_data_ = args;
                mode_       = EN_RESULT_ERROR;
            }

            inline void swap(result_base_t& other) LIBCOPP_MACRO_NOEXCEPT {
                using std::swap;
                if (is_success()) {
                    swap(success_data_, other.success_data_);
                } else {
                    swap(error_data_, other.error_data_);
                }
                swap(mode_, other.mode_);
            }

            friend UTIL_FORCEINLINE void swap(result_base_t& l, result_base_t& r) LIBCOPP_MACRO_NOEXCEPT { l.swap(r); }

        private:
            union {
                success_type success_data_;
                error_type   error_data_;
            };
            mode_t mode_;
        };

        template <class TOK, class TERR>
        class LIBCOPP_COPP_API_HEAD_ONLY result_base_t<TOK, TERR, false> {
        public:
            typedef TOK  success_type;
            typedef TERR error_type;
            enum mode_t {
                EN_RESULT_SUCCESS = 0,
                EN_RESULT_ERROR   = 1,
                EN_RESULT_NONE    = 2,
            };

            UTIL_FORCEINLINE bool is_success() const LIBCOPP_MACRO_NOEXCEPT { return mode_ == EN_RESULT_SUCCESS; }
            UTIL_FORCEINLINE bool is_error() const LIBCOPP_MACRO_NOEXCEPT { return mode_ == EN_RESULT_ERROR; }

            UTIL_FORCEINLINE const success_type *get_success() const LIBCOPP_MACRO_NOEXCEPT {
                return is_success() ? success_storage_type::unwrap(success_data_) : NULL;
            }
            UTIL_FORCEINLINE success_type *get_success() LIBCOPP_MACRO_NOEXCEPT {
                return is_success() ? success_storage_type::unwrap(success_data_) : NULL;
            }
            UTIL_FORCEINLINE const error_type *get_error() const LIBCOPP_MACRO_NOEXCEPT {
                return is_error() ? error_storage_type::unwrap(error_data_) : NULL;
            }
            UTIL_FORCEINLINE error_type *get_error() LIBCOPP_MACRO_NOEXCEPT { return is_error() ? error_storage_type::unwrap(error_data_) : NULL; }

            result_base_t() : mode_(EN_RESULT_NONE) {
                success_storage_type::construct_default_storage(success_data_);
                error_storage_type::construct_default_storage(error_data_);
            }
            ~result_base_t() { reset(); }

            result_base_t(result_base_t&& other) : mode_(EN_RESULT_NONE) {
                success_storage_type::construct_default_storage(success_data_);
                error_storage_type::construct_default_storage(error_data_);

                swap(other);
            }

            result_base_t& operator=(result_base_t&& other) {
                swap(other);
                other.reset();
                return *this;
            }

            inline void swap(result_base_t& other) LIBCOPP_MACRO_NOEXCEPT {
                using std::swap;
                success_storage_type::swap(success_data_, other.success_data_);
                error_storage_type::swap(error_data_, other.error_data_);
                swap(mode_, other.mode_);
            }

            friend UTIL_FORCEINLINE void swap(result_base_t& l, result_base_t& r) LIBCOPP_MACRO_NOEXCEPT { l.swap(r); }

        private:
            template <class UOK, class UERR>
            friend class result_t;
            template <class TRESULT, bool>
            friend struct _make_result_instance_helper;

            template <class... TARGS>
            inline void construct_success(TARGS &&... args) {
                reset();
                success_storage_type::construct_storage(success_data_, std::forward<TARGS>(args)...);
                mode_ = EN_RESULT_SUCCESS;
            }

            template <class... TARGS>
            inline void construct_error(TARGS &&... args) {
                reset();
                error_storage_type::construct_storage(error_data_, std::forward<TARGS>(args)...);
                mode_ = EN_RESULT_ERROR;
            }

            template <class... TARGS>
            inline void make_success_base(TARGS &&... args) {
                reset();
                make_object<success_storage_type>(success_data_, std::forward<TARGS>(args)...);
                mode_ = EN_RESULT_SUCCESS;
            }

            template <class... TARGS>
            inline void make_error_base(TARGS &&... args) {
                reset();
                make_object<error_storage_type>(error_data_, std::forward<TARGS>(args)...);
                mode_ = EN_RESULT_ERROR;
            }

            void reset() {
                if (EN_RESULT_SUCCESS == mode_) {
                    success_storage_type::destroy_storage(success_data_);
                } else if (EN_RESULT_ERROR == mode_) {
                    error_storage_type::destroy_storage(error_data_);
                }

                mode_ = EN_RESULT_NONE;
            }

        private:
            template <class TSTORAGE, class... TARGS>
            static inline void make_object(typename TSTORAGE::storage_type &out, TARGS &&... args) {
                TSTORAGE::construct_storage(out, std::forward<TARGS>(args)...);
            }

            template <class TSTORAGE, class... TARGS>
            static inline void make_object(std::shared_ptr<typename TSTORAGE::storage_type> &out, TARGS &&... args) {
                TSTORAGE::construct_storage(out, std::make_shared<typename TSTORAGE::storage_type>(std::forward<TARGS>(args)...));
            }

            typedef typename default_compact_storage_t<success_type>::type success_storage_type;
            typedef typename default_compact_storage_t<error_type>::type error_storage_type;

            typename success_storage_type::storage_type success_data_;
            typename error_storage_type::storage_type   error_data_;
            mode_t                                      mode_;
        };

        template <class TRESULT, bool>
        struct LIBCOPP_COPP_API_HEAD_ONLY _make_result_instance_helper;

        template <class TRESULT>
        struct LIBCOPP_COPP_API_HEAD_ONLY _make_result_instance_helper<TRESULT, false> {
            typedef std::unique_ptr<TRESULT> type;

            template <class... TARGS>
            static inline type make_success(TARGS &&... args) {
                type ret = copp::future::make_unique<TRESULT>();
                if (ret) {
                    ret->make_success_base(std::forward<TARGS>(args)...);
                }

                return ret;
            }

            template <class... TARGS>
            static inline type make_error(TARGS &&... args) {
                type ret = copp::future::make_unique<TRESULT>();
                if (ret) {
                    ret->make_error_base(std::forward<TARGS>(args)...);
                }

                return ret;
            }
        };

        template <class TRESULT>
        struct LIBCOPP_COPP_API_HEAD_ONLY _make_result_instance_helper<TRESULT, true> {
            typedef TRESULT type;

            template <class... TARGS>
            static inline type make_success(TARGS &&... args) {
                TRESULT ret;
                ret.make_success_base(std::forward<TARGS>(args)...);
                return ret;
            }

            template <class... TARGS>
            static inline type make_error(TARGS &&... args) {
                TRESULT ret;
                ret.make_error_base(std::forward<TARGS>(args)...);
                return ret;
            }
        };

        template <class TOK, class TERR>
        class LIBCOPP_COPP_API_HEAD_ONLY result_t
            : public result_base_t<TOK, TERR, default_compact_storage_t<TOK>::value && default_compact_storage_t<TERR>::value> {
        public:
            typedef result_base_t<TOK, TERR, default_compact_storage_t<TOK>::value && default_compact_storage_t<TERR>::value> base_type;
            typedef result_t<TOK, TERR>                                                                                       self_type;

        private:
            typedef _make_result_instance_helper<self_type,
                                                 poll_storage_base_t<base_type, typename poll_storage_select_ptr_t<base_type>::type>::value>
                _make_instance_type;
        public: 
            typedef typename _make_instance_type::type storage_type;

            template <class... TARGS>
            static inline self_type create_success(TARGS &&... args) {
                self_type ret;
                ret.construct_success(std::forward<TARGS>(args)...);
                return ret;
            }

            template <class... TARGS>
            static inline self_type create_error(TARGS &&... args) {
                self_type ret;
                ret.construct_error(std::forward<TARGS>(args)...);
                return ret;
            }

        public:
            template <class... TARGS>
            static inline storage_type make_success(TARGS &&... args) {
                return _make_instance_type::make_success(std::forward<TARGS>(args)...);
            }

            template <class... TARGS>
            static inline storage_type make_error(TARGS &&... args) {
                return _make_instance_type::make_error(std::forward<TARGS>(args)...);
            }
        };
    } // namespace future
} // namespace copp

#endif

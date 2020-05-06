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
            typename typename std::remove_extent<T>::type TELEM;
#endif
            return std::unique_ptr<T>(new TELEM[sz]());
        }

        template <class T, class... TARGS, typename std::enable_if<std::extent<T>::value != 0, int>::type = 0>
        void make_unique(TARGS &&...) UTIL_CONFIG_DELETED_FUNCTION;

        template <class T>
        struct LIBCOPP_COPP_API_HEAD_ONLY small_object_optimize_storage_delete_t {
            inline void operator()(T *) const UTIL_CONFIG_NOEXCEPT {
                // Do nothing
            }
            template <class U>
            inline void operator()(U *) const UTIL_CONFIG_NOEXCEPT {
                // Do nothing
            }
        };

        template <class T>
        struct poll_storage_select_ptr_t {
            typedef typename std::conditional<std::is_trivial<T>::value && sizeof(T) < (sizeof(size_t) << 2),
                                              std::unique_ptr<T, small_object_optimize_storage_delete_t<T> >,
                                              std::unique_ptr<T, std::default_delete<T> > >::type type;
        };

        template <class T>
        struct context_storage_select_t {
            typedef
                typename std::conditional<std::is_trivial<T>::value && sizeof(T) <= (sizeof(size_t) << 2),
                                          std::unique_ptr<T, small_object_optimize_storage_delete_t<T> >, std::shared_ptr<T> >::type type;
        };

        template <class T, class TPTR>
        struct LIBCOPP_COPP_API_HEAD_ONLY poll_storage_base_t;

        template <class T>
        struct LIBCOPP_COPP_API_HEAD_ONLY poll_storage_base_t<T, std::unique_ptr<T, small_object_optimize_storage_delete_t<T> > > {
            typedef T                                                              value_type;
            typedef std::unique_ptr<T, small_object_optimize_storage_delete_t<T> > ptr_type;
            typedef std::pair<T, ptr_type>                                         storage_type;

            static inline void construct_default_storage(storage_type &out) UTIL_CONFIG_NOEXCEPT {
                memset(&out.first, 0, sizeof(out.first));
                out.second.reset();
            }

            template <class U, class UDELETOR,
                      typename std::enable_if<std::is_base_of<T, typename std::decay<U>::type>::value ||
                                                  std::is_convertible<typename std::decay<U>::type, T>::value,
                                              bool>::type = false>
            static inline void construct_storage(storage_type &out, std::unique_ptr<U, UDELETOR> &&in) UTIL_CONFIG_NOEXCEPT {
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
            static inline void construct_storage(storage_type &out, U &&in) UTIL_CONFIG_NOEXCEPT {
                out.first = in;
                out.second.reset(&out.first);
            }

            static inline void move_storage(storage_type &out, storage_type &&in) UTIL_CONFIG_NOEXCEPT {
                if (in.second) {
                    out.first = in.first;
                    out.second.reset(&out.first);
                    in.second.reset();
                } else {
                    memset(&out.first, 0, sizeof(out.first));
                    out.second.reset();
                }
            }

            static inline const ptr_type &unwrap(const storage_type &storage) UTIL_CONFIG_NOEXCEPT { return storage.second; }
            static inline ptr_type &      unwrap(storage_type &storage) UTIL_CONFIG_NOEXCEPT { return storage.second; }
        };

        template <class T, class TPTR>
        struct LIBCOPP_COPP_API_HEAD_ONLY poll_storage_base_t {
            typedef T        value_type;
            typedef TPTR     ptr_type;
            typedef ptr_type storage_type;

            static inline void construct_default_storage(storage_type &out) UTIL_CONFIG_NOEXCEPT { out.reset(); }

            template <class U, class UDELETOR,
                      typename std::enable_if<std::is_base_of<T, typename std::decay<U>::type>::value, bool>::type = false>
            static inline void construct_storage(storage_type &out, std::unique_ptr<U, UDELETOR> &&in) UTIL_CONFIG_NOEXCEPT {
                out = std::move(in);
            }

            template <class U, typename std::enable_if<std::is_base_of<T, typename std::decay<U>::type>::value &&
                                                           type_traits::is_shared_ptr<ptr_type>::value,
                                                       bool>::type = false>
            static inline void construct_storage(storage_type &out, std::shared_ptr<U> &&in) UTIL_CONFIG_NOEXCEPT {
                out = std::move(std::static_pointer_cast<typename ptr_type::element_type>(in));
            }

            static inline void move_storage(storage_type &out, storage_type &&in) UTIL_CONFIG_NOEXCEPT { out = std::move(in); }

            static inline const ptr_type &unwrap(const storage_type &storage) UTIL_CONFIG_NOEXCEPT { return storage; }
            static inline ptr_type &      unwrap(storage_type &storage) UTIL_CONFIG_NOEXCEPT { return storage; }
        };

        template <class T, class TPTR>
        struct LIBCOPP_COPP_API_HEAD_ONLY context_storage_base_t;

        template <class T>
        struct LIBCOPP_COPP_API_HEAD_ONLY context_storage_base_t<T, std::unique_ptr<T, small_object_optimize_storage_delete_t<T> > > {
            typedef T                                                              value_type;
            typedef std::unique_ptr<T, small_object_optimize_storage_delete_t<T> > ptr_type;
            typedef T                                                              storage_type;

            static inline bool is_shared_storage() UTIL_CONFIG_NOEXCEPT { return false; }
            static inline void destroy_storage(storage_type &) {
                // do nothing for trival copyable object
            }
            static inline void construct_default_storage(storage_type &out) UTIL_CONFIG_NOEXCEPT { memset(&out, 0, sizeof(out)); }

            static inline void construct_storage(storage_type &out) UTIL_CONFIG_NOEXCEPT { construct_default_storage(out); }

            template <class U, class UDELETOR,
                      typename std::enable_if<std::is_base_of<T, typename std::decay<U>::type>::value ||
                                                  std::is_convertible<typename std::decay<U>::type, T>::value,
                                              bool>::type = false>
            static inline void construct_storage(storage_type &out, std::unique_ptr<U, UDELETOR> &&in) UTIL_CONFIG_NOEXCEPT {
                if (in) {
                    out = *in;
                } else {
                    memset(&out, 0, sizeof(out));
                }
            }

            template <class U, typename std::enable_if<std::is_base_of<T, typename std::decay<U>::type>::value ||
                                                           std::is_convertible<typename std::decay<U>::type, T>::value,
                                                       bool>::type = false>
            static inline void construct_storage(storage_type &out, U &&in) UTIL_CONFIG_NOEXCEPT {
                out = in;
            }

            static inline void clone_storage(storage_type &out, const storage_type &in) UTIL_CONFIG_NOEXCEPT {
                memcpy(&out, &in, sizeof(out));
            }

            static inline void move_storage(storage_type &out, storage_type &&in) UTIL_CONFIG_NOEXCEPT {
                memcpy(&out, &in, sizeof(out));
                memset(&in, 0, sizeof(in));
            }

            static inline value_type *      unwrap(storage_type &storage) UTIL_CONFIG_NOEXCEPT { return &storage; }
            static inline const value_type *unwrap(const storage_type &storage) UTIL_CONFIG_NOEXCEPT { return &storage; }
            static inline ptr_type          clone_ptr(storage_type &storage) UTIL_CONFIG_NOEXCEPT { return ptr_type(&storage); }
        };

        template <class T>
        struct LIBCOPP_COPP_API_HEAD_ONLY context_storage_base_t<T, std::shared_ptr<T> > {
            typedef T                  value_type;
            typedef std::shared_ptr<T> ptr_type;
            typedef ptr_type           storage_type;

            static inline bool is_shared_storage() UTIL_CONFIG_NOEXCEPT { return true; }
            static inline void destroy_storage(storage_type &out) { out.reset(); }
            static inline void construct_default_storage(storage_type &out) { out.reset(); }

            template <class U, class UDELETOR,
                      typename std::enable_if<std::is_base_of<T, typename std::decay<U>::type>::value, bool>::type = false>
            static inline void construct_storage(storage_type &out, std::unique_ptr<U, UDELETOR> &&in) {
                if (in) {
                    out = std::move(in);
                } else {
                    out.reset();
                }
            }

            template <class U, typename std::enable_if<std::is_base_of<T, typename std::decay<U>::type>::value, bool>::type = false>
            static inline void construct_storage(storage_type &out, std::shared_ptr<U> &&in) {
                if (in) {
                    out = std::static_pointer_cast<T>(in);
                } else {
                    out.reset();
                }
            }

            template <class... TARGS>
            static inline void construct_storage(storage_type &out, TARGS &&... in) {
                out = std::make_shared<T>(std::forward<TARGS>(in)...);
            }

            static inline void clone_storage(storage_type &out, const storage_type &in) { out = in; }
            static inline void move_storage(storage_type &out, storage_type &&in) UTIL_CONFIG_NOEXCEPT {
                out.swap(in);
                in.reset();
            }

            static inline value_type *      unwrap(storage_type &storage) UTIL_CONFIG_NOEXCEPT { return storage.get(); }
            static inline const value_type *unwrap(const storage_type &storage) UTIL_CONFIG_NOEXCEPT { return storage.get(); }
            static inline ptr_type          clone_ptr(storage_type &storage) UTIL_CONFIG_NOEXCEPT { return storage; }
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

            inline bool is_success() const UTIL_CONFIG_NOEXCEPT { return mode_ == EN_RESULT_SUCCESS; }
            inline bool is_error() const UTIL_CONFIG_NOEXCEPT { return mode_ == EN_RESULT_ERROR; }

            inline const success_type *get_success() const UTIL_CONFIG_NOEXCEPT { return is_success() ? &success_data_ : NULL; }
            inline success_type *      get_success() UTIL_CONFIG_NOEXCEPT { return is_success() ? &success_data_ : NULL; }
            inline const error_type *  get_error() const UTIL_CONFIG_NOEXCEPT { return is_error() ? &error_data_ : NULL; }
            inline error_type *        get_error() UTIL_CONFIG_NOEXCEPT { return is_error() ? &error_data_ : NULL; }

        private:
            template <class UOK, class UERR>
            friend class result_t;

            template <class TARGS>
            inline void construct_success(TARGS &&args) UTIL_CONFIG_NOEXCEPT {
                success_data_ = args;
                mode_         = EN_RESULT_SUCCESS;
            }

            template <class TARGS>
            inline void construct_error(TARGS &&args) UTIL_CONFIG_NOEXCEPT {
                error_data_ = args;
                mode_       = EN_RESULT_ERROR;
            }

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
            };

            inline bool is_success() const UTIL_CONFIG_NOEXCEPT { return mode_ == EN_RESULT_SUCCESS; }
            inline bool is_error() const UTIL_CONFIG_NOEXCEPT { return mode_ == EN_RESULT_ERROR; }

            inline const success_type *get_success() const UTIL_CONFIG_NOEXCEPT {
                return is_success() ? success_storage_type::unwrap(success_data_) : NULL;
            }
            inline success_type *get_success() UTIL_CONFIG_NOEXCEPT {
                return is_success() ? success_storage_type::unwrap(success_data_) : NULL;
            }
            inline const error_type *get_error() const UTIL_CONFIG_NOEXCEPT {
                return is_error() ? error_storage_type::unwrap(error_data_) : NULL;
            }
            inline error_type *get_error() UTIL_CONFIG_NOEXCEPT { return is_error() ? error_storage_type::unwrap(error_data_) : NULL; }

            result_base_t() : mode_(EN_RESULT_INNER_NONE) {
                success_storage_type::construct_default_storage(success_data_);
                error_storage_type::construct_default_storage(error_data_);
            }
            ~result_base_t() { reset(); }

        private:
            template <class UOK, class UERR>
            friend class result_t;

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

            void reset() {
                if (EN_RESULT_SUCCESS == mode_) {
                    success_storage_type::destroy_storage(success_data_);
                } else if (EN_RESULT_ERROR == mode_) {
                    error_storage_type::destroy_storage(error_data_);
                }

                mode_ = EN_RESULT_INNER_NONE;
            }

        private:
            enum inner_mode_t {
                EN_RESULT_INNER_NONE = 2,
            };

            typedef context_storage_base_t<success_type, typename context_storage_select_t<success_type>::type> success_storage_type;
            typedef context_storage_base_t<error_type, typename context_storage_select_t<error_type>::type>     error_storage_type;

            typename success_storage_type::storage_type success_data_;
            typename error_storage_type::storage_type   error_data_;
            uint8_t                                     mode_;
        };

        template <class TOK, class TERR>
        class LIBCOPP_COPP_API_HEAD_ONLY result_t
            : public result_base_t<TOK, TERR, std::is_trivial<TOK>::value && std::is_trivial<TERR>::value> {
        public:
            typedef result_t<TOK, TERR> self_type;

            template <class... TARGS>
            static self_type create_success(TARGS &&... args) {
                self_type ret;
                ret.construct_success(std::forward<TARGS>(args)...);
                return ret;
            }

            template <class... TARGS>
            static self_type create_error(TARGS &&... args) {
                self_type ret;
                ret.construct_error(std::forward<TARGS>(args)...);
                return ret;
            }
        };
    } // namespace future
} // namespace copp

#endif

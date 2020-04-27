#ifndef COPP_FUTURE_POLL_H
#define COPP_FUTURE_POLL_H

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
        template <class T>
        struct LIBCOPP_COTASK_API_HEAD_ONLY small_object_optimize_storage_delete_t {
            inline void operator()(T *) const UTIL_CONFIG_NOEXCEPT {
                // Do nothing
            }
            template <class U>
            inline void operator()(U *) const UTIL_CONFIG_NOEXCEPT {
                // Do nothing
            }
        };

        template <class T, class TPTR>
        struct LIBCOPP_COTASK_API_HEAD_ONLY poll_storage_base_t;

        template <class T>
        struct LIBCOPP_COTASK_API_HEAD_ONLY poll_storage_base_t<T, std::unique_ptr<T, small_object_optimize_storage_delete_t<T> > > {
            typedef T                                                              result_type;
            typedef std::unique_ptr<T, small_object_optimize_storage_delete_t<T> > ptr_type;
            typedef std::pair<T, ptr_type>                                         storage_type;

            static inline void construct_default_storage(storage_type &out) UTIL_CONFIG_NOEXCEPT {
                memset(&out.first, 0, sizeof(out.first));
                out.second.reset();
            }

            template <class U, class UDELETOR,
                      typename std::enable_if<std::is_base_of<T, typename type_traits::remove_cvref<U>::type>::value ||
                                                  std::is_convertible<typename type_traits::remove_cvref<U>::type, T>::value,
                                              bool>::type = false>
            static inline void construct_storage(storage_type &                                 out,
                                                 std::unique_ptr<U, UDELETOR> COPP_MACRO_RV_REF in) UTIL_CONFIG_NOEXCEPT {
                if (in) {
                    out.first = *in;
                    out.second.reset(&out.first);
                    in.reset();
                } else {
                    memset(&out.first, 0, sizeof(out.first));
                    out.second.reset();
                }
            }

            template <class U, typename std::enable_if<std::is_base_of<T, typename type_traits::remove_cvref<U>::type>::value ||
                                                           std::is_convertible<typename type_traits::remove_cvref<U>::type, T>::value,
                                                       bool>::type = false>
            static inline void construct_storage(storage_type &out, U COPP_MACRO_RV_REF in) UTIL_CONFIG_NOEXCEPT {
                out.first = in;
                out.second.reset(&out.first);
            }

            static inline const ptr_type &unwrap(const storage_type &storage) UTIL_CONFIG_NOEXCEPT { return storage.second; }
            static inline ptr_type &      unwrap(storage_type &storage) UTIL_CONFIG_NOEXCEPT { return storage.second; }
        };

        template <class T, class TPTR>
        struct LIBCOPP_COTASK_API_HEAD_ONLY poll_storage_base_t {
            typedef T        result_type;
            typedef TPTR     ptr_type;
            typedef ptr_type storage_type;

            static inline void construct_default_storage(storage_type &out) UTIL_CONFIG_NOEXCEPT { out.reset(); }

            template <class U, class UDELETOR,
                      typename std::enable_if<std::is_base_of<T, typename type_traits::remove_cvref<U>::type>::value, bool>::type = false>
            static inline void construct_storage(storage_type &                                 out,
                                                 std::unique_ptr<U, UDELETOR> COPP_MACRO_RV_REF in) UTIL_CONFIG_NOEXCEPT {
                out = COPP_MACRO_STD_MOVE(in);
            }

            template <class U, typename std::enable_if<std::is_base_of<T, typename type_traits::remove_cvref<U>::type>::value &&
                                                           type_traits::is_shared_ptr<ptr_type>::value,
                                                       bool>::type = false>
            static inline void construct_storage(storage_type &out, std::shared_ptr<U> COPP_MACRO_RV_REF in) UTIL_CONFIG_NOEXCEPT {
                out = COPP_MACRO_STD_MOVE(std::static_pointer_cast<typename ptr_type::element_type>(in));
            }

            static inline const ptr_type &unwrap(const storage_type &storage) UTIL_CONFIG_NOEXCEPT { return storage; }
            static inline ptr_type &      unwrap(storage_type &storage) UTIL_CONFIG_NOEXCEPT { return storage; }
        };

        template <class T>
        struct poll_storage_select_ptr_t {
            typedef typename std::conditional<std::is_trivial<T>::value && sizeof(T) < (sizeof(size_t) << 2),
                                              std::unique_ptr<T, small_object_optimize_storage_delete_t<T> >,
                                              std::unique_ptr<T, std::default_delete<T> > >::type type;
        };

        template <class T, class TPTR = typename poll_storage_select_ptr_t<T>::type>
        class LIBCOPP_COTASK_API_HEAD_ONLY poll_t {
        public:
            // If T is trivial and is smaller than size of four pointer, using small object optimization
            typedef poll_storage_base_t<T, TPTR>          poll_storage_t;
            typedef typename poll_storage_t::storage_type storage_type;
            typedef typename poll_storage_t::ptr_type     ptr_type;
            typedef typename poll_storage_t::result_type  result_type;

        public:
            poll_t() UTIL_CONFIG_NOEXCEPT { poll_storage_t::construct_default_storage(storage_data_); }

            template <class U, class UDELETER,
                      typename std::enable_if<std::is_base_of<T, typename type_traits::remove_cvref<U>::type>::value, bool>::type = false>
            poll_t(std::unique_ptr<U, UDELETER> COPP_MACRO_RV_REF in) UTIL_CONFIG_NOEXCEPT {
                poll_storage_t::template construct_storage(storage_data_, COPP_MACRO_STD_MOVE(in));
            }

            template <class U>
            poll_t(U COPP_MACRO_RV_REF in) UTIL_CONFIG_NOEXCEPT {
                poll_storage_t::template construct_storage(storage_data_, COPP_MACRO_STD_FORWARD(U, in));
            }

            inline bool is_ready() const UTIL_CONFIG_NOEXCEPT { return !!poll_storage_t::unwrap(storage_data_); }

            inline bool is_pending() const UTIL_CONFIG_NOEXCEPT { return !poll_storage_t::unwrap(storage_data_); }

            inline const result_type *data() const UTIL_CONFIG_NOEXCEPT { return poll_storage_t::unwrap(storage_data_).get(); }
            inline result_type *      data() UTIL_CONFIG_NOEXCEPT { return poll_storage_t::unwrap(storage_data_).get(); }

            inline const storage_type &raw_ptr() const UTIL_CONFIG_NOEXCEPT { return storage_data_; }
            inline storage_type &      raw_ptr() UTIL_CONFIG_NOEXCEPT { return storage_data_; }

        private:
            storage_type storage_data_;
        };
    } // namespace future
} // namespace copp

#endif
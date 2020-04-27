#ifndef COPP_FUTURE_CONTEXT_H
#define COPP_FUTURE_CONTEXT_H

#pragma once

#include "poll.h"

namespace copp {
    namespace future {
        template <class T, class TPTR>
        struct LIBCOPP_COTASK_API_HEAD_ONLY context_storage_base_t;

        template <class T>
        struct LIBCOPP_COTASK_API_HEAD_ONLY context_storage_base_t<T, std::unique_ptr<T, small_object_optimize_storage_delete_t<T> > > {
            typedef T                                                              result_type;
            typedef std::unique_ptr<T, small_object_optimize_storage_delete_t<T> > ptr_type;
            typedef T                                                              storage_type;

            static inline void construct_default_storage(storage_type &out) UTIL_CONFIG_NOEXCEPT { memset(&out, 0, sizeof(out)); }

            template <class U, class UDELETOR,
                      typename std::enable_if<std::is_base_of<T, typename type_traits::remove_cvref<U>::type>::value ||
                                                  std::is_convertible<typename type_traits::remove_cvref<U>::type, T>::value,
                                              bool>::type = false>
            static inline void construct_storage(storage_type &                                 out,
                                                 std::unique_ptr<U, UDELETOR> COPP_MACRO_RV_REF in) UTIL_CONFIG_NOEXCEPT {
                if (in) {
                    out = *in;
                } else {
                    memset(&out, 0, sizeof(out));
                }
            }

            template <class U, typename std::enable_if<std::is_base_of<T, typename type_traits::remove_cvref<U>::type>::value ||
                                                           std::is_convertible<typename type_traits::remove_cvref<U>::type, T>::value,
                                                       bool>::type = false>
            static inline void construct_storage(storage_type &out, U COPP_MACRO_RV_REF in) UTIL_CONFIG_NOEXCEPT {
                out = in;
            }

            static inline const ptr_type &unwrap(const storage_type &storage) UTIL_CONFIG_NOEXCEPT { return ptr_type(&storage); }
            static inline ptr_type &      unwrap(storage_type &storage) UTIL_CONFIG_NOEXCEPT { return ptr_type(&storage); }
        };

        template <class T>
        struct LIBCOPP_COTASK_API_HEAD_ONLY context_storage_base_t<T, std::shared_ptr<T> > {
            typedef T                  result_type;
            typedef std::shared_ptr<T> ptr_type;
            typedef ptr_type           storage_type;

            static inline void construct_default_storage(storage_type &out) { out.reset(); }

            template <class U, class UDELETOR,
                      typename std::enable_if<std::is_base_of<T, typename type_traits::remove_cvref<U>::type>::value, bool>::type = false>
            static inline void construct_storage(storage_type &out, std::unique_ptr<U, UDELETOR> COPP_MACRO_RV_REF in) {
                if (in) {
                    out = COPP_MACRO_STD_MOVE(in);
                } else {
                    out.reset();
                }
            }

            template <class U
                      typename std::enable_if<std::is_base_of<T, typename type_traits::remove_cvref<U>::type>::value, bool>::type = false>
            static inline void construct_storage(storage_type &out, std::shared_ptr<U> COPP_MACRO_RV_REF in) {
                if (in) {
                    out = std::static_pointer_cast<T>(in);
                } else {
                    out.reset();
                }
            }

            template <class... TARGS>
            static inline void construct_storage(storage_type &out, TARGS COPP_MACRO_RV_REF... in) {
                out = std::make_shared<T>(COPP_MACRO_STD_FORWARD(TARGS, in)...);
            }

            static inline const ptr_type &unwrap(const storage_type &storage) UTIL_CONFIG_NOEXCEPT { return storage; }
            static inline ptr_type &      unwrap(storage_type &storage) UTIL_CONFIG_NOEXCEPT { return storage; }
        };

        template <class T>
        struct context_storage_select_ptr_t {
            typedef
                typename std::conditional<std::is_trivial<T>::value && sizeof(T) <= (sizeof(size_t) << 2),
                                          std::unique_ptr<T, small_object_optimize_storage_delete_t<T> >, std::shared_ptr<T> >::type type;
        };

        /**
         * @brief context_t
         * @note TPD::operator()(poll_t& out) will be called when related event state changes, and the "out" must be set if the action is
         * ready
         */
        template <class TPD>
        class LIBCOPP_COTASK_API_HEAD_ONLY context_t {
        public:
            typedef context_t<TPD>                                                                self_type;
            typedef context_storage_base_t<TPD, typename context_storage_select_ptr_t<TPD>::type> private_data_storage_type;
            typedef std::function<void(self_type &)>                                              wake_fn_t;

        public:
            template <class... TARGS>
            context_t(TARGS &&... args) : private_data_(COPP_MACRO_STD_FORWARD(TARGS, args)...) {}

            template <class T, class TPTR>
            void poll(poll_t<T, TPTR> &out) {
                if (private_data_storage_type::unwrap(private_data_)) {
                    (*private_data_storage_type::unwrap(private_data_))(out);
                }
            }

            void wake() {
                if (wake_fn_) {
                    wake_fn_(*this);
                }
            }

            inline void             set_wake_fn(wake_fn_t fn) { wake_fn_ = fn; }
            inline const wake_fn_t &get_wake_fn() const { return wake_fn_; }
            inline wake_fn_t &      get_wake_fn() { return wake_fn_; }

            inline TPD *get_private_data() UTIL_CONFIG_NOEXCEPT {
                if (private_data_storage_type::unwrap(private_data_)) {
                    return private_data_storage_type::unwrap(private_data_).get();
                }

                return NULL;
            }

            inline const TPD *get_private_data() const UTIL_CONFIG_NOEXCEPT {
                if (private_data_storage_type::unwrap(private_data_)) {
                    return private_data_storage_type::unwrap(private_data_).get();
                }

                return NULL;
            }

        private:
            private_data_storage_type private_data_;
            wake_fn_t                 wake_fn_;
        };
    } // namespace future
} // namespace copp

#endif
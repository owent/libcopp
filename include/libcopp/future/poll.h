#ifndef COPP_FUTURE_POLL_H
#define COPP_FUTURE_POLL_H

#pragma once

#include "storage.h"

namespace copp {
    namespace future {
        template <class T, class TPTR = typename poll_storage_select_ptr_t<T>::type>
        class LIBCOPP_COPP_API_HEAD_ONLY poll_t {
        public:
            typedef poll_t<T, TPTR> self_type;
            // If T is trivial and is smaller than size of four pointer, using small object optimization
            typedef poll_storage_base_t<T, TPTR>          poll_storage_t;
            typedef typename poll_storage_t::storage_type storage_type;
            typedef typename poll_storage_t::ptr_type     ptr_type;
            typedef typename poll_storage_t::value_type   value_type;

        public:
            poll_t() UTIL_CONFIG_NOEXCEPT { poll_storage_t::construct_default_storage(storage_data_); }

            template <class U>
            poll_t(U COPP_MACRO_RV_REF in) UTIL_CONFIG_NOEXCEPT {
                setup_from(COPP_MACRO_STD_FORWARD(U, in));
            }

            poll_t(self_type COPP_MACRO_RV_REF other) UTIL_CONFIG_NOEXCEPT { setup_from(COPP_MACRO_STD_MOVE(other)); }

            poll_t &operator=(self_type COPP_MACRO_RV_REF other) UTIL_CONFIG_NOEXCEPT {
                setup_from(COPP_MACRO_STD_MOVE(other));
                return *this;
            }

            inline bool is_ready() const UTIL_CONFIG_NOEXCEPT { return !!poll_storage_t::unwrap(storage_data_); }

            inline bool is_pending() const UTIL_CONFIG_NOEXCEPT { return !poll_storage_t::unwrap(storage_data_); }

            inline const value_type *data() const UTIL_CONFIG_NOEXCEPT { return poll_storage_t::unwrap(storage_data_).get(); }
            inline value_type *      data() UTIL_CONFIG_NOEXCEPT { return poll_storage_t::unwrap(storage_data_).get(); }

            inline const ptr_type &raw_ptr() const UTIL_CONFIG_NOEXCEPT { return poll_storage_t::unwrap(storage_data_); }
            inline ptr_type &      raw_ptr() UTIL_CONFIG_NOEXCEPT { return poll_storage_t::unwrap(storage_data_); }

        private:
            template <class U, class UDELETER,
                      typename std::enable_if<std::is_base_of<T, typename std::decay<U>::type>::value, bool>::type = false>
            void setup_from(std::unique_ptr<U, UDELETER> COPP_MACRO_RV_REF in) {
                poll_storage_t::construct_storage(storage_data_, COPP_MACRO_STD_MOVE(in));
            }

            template <class TARGS>
            void setup_from(TARGS COPP_MACRO_RV_REF args) {
                poll_storage_t::construct_storage(storage_data_, COPP_MACRO_STD_FORWARD(TARGS, args));
            }

            void setup_from(self_type COPP_MACRO_RV_REF other) {
                poll_storage_t::move_storage(storage_data_, COPP_MACRO_STD_MOVE(other.storage_data_));
            }

        private:
            storage_type storage_data_;
        };
    } // namespace future
} // namespace copp

#endif
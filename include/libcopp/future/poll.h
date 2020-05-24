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
            poll_t() LIBCOPP_MACRO_NOEXCEPT { poll_storage_t::construct_default_storage(storage_data_); }

            template <class U>
            poll_t(U &&in) LIBCOPP_MACRO_NOEXCEPT {
                setup_from(std::forward<U>(in));
            }

            poll_t(self_type &&other) LIBCOPP_MACRO_NOEXCEPT { setup_from(std::move(other)); }

            poll_t &operator=(self_type &&other) LIBCOPP_MACRO_NOEXCEPT {
                setup_from(std::move(other));
                return *this;
            }

            UTIL_FORCEINLINE bool is_ready() const LIBCOPP_MACRO_NOEXCEPT { return !!poll_storage_t::unwrap(storage_data_); }

            UTIL_FORCEINLINE bool is_pending() const LIBCOPP_MACRO_NOEXCEPT { return !poll_storage_t::unwrap(storage_data_); }

            UTIL_FORCEINLINE const value_type *data() const LIBCOPP_MACRO_NOEXCEPT { return poll_storage_t::unwrap(storage_data_).get(); }
            UTIL_FORCEINLINE value_type *      data() LIBCOPP_MACRO_NOEXCEPT { return poll_storage_t::unwrap(storage_data_).get(); }

            UTIL_FORCEINLINE const ptr_type &raw_ptr() const LIBCOPP_MACRO_NOEXCEPT { return poll_storage_t::unwrap(storage_data_); }
            UTIL_FORCEINLINE ptr_type &      raw_ptr() LIBCOPP_MACRO_NOEXCEPT { return poll_storage_t::unwrap(storage_data_); }

            UTIL_FORCEINLINE void reset() { poll_storage_t::reset(storage_data_); }
            UTIL_FORCEINLINE void swap(self_type& other) LIBCOPP_MACRO_NOEXCEPT {
                poll_storage_t::swap(storage_data_, other.storage_data_);
            }
            friend UTIL_FORCEINLINE void swap(self_type& l, self_type& r) LIBCOPP_MACRO_NOEXCEPT { l.swap(r); }
        private:
            template <class U, class UDELETER,
                      typename std::enable_if<std::is_base_of<T, typename std::decay<U>::type>::value, bool>::type = false>
            void setup_from(std::unique_ptr<U, UDELETER> &&in) {
                poll_storage_t::construct_storage(storage_data_, std::move(in));
            }

            template <class TARGS>
            void setup_from(TARGS &&args) {
                poll_storage_t::construct_storage(storage_data_, std::forward<TARGS>(args));
            }

            void setup_from(self_type &&other) { poll_storage_t::move_storage(storage_data_, std::move(other.storage_data_)); }

        private:
            storage_type storage_data_;
        };
    } // namespace future
} // namespace copp

#endif
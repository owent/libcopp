#ifndef COPP_FUTURE_CONTEXT_H
#define COPP_FUTURE_CONTEXT_H

#pragma once

#include "poll.h"

namespace copp {
    namespace future {
        /**
         * @brief context_t
         * @note TPD::operator()(context_t<TPD>&, poll_t&) and TPD::operator()(context_t<TPD>&) must be declared
         *       for private data type 'TPD' (template argument).
         *       TPD::operator()(context_t<TPD>&, poll_t&) will be called with private_data_(*this, out) when related event state changes.
         *          and the "poll_t& out" must be set if all related asynchronous jobs is done.
         *       TPD::operator()(context_t<TPD>&) will be called when context is created or assigned.
         *          (including copy construction/assignment or moved construction/assignment)
         *
         * @note context_t<TPD> may hold the shared_ptr<TPD> to keep private data always available before context_t<TPD> is destroyed.
         *       So you must not hold the context in any member of TPD
         */
        template <class TPD>
        class LIBCOPP_COPP_API_HEAD_ONLY context_t {
        public:
            typedef context_t<TPD>                                                            self_type;
            typedef context_storage_base_t<TPD, typename context_storage_select_t<TPD>::type> private_data_storage_type;
            typedef std::function<void(self_type &)>                                          wake_fn_t;
            typedef typename private_data_storage_type::value_type                            value_type;

        public:
            template <class... TARGS>
            context_t(TARGS COPP_MACRO_RV_REF... args) {
                setup_from(COPP_MACRO_STD_FORWARD(TARGS, args)...);
            }

            context_t(const self_type &other) { copy_from(other); }
            context_t(self_type COPP_MACRO_RV_REF other) { move_from(COPP_MACRO_STD_MOVE(other)); }

            context_t &operator=(const self_type &other) {
                copy_from(other);
                return *this;
            }

            context_t &operator=(self_type COPP_MACRO_RV_REF other) {
                move_from(COPP_MACRO_STD_MOVE(other));
                return *this;
            }

            template <class T, class TPTR>
            void poll(poll_t<T, TPTR> &out) {
                if (private_data_storage_type::unwrap(private_data_)) {
                    (*private_data_storage_type::unwrap(private_data_))(*this, out);
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

            inline value_type *get_private_data() UTIL_CONFIG_NOEXCEPT {
                if (private_data_storage_type::unwrap(private_data_)) {
                    return private_data_storage_type::unwrap(private_data_);
                }

                return NULL;
            }

            inline const value_type *get_private_data() const UTIL_CONFIG_NOEXCEPT {
                if (private_data_storage_type::unwrap(private_data_)) {
                    return private_data_storage_type::unwrap(private_data_);
                }

                return NULL;
            }

            static inline bool is_shared_storage() UTIL_CONFIG_NOEXCEPT { return private_data_storage_type::is_shared_storage(); }

        private:
            void copy_from(const self_type &other) {
                wake_fn_ = other.wake_fn_;
                private_data_storage_type::clone_storage(private_data_, other.private_data_);

                value_type *pd = get_private_data();
                if (NULL != pd) {
                    (*pd)(*this);
                }
            }

            void move_from(self_type COPP_MACRO_RV_REF other) {
                wake_fn_.swap(other.wake_fn_);
                private_data_storage_type::move_storage(private_data_, COPP_MACRO_STD_MOVE(other.private_data_));

                value_type *pd = get_private_data();
                if (NULL != pd) {
                    (*pd)(*this);
                }
            }

            template <class... TARGS>
            void setup_from(TARGS COPP_MACRO_RV_REF... args) {
                private_data_storage_type::construct_storage(private_data_, COPP_MACRO_STD_FORWARD(TARGS, args)...);

                value_type *pd = get_private_data();
                if (NULL != pd) {
                    (*pd)(*this);
                }
            }

            void setup_from(const self_type &other) { copy_from(other); }
#if defined(UTIL_CONFIG_COMPILER_CXX_RVALUE_REFERENCES) && UTIL_CONFIG_COMPILER_CXX_RVALUE_REFERENCES
            void setup_from(self_type &other) { copy_from(other); }
            void setup_from(self_type &&other) { move_from(other); }
#else
            void setup_from(self_type &other) { move_from(other); }
#endif

        private:
            typename private_data_storage_type::storage_type private_data_;
            wake_fn_t                                        wake_fn_;
        };

        /**
         * @brief context_t<void>
         * @note pool_fn(self_type&, poll_event_data_t) will be called with pool_fn(*this, {&out, private_data_}) when related event state
         *       changes, and the "out" must be set if the action is ready
         */
        template <>
        class LIBCOPP_COPP_API_HEAD_ONLY context_t<void> {
        public:
            struct poll_event_data_t {
                void *poll_output;  // set to address of **out** when call poll(poll_t<T, TPTR> &out)
                void *private_data; // set to ptr passed by constructor
            };

            typedef context_t<void>                                     self_type;
            typedef void *                                              private_data_storage_type;
            typedef std::function<void(self_type &)>                    wake_fn_t;
            typedef std::function<void(self_type &, poll_event_data_t)> poll_fn_t;
            typedef void *                                              value_type;

        public:
            context_t(poll_fn_t pool_fn) : poll_fn_(pool_fn), private_data_(NULL) {}
            context_t(poll_fn_t pool_fn, void *ptr) : poll_fn_(pool_fn), private_data_(ptr) {}

            context_t(const self_type &other) { copy_from(other); }

            context_t(self_type COPP_MACRO_RV_REF other) { move_from(COPP_MACRO_STD_MOVE(other)); }

            context_t &operator=(const self_type &other) {
                copy_from(other);
                return *this;
            }

            context_t &operator=(self_type COPP_MACRO_RV_REF other) {
                move_from(COPP_MACRO_STD_MOVE(other));
                return *this;
            }

            template <class T, class TPTR>
            void poll(poll_t<T, TPTR> &out) {
                if (poll_fn_) {
                    poll_event_data_t data;
                    data.poll_output  = reinterpret_cast<void *>(&out);
                    data.private_data = private_data_;
                    poll_fn_(*this, data);
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

            inline value_type get_private_data() UTIL_CONFIG_NOEXCEPT { return private_data_; }

            inline const value_type get_private_data() const UTIL_CONFIG_NOEXCEPT { return private_data_; }

        private:
            void copy_from(const self_type &other) {
                private_data_ = other.private_data_;
                wake_fn_      = other.wake_fn_;
                poll_fn_      = other.poll_fn_;
            }

            void move_from(self_type COPP_MACRO_RV_REF other) {
                private_data_ = other.private_data_;
                wake_fn_.swap(other.wake_fn_);
                poll_fn_.swap(other.poll_fn_);

                other.private_data_ = NULL;
            }

        private:
            private_data_storage_type private_data_;
            wake_fn_t                 wake_fn_;
            poll_fn_t                 poll_fn_;
        };
    } // namespace future
} // namespace copp

#endif
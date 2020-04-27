#ifndef COPP_FUTURE_FUTURE_H
#define COPP_FUTURE_FUTURE_H

#pragma once

#include "context.h"
#include "poll.h"


namespace copp {
    namespace future {
        template <class T, class TPTR = typename poll_storage_select_ptr_t<T>::type>
        class LIBCOPP_COTASK_API_HEAD_ONLY future_t {
        public:
            typedef future_t<T, TPTR> self_type;
            typedef poll_t<T, TPTR>   poll_type;

        public:
            inline bool is_ready() const UTIL_CONFIG_NOEXCEPT { return poll_data_.is_ready(); }

            inline bool is_pending() const UTIL_CONFIG_NOEXCEPT { return poll_data_.is_pending(); }

            template <class TPD>
            void poll(context_t<TPD> &ctx) {
                if (is_ready()) {
                    return;
                }

                ctx.poll(poll_data_);

                if (!ctx.get_wake_fn() && !is_ready()) {
                    ctx.set_wake_fn(wake_future_t<TPD>(*this));
                }
            }

        private:
            template <class TPD>
            struct wake_future_t {
                self_type *self_;
                wake_future_t(self_type &s) : self_(&s) {}
                void operator()(context_t<TPD> &ctx) {
                    if (NULL != self_) {
                        self_->poll(ctx);
                    }
                }
            };

        private:
            poll_type poll_data_;
        };

    } // namespace future
} // namespace copp

#endif
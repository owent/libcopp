#ifndef COPP_FUTURE_FUTURE_H
#define COPP_FUTURE_FUTURE_H

#pragma once

#include "context.h"
#include "poll.h"


namespace copp {
    namespace future {
        template <class T, class TPTR = typename poll_storage_select_ptr_t<T>::type>
        class LIBCOPP_COPP_API_HEAD_ONLY future_t {
        public:
            typedef future_t<T, TPTR>                self_type;
            typedef poll_t<T, TPTR>                  poll_type;
            typedef typename poll_type::storage_type storage_type;
            typedef typename poll_type::value_type   value_type;
            typedef typename poll_type::ptr_type     ptr_type;

        public:
            inline bool is_ready() const UTIL_CONFIG_NOEXCEPT { return poll_data_.is_ready(); }

            inline bool is_pending() const UTIL_CONFIG_NOEXCEPT { return poll_data_.is_pending(); }

            template <class TPD>
            void poll(context_t<TPD> &ctx) {
                if (is_ready()) {
                    return;
                }

                ctx.poll(poll_data_);

                if (is_ready()) {
                    if (ctx.get_wake_fn()) {
                        ctx.set_wake_fn(NULL);
                    }
                } else {
                    if (!ctx.get_wake_fn()) {
                        ctx.set_wake_fn(wake_future_t<TPD>(*this));
                    }
                }
            }

            inline const value_type *data() const UTIL_CONFIG_NOEXCEPT {
                if (!is_ready()) {
                    return NULL;
                }
                return poll_data_.data();
            }

            inline value_type *data() UTIL_CONFIG_NOEXCEPT {
                if (!is_ready()) {
                    return NULL;
                }

                return poll_data_.data();
            }

            inline const ptr_type &raw_ptr() const UTIL_CONFIG_NOEXCEPT { return poll_data_.raw_ptr(); }

            inline ptr_type &raw_ptr() UTIL_CONFIG_NOEXCEPT { return poll_data_.raw_ptr(); }


            // ================= C++20 Coroutine Support =================
        public:
            struct promise_type {
                self_type *bind_future_;

                self_type get_return_object() { return self_type(); }
#if defined(LIBCOPP_MACRO_ENABLE_STD_COROUTINE) && LIBCOPP_MACRO_ENABLE_STD_COROUTINE
#if defined(LIBCOPP_MACRO_USE_STD_EXPERIMENTAL_COROUTINE) && LIBCOPP_MACRO_USE_STD_EXPERIMENTAL_COROUTINE
                auto initial_suspend() { return std::experimental::suspend_never(); }
                auto final_suspend() { return std::experimental::suspend_never(); }
#else
                auto initial_suspend() { return std::suspend_never(); }
                auto final_suspend() { return std::suspend_never(); }
#endif
#endif

                void unhandled_exception() {}
                void return_void() {}

                template <class U>
                void return_value(U &&in) {}

                template <class... U>
                auto await_transform(U &&...) {}
            };

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


#if defined(LIBCOPP_MACRO_ENABLE_STD_COROUTINE) && LIBCOPP_MACRO_ENABLE_STD_COROUTINE
        // template <class T, class TPTR>
        // struct std::experimental::coroutine_traits<future_t<T, TPTR> > {
        //     using promise_type = std::unique_ptr<future<T, TPTR> >;
        // };

        // std::experimental::coroutine_traits<>
        template <class TFUTURE, class TPD>
        class LIBCOPP_COPP_API_HEAD_ONLY future_awaiter_t {
        public:
            typedef future_awaiter_t<TFUTURE, TPD> self_type;
            typedef TFUTURE                        future_type;
            typedef context_t<TPD>                 context_type;

        public:
            template <class... TARGS>
            future_awaiter_t(future_type &fut, TARGS &&... args) : future_(&fut), context_(COPP_MACRO_STD_FORWARD(TARGS, args)...) {}

            bool await_ready() UTIL_CONFIG_NOEXCEPT {
                if (nullptr != future_ && !future_->is_ready()) {
                    future_->poll(context_);
                }

                return nullptr == future_ || future_->is_ready();
            }

            void await_suspend(LIBCOPP_MACRO_FUTURE_COROUTINE_VOID h) UTIL_CONFIG_NOEXCEPT {
                handle_ = h;

                // set waker into call handle_.resume();
                context_.set_wake_fn(wake_future_awaiter_t{*this});
            }

            template <class U>
            void await_suspend(LIBCOPP_MACRO_FUTURE_COROUTINE_TYPE(U) h) UTIL_CONFIG_NOEXCEPT {
                handle_ = LIBCOPP_MACRO_FUTURE_COROUTINE_VOID::from_address(h.address());

                // set waker into call handle_.resume();
                context_.set_wake_fn(wake_future_awaiter_t{*this});
            }

            void await_resume() UTIL_CONFIG_NOEXCEPT {
                handle_ = nullptr;
                if (nullptr != future_) {
                    future_->poll(context_, false);
                }
            }

        private:
            struct wake_future_awaiter_t {
                self_type *self_;
                wake_future_awaiter_t(self_type &s) : self_(&s) {}
                void operator()(context_type &ctx) {
                    if (nullptr != self_) {
                        if (nullptr != self_->handle_.address() && !self_->handle_.done()) {
                            self_->handle_.resume();
                        }
                    }
                }
            };

        private:
            LIBCOPP_MACRO_FUTURE_COROUTINE_VOID handle_;
            future_type *                       future_;
            context_type                        context_;
        };


        template <class TPD, class TFUTURE, class... TARGS>
        LIBCOPP_COPP_API_HEAD_ONLY inline future_awaiter_t<TFUTURE, TPD> make_awaiter(TFUTURE &fut, TARGS &&... args) {
            return future_awaiter_t{fut, COPP_MACRO_STD_FORWARD(TARGS, args)...};
        }

        template <class TFUTURE, class TPD>
        LIBCOPP_COPP_API_HEAD_ONLY future_awaiter_t<TFUTURE, TPD>
                                   operator co_await(future_awaiter_t<TFUTURE, TPD> &&ret) UTIL_CONFIG_NOEXCEPT {
            return ret;
        }
#endif
    } // namespace future
} // namespace copp

#endif
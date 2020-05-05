#ifndef COPP_FUTURE_FUTURE_H
#define COPP_FUTURE_FUTURE_H

#pragma once

#include "context.h"
#include "poll.h"


namespace copp {
    namespace future {
        template <class T, class TPTR>
        class task_t;

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

            inline const ptr_type & raw_ptr() const UTIL_CONFIG_NOEXCEPT { return poll_data_.raw_ptr(); }
            inline ptr_type &       raw_ptr() UTIL_CONFIG_NOEXCEPT { return poll_data_.raw_ptr(); }
            inline const poll_type &get_poll_data() const UTIL_CONFIG_NOEXCEPT { return poll_data_; }
            inline poll_type &      get_poll_data() UTIL_CONFIG_NOEXCEPT { return poll_data_; }


            // ================= C++20 Coroutine Support =================
        public:
            task_t<T, TPTR> get_return_object() UTIL_CONFIG_NOEXCEPT;
#if defined(LIBCOPP_MACRO_ENABLE_STD_COROUTINE) && LIBCOPP_MACRO_ENABLE_STD_COROUTINE
#if defined(LIBCOPP_MACRO_USE_STD_EXPERIMENTAL_COROUTINE) && LIBCOPP_MACRO_USE_STD_EXPERIMENTAL_COROUTINE
            auto initial_suspend() UTIL_CONFIG_NOEXCEPT { return std::experimental::suspend_always(); }
            auto final_suspend() UTIL_CONFIG_NOEXCEPT { return std::experimental::suspend_never(); }
#else
            auto                      initial_suspend() UTIL_CONFIG_NOEXCEPT { return std::suspend_always(); }
            auto                      final_suspend() UTIL_CONFIG_NOEXCEPT { return std::suspend_never(); }
#endif
            void unhandled_exception() UTIL_CONFIG_NOEXCEPT {
                // exception_ptr_ = std::new (static_cast<void*>(std::addressof(m_exception))) std::exception_ptr(std::current_exception());
            }

            void return_void() UTIL_CONFIG_NOEXCEPT {}

            template <class U>
            void return_value(U &&in) UTIL_CONFIG_NOEXCEPT {
                poll_data_ = COPP_MACRO_STD_FORWARD(U, in);
            }

            template <class... U>
            auto await_transform(U &&...) UTIL_CONFIG_NOEXCEPT {}

            struct final_awaiter {
                bool await_ready() const UTIL_CONFIG_NOEXCEPT { return false; }

                void await_suspend(LIBCOPP_MACRO_FUTURE_COROUTINE_TYPE(self_type) h) UTIL_CONFIG_NOEXCEPT {
                    // handle_ = LIBCOPP_MACRO_FUTURE_COROUTINE_VOID::from_address(h.address());

                    // set waker into call handle_.resume();
                    // context_.set_wake_fn(wake_future_awaiter_t{*this});
                }

                void await_resume() noexcept {}
            };
#endif

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
            typedef future_awaiter_t<TFUTURE, TPD>  self_type;
            typedef TFUTURE                         future_type;
            typedef context_t<TPD>                  context_type;
            typedef typename future_type::poll_type poll_type;

        public:
            template <class... TARGS>
            future_awaiter_t(future_type &fut, TARGS &&... args) : future_(&fut), context_(COPP_MACRO_STD_FORWARD(TARGS, args)...) {}

            bool await_ready() const UTIL_CONFIG_NOEXCEPT {
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

            poll_type await_resume() UTIL_CONFIG_NOEXCEPT {
                handle_ = nullptr;
                if (nullptr != future_) {
                    future_->poll(context_, false);
                    if (future_->is_ready()) {
                        return COPP_MACRO_STD_MOVE(future_->get_poll_data());
                    }
                }

                return poll_type();
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

        template <class T, class TPTR = typename poll_storage_select_ptr_t<T>::type>
        class task_t {
        public:
#if defined(LIBCOPP_MACRO_ENABLE_STD_COROUTINE) && LIBCOPP_MACRO_ENABLE_STD_COROUTINE
            using promise_type = future_t<T, TPTR>;
#else
            typedef future_t<T, TPTR> promise_type;
#endif
        };

        template <typename T, class TPTR>
        task_t<T, TPTR> future_t<T, TPTR>::get_return_object() UTIL_CONFIG_NOEXCEPT {
            return task_t<T, TPTR>{
                // std::experimental::coroutine_handle<future_t<T> >::from_promise(*this)
            };
        }

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
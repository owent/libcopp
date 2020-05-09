#ifndef COPP_FUTURE_STD_COROUTINE_GENERATOR_H
#define COPP_FUTURE_STD_COROUTINE_GENERATOR_H

#pragma once

#include "future.h"

namespace copp {
    namespace future {

#if defined(LIBCOPP_MACRO_ENABLE_STD_COROUTINE) && LIBCOPP_MACRO_ENABLE_STD_COROUTINE
        template <class T, class TPTR = typename poll_storage_select_ptr_t<T>::type>
        class LIBCOPP_COPP_API_HEAD_ONLY generator_future_t : public future_t<T, TPTR> {
        public:
#if defined(UTIL_CONFIG_COMPILER_CXX_ALIAS_TEMPLATES) && UTIL_CONFIG_COMPILER_CXX_ALIAS_TEMPLATES
            using self_type = generator_future_t<T, TPD>;
#else
            typedef generator_future_t<T, TPD>      self_type;
#endif

        public:
            template <class... TARGS>
            generator_future_t(TARGS &&... args) : future_t<T, TPTR>(std::forward<TARGS>(args)...), await_handle_(nullptr) {}

            inline const LIBCOPP_MACRO_FUTURE_COROUTINE_VOID &get_handle() const { return await_handle_; }
            inline LIBCOPP_MACRO_FUTURE_COROUTINE_VOID &      get_handle() { return await_handle_; }
            inline void                                       set_handle(LIBCOPP_MACRO_FUTURE_COROUTINE_VOID h) { await_handle_ = h; }

        private:
            LIBCOPP_MACRO_FUTURE_COROUTINE_VOID await_handle_;
        };

        template <class T, class TPD = void, class TPTR = typename poll_storage_select_ptr_t<T>::type>
        class LIBCOPP_COPP_API_HEAD_ONLY EXPLICIT_NODISCARD_ATTR generator_t {
        public:
#if defined(UTIL_CONFIG_COMPILER_CXX_ALIAS_TEMPLATES) && UTIL_CONFIG_COMPILER_CXX_ALIAS_TEMPLATES
            using self_type    = generator_t<T, TPD, TPTR>;
            using context_type = context_t<TPD>;
            using future_type  = generator_future_t<T, TPTR>;
            using poll_type    = typename future_type::poll_type;
#else
            typedef generator_t<T, TPD, TPTR>       self_type;
            typedef context_t<TPD>                  context_type;
            typedef generator_future_t<T, TPTR>     future_type;
            typedef typename future_type::poll_type poll_type;
#endif

        private:
            class awaitable_t {
            private:
                awaitable_t(const awaitable_t &) UTIL_CONFIG_DELETED_FUNCTION;
                awaitable_t &operator=(const awaitable_t &) UTIL_CONFIG_DELETED_FUNCTION;

            public:
                awaitable_t(future_type &fut) : future_(&fut) {}

                awaitable_t(awaitable_t &&other) : future_(other.future_) { other.future_ = nullptr; }
                awaitable_t &operator=(awaitable_t &&other) {
                    future_ = other.future_;

                    other.future_ = nullptr;
                }

                bool await_ready() const UTIL_CONFIG_NOEXCEPT { return !future_ || !future_->get_handle() || future_->get_handle().done(); }

                void await_suspend(LIBCOPP_MACRO_FUTURE_COROUTINE_VOID h) UTIL_CONFIG_NOEXCEPT {
                    if (future_) {
                        // This should never triggered, because a generator can only be co_await once
                        if (future_->get_handle() && !future_->get_handle().done()) {
                            future_->get_handle().resume();
                        }

                        future_->set_handle(h);
                    }
                }

                decltype(auto) await_resume() UTIL_CONFIG_NOEXCEPT {
                    await_handle_ = nullptr;

                    if (future_) {
                        if (future_->is_ready()) {
                            future_type *fut = future_;
                            future_          = nullptr;

                            return std::move(fut->poll_data());
                        }

                        future_->set_handle(nullptr);
                    }

                    return poll_type{};
                }

            protected:
                future_type *future_;
            };

            struct wake_awaitable_t {
                future_type *                                  future;
                std::shared_ptr<typename future_type::waker_t> waker;
                wake_awaitable_t(future_type &fut, const std::shared_ptr<typename future_type::waker_t> &w) : future(&fut), waker(w) {}
                void operator()(context_type &ctx) {
                    // if waker->self == nullptr, the future is already destroyed, then handle is also invalid
                    if (waker && waker->self) {
                        if (future) {
                            if (!future->is_ready()) {
                                future->poll(ctx);
                            }

                            if (future->is_ready()) {
                                while (future->get_handle() && !future->get_handle().done()) {
                                    future->get_handle().resume();
                                }
                            }
                        }
                    }
                }
            };

        public:
            template <class... TARGS>
            generator_t(TARGS &&... args) : context_(std::forward<TARGS>(args)...) {
                future_.poll(context_);
                // setup waker
                if (!future_.is_ready()) {
                    context_.set_wake_fn(wake_awaitable_t{&future_, future_.mutable_waker()});
                }
            }

            auto operator co_await() && UTIL_CONFIG_NOEXCEPT { return awaitable_t{future_}; }

        protected:
            context_type context_;
            future_type  future_;
        };
#endif
    } // namespace future
} // namespace copp

#endif
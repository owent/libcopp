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
            using self_type = generator_future_t<T, TPTR>;
#else
            typedef generator_future_t<T, TPTR>      self_type;
#endif

        public:
            template <class... TARGS>
            generator_future_t(TARGS &&... args) : future_t<T, TPTR>(std::forward<TARGS>(args)...), await_handle_(nullptr) {}
            ~generator_future_t() {}

            UTIL_FORCEINLINE const LIBCOPP_MACRO_FUTURE_COROUTINE_VOID &get_handle() const { return await_handle_; }
            UTIL_FORCEINLINE LIBCOPP_MACRO_FUTURE_COROUTINE_VOID &      get_handle() { return await_handle_; }
            UTIL_FORCEINLINE void                                       set_handle(LIBCOPP_MACRO_FUTURE_COROUTINE_VOID h) { await_handle_ = h; }

            template <class TCONTEXT>
            void poll(TCONTEXT &&ctx) {
                future_t<T, TPTR>::template poll_as<self_type>(*this, std::forward<TCONTEXT>(ctx));
            }

        private:
            LIBCOPP_MACRO_FUTURE_COROUTINE_VOID await_handle_;
        };

        template <class TPD>
        class LIBCOPP_COPP_API_HEAD_ONLY generator_context_t : public context_t<TPD> {
        public:
            typedef generator_context_t<TPD> self_type;

        private:
            // context can not be copy or moved.
            generator_context_t(const generator_context_t &) UTIL_CONFIG_DELETED_FUNCTION;
            generator_context_t &operator=(const generator_context_t &) UTIL_CONFIG_DELETED_FUNCTION;
            generator_context_t(generator_context_t &&) UTIL_CONFIG_DELETED_FUNCTION;
            generator_context_t &operator=(generator_context_t &&) UTIL_CONFIG_DELETED_FUNCTION;

        public:
            template <class... TARGS>
            generator_context_t(TARGS &&... args) : context_t<TPD>(std::forward<TARGS>(args)...) {}
        };

        template <class T, class TPD = void, class TPTR = typename poll_storage_select_ptr_t<T>::type>
        class LIBCOPP_COPP_API_HEAD_ONLY generator_t {
        public:
#if defined(UTIL_CONFIG_COMPILER_CXX_ALIAS_TEMPLATES) && UTIL_CONFIG_COMPILER_CXX_ALIAS_TEMPLATES
            using self_type    = generator_t<T, TPD, TPTR>;
            using context_type = generator_context_t<TPD>;
            using future_type  = generator_future_t<T, TPTR>;
            using poll_type    = typename future_type::poll_type;
            using value_type   = typename future_type::value_type;
#else
            typedef generator_t<T, TPD, TPTR>        self_type;
            typedef generator_context_t<TPD>         context_type;
            typedef generator_future_t<T, TPTR>      future_type;
            typedef typename future_type::poll_type  poll_type;
            typedef typename future_type::value_type value_type;
#endif

        private:
            class awaitable_base_t {
            private:
                awaitable_base_t(const awaitable_base_t &) UTIL_CONFIG_DELETED_FUNCTION;
                awaitable_base_t &operator=(const awaitable_base_t &) UTIL_CONFIG_DELETED_FUNCTION;

            public:
                awaitable_base_t(future_type &fut, context_type &ctx) : future_(&fut), context_(&ctx) {}

                awaitable_base_t(awaitable_base_t &&other) : future_(other.future_), context_(&other.context_) {
                    other.future_  = nullptr;
                    other.context_ = nullptr;
                }
                awaitable_base_t &operator=(awaitable_base_t &&other) {
                    future_  = other.future_;
                    context_ = other.context_;

                    other.future_  = nullptr;
                    other.context_ = nullptr;
                }

                inline bool await_ready() const LIBCOPP_MACRO_NOEXCEPT { return !future_ || future_->is_ready(); }

                inline void await_suspend(LIBCOPP_MACRO_FUTURE_COROUTINE_VOID h) LIBCOPP_MACRO_NOEXCEPT {
                    if (future_) {
                        // This should never triggered, because a generator can only be co_await once
                        if (future_->get_handle() && !future_->get_handle().done()) {
                            future_->get_handle().resume();
                        }

                        future_->set_handle(h);
                    }
                }

                inline future_type *await_resume() LIBCOPP_MACRO_NOEXCEPT {
                    if (likely(future_)) {
                        future_->set_handle(nullptr);

                        if (future_->is_ready()) {
                            future_type *fut = future_;
                            future_          = nullptr;

                            return fut;
                        }
                    }

                    return nullptr;
                }

            protected:
                future_type * future_;
                context_type *context_;
            };

            class reference_awaitable_t : public awaitable_base_t {
            public:
                using awaitable_base_t::awaitable_base_t;

                value_type *await_resume() LIBCOPP_MACRO_NOEXCEPT {
                    future_type *fut = awaitable_base_t::await_resume();
                    if (nullptr != fut) {
                        return fut->data();
                    }
                    return nullptr;
                }
            };

            class rvalue_awaitable_t : public awaitable_base_t {
            public:
                using awaitable_base_t::awaitable_base_t;

                poll_type await_resume() LIBCOPP_MACRO_NOEXCEPT {
                    future_type *fut = awaitable_base_t::await_resume();
                    if (nullptr != fut) {
                        return std::move(fut->poll_data());
                    }
                    return nullptr;
                }
            };

            struct generator_waker_t {
                future_type *future;
                generator_waker_t(future_type &fut) : future(&fut) {}

                void operator()(context_type &ctx) {
                    if (likely(future)) {
                        if (!future->is_ready()) {
                            future->template poll_as<future_type>(ctx);
                        }

                        // waker may be destroyed when call poll, so copy waker and future into stack
                        if (future->is_ready() && future->get_handle() && !future->get_handle().done()) {
                            // This may lead to deallocation of generator_waker_t later, and we need not to resume again
                            future->get_handle().resume();
                        }
                    }
                }

                void operator()(context_t<TPD> &ctx) { (*this)(*static_cast<context_type *>(&ctx)); }
            };

        public:
            template <class... TARGS>
            generator_t(TARGS &&... args) : context_(std::forward<TARGS>(args)...) {
                context_.set_wake_fn(generator_waker_t{future_});
                // Can not set waker clear functor, because even future is polled outside
                //   we still need to resume handle after event finished
                // future_.set_ctx_waker(context_);

                future_.template poll_as<future_type>(context_);
            }
            ~generator_t() { context_.set_wake_fn(NULL); }

            auto operator co_await() & LIBCOPP_MACRO_NOEXCEPT { return reference_awaitable_t{future_, context_}; }

            // co_await a temporary generator_t in GCC 10.1.0 will destroy generator_t first, which will cause all resources unavailable
            // auto operator co_await() && LIBCOPP_MACRO_NOEXCEPT { return rvalue_awaitable_t{future_, context_}; }

            UTIL_FORCEINLINE value_type *        data() LIBCOPP_MACRO_NOEXCEPT { return future_.data(); }
            UTIL_FORCEINLINE const value_type *  data() const LIBCOPP_MACRO_NOEXCEPT { return future_.data(); }
            UTIL_FORCEINLINE poll_type &         poll_data() LIBCOPP_MACRO_NOEXCEPT { return future_.poll_data(); }
            UTIL_FORCEINLINE const poll_type &   poll_data() const LIBCOPP_MACRO_NOEXCEPT { return future_.poll_data(); }
            UTIL_FORCEINLINE context_type &      get_context() LIBCOPP_MACRO_NOEXCEPT { return context_; }
            UTIL_FORCEINLINE const context_type &get_context() const LIBCOPP_MACRO_NOEXCEPT { return context_; }

            UTIL_FORCEINLINE operator bool() const LIBCOPP_MACRO_NOEXCEPT { return has_data(); }
            UTIL_FORCEINLINE bool has_data() const LIBCOPP_MACRO_NOEXCEPT { return future_.is_ready(); }
            UTIL_FORCEINLINE void reset_data() { future_.reset_data(); }
        private:
            // generator can not be copy or moved.
            generator_t(const generator_t &) UTIL_CONFIG_DELETED_FUNCTION;
            generator_t &operator=(const generator_t &) UTIL_CONFIG_DELETED_FUNCTION;
            generator_t(generator_t &&) UTIL_CONFIG_DELETED_FUNCTION;
            generator_t &operator=(generator_t &&) UTIL_CONFIG_DELETED_FUNCTION;

        protected:
            context_type context_;
            future_type  future_;
        };

        template <class T, class... TARGS>
        inline T make_generator(TARGS &&... args) {
            return T{std::forward<TARGS>(args)...};
        }
#endif
    } // namespace future
} // namespace copp

#endif

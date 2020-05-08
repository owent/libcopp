#ifndef COPP_FUTURE_FUTURE_H
#define COPP_FUTURE_FUTURE_H

#pragma once

#include <list>

#include "context.h"
#include "poll.h"


namespace copp {
    namespace future {

        template <class T, class TPTR = typename poll_storage_select_ptr_t<T>::type>
        class LIBCOPP_COPP_API_HEAD_ONLY future_t {
        public:
#if defined(UTIL_CONFIG_COMPILER_CXX_ALIAS_TEMPLATES) && UTIL_CONFIG_COMPILER_CXX_ALIAS_TEMPLATES
            using self_type    = future_t<T, TPTR>;
            using poll_type    = poll_t<T, TPTR>;
            using storage_type = typename poll_type::storage_type;
            using value_type   = typename poll_type::value_type;
            using ptr_type     = typename poll_type::ptr_type;
#else
            typedef future_t<T, TPTR>                self_type;
            typedef poll_t<T, TPTR>                  poll_type;
            typedef typename poll_type::storage_type storage_type;
            typedef typename poll_type::value_type   value_type;
            typedef typename poll_type::ptr_type     ptr_type;
#endif

            struct waker_t {
                self_type *self;
            };

        private:
            // future can not be copy or moved.
            future_t(const future_t &) UTIL_CONFIG_DELETED_FUNCTION;
            future_t &operator=(const future_t &) UTIL_CONFIG_DELETED_FUNCTION;
            future_t(future_t &&) UTIL_CONFIG_DELETED_FUNCTION;
            future_t &operator=(future_t &&) UTIL_CONFIG_DELETED_FUNCTION;

        public:
            future_t() {}
            ~future_t() {
                if (shared_waker_) {
                    shared_waker_->self = NULL;
                }
            }

            inline bool is_ready() const UTIL_CONFIG_NOEXCEPT { return poll_data_.is_ready(); }

            inline bool is_pending() const UTIL_CONFIG_NOEXCEPT { return poll_data_.is_pending(); }

            template <class TPD>
            void poll(context_t<TPD> &ctx) {
                if (is_ready()) {
                    return;
                }

                // Set waker first, and then context can be moved or copyed in private data callback
                if (!ctx.get_wake_fn()) {
                    ctx.set_wake_fn(wake_future_t<TPD>(mutable_waker()));
                }

                ctx.poll(poll_data_);

                if (is_ready() && ctx.get_wake_fn()) {
                    ctx.set_wake_fn(NULL);
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

            const std::shared_ptr<waker_t> &mutable_waker() {
                if (shared_waker_) {
                    return shared_waker_;
                }

                shared_waker_ = std::make_shared<waker_t>();
                if (shared_waker_) {
                    shared_waker_->self = this;
                }

                return shared_waker_;
            }

        private:
            template <class TPD>
            struct wake_future_t {
                std::shared_ptr<waker_t> waker;
                wake_future_t(const std::shared_ptr<waker_t> &w) : waker(w) {}
                void operator()(context_t<TPD> &ctx) {
                    if (waker && NULL != waker->self) {
                        waker->self->poll(ctx);
                    }
                }
            };

        private:
            std::shared_ptr<waker_t> shared_waker_;
            poll_type                poll_data_;
        };


#if defined(LIBCOPP_MACRO_ENABLE_STD_COROUTINE) && LIBCOPP_MACRO_ENABLE_STD_COROUTINE
        template <class T, class TPD, class TPTR, class TWAKELISTALLOC>
        class LIBCOPP_COPP_API_HEAD_ONLY task_t;

        template <class T, class TPD, class TPTR = typename poll_storage_select_ptr_t<T>::type,
                  class TWAKELISTALLOC = std::allocator<LIBCOPP_MACRO_FUTURE_COROUTINE_VOID> >
        class LIBCOPP_COPP_API_HEAD_ONLY coroutine_future_t : public future_t<T, TPTR> {
        public:
#if defined(UTIL_CONFIG_COMPILER_CXX_ALIAS_TEMPLATES) && UTIL_CONFIG_COMPILER_CXX_ALIAS_TEMPLATES
            using self_type      = coroutine_future_t<T, TPD, TPTR, TWAKELISTALLOC>;
            using context_type   = context_t<TPD>;
            using wake_list_type = std::list<LIBCOPP_MACRO_FUTURE_COROUTINE_VOID, TWAKELISTALLOC>;
#else
            typedef coroutine_future_t<T, TPD, TPTR, TWAKELISTALLOC> self_type;
            typedef context_t<TPD>                                   context_type;
            typedef std::list<LIBCOPP_MACRO_FUTURE_COROUTINE_VOID>   wake_list_type;
#endif

            // ================= C++20 Coroutine Support =================
            struct final_awaitable {
                bool await_ready() const UTIL_CONFIG_NOEXCEPT { return false; }

                template <typename U>
                void await_suspend(std::experimental::coroutine_handle<U> handle) {
                    handle.promise().wake_all();
                }

                void await_resume() UTIL_CONFIG_NOEXCEPT {}
            };

        public:
            template <class... TARGS>
            coroutine_future_t(TARGS &&... args) : context_(std::forward<TARGS>(args)...) {}

            task_t<T, TPD, TPTR, TWAKELISTALLOC> get_return_object() UTIL_CONFIG_NOEXCEPT;

            auto initial_suspend() UTIL_CONFIG_NOEXCEPT { return LIBCOPP_MACRO_STD_COROUTINE_NAMESPACE suspend_never{}; }
            auto final_suspend() UTIL_CONFIG_NOEXCEPT { return final_awaitable{}; }
            void unhandled_exception() UTIL_CONFIG_NOEXCEPT {
                // exception_ptr_ = std::new (static_cast<void*>(std::addressof(m_exception))) std::exception_ptr(std::current_exception());
            }

            // void return_void() UTIL_CONFIG_NOEXCEPT {}

            template <class U>
            void return_value(U &&in) UTIL_CONFIG_NOEXCEPT {
                this->get_poll_data() = std::forward<U>(in);
            }

            // template <class... TARGS>
            // auto await_transform(TARGS &&...) UTIL_CONFIG_NOEXCEPT;

            const context_type &get_context() const UTIL_CONFIG_NOEXCEPT { return context_; }
            context_type &      get_context() UTIL_CONFIG_NOEXCEPT { return context_; }

            typename wake_list_type::iterator begin_wake_handles() UTIL_CONFIG_NOEXCEPT { return await_handles_.begin(); }
            typename wake_list_type::iterator end_wake_handles() UTIL_CONFIG_NOEXCEPT { return await_handles_.end(); }
            typename wake_list_type::iterator add_wake_handle(LIBCOPP_MACRO_FUTURE_COROUTINE_VOID h) {
                if (h) {
                    return await_handles_.insert(await_handles_.end(), h);
                }

                return end_wake_handles();
            }
            void remove_wake_handle(typename wake_list_type::iterator &iter) {
                if (iter != await_handles_.end()) {
                    await_handles_.erase(iter);
                    iter = await_handles_.end();
                }
            }

            void wake_all() {
                for (typename wake_list_type::iterator iter = await_handles_.begin(); iter != await_handles_.end();) {
                    typename wake_list_type::iterator wake_iter = iter++;
                    if (*wake_iter) {
                        (*wake_iter).resume();
                    }
                }

                await_handles_.clear();
            }

        private:
            wake_list_type await_handles_;
            context_type   context_;
        };

        template <class T, class TPD = void, class TPTR = typename poll_storage_select_ptr_t<T>::type,
                  class TWAKELISTALLOC = std::allocator<LIBCOPP_MACRO_FUTURE_COROUTINE_VOID> >
        class LIBCOPP_COPP_API_HEAD_ONLY EXPLICIT_NODISCARD_ATTR task_t {
        public:
#if defined(UTIL_CONFIG_COMPILER_CXX_ALIAS_TEMPLATES) && UTIL_CONFIG_COMPILER_CXX_ALIAS_TEMPLATES
            using self_type      = task_t<T, TPD, TPTR, TWAKELISTALLOC>;
            using promise_type   = coroutine_future_t<T, TPD, TPTR, TWAKELISTALLOC>;
            using context_type   = typename promise_type::context_type;
            using poll_type      = typename promise_type::poll_type;
            using wake_list_type = typename promise_type::wake_list_type;
            using storage_type   = typename poll_type::storage_type;
            using value_type     = typename poll_type::value_type;
            using ptr_type       = typename poll_type::ptr_type;
#else
            typedef task_t<T, TPD, TPTR, TWAKELISTALLOC>             self_type;
            typedef coroutine_future_t<T, TPD, TPTR, TWAKELISTALLOC> promise_type;
            typedef typename promise_type::context_type              context_type;
            typedef typename promise_type::poll_type                 poll_type;
            typedef typename promise_type::wake_list_type            wake_list_type;
            typedef typename poll_type::storage_type                 storage_type;
            typedef typename poll_type::value_type                   value_type;
            typedef typename poll_type::ptr_type                     ptr_type;
#endif

        private:
            class awaitable_base_t {
            private:
                awaitable_base_t(const awaitable_base_t &) UTIL_CONFIG_DELETED_FUNCTION;
                awaitable_base_t &operator=(const awaitable_base_t &) UTIL_CONFIG_DELETED_FUNCTION;

            public:
                awaitable_base_t(LIBCOPP_MACRO_STD_COROUTINE_NAMESPACE coroutine_handle<promise_type> handle) : handle_(handle) {
                    if (handle) {
                        await_iterator_ = handle.promise().end_wake_handles();
                    }
                }

                awaitable_base_t(awaitable_base_t &&other) : await_iterator_(other.await_iterator_), handle_(other.handle_) {
                    other.handle_ = nullptr;
                }
                awaitable_base_t &operator=(awaitable_base_t &&other) {
                    await_iterator_ = other.await_iterator_;
                    handle_         = other.handle_;

                    other.handle_ = nullptr;
                    if (handle_) {
                        other.await_iterator_ = handle_.promise().end_wake_handles();
                    }
                }

                bool await_ready() const UTIL_CONFIG_NOEXCEPT { return !handle_ || handle_.done(); }

                void await_suspend(LIBCOPP_MACRO_FUTURE_COROUTINE_VOID h) UTIL_CONFIG_NOEXCEPT {
                    if (handle_ && h != handle_) {
                        await_iterator_ = handle_.promise().add_wake_handle(h);
                    }
                }

                void await_resume() UTIL_CONFIG_NOEXCEPT {
                    if (handle_ && await_iterator_ != handle_.promise().end_wake_handles()) {
                        handle_.promise().remove_wake_handle(await_iterator_);
                    }
                }

            protected:
                wake_list_type                        await_iterator_;
                LIBCOPP_MACRO_STD_COROUTINE_NAMESPACE coroutine_handle<promise_type> handle_;
            };

            struct wake_awaitable_t {
                std::shared_ptr<typename promise_type::waker_t> waker;
                LIBCOPP_MACRO_STD_COROUTINE_NAMESPACE coroutine_handle<promise_type> handle;
                wake_awaitable_t(const std::shared_ptr<typename promise_type::waker_t> &w,
                                 LIBCOPP_MACRO_STD_COROUTINE_NAMESPACE coroutine_handle<promise_type> &h)
                    : waker(w), handle(&h) {}
                void operator()(context_type &ctx) {
                    // if waker->self == nullptr, the future is already destroyed, then handle is also invalid
                    if (waker && waker->self && handle) {
                        promise_type &promise = handle.promise();
                        if (!promise.is_ready()) {
                            promise.poll(ctx);
                        }

                        if (promise.is_ready()) {
                            while (!handle.done()) {
                                handle.resume();
                            }
                        }
                    }
                }
            };

        private:
            task_t(const task_t &) UTIL_CONFIG_DELETED_FUNCTION;
            task_t &operator=(const task_t &) UTIL_CONFIG_DELETED_FUNCTION;

        public:
            task_t() : handle_(nullptr) {}
            task_t(LIBCOPP_MACRO_STD_COROUTINE_NAMESPACE coroutine_handle<promise_type> handle) : handle_(handle) {
                if (handle && !handle.promise().is_ready()) {
                    handle.promise().poll(handle.promise().get_context());

                    if (!handle.promise().is_ready()) {
                        handle.promise().get_context().set_wake_fn(wake_awaitable_t{handle.promise().mutable_waker(), handle});
                    }
                }
            }
            task_t(task_t &&other) : handle_(other.handle_) { other.handle_ = nullptr; }
            task_t &operator=(task_t &&other) {
                handle_       = other.handle_;
                other.handle_ = nullptr;
            }


            auto operator co_await() const &UTIL_CONFIG_NOEXCEPT {
                struct awaitable_t : awaitable_base_t {
                    using awaitable_base_t::awaitable_base_t;

                    decltype(auto) await_resume() {
                        awaitable_base_t::await_resume();
                        return this->handle_.promise().data();
                    }
                };

                return awaitable_t{handle_};
            }

            auto operator co_await() const &&UTIL_CONFIG_NOEXCEPT {
                struct awaitable_t : awaitable_base_t {
                    using awaitable_base_t::awaitable_base_t;

                    decltype(auto) await_resume() {
                        awaitable_base_t::await_resume();

                        promise_type &promise = this->handle.promise();
                        if (promise.is_ready()) {
                            return std::move(promise.get_poll_data());
                        }

                        return poll_type();
                    }
                };

                return awaitable_t{handle_};
            }

        private:
            LIBCOPP_MACRO_STD_COROUTINE_NAMESPACE coroutine_handle<promise_type> handle_;
        };

        template <typename T, class TPD, class TPTR, class TWAKELISTALLOC>
        task_t<T, TPD, TPTR, TWAKELISTALLOC> coroutine_future_t<T, TPD, TPTR, TWAKELISTALLOC>::get_return_object() UTIL_CONFIG_NOEXCEPT {
            return task_t<T, TPD, TPTR, TWAKELISTALLOC>{
                LIBCOPP_MACRO_STD_COROUTINE_NAMESPACE
                    coroutine_handle<typename task_t<T, TPD, TPTR, TWAKELISTALLOC>::promise_type>::from_promise(*this)};
        }
#endif
    } // namespace future
} // namespace copp

#endif
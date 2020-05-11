#ifndef COPP_FUTURE_STD_COROUTINE_TASK_H
#define COPP_FUTURE_STD_COROUTINE_TASK_H

#pragma once

#include <assert.h>

#include "future.h"

namespace copp {
    namespace future {

#if defined(LIBCOPP_MACRO_ENABLE_STD_COROUTINE) && LIBCOPP_MACRO_ENABLE_STD_COROUTINE
        struct macro_task {
#if defined(UTIL_CONFIG_COMPILER_CXX_ALIAS_TEMPLATES) && UTIL_CONFIG_COMPILER_CXX_ALIAS_TEMPLATES
            using coroutine_handle_allocator = std::allocator<LIBCOPP_MACRO_FUTURE_COROUTINE_VOID>;
#else
            typedef std::allocator<LIBCOPP_MACRO_FUTURE_COROUTINE_VOID> coroutine_handle_allocator;
#endif
        };

        template <class TPD>
        class LIBCOPP_COPP_API_HEAD_ONLY task_context_t : public context_t<TPD> {
        public:
            typedef task_context_t<TPD> self_type;

        private:
            // context can not be copy or moved.
            task_context_t(const task_context_t &) UTIL_CONFIG_DELETED_FUNCTION;
            task_context_t &operator=(const task_context_t &) UTIL_CONFIG_DELETED_FUNCTION;
            task_context_t(task_context_t &&) UTIL_CONFIG_DELETED_FUNCTION;
            task_context_t &operator=(task_context_t &&) UTIL_CONFIG_DELETED_FUNCTION;

        public:
            template <class... TARGS>
            task_context_t(TARGS &&... args) : context_t<TPD>(std::forward<TARGS>(args)...) {}
        };

        template <class T, class TPD, class TPTR, class TMACRO>
        class LIBCOPP_COPP_API_HEAD_ONLY task_t;

        template <class T, class TPD, class TPTR, class TMACRO>
        class LIBCOPP_COPP_API_HEAD_ONLY task_promise_base_t;

        template <class T, class TPD, class TPTR, class TMACRO>
        class LIBCOPP_COPP_API_HEAD_ONLY task_promise_base_t {
        public:
#if defined(UTIL_CONFIG_COMPILER_CXX_ALIAS_TEMPLATES) && UTIL_CONFIG_COMPILER_CXX_ALIAS_TEMPLATES
            using self_type      = task_promise_base_t<T, TPD, TPTR, TMACRO>;
            using future_type    = future_t<T, TPTR>;
            using poll_type      = typename future_type::poll_type;
            using waker_type     = typename future_type::waker_t;
            using context_type   = task_context_t<TPD>;
            using wake_list_type = std::list<LIBCOPP_MACRO_FUTURE_COROUTINE_VOID, typename macro_task::coroutine_handle_allocator>;
#else
            typedef task_promise_base_t<T, TPD, TPTR, TMACRO>           self_type;
            typedef future_t<T, TPTR>                                   future_type;
            typedef typename future_type::poll_type                     poll_type;
            typedef typename future_type::waker_t                       waker_type;
            typedef task_context_t<TPD>                                 context_type;
            typedef std::list<LIBCOPP_MACRO_FUTURE_COROUTINE_VOID, typename macro_task::coroutine_handle_allocator> wake_list_type;
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

        private:
            // future can not be copy or moved.
            task_promise_base_t(const task_promise_base_t &) UTIL_CONFIG_DELETED_FUNCTION;
            task_promise_base_t &operator=(const task_promise_base_t &) UTIL_CONFIG_DELETED_FUNCTION;
            task_promise_base_t(task_promise_base_t &&) UTIL_CONFIG_DELETED_FUNCTION;
            task_promise_base_t &operator=(task_promise_base_t &&) UTIL_CONFIG_DELETED_FUNCTION;

        public:
            template <class... TARGS>
            task_promise_base_t(TARGS &&... args) : context_(std::forward<TARGS>(args)...), task_future_(nullptr) {}
            ~task_promise_base_t() {
                // printf("~task_promise_base_t %p\n", this);
                // cleanup: await_handles_ should be already cleanup in final_awaitable::await_suspend
                assert(await_handles_.empty());
                // wake_all();
            }

            auto initial_suspend() UTIL_CONFIG_NOEXCEPT { return LIBCOPP_MACRO_STD_COROUTINE_NAMESPACE suspend_never{}; }
            auto final_suspend() UTIL_CONFIG_NOEXCEPT { return final_awaitable{}; }
            void unhandled_exception() UTIL_CONFIG_NOEXCEPT {
                // exception_ptr_ = std::new (static_cast<void*>(std::addressof(m_exception))) std::exception_ptr(std::current_exception());
            }

            // template <class... TARGS>
            // auto await_transform(TARGS &&...) UTIL_CONFIG_NOEXCEPT;

            inline const context_type &get_context() const UTIL_CONFIG_NOEXCEPT { return context_; }
            inline context_type &      get_context() UTIL_CONFIG_NOEXCEPT { return context_; }
            inline const future_type * get_bind_future() const UTIL_CONFIG_NOEXCEPT { return task_future_; }
            inline future_type *       get_bind_future() UTIL_CONFIG_NOEXCEPT { return task_future_; }
            inline void                set_bind_future(future_type *fut) UTIL_CONFIG_NOEXCEPT { task_future_ = fut; }

            inline typename wake_list_type::iterator begin_wake_handles() UTIL_CONFIG_NOEXCEPT { return await_handles_.begin(); }
            inline typename wake_list_type::iterator end_wake_handles() UTIL_CONFIG_NOEXCEPT { return await_handles_.end(); }
            inline typename wake_list_type::iterator add_wake_handle(LIBCOPP_MACRO_FUTURE_COROUTINE_VOID h) {
                if (h) {
                    return await_handles_.insert(await_handles_.end(), h);
                }

                return end_wake_handles();
            }
            inline void remove_wake_handle(typename wake_list_type::iterator &iter) {
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
            future_type *  task_future_;
        };

        template <class T, class TPD, class TPTR, class TMACRO>
        class LIBCOPP_COPP_API_HEAD_ONLY task_promise_t;

        template <class T, class TPD, class TPTR, class TMACRO>
        class LIBCOPP_COPP_API_HEAD_ONLY task_promise_t : public task_promise_base_t<T, TPD, TPTR, TMACRO> {
        public:
#if defined(UTIL_CONFIG_COMPILER_CXX_ALIAS_TEMPLATES) && UTIL_CONFIG_COMPILER_CXX_ALIAS_TEMPLATES
            using self_type = task_promise_t<T, TPD, TPTR, TMACRO>;
#else
            typedef task_promise_t<T, TPD, TPTR, TMACRO>                                                            self_type;
#endif

        public:
            task_t<T, TPD, TPTR, TMACRO> get_return_object() UTIL_CONFIG_NOEXCEPT;

            template <class U>
            void return_value(U &&in) UTIL_CONFIG_NOEXCEPT {
                // Maybe set error data on custom poller, ignore co_return here.
                if (nullptr != get_bind_future() && !get_bind_future()->is_ready()) {
                    get_bind_future()->poll_data() = std::forward<U>(in);
                }
            }
        };


        template <class TPD, class TPTR, class TMACRO>
        class LIBCOPP_COPP_API_HEAD_ONLY task_promise_t<void, TPD, TPTR, TMACRO> : public task_promise_base_t<void, TPD, TPTR, TMACRO> {
        public:
#if defined(UTIL_CONFIG_COMPILER_CXX_ALIAS_TEMPLATES) && UTIL_CONFIG_COMPILER_CXX_ALIAS_TEMPLATES
            using self_type = task_promise_t<void, TPD, TPTR, TMACRO>;
#else
            typedef task_promise_t<void, TPD, TPTR, TMACRO>                                                         self_type;
#endif

        public:
            task_t<void, TPD, TPTR, TMACRO> get_return_object() UTIL_CONFIG_NOEXCEPT;

            void return_void() UTIL_CONFIG_NOEXCEPT {
                // Maybe set error data on custom poller, ignore co_return here.
                if (nullptr != get_bind_future() && !get_bind_future()->is_ready()) {
                    get_bind_future()->poll_data() = true;
                }
            }
        };

        template <class TPROMISE>
        struct task_wake_awaitable_t {
            std::shared_ptr<typename TPROMISE::waker_type> waker;
            LIBCOPP_MACRO_STD_COROUTINE_NAMESPACE coroutine_handle<TPROMISE> handle;
            task_wake_awaitable_t(const std::shared_ptr<typename TPROMISE::waker_type> &w,
                                  LIBCOPP_MACRO_STD_COROUTINE_NAMESPACE coroutine_handle<TPROMISE> &h)
                : waker(w), handle(h) {}


            template <class TCONTEXT>
            void operator()(TCONTEXT &&ctx) {
                // if waker->self == nullptr, the future is already destroyed, then handle is also invalid
                if (waker && waker->self && handle) {
                    call_poll(waker, handle, std::forward<TCONTEXT>(ctx));
                }
            }

            template <class TCONTEXT>
            static void call_poll(std::shared_ptr<typename TPROMISE::waker_type> waker,
                                  LIBCOPP_MACRO_STD_COROUTINE_NAMESPACE coroutine_handle<TPROMISE> handle, TCONTEXT &&ctx) {
                TPROMISE &promise = handle.promise();
                if (promise.get_bind_future() != nullptr) {
                    if (!promise.get_bind_future()->is_ready()) {
                        promise.get_bind_future()->poll(std::forward<TCONTEXT>(ctx));
                    }
                    // TODO check type
                }

                // waker may be destroyed when call poll, so copy waker and handle into stack
                while (!handle.done() && (promise.get_bind_future() == nullptr || promise.get_bind_future()->is_ready())) {
                    handle.resume();
                }
            }
        };

        template <class T, class TPD = void, class TPTR = typename poll_storage_select_ptr_t<T>::type,
                  class TMACRO = std::allocator<LIBCOPP_MACRO_FUTURE_COROUTINE_VOID> >
        class LIBCOPP_COPP_API_HEAD_ONLY EXPLICIT_NODISCARD_ATTR task_t {
        public:
#if defined(UTIL_CONFIG_COMPILER_CXX_ALIAS_TEMPLATES) && UTIL_CONFIG_COMPILER_CXX_ALIAS_TEMPLATES
            using self_type      = task_t<T, TPD, TPTR, TMACRO>;
            using promise_type   = task_promise_t<T, TPD, TPTR, TMACRO>;
            using future_type    = typename promise_type::future_type;
            using context_type   = typename promise_type::context_type;
            using poll_type      = typename promise_type::poll_type;
            using wake_list_type = typename promise_type::wake_list_type;
            using storage_type   = typename poll_type::storage_type;
            using value_type     = typename poll_type::value_type;
            using ptr_type       = typename poll_type::ptr_type;
#else
            typedef task_t<T, TPD, TPTR, TMACRO>                                                                    self_type;
            typedef task_promise_t<T, TPD, TPTR, TMACRO>                                                            promise_type;
            typedef typename promise_type::future_type                                                              future_type;
            typedef typename promise_type::context_type                                                             context_type;
            typedef typename promise_type::poll_type                                                                poll_type;
            typedef typename promise_type::wake_list_type                                                           wake_list_type;
            typedef typename poll_type::storage_type                                                                storage_type;
            typedef typename poll_type::value_type                                                                  value_type;
            typedef typename poll_type::ptr_type                                                                    ptr_type;
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
                typename wake_list_type::iterator     await_iterator_;
                LIBCOPP_MACRO_STD_COROUTINE_NAMESPACE coroutine_handle<promise_type> handle_;
            };

        private:
            task_t(const task_t &) UTIL_CONFIG_DELETED_FUNCTION;
            task_t &operator=(const task_t &) UTIL_CONFIG_DELETED_FUNCTION;

        public:
            task_t() : handle_(nullptr) {}
            task_t(LIBCOPP_MACRO_STD_COROUTINE_NAMESPACE coroutine_handle<promise_type> handle) : handle_(handle) {
                if (handle_) {
                    if (handle_.promise().get_bind_future() == nullptr) {
                        handle_.promise().set_bind_future(&future_);
                    }

                    handle.promise().get_context().set_wake_fn(task_wake_awaitable_t{future_.mutable_waker(), handle});

                    if (!future_.is_ready()) {
                        future_.poll(handle.promise().get_context());
                    }
                }
            }
            ~task_t() {
                if (handle_) {
                    if (handle_.promise().get_bind_future() == &future_) {
                        handle_.promise().set_bind_future(nullptr);
                    }
                }
            }

            task_t(task_t &&other) : handle_(other.handle_) {
                other.handle_ = nullptr;

                if (handle_) {
                    if (handle_.promise().get_bind_future() == &other.future_) {
                        // rebind future and reset waker
                        handle_.promise().set_bind_future(&future_);
                        handle_.promise().get_context().set_wake_fn(task_wake_awaitable_t{future_.mutable_waker(), handle_});
                    }
                }
            }

            task_t &operator=(task_t &&other) {
                handle_       = other.handle_;
                other.handle_ = nullptr;

                if (handle_) {
                    if (handle_.promise().get_bind_future() == &other.future_) {
                        // rebind future and reset waker
                        handle_.promise().set_bind_future(&future_);
                        handle_.promise().get_context().set_wake_fn(task_wake_awaitable_t{future_.mutable_waker(), handle_});
                    }
                }
            }

            auto operator co_await() && UTIL_CONFIG_NOEXCEPT {
                struct awaitable_t : awaitable_base_t {
                    using awaitable_base_t::awaitable_base_t;

                    poll_type await_resume() {
                        awaitable_base_t::await_resume();
                        if (handle_ && nullptr != handle_.promise().get_bind_future()) {
                            return std::move(handle_.promise().get_bind_future()->poll_data());
                        }

                        return poll_type{};
                    }
                };

                // TODO if (future_.is_ready()) { return awaitable_t{nullptr}; }
                return awaitable_t{handle_};
            }

            auto operator co_await() & UTIL_CONFIG_NOEXCEPT {
                struct awaitable_t : awaitable_base_t {
                    using awaitable_base_t::awaitable_base_t;

                    value_type *await_resume() {
                        awaitable_base_t::await_resume();
                        if (handle_ && nullptr != handle_.promise().get_bind_future()) {
                            return handle_.promise().get_bind_future()->data();
                        }

                        return nullptr;
                    }
                };

                // TODO if (future_.is_ready()) { return awaitable_t{nullptr}; }
                return awaitable_t{handle_};
            }

            inline bool is_finished() const { return !handle_ || handle_.done(); }

            inline const value_type *data() const UTIL_CONFIG_NOEXCEPT { return future_.data(); }
            inline value_type *      data() UTIL_CONFIG_NOEXCEPT { return future_.data(); }
            inline const poll_type & poll_data() const UTIL_CONFIG_NOEXCEPT { return future_.poll_data(); }
            inline poll_type &       poll_data() UTIL_CONFIG_NOEXCEPT { return future_.poll_data(); }

        private:
            LIBCOPP_MACRO_STD_COROUTINE_NAMESPACE coroutine_handle<promise_type> handle_;
            future_type                                                          future_;
        };

        template <typename T, class TPD, class TPTR, class TMACRO>
        task_t<T, TPD, TPTR, TMACRO> task_promise_t<T, TPD, TPTR, TMACRO>::get_return_object() UTIL_CONFIG_NOEXCEPT {
            return task_t<T, TPD, TPTR, TMACRO>{
                LIBCOPP_MACRO_STD_COROUTINE_NAMESPACE coroutine_handle<typename task_t<T, TPD, TPTR, TMACRO>::promise_type>::from_promise(
                    *this)};
        }

        template <class TPD, class TPTR, class TMACRO>
        task_t<void, TPD, TPTR, TMACRO> task_promise_t<void, TPD, TPTR, TMACRO>::get_return_object() UTIL_CONFIG_NOEXCEPT {
            return task_t<void, TPD, TPTR, TMACRO>{
                LIBCOPP_MACRO_STD_COROUTINE_NAMESPACE
                    coroutine_handle<typename task_t<void, TPD, TPTR, TMACRO>::promise_type>::from_promise(*this)};
        }
#endif
    } // namespace future
} // namespace copp


#endif
#ifndef COPP_FUTURE_STD_COROUTINE_TASK_H
#define COPP_FUTURE_STD_COROUTINE_TASK_H

#pragma once

#include <assert.h>

#ifdef __cpp_impl_three_way_comparison
#include <compare>
#endif

#include <libcopp/utils/uint64_id_allocator.h>

#include "future.h"

#if defined(LIBCOPP_MACRO_ENABLE_STD_EXCEPTION_PTR) && LIBCOPP_MACRO_ENABLE_STD_EXCEPTION_PTR
#include <exception>
#endif

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
            task_context_t(uint64_t tid, TARGS &&... args) : context_t<TPD>(std::forward<TARGS>(args)...), task_id_(tid) {}

            inline uint64_t get_task_id() const LIBCOPP_MACRO_NOEXCEPT { return task_id_; }
        private:
            uint64_t task_id_;
        };

        template <class T, class TPTR = typename poll_storage_select_ptr_t<T>::type>
        class LIBCOPP_COPP_API_HEAD_ONLY task_future_t : public future_t<T, TPTR> {
        public:
            using future_t<T, TPTR>::future_t;
        };

        template <class T, class TPD, class TPTR, class TMACRO>
        class LIBCOPP_COPP_API_HEAD_ONLY task_t;

        template <class T, class TPD, class TPTR, class TMACRO>
        class LIBCOPP_COPP_API_HEAD_ONLY task_promise_base_t;

        template <class T, class TPD, class TPTR, class TMACRO>
        class LIBCOPP_COPP_API_HEAD_ONLY task_promise_t;

        template <class T, class TPD, class TPTR, class TMACRO>
        struct LIBCOPP_COPP_API_HEAD_ONLY task_runtime_t;

        enum class LIBCOPP_COPP_API_HEAD_ONLY task_status_t {
            CREATED = 0,
            RUNNING = 1,
            DONE    = 2,
        };

        template <class T, class TPD, class TPTR, class TMACRO>
        struct LIBCOPP_COPP_API_HEAD_ONLY task_common_types_t {
#if defined(UTIL_CONFIG_COMPILER_CXX_ALIAS_TEMPLATES) && UTIL_CONFIG_COMPILER_CXX_ALIAS_TEMPLATES
            using future_type    = task_future_t<T, TPTR>;
            using poll_type      = typename future_type::poll_type;
            using waker_type     = typename future_type::waker_t;
            using context_type   = task_context_t<TPD>;
            using wake_list_type = std::list<LIBCOPP_MACRO_FUTURE_COROUTINE_VOID, typename macro_task::coroutine_handle_allocator>;
#else
            typedef task_future_t<T, TPTR>          future_type;
            typedef typename future_type::poll_type poll_type;
            typedef typename future_type::waker_t   waker_type;
            typedef task_context_t<TPD>             context_type;
            typedef std::list<LIBCOPP_MACRO_FUTURE_COROUTINE_VOID, typename macro_task::coroutine_handle_allocator> wake_list_type;
#endif
        };

        template <class T, class TPD, class TPTR, class TMACRO>
        struct LIBCOPP_COPP_API_HEAD_ONLY task_runtime_t {
#if defined(UTIL_CONFIG_COMPILER_CXX_ALIAS_TEMPLATES) && UTIL_CONFIG_COMPILER_CXX_ALIAS_TEMPLATES
            using self_type      = task_promise_base_t<T, TPD, TPTR, TMACRO>;
            using future_type    = typename task_common_types_t<T, TPD, TPTR, TMACRO>::future_type;
            using waker_type     = typename task_common_types_t<T, TPD, TPTR, TMACRO>::waker_type;
            using poll_type      = typename task_common_types_t<T, TPD, TPTR, TMACRO>::poll_type;
            using context_type   = typename task_common_types_t<T, TPD, TPTR, TMACRO>::context_type;
            using wake_list_type = typename task_common_types_t<T, TPD, TPTR, TMACRO>::wake_list_type;
#else
            typedef task_promise_base_t<T, TPD, TPTR, TMACRO>           self_type;
            typedef typename task_common_types_t<T, TPD, TPTR, TMACRO>::future_type    future_type;
            typedef typename task_common_types_t<T, TPD, TPTR, TMACRO>::poll_type      poll_type;
            typedef typename task_common_types_t<T, TPD, TPTR, TMACRO>::waker_type     waker_type;
            typedef typename task_common_types_t<T, TPD, TPTR, TMACRO>::context_type   context_type;
            typedef typename task_common_types_t<T, TPD, TPTR, TMACRO>::wake_list_type wake_list_type;
#endif

            task_runtime_t() : task_id(0), status(task_status_t::CREATED), handle(NULL) {}

#ifdef __cpp_impl_three_way_comparison
            friend inline std::strong_ordering operator<=>(const task_runtime_t &l, const task_runtime_t &r) LIBCOPP_MACRO_NOEXCEPT {
                return l.task_id <=> r.task_id;
            }
#else
            friend inline bool operator!=(const task_runtime_t &l, const task_runtime_t &r) LIBCOPP_MACRO_NOEXCEPT { return l.task_id != r.task_id; }
            friend inline bool operator<(const task_runtime_t &l, const task_runtime_t &r) LIBCOPP_MACRO_NOEXCEPT { return l.task_id < r.task_id; }
            friend inline bool operator<=(const task_runtime_t &l, const task_runtime_t &r) LIBCOPP_MACRO_NOEXCEPT { return l.task_id <= r.task_id; }
            friend inline bool operator>(const task_runtime_t &l, const task_runtime_t &r) LIBCOPP_MACRO_NOEXCEPT { return l.task_id > r.task_id; }
            friend inline bool operator>=(const task_runtime_t &l, const task_runtime_t &r) LIBCOPP_MACRO_NOEXCEPT { return l.task_id >= r.task_id; }
#endif

            inline bool done() const LIBCOPP_MACRO_NOEXCEPT {
                return status == task_status_t::DONE || !handle || handle.done();
            }

            uint64_t      task_id;
            task_status_t status;
            future_type   future;
            LIBCOPP_MACRO_STD_COROUTINE_NAMESPACE
            coroutine_handle<task_promise_t<T, TPD, TPTR, TMACRO> > handle;
#if defined(LIBCOPP_MACRO_ENABLE_STD_EXCEPTION_PTR) && LIBCOPP_MACRO_ENABLE_STD_EXCEPTION_PTR
            std::exception_ptr unhandle_exception;
#endif
        private:
            task_runtime_t(const task_runtime_t &) UTIL_CONFIG_DELETED_FUNCTION;
            task_runtime_t &operator=(const task_runtime_t &) UTIL_CONFIG_DELETED_FUNCTION;
            task_runtime_t(task_runtime_t &&) UTIL_CONFIG_DELETED_FUNCTION;
            task_runtime_t &operator=(task_runtime_t &&) UTIL_CONFIG_DELETED_FUNCTION;
        };

        template <class TPROMISE>
        struct task_waker_t {
            typedef typename TPROMISE::context_type context_type;
            std::shared_ptr<typename TPROMISE::runtime_type> runtime;
            task_waker_t(std::shared_ptr<typename TPROMISE::runtime_type> &r): runtime(r) {}

            void operator()(context_type &ctx) {
                // if waker->self == nullptr, the future is already destroyed, then handle is also invalid
                if (likely(runtime)) {
                    if (!runtime->done() && !runtime->future.is_ready()) {
                        runtime->future.template poll_as<typename TPROMISE::future_type>(ctx);
                    }

                    // once set ready, it must be polled to the end
                    if (runtime->done() || runtime->future.is_ready()) {
                        while (runtime->handle && !runtime->handle.done()) {
                            runtime->handle.resume();
                        }
                    }
                }
                // TODO check type
            }

            template<class UPD>
            void operator()(context_t<UPD> &ctx) {
#if defined(UTIL_CONFIG_COMPILER_CXX_STATIC_ASSERT) && UTIL_CONFIG_COMPILER_CXX_STATIC_ASSERT
                static_assert(!std::is_same<context_type, context_t<UPD> >::value,
                              "task context type must be drive of task_context_t");
#endif
                (*this)(*static_cast<context_type*>(&ctx));
            }
        };

        template <class T, class TPD, class TPTR, class TMACRO>
        class LIBCOPP_COPP_API_HEAD_ONLY task_promise_base_t {
        public:
#if defined(UTIL_CONFIG_COMPILER_CXX_ALIAS_TEMPLATES) && UTIL_CONFIG_COMPILER_CXX_ALIAS_TEMPLATES
            using runtime_type   = task_runtime_t<T, TPD, TPTR, TMACRO>;
            using self_type      = task_promise_base_t<T, TPD, TPTR, TMACRO>;
            using future_type    = typename task_common_types_t<T, TPD, TPTR, TMACRO>::future_type;
            using waker_type     = typename task_common_types_t<T, TPD, TPTR, TMACRO>::waker_type;
            using poll_type      = typename task_common_types_t<T, TPD, TPTR, TMACRO>::poll_type;
            using context_type   = typename task_common_types_t<T, TPD, TPTR, TMACRO>::context_type;
            using wake_list_type = typename task_common_types_t<T, TPD, TPTR, TMACRO>::wake_list_type;
#else
            typedef typename task_runtime_t<T, TPD, TPTR, TMACRO>                                                   runtime_type;
            typedef task_promise_base_t<T, TPD, TPTR, TMACRO>                                                       self_type;
            typedef typename task_common_types_t<T, TPD, TPTR, TMACRO>::future_type    future_type;
            typedef typename task_common_types_t<T, TPD, TPTR, TMACRO>::poll_type      poll_type;
            typedef typename task_common_types_t<T, TPD, TPTR, TMACRO>::waker_type     waker_type;
            typedef typename task_common_types_t<T, TPD, TPTR, TMACRO>::context_type   context_type;
            typedef typename task_common_types_t<T, TPD, TPTR, TMACRO>::wake_list_type wake_list_type;
#endif

        private:
            // ================= C++20 Coroutine Support =================
            struct initial_awaitable {
                self_type *promise;
                initial_awaitable(self_type &s) : promise(&s) {}

                inline bool await_ready() const LIBCOPP_MACRO_NOEXCEPT {
                    // only setup handle once
                    // return promise && promise->runtime_ && promise->runtime_->handle;
                    return false;
                }

                template <typename U>
                void await_suspend(LIBCOPP_MACRO_STD_COROUTINE_NAMESPACE coroutine_handle<U> handle) {
                    if (likely(promise && promise->runtime_)) {
                        if (unlikely(promise->runtime_->handle)) {
                            promise->runtime_->handle.resume();
                        }
                        promise->runtime_->handle = handle;
                    }

                    if (likely(handle)) {
                        if (likely(handle.promise().runtime_)) {
                            runtime_type& runtime = *handle.promise().runtime_;
                            handle.promise().get_context().set_wake_fn(task_waker_t<U>{handle.promise().runtime_});
                            // Can not set waker clear functor, because even future is polled outside
                            //   we still need to resume handle after event finished
                            // runtime.future.set_ctx_waker(handle.promise().get_context());

                            if (!runtime.future.is_ready()) {
                                runtime.future.template poll_as<future_type>(handle.promise().get_context());
                            }
                        }

                        handle.resume();
                    }
                }

                inline void await_resume() LIBCOPP_MACRO_NOEXCEPT {
                    if (likely(promise && promise->runtime_)) {
                        promise->runtime_->status = task_status_t::RUNNING;
                    }
                }
            };

            struct final_awaitable {
                self_type *promise;
                final_awaitable(self_type &s) : promise(&s) {}

                inline bool await_ready() const LIBCOPP_MACRO_NOEXCEPT { return true; }

                template <typename U>
                inline void await_suspend(LIBCOPP_MACRO_STD_COROUTINE_NAMESPACE coroutine_handle<U> /*handle*/) LIBCOPP_MACRO_NOEXCEPT {}

                void await_resume() LIBCOPP_MACRO_NOEXCEPT {
                    if (likely(promise)) {
                        if (likely(promise->runtime_)) {
                            promise->runtime_->status = task_status_t::DONE;
                            promise->runtime_->handle = nullptr;

                        }
                        // clear waker
                        promise->get_context().set_wake_fn(NULL);

                        // wake all co_await handles
                        promise->wake_all();
                    }
                }
            };

        private:
            // future can not be copy or moved.
            task_promise_base_t(const task_promise_base_t &) UTIL_CONFIG_DELETED_FUNCTION;
            task_promise_base_t &operator=(const task_promise_base_t &) UTIL_CONFIG_DELETED_FUNCTION;
            task_promise_base_t(task_promise_base_t &&) UTIL_CONFIG_DELETED_FUNCTION;
            task_promise_base_t &operator=(task_promise_base_t &&) UTIL_CONFIG_DELETED_FUNCTION;

            template <class U>
            struct pick_pointer_awaitable {
                U *data;
                pick_pointer_awaitable() : data(nullptr) {}

                bool await_ready() const LIBCOPP_MACRO_NOEXCEPT { return true; }

                template <typename TPROMISE>
                void await_suspend(LIBCOPP_MACRO_STD_COROUTINE_NAMESPACE coroutine_handle<TPROMISE> /*handle*/) LIBCOPP_MACRO_NOEXCEPT {}

                U *await_resume() LIBCOPP_MACRO_NOEXCEPT { return data; }
            };

        public:
            template <class U>
            struct pick_context_awaitable : pick_pointer_awaitable<U> {};

            template <class U>
            struct pick_future_awaitable : pick_pointer_awaitable<U> {};

        public:
            template <class... TARGS>
            task_promise_base_t(TARGS &&... args) : context_(copp::util::uint64_id_allocator::allocate(), std::forward<TARGS>(args)...), 
                runtime_(std::make_shared<runtime_type>()) {
                if (runtime_) {
                    runtime_->task_id = context_.get_task_id();
                }
            }
            ~task_promise_base_t() {
                // printf("~task_promise_base_t %p - %s\n", this, typeid(typename future_type::value_type).name());
                // cleanup: await_handles_ should be already cleanup in final_awaitable::await_suspend
                wake_all();
            }

            auto initial_suspend() LIBCOPP_MACRO_NOEXCEPT { return initial_awaitable{*this}; }
            auto final_suspend() LIBCOPP_MACRO_NOEXCEPT { return final_awaitable{*this}; }

            void unhandled_exception() LIBCOPP_MACRO_NOEXCEPT {
#if defined(LIBCOPP_MACRO_ENABLE_STD_EXCEPTION_PTR) && LIBCOPP_MACRO_ENABLE_STD_EXCEPTION_PTR
                if (likely(runtime_)) {
                    runtime_->unhandle_exception = std::current_exception();
                }
#endif
            }

            // template <class... TARGS>
            // auto await_transform(TARGS &&...) LIBCOPP_MACRO_NOEXCEPT;
            template <class TARGS>
            auto yield_value(TARGS &&args) {
                return std::forward<TARGS>(args);
            }

            template <class U>
            auto yield_value(pick_context_awaitable<U> &&args) {
                args.data = &context_;
                return std::move(args);
            }

            template <class U>
            auto yield_value(pick_future_awaitable<U> &&args) {
                if (likely(runtime_)) {
                    args.data = &runtime_->future;
                } else {
                    args.data = nullptr;
                }
                
                return std::move(args);
            }

            inline const context_type &get_context() const LIBCOPP_MACRO_NOEXCEPT { return context_; }
            inline context_type &      get_context() LIBCOPP_MACRO_NOEXCEPT { return context_; }
            inline const std::shared_ptr<runtime_type>& get_runtime() const LIBCOPP_MACRO_NOEXCEPT { return runtime_; }
            inline std::shared_ptr<runtime_type>&       get_runtime() LIBCOPP_MACRO_NOEXCEPT { return runtime_; }

            inline typename wake_list_type::iterator begin_wake_handles() LIBCOPP_MACRO_NOEXCEPT { return await_handles_.begin(); }
            inline typename wake_list_type::iterator end_wake_handles() LIBCOPP_MACRO_NOEXCEPT { return await_handles_.end(); }
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

            static inline pick_context_awaitable<context_type> current_context() { return pick_context_awaitable<context_type>{}; }
            static inline pick_future_awaitable<future_type>   current_future() { return pick_future_awaitable<future_type>{}; }

        private:
            wake_list_type                await_handles_;
            context_type                  context_;
            std::shared_ptr<runtime_type> runtime_;
        };

        template <class T, class TPD, class TPTR, class TMACRO>
        class LIBCOPP_COPP_API_HEAD_ONLY task_promise_t : public task_promise_base_t<T, TPD, TPTR, TMACRO> {
        public:
#if defined(UTIL_CONFIG_COMPILER_CXX_ALIAS_TEMPLATES) && UTIL_CONFIG_COMPILER_CXX_ALIAS_TEMPLATES
            using self_type = task_promise_t<T, TPD, TPTR, TMACRO>;
#else
            typedef task_promise_t<T, TPD, TPTR, TMACRO>                                                            self_type;
#endif

        public:
            task_t<T, TPD, TPTR, TMACRO> get_return_object() LIBCOPP_MACRO_NOEXCEPT;
            using task_promise_base_t<T, TPD, TPTR, TMACRO>::get_runtime;

            template <class U>
            void return_value(U &&in) LIBCOPP_MACRO_NOEXCEPT {
                // Maybe set error data on custom poller, ignore co_return here.
                if (get_runtime() && !get_runtime()->future.is_ready()) {
                    get_runtime()->future.poll_data() = std::forward<U>(in);
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
            task_t<void, TPD, TPTR, TMACRO> get_return_object() LIBCOPP_MACRO_NOEXCEPT;
            using task_promise_base_t<void, TPD, TPTR, TMACRO>::get_runtime;

            void return_void() LIBCOPP_MACRO_NOEXCEPT {
                // Maybe set error data on custom poller, ignore co_return here.
                if (get_runtime() && !get_runtime()->future.is_ready()) {
                    get_runtime()->future.poll_data() = true;
                }
            }
        };


        template <class T, class TPD = void, class TPTR = typename poll_storage_select_ptr_t<T>::type,
                  class TMACRO = std::allocator<LIBCOPP_MACRO_FUTURE_COROUTINE_VOID> >
        class LIBCOPP_COPP_API_HEAD_ONLY task_t {
        public:
#if defined(UTIL_CONFIG_COMPILER_CXX_ALIAS_TEMPLATES) && UTIL_CONFIG_COMPILER_CXX_ALIAS_TEMPLATES
            using self_type      = task_t<T, TPD, TPTR, TMACRO>;
            using promise_type   = task_promise_t<T, TPD, TPTR, TMACRO>;
            using runtime_type   = typename promise_type::runtime_type;
            using future_type    = typename promise_type::future_type;
            using context_type   = typename promise_type::context_type;
            using poll_type      = typename promise_type::poll_type;
            using wake_list_type = typename promise_type::wake_list_type;
            using storage_type   = typename poll_type::storage_type;
            using value_type     = typename poll_type::value_type;
            using status_type    = task_status_t;
#else
            typedef task_t<T, TPD, TPTR, TMACRO>                                                                    self_type;
            typedef task_promise_t<T, TPD, TPTR, TMACRO>                                                            promise_type;
            typedef typename promise_type::runtime_type                                                             runtime_type;
            typedef typename promise_type::future_type                                                              future_type;
            typedef typename promise_type::context_type                                                             context_type;
            typedef typename promise_type::poll_type                                                                poll_type;
            typedef typename promise_type::wake_list_type                                                           wake_list_type;
            typedef typename poll_type::storage_type                                                                storage_type;
            typedef typename poll_type::value_type                                                                  value_type;
            typedef task_status_t                                                                                   status_type;
#endif

        private:
            class awaitable_base_t {
            private:
                awaitable_base_t(const awaitable_base_t &) UTIL_CONFIG_DELETED_FUNCTION;
                awaitable_base_t &operator=(const awaitable_base_t &) UTIL_CONFIG_DELETED_FUNCTION;

            public:
                awaitable_base_t(self_type *task) : refer_task_(task) {
                    if (refer_task_ && refer_task_->runtime_ && !refer_task_->runtime_->done()) {
                        await_iterator_ = refer_task_->runtime_->handle.promise().end_wake_handles();
                    }
                }

                awaitable_base_t(awaitable_base_t &&other) : await_iterator_(other.await_iterator_), refer_task_(other.refer_task_) {
                    other.refer_task_ = nullptr;
                }
                awaitable_base_t &operator=(awaitable_base_t &&other) {
                    await_iterator_ = other.await_iterator_;
                    refer_task_     = other.refer_task_;

                    other.refer_task_ = nullptr;
                    if (refer_task_ && refer_task_->runtime_ && !refer_task_->runtime_->done()) {
                        refer_task_->runtime_->handle.promise().end_wake_handles();
                    }
                }

                bool await_ready() const LIBCOPP_MACRO_NOEXCEPT {
                    return !refer_task_ || refer_task_->done();
                }

                void await_suspend(LIBCOPP_MACRO_FUTURE_COROUTINE_VOID h) {
                    if (await_ready()) {
                        h.resume();
                        return;
                    }

                    await_iterator_ = refer_task_->runtime_->handle.promise().add_wake_handle(h);
                }

                void await_resume() LIBCOPP_MACRO_NOEXCEPT {
                    if (refer_task_ && refer_task_->runtime_ && refer_task_->runtime_->handle && !refer_task_->runtime_->handle.done() &&
                        await_iterator_ != refer_task_->runtime_->handle.promise().end_wake_handles()) {
                        refer_task_->runtime_->handle.promise().remove_wake_handle(await_iterator_);
                    }
                }

            protected:
                typename wake_list_type::iterator await_iterator_;
                self_type *                       refer_task_;
            };

        public:
            task_t() {}
            task_t(std::shared_ptr<runtime_type> r) : runtime_(r) {}

#ifdef __cpp_impl_three_way_comparison
            friend inline std::strong_ordering operator<=>(const task_t &l, const task_t &r) LIBCOPP_MACRO_NOEXCEPT {
                return l.get_task_id() <=> r.get_task_id();
            }
#else
            friend inline bool operator!=(const task_t &l, const task_t &r) LIBCOPP_MACRO_NOEXCEPT { return l.get_task_id() != r.get_task_id(); }
            friend inline bool operator<(const task_t &l, const task_t &r) LIBCOPP_MACRO_NOEXCEPT { return l.get_task_id() < r.get_task_id(); }
            friend inline bool operator<=(const task_t &l, const task_t &r) LIBCOPP_MACRO_NOEXCEPT { return l.get_task_id() <= r.get_task_id(); }
            friend inline bool operator>(const task_t &l, const task_t &r) LIBCOPP_MACRO_NOEXCEPT { return l.get_task_id() > r.get_task_id(); }
            friend inline bool operator>=(const task_t &l, const task_t &r) LIBCOPP_MACRO_NOEXCEPT { return l.get_task_id() >= r.get_task_id(); }
#endif

            // co_await a temporary task_t in GCC 10.1.0 will destroy task_t first, which may cause all resources unavailable
            // auto operator co_await() && LIBCOPP_MACRO_NOEXCEPT {
            //     struct awaitable_t : awaitable_base_t {
            //         using awaitable_base_t::awaitable_base_t;
            //         using awaitable_base_t::refer_task_;
            // 
            //         poll_type await_resume() {
            //             awaitable_base_t::await_resume();
            //             if (likely(refer_task_)) {
            //                 poll_type *ret = refer_task_->poll_data();
            //                 if (nullptr != ret) {
            //                     return std::move(*ret);
            //                 }
            //             }
            // 
            // 
            //             return poll_type{};
            //         }
            //     };
            // 
            //     return awaitable_t{this};
            // }

            auto operator co_await() & LIBCOPP_MACRO_NOEXCEPT {
                struct awaitable_t : awaitable_base_t {
                    using awaitable_base_t::awaitable_base_t;
                    using awaitable_base_t::refer_task_;

                    value_type *await_resume() {
                        awaitable_base_t::await_resume();
                        if (likely(refer_task_)) {
                            return refer_task_->data();
                        }

                        return nullptr;
                    }
                };

                return awaitable_t{this};
            }

            inline bool done() const LIBCOPP_MACRO_NOEXCEPT {
                if (likely(runtime_)) {
                    return runtime_->done();
                }

                return true;
            }

#if defined(LIBCOPP_MACRO_ENABLE_STD_EXCEPTION_PTR) && LIBCOPP_MACRO_ENABLE_STD_EXCEPTION_PTR
            inline const value_type *data() const {
#else
            inline const value_type *data() const LIBCOPP_MACRO_NOEXCEPT {
#endif
                if (likely(runtime_)) {
#if defined(LIBCOPP_MACRO_ENABLE_STD_EXCEPTION_PTR) && LIBCOPP_MACRO_ENABLE_STD_EXCEPTION_PTR
                    maybe_rethrow();
#endif
                    return runtime_->future.data();
                }

                return nullptr;
            }

#if defined(LIBCOPP_MACRO_ENABLE_STD_EXCEPTION_PTR) && LIBCOPP_MACRO_ENABLE_STD_EXCEPTION_PTR
            inline value_type *data() {
#else
            inline value_type *data() LIBCOPP_MACRO_NOEXCEPT {
#endif
                if (likely(runtime_)) {
#if defined(LIBCOPP_MACRO_ENABLE_STD_EXCEPTION_PTR) && LIBCOPP_MACRO_ENABLE_STD_EXCEPTION_PTR
                    maybe_rethrow();
#endif
                    return runtime_->future.data();
                }

                return nullptr;
            }

            inline const poll_type *poll_data() const LIBCOPP_MACRO_NOEXCEPT {
                if (likely(runtime_)) {
                    return &runtime_->future.poll_data();
                }

                return nullptr;
            }

            inline poll_type *poll_data() LIBCOPP_MACRO_NOEXCEPT {
                if (likely(runtime_)) {
                    return &runtime_->future.poll_data();
                }

                return nullptr;
            }

            inline status_type get_status() const LIBCOPP_MACRO_NOEXCEPT {
                if (runtime_) {
                    return runtime_->status;
                }

                return status_type::DONE;
            }

            inline context_type *get_context() LIBCOPP_MACRO_NOEXCEPT {
                if (done()) {
                    return nullptr;
                }

                return &runtime_->handle.promise().get_context();
            }

            inline const context_type *get_context() const LIBCOPP_MACRO_NOEXCEPT {
                if (done()) {
                    return nullptr;
                }

                return &runtime_->handle.promise().get_context();
            }

            inline uint64_t get_task_id() const LIBCOPP_MACRO_NOEXCEPT {
                if (likely(runtime_)) {
                    return runtime_->task_id;
                }

                const context_type * ctx = get_context();
                if (ctx) {
                    return ctx->get_task_id();
                }

                return 0; 
            }

            static typename promise_type::template pick_context_awaitable<context_type> current_context() { return promise_type::current_context(); }
            static typename promise_type::template pick_future_awaitable<future_type>   current_future() { return promise_type::current_future(); }

        private:
#if defined(LIBCOPP_MACRO_ENABLE_STD_EXCEPTION_PTR) && LIBCOPP_MACRO_ENABLE_STD_EXCEPTION_PTR
            inline void maybe_rethrow() {
                if (likely(runtime_)) {
                    if (unlikely(runtime_->unhandle_exception)) {
                        std::exception_ptr eptr;
                        std::swap(eptr, runtime_->unhandle_exception);
                        std::rethrow_exception(eptr);
                    }
                }
            }
#endif

        private:
            std::shared_ptr<runtime_type> runtime_;
        };

        template <typename T, class TPD, class TPTR, class TMACRO>
        task_t<T, TPD, TPTR, TMACRO> task_promise_t<T, TPD, TPTR, TMACRO>::get_return_object() LIBCOPP_MACRO_NOEXCEPT {
            // if (get_runtime() && !get_runtime()->handle) {
            //     get_runtime()->handle = LIBCOPP_MACRO_STD_COROUTINE_NAMESPACE
            //         coroutine_handle<typename task_t<T, TPD, TPTR, TMACRO>::promise_type>::from_promise(*this);
            // }
            return task_t<T, TPD, TPTR, TMACRO>{get_runtime()};
        }

        template <class TPD, class TPTR, class TMACRO>
        task_t<void, TPD, TPTR, TMACRO> task_promise_t<void, TPD, TPTR, TMACRO>::get_return_object() LIBCOPP_MACRO_NOEXCEPT {
            // if (get_runtime() && !get_runtime()->handle) {
            //     get_runtime()->handle = LIBCOPP_MACRO_STD_COROUTINE_NAMESPACE
            //     coroutine_handle<typename task_t<void, TPD, TPTR, TMACRO>::promise_type>::from_promise(*this);
            // }
            return task_t<void, TPD, TPTR, TMACRO>{get_runtime()};
        }
#endif
    } // namespace future
} // namespace copp


#endif

// Copyright 2022 owent

#pragma once

#include <libcopp/utils/config/libcopp_build_features.h>

// clang-format off
#include <libcopp/utils/config/stl_include_prefix.h>  // NOLINT(build/include_order)
// clang-format on
#include <assert.h>
#include <list>
#include <type_traits>
#include <vector>
// clang-format off
#include <libcopp/utils/config/stl_include_suffix.h>  // NOLINT(build/include_order)
// clang-format on

#include "libcopp/coroutine/std_coroutine_common.h"

#if defined(LIBCOPP_MACRO_ENABLE_STD_COROUTINE) && LIBCOPP_MACRO_ENABLE_STD_COROUTINE

LIBCOPP_COPP_NAMESPACE_BEGIN

template <class TVALUE>
class LIBCOPP_COPP_API_HEAD_ONLY callable_future;

template <class TFUTURE>
struct LIBCOPP_COPP_API_HEAD_ONLY some_ready {
  using reference_type = std::reference_wrapper<TFUTURE>;
  using type = std::vector<reference_type>;
};

template <class TFUTURE>
struct LIBCOPP_COPP_API_HEAD_ONLY any_ready {
  using reference_type = typename some_ready<TFUTURE>::reference_type;
  using type = typename some_ready<TFUTURE>::type;
};

template <class TFUTURE>
struct LIBCOPP_COPP_API_HEAD_ONLY all_ready {
  using reference_type = typename some_ready<TFUTURE>::reference_type;
  using type = typename some_ready<TFUTURE>::type;
};

template <class TELEMENT>
struct LIBCOPP_COPP_API_HEAD_ONLY remove_reference_wrapper;

template <class TELEMENT>
struct LIBCOPP_COPP_API_HEAD_ONLY remove_reference_wrapper<std::reference_wrapper<TELEMENT>> {
  using type = TELEMENT;
};

template <class TELEMENT>
struct LIBCOPP_COPP_API_HEAD_ONLY remove_reference_wrapper {
  using type = TELEMENT;
};

template <class TCONTAINER>
struct LIBCOPP_COPP_API_HEAD_ONLY some_ready_container {
  using container_type = typename std::decay<TCONTAINER>::type;
  using value_type = typename std::decay<typename container_type::value_type>::type;
};

template <class TCONTAINER>
struct LIBCOPP_COPP_API_HEAD_ONLY some_ready_reference_container {
  using reference_wrapper_type = typename some_ready_container<TCONTAINER>::value_type;
  using value_type = typename remove_reference_wrapper<typename reference_wrapper_type::type>::type;
};

template <class TELEMENT>
struct LIBCOPP_COPP_API_HEAD_ONLY pick_some_reference;

template <class TELEMENT>
struct LIBCOPP_COPP_API_HEAD_ONLY pick_some_reference<std::reference_wrapper<TELEMENT>> {
  inline static TELEMENT& unwrap(std::reference_wrapper<TELEMENT>& input) noexcept { return input.get(); }
  inline static TELEMENT& unwrap(const std::reference_wrapper<TELEMENT>& input) noexcept { return input.get(); }
};

template <class TELEMENT>
struct LIBCOPP_COPP_API_HEAD_ONLY pick_some_reference {
  inline static TELEMENT& unwrap(TELEMENT& input) noexcept { return input; }
};

// some delegate
template <class TFUTURE>
struct LIBCOPP_COPP_API_HEAD_ONLY some_delegate_context {
  using future_type = TFUTURE;
  using ready_output_type = typename some_ready<future_type>::type;

  std::list<future_type*> pending;
  ready_output_type ready;
  size_t ready_bound = 0;
  size_t scan_bound = 0;
  promise_status status = promise_status::kCreated;
  promise_caller_manager::handle_delegate caller_handle = promise_caller_manager::handle_delegate(nullptr);
};

template <class TFUTURE, class TDELEGATE_ACTION>
class LIBCOPP_COPP_API_HEAD_ONLY some_delegate_base {
 public:
  using future_type = TFUTURE;
  using value_type = future_type::value_type;
  using context_type = some_delegate_context<future_type>;
  using ready_output_type = typename context_type::ready_output_type;
  using delegate_action_type = TDELEGATE_ACTION;

 private:
  static void force_resume_all(context_type& context) {
    for (auto& pending_future : context.pending) {
      delegate_action_type::resume_future(context.caller_handle, *pending_future);
    }

    if (context.status < promise_status::kDone && nullptr != context.caller_handle.promise) {
      context.status = context.caller_handle.promise->get_status();
    }

    context.caller_handle = nullptr;
    if (context.status < promise_status::kDone) {
      context.status = promise_status::kKilled;
    }
  }

  static void scan_ready(context_type& context) {
    auto iter = context.pending.begin();

    while (iter != context.pending.end()) {
      if (delegate_action_type::is_pending(**iter)) {
        ++iter;
        continue;
      }
      future_type& future = **iter;
      context.ready.push_back(std::ref(future));
      iter = context.pending.erase(iter);

      delegate_action_type::resume_future(context.caller_handle, future);
    }
  }

 public:
  class awaitable_type : public awaitable_base_type {
   public:
    awaitable_type(context_type* context) : context_(context) {}

    inline bool await_ready() noexcept {
      if (nullptr == context_) {
        return true;
      }

      if (context_->status >= promise_status::kDone) {
        return true;
      }

      return context_->pending.empty();
    }

#  if defined(LIBCOPP_MACRO_ENABLE_CONCEPTS) && LIBCOPP_MACRO_ENABLE_CONCEPTS
    template <DerivedPromiseBaseType TCPROMISE>
#  else
    template <class TCPROMISE, typename = std::enable_if_t<std::is_base_of<promise_base_type, TCPROMISE>::value>>
#  endif
    inline void await_suspend(LIBCOPP_MACRO_STD_COROUTINE_NAMESPACE coroutine_handle<TCPROMISE> caller) noexcept {
      if (nullptr == context_ || caller.promise().get_status() >= promise_status::kDone) {
        // Already done and can not suspend again
        caller.resume();
        return;
      }

      set_caller(caller);

      // Allow kill resume to forward error information
      caller.promise().set_flag(promise_flag::kInternalWaitting, true);

      // set caller for all futures
      if (!context_->caller_handle) {
        context_->caller_handle = caller;
        // Copy pending here, the callback may call resume and will change the pending list
        std::list<future_type*> copy_pending = context_->pending;
        for (auto& pending_future : copy_pending) {
          delegate_action_type::suspend_future(context_->caller_handle, *pending_future);
        }
      }
    }

    void await_resume() {
      // caller maybe null if the callable is already ready when co_await
      auto caller = get_caller();
      if (caller) {
        if (nullptr != caller.promise) {
          caller.promise->set_flag(promise_flag::kInternalWaitting, false);
        }
        set_caller(nullptr);
      }

      if (nullptr == context_) {
        return;
      }

      ++context_->scan_bound;
      if (context_->scan_bound >= context_->ready_bound) {
        scan_ready(*context_);
        context_->scan_bound = context_->ready.size();

        if (context_->scan_bound >= context_->ready_bound && context_->status < promise_status::kDone) {
          context_->status = promise_status::kDone;
        }
      }
    }

   private:
    context_type* context_;
  };

 public:
  struct promise_type {
    context_type* context_;

    promise_type(context_type* context) : context_(context) {}
    promise_type(const promise_type&) = delete;
    promise_type(promise_type&&) = delete;
    promise_type& operator=(const promise_type&) = delete;
    promise_type& operator=(promise_type&&) = delete;
    ~promise_type() {
      COPP_LIKELY_IF (nullptr != context_ && !!context_->caller_handle) {
        force_resume_all(*context_);
      }
    }

    inline awaitable_type operator co_await() & { return awaitable_type{context_}; }
  };

  template <class TCONTAINER>
  static callable_future<promise_status> run(ready_output_type& ready_futures, size_t ready_count,
                                             TCONTAINER&& futures) {
    using container_type = typename std::decay<TCONTAINER>::type;
    context_type context;
    context.ready.reserve(futures.size());

    for (auto& future_object : futures) {
      auto& future_ref = pick_some_reference<typename container_type::value_type>::unwrap(future_object);
      if (delegate_action_type::is_pending(future_ref)) {
        context.pending.push_back(&future_ref);
      } else {
        context.ready.push_back(std::ref(future_ref));
      }
    }

    if (context.ready.size() >= ready_count) {
      context.ready.swap(ready_futures);
      co_return promise_status::kDone;
    }

    if (ready_count >= context.pending.size() + ready_futures.size()) {
      ready_count = context.pending.size() + ready_futures.size();
    }
    context.ready_bound = ready_count;
    context.scan_bound = context.ready.size();
    context.status = promise_status::kRunning;

    {
      promise_type some_promise{&context};
      while (context.status < promise_status::kDone) {
        // Killed by caller
        auto current_status = co_yield callable_future<promise_status>::yield_status();
        if (current_status >= promise_status::kDone) {
          context.status = current_status;
          break;
        }

        co_await some_promise;
      }

      // destroy promise object and detach handles
    }

    context.ready.swap(ready_futures);
    co_return context.status;
  }

 private:
  std::shared_ptr<context_type> context_;
};

LIBCOPP_COPP_NAMESPACE_END

#endif

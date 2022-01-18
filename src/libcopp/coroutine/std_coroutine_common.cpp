// Copyright 2022 owent

#include "libcopp/coroutine/std_coroutine_common.h"

#include <libcopp/utils/config/libcopp_build_features.h>

#include <assert.h>
#include <stdint.h>

#if defined(LIBCOPP_MACRO_ENABLE_STD_COROUTINE) && LIBCOPP_MACRO_ENABLE_STD_COROUTINE

LIBCOPP_COPP_NAMESPACE_BEGIN

LIBCOPP_COPP_API promise_base_type::pick_promise_status_awaitable::pick_promise_status_awaitable() noexcept
    : data(promise_status::kInvalid) {}

LIBCOPP_COPP_API promise_base_type::pick_promise_status_awaitable::pick_promise_status_awaitable(
    pick_promise_status_awaitable &&other) noexcept
    : data(other.data) {}

LIBCOPP_COPP_API promise_base_type::pick_promise_status_awaitable::~pick_promise_status_awaitable() {}

LIBCOPP_COPP_API promise_base_type::promise_base_type()
    : status_{static_cast<uint8_t>(promise_status::kCreated)}, current_waiting_{nullptr}, unique_caller_{nullptr} {}

LIBCOPP_COPP_API promise_base_type::~promise_base_type() {}

LIBCOPP_COPP_API bool promise_base_type::set_status(promise_status value, promise_status *expect) noexcept {
  if (nullptr == expect) {
    status_.store(static_cast<uint8_t>(value));
    return true;
  }

  uint8_t convert_value = static_cast<uint8_t>(value);
  uint8_t convert_except = static_cast<uint8_t>(*expect);

  bool result = status_.compare_exchange_strong(convert_except, convert_value);
  *expect = static_cast<promise_status>(convert_except);
  return result;
}

LIBCOPP_COPP_API promise_status promise_base_type::get_status() const noexcept {
  return static_cast<promise_status>(status_.load());
}

LIBCOPP_COPP_API void promise_base_type::set_waiting_handle(std::nullptr_t) noexcept { current_waiting_ = nullptr; }

LIBCOPP_COPP_API void promise_base_type::set_waiting_handle(handle_delegate handle) { current_waiting_ = handle; }

LIBCOPP_COPP_API void promise_base_type::resume_waiting(handle_delegate current_delegate, bool inherit_status) {
  // Atfer resume(), this object maybe destroyed.
  auto waiting_delegate = current_waiting_;
  if (waiting_delegate.handle && !waiting_delegate.handle.done()) {
    current_waiting_ = nullptr;
    // Prevent the waiting coroutine remuse this again.
    assert(waiting_delegate.promise);
    waiting_delegate.promise->remove_caller(current_delegate, inherit_status);
    waiting_delegate.handle.resume();
  }
}

LIBCOPP_COPP_API void promise_base_type::add_caller(handle_delegate delegate) noexcept {
  if (!delegate.handle || delegate.handle.done()) {
    return;
  }

  if (!unique_caller_.handle) {
    unique_caller_ = delegate;
    return;
  }

  if (!multiple_callers_) {
    multiple_callers_.reset(new std::unordered_set<handle_delegate, handle_delegate_hash>());
  }
  multiple_callers_->insert(delegate);
}

LIBCOPP_COPP_API void promise_base_type::remove_caller(handle_delegate delegate, bool inherit_status) noexcept {
  bool has_caller = false;
  do {
    if (unique_caller_.handle == delegate.handle) {
      unique_caller_ = nullptr;
      has_caller = true;
      break;
    }

    if (multiple_callers_) {
      has_caller = multiple_callers_->erase(delegate) > 0;
    }
  } while (false);

  if (has_caller && inherit_status && nullptr != delegate.promise && get_status() < promise_status::kDone &&
      delegate.promise->get_status() > promise_status::kDone) {
    set_status(delegate.promise->get_status());
  }
}

LIBCOPP_COPP_API promise_base_type::pick_promise_status_awaitable promise_base_type::yield_value(
    pick_promise_status_awaitable &&args) const noexcept {
  args.data = static_cast<promise_status>(status_.load());
  return args;
}

LIBCOPP_COPP_API void promise_base_type::resume_callers() {
  auto unique_caller = unique_caller_;
  unique_caller_ = nullptr;
  std::unique_ptr<std::unordered_set<handle_delegate, handle_delegate_hash>> multiple_callers;
  multiple_callers.swap(multiple_callers_);

  // The promise object may be destroyed after first caller.resume()
  if (unique_caller.handle && !unique_caller.handle.done()) {
    unique_caller.handle.resume();
  }

  if (multiple_callers) {
    for (auto &caller : *multiple_callers) {
      if (caller.handle && !caller.handle.done()) {
        caller.handle.resume();
      }
    }
  }
}

LIBCOPP_COPP_API awaitable_base_type::awaitable_base_type() : caller_{nullptr} {}
LIBCOPP_COPP_API awaitable_base_type::~awaitable_base_type() {}

LIBCOPP_COPP_API promise_base_type::handle_delegate awaitable_base_type::get_caller() const noexcept { return caller_; }

LIBCOPP_COPP_API void awaitable_base_type::set_caller(promise_base_type::handle_delegate caller) noexcept {
  caller_ = caller;
}

LIBCOPP_COPP_API void awaitable_base_type::set_caller(std::nullptr_t) noexcept { caller_ = nullptr; }

LIBCOPP_COPP_NAMESPACE_END

#endif

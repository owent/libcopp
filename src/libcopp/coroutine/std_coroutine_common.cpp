// Copyright 2023 owent

#include "libcopp/coroutine/std_coroutine_common.h"

#include <libcopp/utils/config/libcopp_build_features.h>

// clang-format off
#include <libcopp/utils/config/stl_include_prefix.h>  // NOLINT(build/include_order)
// clang-format on
#include <assert.h>
#include <stdint.h>
// clang-format off
#include <libcopp/utils/config/stl_include_suffix.h>  // NOLINT(build/include_order)
// clang-format on

#if defined(LIBCOPP_MACRO_ENABLE_STD_COROUTINE) && LIBCOPP_MACRO_ENABLE_STD_COROUTINE

LIBCOPP_COPP_NAMESPACE_BEGIN

LIBCOPP_COPP_API promise_caller_manager::promise_caller_manager()
    :
#  if defined(LIBCOPP_MACRO_ENABLE_STD_VARIANT) && LIBCOPP_MACRO_ENABLE_STD_VARIANT
      callers_(handle_delegate{nullptr})
#  else
      unique_caller_(nullptr)
#  endif
{
}

LIBCOPP_COPP_API promise_caller_manager::~promise_caller_manager() {}

LIBCOPP_COPP_API void promise_caller_manager::add_caller(handle_delegate delegate) noexcept {
  if (!delegate.handle || delegate.handle.done()) {
    return;
  }

#  if defined(LIBCOPP_MACRO_ENABLE_STD_VARIANT) && LIBCOPP_MACRO_ENABLE_STD_VARIANT
  if (std::holds_alternative<multi_caller_set>(callers_)) {
    std::get<multi_caller_set>(callers_).insert(delegate);
    return;
  }

  if (!std::get<handle_delegate>(callers_)) {
    std::get<handle_delegate>(callers_) = delegate;
    return;
  }

  // convert to multiple caller
  multi_caller_set callers;
  callers.insert(std::get<handle_delegate>(callers_));
  callers.insert(delegate);
  callers_.emplace<multi_caller_set>(std::move(callers));
#  else
  if (!unique_caller_.handle) {
    unique_caller_ = delegate;
    return;
  }

  if (!multiple_callers_) {
    multiple_callers_.reset(new multi_caller_set());
  }
  multiple_callers_->insert(delegate);
#  endif
}

LIBCOPP_COPP_API bool promise_caller_manager::remove_caller(handle_delegate delegate) noexcept {
  bool has_caller = false;
  do {
#  if defined(LIBCOPP_MACRO_ENABLE_STD_VARIANT) && LIBCOPP_MACRO_ENABLE_STD_VARIANT
    if (std::holds_alternative<multi_caller_set>(callers_)) {
      has_caller = std::get<multi_caller_set>(callers_).erase(delegate) > 0;
      break;
    }

    if (std::get<handle_delegate>(callers_).handle == delegate.handle) {
      std::get<handle_delegate>(callers_) = nullptr;
      has_caller = true;
    }
#  else
    if (unique_caller_.handle == delegate.handle) {
      unique_caller_ = nullptr;
      has_caller = true;
      break;
    }

    if (multiple_callers_) {
      has_caller = multiple_callers_->erase(delegate) > 0;
    }
#  endif
  } while (false);

  return has_caller;
}

LIBCOPP_COPP_API size_t promise_caller_manager::resume_callers() {
  size_t resume_count = 0;
#  if defined(LIBCOPP_MACRO_ENABLE_STD_VARIANT) && LIBCOPP_MACRO_ENABLE_STD_VARIANT
  if (std::holds_alternative<handle_delegate>(callers_)) {
    auto caller = std::get<handle_delegate>(callers_);
    std::get<handle_delegate>(callers_) = nullptr;
    if (caller.handle && !caller.handle.done() &&
        (nullptr == caller.promise || !caller.promise->check_flag(promise_flag::kDestroying))) {
      caller.handle.resume();
      ++resume_count;
    }
  } else if (std::holds_alternative<multi_caller_set>(callers_)) {
    multi_caller_set callers;
    callers.swap(std::get<multi_caller_set>(callers_));
    for (auto &caller : callers) {
      if (caller.handle && !caller.handle.done() &&
          (nullptr == caller.promise || !caller.promise->check_flag(promise_flag::kDestroying))) {
        type_erased_handle_type handle = caller.handle;
        handle.resume();
        ++resume_count;
      }
    }
  }
#  else
  auto unique_caller = unique_caller_;
  unique_caller_ = nullptr;
  std::unique_ptr<multi_caller_set> multiple_callers;
  multiple_callers.swap(multiple_callers_);

  // The promise object may be destroyed after first caller.resume()
  if (unique_caller.handle && !unique_caller.handle.done() &&
      (nullptr == unique_caller.promise || !unique_caller.promise->check_flag(promise_flag::kDestroying))) {
    unique_caller.handle.resume();
    ++resume_count;
  }

  if (multiple_callers) {
    for (auto &caller : *multiple_callers) {
      if (caller.handle && !caller.handle.done() &&
          (nullptr == caller.promise || !caller.promise->check_flag(promise_flag::kDestroying))) {
        type_erased_handle_type handle = caller.handle;
        handle.resume();
        ++resume_count;
      }
    }
  }
#  endif
  return resume_count;
}

LIBCOPP_COPP_API bool promise_caller_manager::has_multiple_callers() const noexcept {
#  if defined(LIBCOPP_MACRO_ENABLE_STD_VARIANT) && LIBCOPP_MACRO_ENABLE_STD_VARIANT
  if (std::holds_alternative<handle_delegate>(callers_)) {
    return false;
  } else if (std::holds_alternative<multi_caller_set>(callers_)) {
    return std::get<multi_caller_set>(callers_).size() > 1;
  }
  return false;
#  else
  size_t count = 0;
  if (unique_caller_.handle && !unique_caller_.handle.done() &&
      (nullptr == unique_caller_.promise || !unique_caller_.promise->check_flag(promise_flag::kDestroying))) {
    ++count;
  }

  if (multiple_callers_) {
    count += multiple_callers_->size();
  }
  return count > 1;
#  endif
}

LIBCOPP_COPP_API promise_base_type::pick_promise_status_awaitable::pick_promise_status_awaitable() noexcept
    : data(promise_status::kInvalid) {}

LIBCOPP_COPP_API promise_base_type::pick_promise_status_awaitable::pick_promise_status_awaitable(
    promise_status status) noexcept
    : data(status) {}

LIBCOPP_COPP_API promise_base_type::pick_promise_status_awaitable::pick_promise_status_awaitable(
    pick_promise_status_awaitable &&other) noexcept
    : data(other.data) {}

LIBCOPP_COPP_API promise_base_type::pick_promise_status_awaitable &
promise_base_type::pick_promise_status_awaitable::operator=(pick_promise_status_awaitable &&other) noexcept {
  data = other.data;
  return *this;
}

LIBCOPP_COPP_API promise_base_type::pick_promise_status_awaitable::~pick_promise_status_awaitable() {}

LIBCOPP_COPP_API promise_base_type::promise_base_type()
    : flags_(0), status_{promise_status::kCreated}, current_waiting_{nullptr} {}

LIBCOPP_COPP_API promise_base_type::~promise_base_type() {}

LIBCOPP_COPP_API bool promise_base_type::is_waiting() const noexcept {
  return current_waiting_ || check_flag(promise_flag::kInternalWaitting);
}

LIBCOPP_COPP_API void promise_base_type::set_waiting_handle(std::nullptr_t) noexcept { current_waiting_ = nullptr; }

LIBCOPP_COPP_API void promise_base_type::set_waiting_handle(handle_delegate handle) { current_waiting_ = handle; }

LIBCOPP_COPP_API void promise_base_type::resume_waiting(handle_delegate current_delegate, bool inherit_status) {
  // Atfer resume(), this object maybe destroyed.
  auto waiting_delegate = current_waiting_;

  // Resume the waiting promise.
  if (waiting_delegate.handle && !waiting_delegate.handle.done()) {
    current_waiting_ = nullptr;
    // Prevent the waiting coroutine remuse this again.
    if (nullptr != waiting_delegate.promise) {
      waiting_delegate.promise->remove_caller(current_delegate, inherit_status);
    }
    waiting_delegate.handle.resume();
  } else if (current_delegate.handle && !current_delegate.handle.done() &&
             check_flag(promise_flag::kInternalWaitting)) {
    // If we are waiting for a internal awaitable object, we also allow to resume it.
    current_delegate.handle.resume();
  }
}

LIBCOPP_COPP_API promise_base_type::pick_promise_status_awaitable promise_base_type::yield_value(
    pick_promise_status_awaitable &&args) const noexcept {
  args.data = get_status();
  return pick_promise_status_awaitable{args.data};
}

LIBCOPP_COPP_API void promise_base_type::add_caller(handle_delegate delegate) noexcept {
  caller_manager_.add_caller(delegate);
}

LIBCOPP_COPP_API void promise_base_type::remove_caller(handle_delegate delegate, bool inherit_status) noexcept {
  bool remove_caller_success = caller_manager_.remove_caller(delegate);
  if (remove_caller_success && inherit_status && nullptr != delegate.promise && get_status() < promise_status::kDone &&
      delegate.promise->get_status() > promise_status::kDone) {
    set_status(delegate.promise->get_status());
  }
}

LIBCOPP_COPP_API void promise_base_type::resume_callers() { caller_manager_.resume_callers(); }

LIBCOPP_COPP_API awaitable_base_type::awaitable_base_type() : caller_{nullptr} {}
LIBCOPP_COPP_API awaitable_base_type::~awaitable_base_type() {}

LIBCOPP_COPP_API promise_base_type::handle_delegate awaitable_base_type::get_caller() const noexcept { return caller_; }

LIBCOPP_COPP_API void awaitable_base_type::set_caller(promise_base_type::handle_delegate caller) noexcept {
  caller_ = caller;
}

LIBCOPP_COPP_API void awaitable_base_type::set_caller(std::nullptr_t) noexcept { caller_ = nullptr; }

LIBCOPP_COPP_NAMESPACE_END

#endif

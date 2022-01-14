// Copyright 2022 owent

#include "libcopp/coroutine/std_coroutine_common.h"

#include <libcopp/utils/config/libcopp_build_features.h>

#include <stdint.h>

#if defined(LIBCOPP_MACRO_ENABLE_STD_COROUTINE) && LIBCOPP_MACRO_ENABLE_STD_COROUTINE

LIBCOPP_COPP_NAMESPACE_BEGIN

LIBCOPP_COPP_API promise_base_type::promise_base_type()
    : status_{static_cast<uint8_t>(promise_status::kCreated)}, current_waiting_{nullptr} {}

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

LIBCOPP_COPP_API LIBCOPP_MACRO_STD_COROUTINE_NAMESPACE coroutine_handle<promise_base_type>
promise_base_type::get_waiting_handle() const noexcept {
  return LIBCOPP_MACRO_STD_COROUTINE_NAMESPACE coroutine_handle<promise_base_type>::from_address(
      current_waiting_.address());
}

LIBCOPP_COPP_API void promise_base_type::set_waiting_handle(std::nullptr_t) noexcept { current_waiting_ = nullptr; }

LIBCOPP_COPP_API void promise_base_type::set_waiting_handle(
    LIBCOPP_MACRO_STD_COROUTINE_NAMESPACE coroutine_handle<> handle) {
  current_waiting_ = handle;
}

LIBCOPP_COPP_API void promise_base_type::resume_waiting() {
  while (current_waiting_ && !current_waiting_.done()) {
    current_waiting_.resume();
  }
  current_waiting_ = nullptr;
}

LIBCOPP_COPP_API awaitable_base_type::awaitable_base_type() : caller_{nullptr} {}
LIBCOPP_COPP_API awaitable_base_type::~awaitable_base_type() {}

LIBCOPP_COPP_API LIBCOPP_MACRO_STD_COROUTINE_NAMESPACE coroutine_handle<promise_base_type>
awaitable_base_type::get_caller() const noexcept {
  return caller_;
}

LIBCOPP_COPP_API void awaitable_base_type::set_caller(
    const LIBCOPP_MACRO_STD_COROUTINE_NAMESPACE coroutine_handle<promise_base_type> &handle) noexcept {
  caller_ = handle;
}

LIBCOPP_COPP_NAMESPACE_END

#endif

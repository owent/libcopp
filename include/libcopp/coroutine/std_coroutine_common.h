// Copyright 2022 owent

#pragma once

#include <libcopp/utils/config/libcopp_build_features.h>
#include <libcopp/utils/std/coroutine.h>

#include <assert.h>
#include <type_traits>

#if defined(LIBCOPP_MACRO_ENABLE_STD_EXCEPTION_PTR) && LIBCOPP_MACRO_ENABLE_STD_EXCEPTION_PTR
#  include <exception>
#endif

#include "libcopp/future/future.h"
#include "libcopp/utils/atomic_int_type.h"

#if defined(LIBCOPP_MACRO_ENABLE_STD_COROUTINE) && LIBCOPP_MACRO_ENABLE_STD_COROUTINE

LIBCOPP_COPP_NAMESPACE_BEGIN

enum class LIBCOPP_COPP_API_HEAD_ONLY promise_status : uint8_t {
  kCreated = 0,
  kRunning = 1,
  kDone = 2,
  kCancle = 3,
  kKilled = 4,
  kTimeout = 5,
};

class promise_base_type;

#  if defined(LIBCOPP_MACRO_ENABLE_CONCEPTS) && LIBCOPP_MACRO_ENABLE_CONCEPTS
template <class T>
concept DerivedPromiseBaseType = std::is_base_of<promise_base_type, T>::value;
#  endif

class promise_base_type {
 public:
  LIBCOPP_COPP_API promise_base_type();
  LIBCOPP_COPP_API ~promise_base_type();

  LIBCOPP_COPP_API bool set_status(promise_status value, promise_status *expect = nullptr) noexcept;
  LIBCOPP_COPP_API promise_status get_status() const noexcept;

  LIBCOPP_COPP_API LIBCOPP_MACRO_STD_COROUTINE_NAMESPACE coroutine_handle<promise_base_type> get_waiting_handle()
      const noexcept;

  LIBCOPP_COPP_API void set_waiting_handle(std::nullptr_t) noexcept;
#  if defined(LIBCOPP_MACRO_ENABLE_CONCEPTS) && LIBCOPP_MACRO_ENABLE_CONCEPTS
  template <DerivedPromiseBaseType TPROMISE>
#  else
  template <class TPROMISE, typename = std::enable_if_t<std::is_base_of<promise_base_type, TPROMISE>::value>>
#  endif
  LIBCOPP_COPP_API_HEAD_ONLY void set_waiting_handle(
      const LIBCOPP_MACRO_STD_COROUTINE_NAMESPACE coroutine_handle<TPROMISE> &handle) noexcept {
    if (nullptr == handle) {
      set_waiting_handle(nullptr);
    } else {
      set_waiting_handle(LIBCOPP_MACRO_STD_COROUTINE_NAMESPACE coroutine_handle<>::from_address(handle.address()));
    }
  }

  LIBCOPP_COPP_API void resume_waiting();

 private:
  LIBCOPP_COPP_API void set_waiting_handle(LIBCOPP_MACRO_STD_COROUTINE_NAMESPACE coroutine_handle<> handle);

 private:
  // promise_status
  util::lock::atomic_int_type<uint8_t> status_;
  // We must erase type here, because MSVC use is_empty_v<coroutine_handle<...>>, which need to calculate the type size
  LIBCOPP_MACRO_STD_COROUTINE_NAMESPACE coroutine_handle<> current_waiting_;
};

class awaitable_base_type {
 public:
  LIBCOPP_COPP_API awaitable_base_type();
  LIBCOPP_COPP_API ~awaitable_base_type();

  LIBCOPP_COPP_API LIBCOPP_MACRO_STD_COROUTINE_NAMESPACE coroutine_handle<promise_base_type> get_caller()
      const noexcept;

  LIBCOPP_COPP_API void set_caller(
      const LIBCOPP_MACRO_STD_COROUTINE_NAMESPACE coroutine_handle<promise_base_type> &handle) noexcept;

#  if defined(LIBCOPP_MACRO_ENABLE_CONCEPTS) && LIBCOPP_MACRO_ENABLE_CONCEPTS
  template <DerivedPromiseBaseType TPROMISE>
#  else
  template <class TPROMISE, typename = std::enable_if_t<std::is_base_of<promise_base_type, TPROMISE>::value>>
#  endif
  LIBCOPP_COPP_API_HEAD_ONLY void set_caller(
      const LIBCOPP_MACRO_STD_COROUTINE_NAMESPACE coroutine_handle<TPROMISE> &handle) noexcept {
    if (nullptr == handle) {
      set_caller(nullptr);
    } else {
      set_caller(
          LIBCOPP_MACRO_STD_COROUTINE_NAMESPACE coroutine_handle<promise_base_type>::from_promise(handle.promise()));
    }
  }

 private:
  LIBCOPP_MACRO_STD_COROUTINE_NAMESPACE coroutine_handle<promise_base_type> caller_;
};

template <class TDATA>
struct LIBCOPP_COPP_API_HEAD_ONLY std_coroutine_default_error_transform;

template <>
struct LIBCOPP_COPP_API_HEAD_ONLY std_coroutine_default_error_transform<void> {
  using type = void;
};

template <class TDATA>
struct LIBCOPP_COPP_API_HEAD_ONLY std_coroutine_default_error_transform {
  using type = TDATA;
  type operator()(promise_status in) const { return type{in}; }
};

template <class TDATA>
struct LIBCOPP_COPP_API_HEAD_ONLY std_coroutine_integer_error_transform {
  using type = TDATA;
  type operator()(promise_status in) const noexcept {
    if (in <= promise_status::kCreated) {
      return static_cast<type>(-1);
    }
    return static_cast<type>(-static_cast<int8_t>(in));
  }
};

template <class TVALUE>
struct LIBCOPP_COPP_API_HEAD_ONLY promise_error_transform
    : public std::conditional<std::is_integral<TVALUE>::value, std_coroutine_integer_error_transform<TVALUE>,
                              std_coroutine_default_error_transform<TVALUE>>::type {
  using type = TVALUE;
};

LIBCOPP_COPP_NAMESPACE_END

#endif

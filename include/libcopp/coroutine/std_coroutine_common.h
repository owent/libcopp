// Copyright 2022 owent

#pragma once

#include <libcopp/utils/config/compile_optimize.h>
#include <libcopp/utils/config/libcopp_build_features.h>
#include <libcopp/utils/std/coroutine.h>

// clang-format off
#include <libcopp/utils/config/stl_include_prefix.h>  // NOLINT(build/include_order)
// clang-format on
#include <assert.h>
#include <bitset>
#include <cstddef>
#include <memory>
#include <type_traits>
#include <unordered_set>

#if defined(LIBCOPP_MACRO_ENABLE_STD_EXCEPTION_PTR) && LIBCOPP_MACRO_ENABLE_STD_EXCEPTION_PTR
#  include <exception>
#  include <list>
#endif

#ifdef __cpp_impl_three_way_comparison
#  include <compare>
#endif

#if defined(__cpp_lib_variant) && __cpp_lib_variant >= 201606L
#  include <variant>
#endif
// clang-format off
#include <libcopp/utils/config/stl_include_suffix.h>  // NOLINT(build/include_order)
// clang-format on

#include "libcopp/future/future.h"
#include "libcopp/utils/atomic_int_type.h"

#if defined(LIBCOPP_MACRO_ENABLE_STD_COROUTINE) && LIBCOPP_MACRO_ENABLE_STD_COROUTINE

LIBCOPP_COPP_NAMESPACE_BEGIN

enum class LIBCOPP_COPP_API_HEAD_ONLY promise_status : uint8_t {
  kInvalid = 0,
  kCreated = 1,
  kRunning = 2,
  kDone = 3,
  kCancle = 4,
  kKilled = 5,
  kTimeout = 6,
};

enum class LIBCOPP_COPP_API_HEAD_ONLY promise_flag : uint8_t {
  kDestroying = 0,
  kFinalSuspend = 1,
  kInternalWaitting = 2,
  kMax,
};

template <class TVALUE, bool ALLOW_MOVE>
struct LIBCOPP_COPP_API_HEAD_ONLY _multiple_callers_constructor;

template <class TVALUE>
struct LIBCOPP_COPP_API_HEAD_ONLY _multiple_callers_constructor<TVALUE, true> {
  UTIL_FORCEINLINE static TVALUE &&return_value(TVALUE &input) noexcept { return std::move(input); }
};

template <class TVALUE>
struct LIBCOPP_COPP_API_HEAD_ONLY _multiple_callers_constructor<TVALUE, false> {
  UTIL_FORCEINLINE static const TVALUE &return_value(TVALUE &input) noexcept { return input; }
};

template <class TVALUE>
struct LIBCOPP_COPP_API_HEAD_ONLY multiple_callers_constructor
    : public _multiple_callers_constructor<
          TVALUE, !(std::is_pointer<TVALUE>::value || std::is_reference<TVALUE>::value ||
                    !std::is_move_constructible<TVALUE>::value ||
                    (std::is_trivially_copyable<TVALUE>::value && sizeof(TVALUE) <= sizeof(std::max_align_t)))> {};

class promise_base_type;

#  if defined(LIBCOPP_MACRO_ENABLE_CONCEPTS) && LIBCOPP_MACRO_ENABLE_CONCEPTS
template <class T>
concept DerivedPromiseBaseType = std::is_base_of<promise_base_type, T>::value;
#  endif

class promise_caller_manager {
 private:
  promise_caller_manager(const promise_caller_manager &) = delete;
  promise_caller_manager(promise_caller_manager &&) = delete;
  promise_caller_manager &operator=(const promise_caller_manager &) = delete;
  promise_caller_manager &operator=(promise_caller_manager &&) = delete;

 public:
  using type_erased_handle_type = LIBCOPP_MACRO_STD_COROUTINE_NAMESPACE coroutine_handle<>;
  struct LIBCOPP_COPP_API_HEAD_ONLY handle_delegate {
    type_erased_handle_type handle;
    promise_base_type *promise;

#  if defined(LIBCOPP_MACRO_ENABLE_CONCEPTS) && LIBCOPP_MACRO_ENABLE_CONCEPTS
    template <DerivedPromiseBaseType TPROMISE>
#  else
    template <class TPROMISE, typename = std::enable_if_t<std::is_base_of<promise_base_type, TPROMISE>::value>>
#  endif
    explicit handle_delegate(
        const LIBCOPP_MACRO_STD_COROUTINE_NAMESPACE coroutine_handle<TPROMISE> &origin_handle) noexcept
        : handle{origin_handle} {
      if (handle) {
        promise = &origin_handle.promise();
      } else {
        promise = nullptr;
      }
    }

    explicit handle_delegate(std::nullptr_t) noexcept : handle{nullptr}, promise{nullptr} {}

    friend inline bool operator==(const handle_delegate &l, const handle_delegate &r) noexcept {
      return l.handle == r.handle;
    }
    friend inline bool operator!=(const handle_delegate &l, const handle_delegate &r) noexcept {
      return l.handle != r.handle;
    }
    friend inline bool operator<(const handle_delegate &l, const handle_delegate &r) noexcept {
      return l.handle < r.handle;
    }
    friend inline bool operator<=(const handle_delegate &l, const handle_delegate &r) noexcept {
      return l.handle <= r.handle;
    }
    friend inline bool operator>(const handle_delegate &l, const handle_delegate &r) noexcept {
      return l.handle > r.handle;
    }
    friend inline bool operator>=(const handle_delegate &l, const handle_delegate &r) noexcept {
      return l.handle >= r.handle;
    }
    inline operator bool() const noexcept { return !!handle; }

#  if defined(LIBCOPP_MACRO_ENABLE_CONCEPTS) && LIBCOPP_MACRO_ENABLE_CONCEPTS
    template <DerivedPromiseBaseType TPROMISE>
#  else
    template <class TPROMISE, typename = std::enable_if_t<std::is_base_of<promise_base_type, TPROMISE>::value>>
#  endif
    inline handle_delegate &operator=(
        const LIBCOPP_MACRO_STD_COROUTINE_NAMESPACE coroutine_handle<TPROMISE> &origin_handle) noexcept {
      handle = origin_handle;
      if (handle) {
        promise = &origin_handle.promise();
      } else {
        promise = nullptr;
      }

      return *this;
    }
    inline handle_delegate &operator=(std::nullptr_t) noexcept {
      handle = nullptr;
      promise = nullptr;
      return *this;
    }
  };

  LIBCOPP_COPP_API promise_caller_manager();
  LIBCOPP_COPP_API ~promise_caller_manager();

  LIBCOPP_COPP_API void add_caller(handle_delegate handle) noexcept;

  /**
   * @brief Remove caller handle delegate
   *
   * @param handle the handle to remove
   * @return true when handle is removed, false if it's not found
   */
  LIBCOPP_COPP_API bool remove_caller(handle_delegate handle) noexcept;

  LIBCOPP_COPP_API size_t resume_callers();

  LIBCOPP_COPP_API bool has_multiple_callers() const noexcept;

 private:
  // hash for handle_delegate
  struct LIBCOPP_COPP_API_HEAD_ONLY handle_delegate_hash {
    inline size_t operator()(const handle_delegate &handle_delegate) const noexcept {
      return std::hash<void *>()(handle_delegate.handle.address());
    }
  };

  using multi_caller_set = std::unordered_set<handle_delegate, handle_delegate_hash>;
#  if defined(__cpp_lib_variant) && __cpp_lib_variant >= 201606L
  std::variant<handle_delegate, multi_caller_set> callers_;
#  else
  handle_delegate unique_caller_;
  // Mostly, there is only one caller for a promise, we needn't hash map to store one handle
  std::unique_ptr<multi_caller_set> multiple_callers_;
#  endif
};

class promise_base_type {
 public:
  using handle_type = LIBCOPP_MACRO_STD_COROUTINE_NAMESPACE coroutine_handle<promise_base_type>;
  using type_erased_handle_type = promise_caller_manager::type_erased_handle_type;
  using handle_delegate = promise_caller_manager::handle_delegate;

  struct pick_promise_status_awaitable {
    promise_status data;

    LIBCOPP_COPP_API pick_promise_status_awaitable() noexcept;
    LIBCOPP_COPP_API pick_promise_status_awaitable(promise_status status) noexcept;
    LIBCOPP_COPP_API pick_promise_status_awaitable(pick_promise_status_awaitable &&other) noexcept;
    LIBCOPP_COPP_API pick_promise_status_awaitable(const pick_promise_status_awaitable &) = delete;
    LIBCOPP_COPP_API pick_promise_status_awaitable &operator=(pick_promise_status_awaitable &&) noexcept;
    LIBCOPP_COPP_API pick_promise_status_awaitable &operator=(const pick_promise_status_awaitable &) = delete;
    LIBCOPP_COPP_API ~pick_promise_status_awaitable();

    LIBCOPP_COPP_API_HEAD_ONLY inline bool await_ready() const noexcept { return true; }
    LIBCOPP_COPP_API_HEAD_ONLY inline promise_status await_resume() const noexcept { return data; }
    LIBCOPP_COPP_API_HEAD_ONLY inline void await_suspend(type_erased_handle_type) noexcept {}
  };

 public:
  LIBCOPP_COPP_API promise_base_type();
  LIBCOPP_COPP_API ~promise_base_type();

  LIBCOPP_COPP_API_HEAD_ONLY inline bool set_status(promise_status value, promise_status *expect = nullptr) noexcept {
    if (nullptr == expect) {
      status_ = value;
      return true;
    }
    if (status_ == *expect) {
      status_ = value;
      return true;
    } else {
      *expect = status_;
      return false;
    }
  }

  UTIL_FORCEINLINE LIBCOPP_COPP_API_HEAD_ONLY promise_status get_status() const noexcept { return status_; }

  LIBCOPP_COPP_API_HEAD_ONLY inline bool check_flag(promise_flag flag) const noexcept {
    if (flag >= promise_flag::kMax) {
      return false;
    }

    return flags_.test(static_cast<size_t>(flag));
  }

  LIBCOPP_COPP_API_HEAD_ONLY inline void set_flag(promise_flag flag, bool value) noexcept {
    if (flag >= promise_flag::kMax) {
      return;
    }
    flags_.set(static_cast<size_t>(flag), value);
  }

  LIBCOPP_COPP_API bool is_waiting() const noexcept;
  LIBCOPP_COPP_API void set_waiting_handle(std::nullptr_t) noexcept;
  LIBCOPP_COPP_API void set_waiting_handle(handle_delegate handle);
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
      set_waiting_handle(handle_delegate{handle});
    }
  }

  /**
   * @brief Resume waiting handle, this should only be called in await_resume and after this call, callee maybe
   * destroyed
   */
#  if defined(LIBCOPP_MACRO_ENABLE_CONCEPTS) && LIBCOPP_MACRO_ENABLE_CONCEPTS
  template <DerivedPromiseBaseType TPROMISE>
#  else
  template <class TPROMISE, typename = std::enable_if_t<std::is_base_of<promise_base_type, TPROMISE>::value>>
#  endif
  LIBCOPP_COPP_API_HEAD_ONLY inline void resume_waiting(
      const LIBCOPP_MACRO_STD_COROUTINE_NAMESPACE coroutine_handle<TPROMISE> &handle, bool inherit_status) {
    resume_waiting(handle_delegate{handle}, inherit_status);
  };

  LIBCOPP_COPP_API void resume_waiting(handle_delegate current_delegate, bool inherit_status);

  // C++20 coroutine
  struct LIBCOPP_COPP_API_HEAD_ONLY final_awaitable {
    inline bool await_ready() const noexcept { return false; }
    inline void await_resume() const noexcept {}

#  if defined(LIBCOPP_MACRO_ENABLE_CONCEPTS) && LIBCOPP_MACRO_ENABLE_CONCEPTS
    template <DerivedPromiseBaseType TPROMISE>
#  else
    template <class TPROMISE, typename = std::enable_if_t<std::is_base_of<promise_base_type, TPROMISE>::value>>
#  endif
    inline void await_suspend(LIBCOPP_MACRO_STD_COROUTINE_NAMESPACE coroutine_handle<TPROMISE> self) noexcept {
      self.promise().set_flag(promise_flag::kFinalSuspend, true);
      self.promise().resume_callers();
    }
  };
  final_awaitable final_suspend() noexcept { return {}; }

  LIBCOPP_COPP_API void add_caller(handle_delegate handle) noexcept;
#  if defined(LIBCOPP_MACRO_ENABLE_CONCEPTS) && LIBCOPP_MACRO_ENABLE_CONCEPTS
  template <DerivedPromiseBaseType TPROMISE>
#  else
  template <class TPROMISE, typename = std::enable_if_t<std::is_base_of<promise_base_type, TPROMISE>::value>>
#  endif
  LIBCOPP_COPP_API_HEAD_ONLY void add_caller(
      const LIBCOPP_MACRO_STD_COROUTINE_NAMESPACE coroutine_handle<TPROMISE> &handle) noexcept {
    add_caller(handle_delegate{handle});
  }

  LIBCOPP_COPP_API void remove_caller(handle_delegate handle, bool inherit_status) noexcept;
#  if defined(LIBCOPP_MACRO_ENABLE_CONCEPTS) && LIBCOPP_MACRO_ENABLE_CONCEPTS
  template <DerivedPromiseBaseType TPROMISE>
#  else
  template <class TPROMISE, typename = std::enable_if_t<std::is_base_of<promise_base_type, TPROMISE>::value>>
#  endif
  LIBCOPP_COPP_API_HEAD_ONLY void remove_caller(
      const LIBCOPP_MACRO_STD_COROUTINE_NAMESPACE coroutine_handle<TPROMISE> &handle, bool inherit_status) noexcept {
    remove_caller(handle_delegate{handle}, inherit_status);
  }

  LIBCOPP_COPP_API pick_promise_status_awaitable yield_value(pick_promise_status_awaitable &&args) const noexcept;
  static LIBCOPP_COPP_API_HEAD_ONLY inline pick_promise_status_awaitable pick_current_status() noexcept { return {}; }

 private:
  LIBCOPP_COPP_API void resume_callers();

 private:
  // promise_flags
  std::bitset<static_cast<size_t>(promise_flag::kMax)> flags_;

  // promise_status
  promise_status status_;

  // We must erase type here, because MSVC use is_empty_v<coroutine_handle<...>>, which need to calculate the type size
  handle_delegate current_waiting_;

  // caller manager
  promise_caller_manager caller_manager_;
};

class awaitable_base_type {
 public:
  LIBCOPP_COPP_API awaitable_base_type();
  LIBCOPP_COPP_API ~awaitable_base_type();

  LIBCOPP_COPP_API promise_base_type::handle_delegate get_caller() const noexcept;

  LIBCOPP_COPP_API void set_caller(promise_base_type::handle_delegate caller) noexcept;
  LIBCOPP_COPP_API void set_caller(std::nullptr_t) noexcept;

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
      set_caller(promise_base_type::handle_delegate{handle});
    }
  }

 private:
  promise_base_type::handle_delegate caller_;
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

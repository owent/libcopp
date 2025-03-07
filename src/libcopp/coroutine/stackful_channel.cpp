// Copyright 2025 owent

#include "libcopp/coroutine/stackful_channel.h"

#include <libcopp/utils/config/libcopp_build_features.h>

// clang-format off
#include <libcopp/utils/config/stl_include_prefix.h>  // NOLINT(build/include_order)
// clang-format on

#include <assert.h>
#include <stdint.h>

// clang-format off
#include <libcopp/utils/config/stl_include_suffix.h>  // NOLINT(build/include_order)
// clang-format on

#include "libcopp/coroutine/coroutine_context.h"
#include "libcopp/coroutine/coroutine_context_fiber.h"

LIBCOPP_COPP_NAMESPACE_BEGIN

LIBCOPP_COPP_API int stackful_channel_resume_handle<coroutine_context>::resume(
    void *invoke_ctx, stackful_channel_context_base *priv_data) {
  return stackful_channel_resume_invoker<coroutine_context>::resume(
      reinterpret_cast<coroutine_context_base *>(invoke_ctx), priv_data);
}

#if defined(LIBCOPP_MACRO_ENABLE_WIN_FIBER) && LIBCOPP_MACRO_ENABLE_WIN_FIBER
LIBCOPP_COPP_API int stackful_channel_resume_handle<coroutine_context_fiber>::resume(
    void *invoke_ctx, stackful_channel_context_base *priv_data) {
  return stackful_channel_resume_invoker<coroutine_context_fiber>::resume(
      reinterpret_cast<coroutine_context_base *>(invoke_ctx), priv_data);
}
#endif

LIBCOPP_COPP_API stackful_channel_context_base::stackful_channel_context_base() noexcept {}

LIBCOPP_COPP_API stackful_channel_context_base::~stackful_channel_context_base() { wake(); }

LIBCOPP_COPP_API void stackful_channel_context_base::add_caller(handle_delegate handle) noexcept {
  if (!handle) {
    return;
  }

#if defined(LIBCOPP_MACRO_ENABLE_STD_VARIANT) && LIBCOPP_MACRO_ENABLE_STD_VARIANT
  if (std::holds_alternative<multi_caller_set>(callers_)) {
    std::get<multi_caller_set>(callers_).insert(handle);
    return;
  }

  if (!std::get<handle_delegate>(callers_)) {
    std::get<handle_delegate>(callers_) = handle;
    return;
  }

  // convert to multiple caller
  multi_caller_set callers;
  callers.insert(std::get<handle_delegate>(callers_));
  callers.insert(handle);
  callers_.emplace<multi_caller_set>(std::move(callers));
#else
  if (!unique_caller_) {
    unique_caller_ = handle;
    return;
  }

  if (!multiple_callers_) {
    multiple_callers_.reset(new multi_caller_set());
  }
  multiple_callers_->insert(handle);
#endif
}

LIBCOPP_COPP_API bool stackful_channel_context_base::remove_caller(handle_delegate handle) noexcept {
  bool has_caller = false;
  do {
#if defined(LIBCOPP_MACRO_ENABLE_STD_VARIANT) && LIBCOPP_MACRO_ENABLE_STD_VARIANT
    if (std::holds_alternative<multi_caller_set>(callers_)) {
      has_caller = std::get<multi_caller_set>(callers_).erase(handle) > 0;
      break;
    }

    if (std::get<handle_delegate>(callers_) == handle) {
      std::get<handle_delegate>(callers_) = nullptr;
      has_caller = true;
    }
#else
    if (unique_caller_ == handle) {
      unique_caller_ = nullptr;
      has_caller = true;
      break;
    }

    if (multiple_callers_) {
      has_caller = multiple_callers_->erase(handle) > 0;
    }
#endif
  } while (false);

  return has_caller;
}

LIBCOPP_COPP_API size_t stackful_channel_context_base::resume_callers() {
  size_t resume_count = 0;
#if defined(LIBCOPP_MACRO_ENABLE_STD_VARIANT) && LIBCOPP_MACRO_ENABLE_STD_VARIANT
  if (std::holds_alternative<handle_delegate>(callers_)) {
    auto caller = std::get<handle_delegate>(callers_);
    std::get<handle_delegate>(callers_) = nullptr;
    if (caller.handle_data && caller.resume_handle) {
      caller.resume_handle(caller.handle_data, this);
      ++resume_count;
    }
  } else if (std::holds_alternative<multi_caller_set>(callers_)) {
    multi_caller_set callers;
    callers.swap(std::get<multi_caller_set>(callers_));
    for (auto &caller : callers) {
      if (caller.handle_data && caller.resume_handle) {
        caller.resume_handle(caller.handle_data, this);
        ++resume_count;
      }
    }
  }
#else
  auto unique_caller = unique_caller_;
  unique_caller_ = nullptr;
  std::unique_ptr<multi_caller_set> multiple_callers;
  multiple_callers.swap(multiple_callers_);

  // The promise object may be destroyed after first caller.resume()
  if (unique_caller.handle_data && unique_caller.resume_handle) {
    unique_caller.resume_handle(unique_caller.handle_data, this);
    ++resume_count;
  }

  if (multiple_callers) {
    for (auto &caller : *multiple_callers) {
      if (caller.handle_data && caller.resume_handle) {
        caller.resume_handle(caller.handle_data, this);
        ++resume_count;
      }
    }
  }
#endif
  return resume_count;
}

LIBCOPP_COPP_API bool stackful_channel_context_base::has_multiple_callers() const noexcept {
#if defined(LIBCOPP_MACRO_ENABLE_STD_VARIANT) && LIBCOPP_MACRO_ENABLE_STD_VARIANT
  if (std::holds_alternative<handle_delegate>(callers_)) {
    return false;
  } else if (std::holds_alternative<multi_caller_set>(callers_)) {
    return std::get<multi_caller_set>(callers_).size() > 1;
  }
  return false;
#else
  size_t count = 0;
  if (unique_caller_.handle_data && unique_caller_.resume_handle) {
    ++count;
  }

  if (multiple_callers_) {
    count += multiple_callers_->size();
  }
  return count > 1;
#endif
}

LIBCOPP_COPP_API void stackful_channel_context_base::wake() {}

LIBCOPP_COPP_NAMESPACE_END

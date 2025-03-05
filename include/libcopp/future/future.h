// Copyright 2023 owent

#pragma once

#include <libcopp/utils/config/libcopp_build_features.h>

// clang-format off
#include <libcopp/utils/config/stl_include_prefix.h>  // NOLINT(build/include_order)
// clang-format on
#include <list>
// clang-format off
#include <libcopp/utils/config/stl_include_suffix.h>  // NOLINT(build/include_order)
// clang-format on

#include "libcopp/future/poller.h"

LIBCOPP_COPP_NAMESPACE_BEGIN
namespace future {

template <class T, class TPTR = typename poll_storage_ptr_selector<T>::type>
class LIBCOPP_COPP_API_HEAD_ONLY future {
 public:
  using self_type = future<T, TPTR>;
  using poller_type = poller<T, TPTR>;
  using storage_type = typename poller_type::storage_type;
  using value_type = typename poller_type::value_type;
  using ptr_type = typename poller_type::ptr_type;

 public:
  future() noexcept(std::is_nothrow_constructible<poller_type>::value) {}
  ~future() {}

  LIBCOPP_UTIL_FORCEINLINE bool is_ready() const noexcept { return poll_data_.is_ready(); }

  LIBCOPP_UTIL_FORCEINLINE bool is_pending() const noexcept { return poll_data_.is_pending(); }

  LIBCOPP_UTIL_FORCEINLINE const value_type *data() const noexcept {
    if (!is_ready()) {
      return nullptr;
    }

    return poll_data_.data();
  }

  LIBCOPP_UTIL_FORCEINLINE value_type *data() noexcept {
    if (!is_ready()) {
      return nullptr;
    }

    return poll_data_.data();
  }

  LIBCOPP_UTIL_FORCEINLINE const ptr_type &raw_ptr() const noexcept { return poll_data_.raw_ptr(); }
  LIBCOPP_UTIL_FORCEINLINE ptr_type &raw_ptr() noexcept { return poll_data_.raw_ptr(); }
  LIBCOPP_UTIL_FORCEINLINE const poller_type &poll_data() const noexcept { return poll_data_; }
  LIBCOPP_UTIL_FORCEINLINE poller_type &poll_data() noexcept { return poll_data_; }
  LIBCOPP_UTIL_FORCEINLINE void reset_data() noexcept(noexcept(poll_data_.reset())) { poll_data_.reset(); }

  template <class U>
  LIBCOPP_UTIL_FORCEINLINE void reset_data(U &&in) noexcept(std::is_nothrow_assignable<poller_type, U>::value) {
    poll_data_ = std::forward<U>(in);
  }

 private:
  poller_type poll_data_;
};

}  // namespace future
LIBCOPP_COPP_NAMESPACE_END

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
  future() {}
  ~future() {}

  UTIL_FORCEINLINE bool is_ready() const LIBCOPP_MACRO_NOEXCEPT { return poll_data_.is_ready(); }

  UTIL_FORCEINLINE bool is_pending() const LIBCOPP_MACRO_NOEXCEPT { return poll_data_.is_pending(); }

  UTIL_FORCEINLINE const value_type *data() const LIBCOPP_MACRO_NOEXCEPT {
    if (!is_ready()) {
      return nullptr;
    }

    return poll_data_.data();
  }

  UTIL_FORCEINLINE value_type *data() LIBCOPP_MACRO_NOEXCEPT {
    if (!is_ready()) {
      return nullptr;
    }

    return poll_data_.data();
  }

  UTIL_FORCEINLINE const ptr_type &raw_ptr() const LIBCOPP_MACRO_NOEXCEPT { return poll_data_.raw_ptr(); }
  UTIL_FORCEINLINE ptr_type &raw_ptr() LIBCOPP_MACRO_NOEXCEPT { return poll_data_.raw_ptr(); }
  UTIL_FORCEINLINE const poller_type &poll_data() const LIBCOPP_MACRO_NOEXCEPT { return poll_data_; }
  UTIL_FORCEINLINE poller_type &poll_data() LIBCOPP_MACRO_NOEXCEPT { return poll_data_; }
  UTIL_FORCEINLINE void reset_data() { poll_data_.reset(); }

  template <class U>
  UTIL_FORCEINLINE void reset_data(U &&in) {
    poll_data_ = std::forward<U>(in);
  }

 private:
  poller_type poll_data_;
};

}  // namespace future
LIBCOPP_COPP_NAMESPACE_END

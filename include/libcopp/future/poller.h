// Copyright 2023 owent

#pragma once

#include <libcopp/utils/config/libcopp_build_features.h>

#include "libcopp/future/storage.h"

LIBCOPP_COPP_NAMESPACE_BEGIN
namespace future {
template <class T, class TPTR = typename poll_storage_ptr_selector<T>::type>
class LIBCOPP_COPP_API_HEAD_ONLY poller {
 public:
  using self_type = poller<T, TPTR>;
  // If T is trivial and is smaller than size of four pointers, using small object optimization
  using poll_storage = poll_storage_base<T, TPTR>;
  using storage_type = typename poll_storage::storage_type;
  using ptr_type = typename poll_storage::ptr_type;
  using value_type = typename poll_storage::value_type;

 public:
  inline poller() noexcept { poll_storage::construct_default_storage(storage_data_); }

  template <class... U>
  inline poller(U &&...in) noexcept(noexcept(poll_storage::construct_storage(std::declval<storage_type &>(),
                                                                             std::forward<U>(in)...))) {
    setup_from(std::forward<U>(in)...);
  }

  inline poller(self_type &&other) noexcept(noexcept(poll_storage::move_storage(std::declval<storage_type &>(),
                                                                                std::declval<storage_type>()))) {
    setup_from(std::move(other));
  }

  inline poller &operator=(self_type &&other) noexcept(
      noexcept(poll_storage::move_storage(std::declval<storage_type &>(), std::declval<storage_type>()))) {
    setup_from(std::move(other));
    return *this;
  }

  template <class U>
  inline poller &operator=(U &&in) noexcept(noexcept(poll_storage::construct_storage(std::declval<storage_type &>(),
                                                                                     std::forward<U>(in)))) {
    setup_from(std::forward<U>(in));
    return *this;
  }

  LIBCOPP_UTIL_FORCEINLINE bool is_ready() const noexcept { return !!poll_storage::unwrap(storage_data_); }

  LIBCOPP_UTIL_FORCEINLINE bool is_pending() const noexcept { return !poll_storage::unwrap(storage_data_); }

  LIBCOPP_UTIL_FORCEINLINE const value_type *data() const noexcept { return poll_storage::unwrap(storage_data_).get(); }
  LIBCOPP_UTIL_FORCEINLINE value_type *data() noexcept { return poll_storage::unwrap(storage_data_).get(); }

  LIBCOPP_UTIL_FORCEINLINE const ptr_type &raw_ptr() const noexcept { return poll_storage::unwrap(storage_data_); }
  LIBCOPP_UTIL_FORCEINLINE ptr_type &raw_ptr() noexcept { return poll_storage::unwrap(storage_data_); }

  LIBCOPP_UTIL_FORCEINLINE void reset() noexcept(noexcept(poll_storage::reset(std::declval<storage_type &>()))) {
    poll_storage::reset(storage_data_);
  }
  LIBCOPP_UTIL_FORCEINLINE void swap(self_type &other) noexcept {
    poll_storage::swap(storage_data_, other.storage_data_);
  }
  LIBCOPP_UTIL_FORCEINLINE friend void swap(self_type &l, self_type &r) noexcept { l.swap(r); }

 private:
  template <class U, class UDELETER,
            typename std::enable_if<std::is_base_of<T, typename std::decay<U>::type>::value, bool>::type = false>
  inline void setup_from(std::unique_ptr<U, UDELETER> &&in) noexcept(
      noexcept(poll_storage::construct_storage(std::declval<storage_type &>(), std::move(in)))) {
    poll_storage::construct_storage(storage_data_, std::move(in));
  }

  template <class... TARGS>
  inline void setup_from(TARGS &&...args) noexcept(
      noexcept(poll_storage::construct_storage(std::declval<storage_type &>(), std::forward<TARGS>(args)...))) {
    poll_storage::construct_storage(storage_data_, std::forward<TARGS>(args)...);
  }

  inline void setup_from(self_type &&other) noexcept(
      noexcept(poll_storage::move_storage(std::declval<storage_type &>(), std::move(other.storage_data_)))) {
    poll_storage::move_storage(storage_data_, std::move(other.storage_data_));
  }

 private:
  storage_type storage_data_;
};
}  // namespace future
LIBCOPP_COPP_NAMESPACE_END

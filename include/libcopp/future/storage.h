// Copyright 2023 owent

#pragma once

#include <libcopp/utils/config/libcopp_build_features.h>
#include <libcopp/utils/memory/default_smart_ptr_trait.h>
#include <libcopp/utils/std/coroutine.h>
#include <libcopp/utils/std/explicit_declare.h>
#include <libcopp/utils/std/type_traits.h>

#include <libcopp/utils/features.h>

// clang-format off
#include <libcopp/utils/config/stl_include_prefix.h>  // NOLINT(build/include_order)
// clang-format on
#include <cstring>
#include <functional>
#include <memory>
// clang-format off
#include <libcopp/utils/config/stl_include_suffix.h>  // NOLINT(build/include_order)
// clang-format on

LIBCOPP_COPP_NAMESPACE_BEGIN
namespace future {
// FUNCTION TEMPLATE make_unique
template <class T, class... TARGS, typename std::enable_if<!std::is_array<T>::value, int>::type = 0>
LIBCOPP_EXPLICIT_NODISCARD_ATTR std::unique_ptr<T> make_unique(TARGS &&...args) {  // make a unique_ptr
  return std::unique_ptr<T>(new T(std::forward<TARGS>(args)...));
}

template <class T, typename std::enable_if<std::is_array<T>::value && std::extent<T>::value == 0, int>::type = 0>
LIBCOPP_EXPLICIT_NODISCARD_ATTR std::unique_ptr<T> make_unique(size_t sz) {  // make a unique_ptr
  using TELEM = typename std::remove_extent<T>::type;
  return std::unique_ptr<T>(new TELEM[sz]());
}

template <class T, class... TARGS, typename std::enable_if<std::extent<T>::value != 0, int>::type = 0>
void make_unique(TARGS &&...) = delete;

template <class T>
struct LIBCOPP_COPP_API_HEAD_ONLY small_object_optimize_storage_deleter {
  LIBCOPP_UTIL_FORCEINLINE void operator()(T *) const noexcept {
    // Do nothing
  }
  template <class U>
  LIBCOPP_UTIL_FORCEINLINE void operator()(U *) const noexcept {
    // Do nothing
  }
};

template <class T>
struct LIBCOPP_COPP_API_HEAD_ONLY poll_storage_ptr_selector;

template <>
struct LIBCOPP_COPP_API_HEAD_ONLY poll_storage_ptr_selector<void> {
  using type = std::unique_ptr<void, small_object_optimize_storage_deleter<void> >;
};

template <class T>
struct LIBCOPP_COPP_API_HEAD_ONLY poll_storage_ptr_selector {
  using type = typename std::conditional<LIBCOPP_IS_TIRVIALLY_COPYABLE_V(T) && sizeof(T) < (sizeof(size_t) << 2),
                                         std::unique_ptr<T, small_object_optimize_storage_deleter<T> >,
                                         std::unique_ptr<T, std::default_delete<T> > >::type;
};

template <class T>
struct LIBCOPP_COPP_API_HEAD_ONLY compact_storage_selector;

template <>
struct LIBCOPP_COPP_API_HEAD_ONLY compact_storage_selector<void> {
  using type = std::unique_ptr<void, small_object_optimize_storage_deleter<void> >;
};

template <class T>
struct LIBCOPP_COPP_API_HEAD_ONLY compact_storage_selector {
  using type = typename std::conditional<LIBCOPP_IS_TIRVIALLY_COPYABLE_V(T) && sizeof(T) <= (sizeof(size_t) << 2),
                                         std::unique_ptr<T, small_object_optimize_storage_deleter<T> >,
                                         LIBCOPP_COPP_NAMESPACE_ID::memory::default_strong_rc_ptr<T> >::type;
};

template <class T, class TPTR>
struct LIBCOPP_COPP_API_HEAD_ONLY poll_storage_base;

template <>
struct LIBCOPP_COPP_API_HEAD_ONLY
poll_storage_base<void, std::unique_ptr<void, small_object_optimize_storage_deleter<void> > > : public std::true_type {
  using value_type = void;
  using ptr_type = std::unique_ptr<void, small_object_optimize_storage_deleter<void> >;
  using storage_type = ptr_type;

  LIBCOPP_UTIL_FORCEINLINE static void construct_default_storage(storage_type &out) noexcept { out.reset(); }

  template <class U, class UDELETOR,
            typename std::enable_if<std::is_convertible<typename std::decay<U>::type, bool>::value, bool>::type = false>
  LIBCOPP_UTIL_FORCEINLINE static void construct_storage(storage_type &out,
                                                         std::unique_ptr<U, UDELETOR> &&in) noexcept {
    if (in) {
      out.reset(reinterpret_cast<void *>(&out));
    } else {
      out.reset();
    }
  }

  template <class U,
            typename std::enable_if<std::is_convertible<typename std::decay<U>::type, bool>::value, bool>::type = false>
  LIBCOPP_UTIL_FORCEINLINE static void construct_storage(storage_type &out, U &&in) noexcept {
    if (in) {
      out.reset(reinterpret_cast<void *>(&out));
    } else {
      out.reset();
    }
  }

  LIBCOPP_UTIL_FORCEINLINE static void move_storage(storage_type &out, storage_type &&in) noexcept {
    if (in) {
      out.reset(reinterpret_cast<void *>(&out));
    } else {
      out.reset();
    }

    in.reset();
  }

  LIBCOPP_UTIL_FORCEINLINE static void reset(storage_type &storage) noexcept { storage.reset(); }
  LIBCOPP_UTIL_FORCEINLINE static void swap(storage_type &l, storage_type &r) noexcept {
    if (!!l == !!r) {
      return;
    }

    if (l) {
      l.reset();
      r.reset(reinterpret_cast<void *>(&r));
    } else {
      l.reset(reinterpret_cast<void *>(&l));
      r.reset();
    }
  }

  LIBCOPP_UTIL_FORCEINLINE static const ptr_type &unwrap(const storage_type &storage) noexcept { return storage; }
  LIBCOPP_UTIL_FORCEINLINE static ptr_type &unwrap(storage_type &storage) noexcept { return storage; }
};

template <class T>
struct LIBCOPP_COPP_API_HEAD_ONLY poll_storage_base<T, std::unique_ptr<T, small_object_optimize_storage_deleter<T> > >
    : public std::true_type {
  using value_type = T;
  using ptr_type = std::unique_ptr<T, small_object_optimize_storage_deleter<T> >;
  using storage_type = std::pair<T, ptr_type>;

  LIBCOPP_UTIL_FORCEINLINE static void construct_default_storage(storage_type &out) noexcept {
    memset(&out.first, 0, sizeof(out.first));
    out.second.reset();
  }

  template <class U, class UDELETOR,
            typename std::enable_if<std::is_base_of<T, typename std::decay<U>::type>::value ||
                                        std::is_convertible<typename std::decay<U>::type, T>::value,
                                    bool>::type = false>
  LIBCOPP_UTIL_FORCEINLINE static void construct_storage(storage_type &out,
                                                         std::unique_ptr<U, UDELETOR> &&in) noexcept {
    if (in) {
      out.first = *in;
      out.second.reset(&out.first);
      in.reset();
    } else {
      memset(&out.first, 0, sizeof(out.first));
      out.second.reset();
    }
  }

  template <class U, typename std::enable_if<std::is_base_of<T, typename std::decay<U>::type>::value ||
                                                 std::is_convertible<typename std::decay<U>::type, T>::value,
                                             bool>::type = false>
  LIBCOPP_UTIL_FORCEINLINE static void construct_storage(storage_type &out, U &&in) noexcept {
    out.first = in;
    out.second.reset(&out.first);
  }

  template <class... U>
  LIBCOPP_UTIL_FORCEINLINE static void construct_storage(storage_type &out, U &&...in) noexcept {
    out.first = value_type(std::forward<U>(in)...);
    out.second.reset(&out.first);
  }

  LIBCOPP_UTIL_FORCEINLINE static void move_storage(storage_type &out, storage_type &&in) noexcept(
      std::is_nothrow_copy_assignable<value_type>::value) {
    if (in.second) {
      out.first = in.first;
      out.second.reset(&out.first);
      in.second.reset();
    } else {
      memset(&out.first, 0, sizeof(out.first));
      out.second.reset();
    }
  }

  LIBCOPP_UTIL_FORCEINLINE static void reset(storage_type &storage) noexcept { storage.second.reset(); }
  LIBCOPP_UTIL_FORCEINLINE static void swap(storage_type &l, storage_type &r) noexcept {
    value_type lv = l.first;
    l.first = r.first;
    r.first = lv;
    if (!!l.second == !!r.second) {
      return;
    }

    if (l.second) {
      l.second.reset();
      r.second.reset(&r.first);
    } else {
      l.second.reset(&l.first);
      r.second.reset();
    }
  }

  LIBCOPP_UTIL_FORCEINLINE static const ptr_type &unwrap(const storage_type &storage) noexcept {
    return storage.second;
  }
  LIBCOPP_UTIL_FORCEINLINE static ptr_type &unwrap(storage_type &storage) noexcept { return storage.second; }
};

template <class T, class TPTR>
struct LIBCOPP_COPP_API_HEAD_ONLY poll_storage_base : public std::false_type {
  using value_type = T;
  using ptr_type = TPTR;
  using storage_type = ptr_type;

  LIBCOPP_UTIL_FORCEINLINE static void construct_default_storage(storage_type &out) noexcept { out.reset(); }

  template <class U, class UDELETOR,
            typename std::enable_if<std::is_base_of<T, typename std::decay<U>::type>::value, bool>::type = false>
  LIBCOPP_UTIL_FORCEINLINE static void construct_storage(storage_type &out,
                                                         std::unique_ptr<U, UDELETOR> &&in) noexcept {
    out = std::move(in);
  }

  template <class U, typename std::enable_if<std::is_base_of<T, typename std::decay<U>::type>::value &&
                                                 type_traits::is_shared_ptr<ptr_type>::value,
                                             bool>::type = false>
  LIBCOPP_UTIL_FORCEINLINE static void construct_storage(
      storage_type &out, LIBCOPP_COPP_NAMESPACE_ID::memory::strong_rc_ptr<U> &&in) noexcept {
    out = std::move(LIBCOPP_COPP_NAMESPACE_ID::memory::static_pointer_cast<typename ptr_type::element_type>(in));
  }

  template <class U, typename std::enable_if<std::is_base_of<T, typename std::decay<U>::type>::value &&
                                                 type_traits::is_shared_ptr<ptr_type>::value,
                                             bool>::type = false>
  LIBCOPP_UTIL_FORCEINLINE static void construct_storage(storage_type &out, ::std::shared_ptr<U> &&in) noexcept {
    out = std::move(::std::static_pointer_cast<typename ptr_type::element_type>(in));
  }

  template <class... U>
  LIBCOPP_UTIL_FORCEINLINE static void construct_storage(storage_type &out, U &&...in) noexcept(
      std::is_nothrow_constructible<value_type, U...>::value) {
    out.reset(new value_type(std::forward<U>(in)...));
  }

  LIBCOPP_UTIL_FORCEINLINE static void move_storage(storage_type &out, storage_type &&in) noexcept {
    out = std::move(in);
  }

  LIBCOPP_UTIL_FORCEINLINE static void reset(storage_type &storage) noexcept { storage.reset(); }
  LIBCOPP_UTIL_FORCEINLINE static void swap(storage_type &l, storage_type &r) noexcept { std::swap(l, r); }

  LIBCOPP_UTIL_FORCEINLINE static const ptr_type &unwrap(const storage_type &storage) noexcept { return storage; }
  LIBCOPP_UTIL_FORCEINLINE static ptr_type &unwrap(storage_type &storage) noexcept { return storage; }
};

template <class T, class TPTR>
struct LIBCOPP_COPP_API_HEAD_ONLY compact_storage;

template <class T>
struct LIBCOPP_COPP_API_HEAD_ONLY compact_storage<T, std::unique_ptr<T, small_object_optimize_storage_deleter<T> > >
    : public std::true_type {
  using value_type = T;
  using ptr_type = std::unique_ptr<T, small_object_optimize_storage_deleter<T> >;
  using storage_type = T;

  LIBCOPP_UTIL_FORCEINLINE static bool is_shared_storage() noexcept { return false; }
  LIBCOPP_UTIL_FORCEINLINE static void destroy_storage(storage_type &) noexcept {
    // do nothing for trival copyable object
  }
  LIBCOPP_UTIL_FORCEINLINE static void construct_default_storage(storage_type &out) noexcept {
    memset(&out, 0, sizeof(out));
  }

  LIBCOPP_UTIL_FORCEINLINE static void construct_storage(storage_type &out) noexcept { construct_default_storage(out); }

  template <class U, class UDELETOR,
            typename std::enable_if<std::is_base_of<T, typename std::decay<U>::type>::value ||
                                        std::is_convertible<typename std::decay<U>::type, T>::value,
                                    bool>::type = false>
  LIBCOPP_UTIL_FORCEINLINE static void construct_storage(storage_type &out, std::unique_ptr<U, UDELETOR> &&in) noexcept(
      std::is_nothrow_assignable<storage_type, U>::value) {
    if (in) {
      out = *in;
    } else {
      memset(&out, 0, sizeof(out));
    }
  }

  template <class U, typename std::enable_if<std::is_base_of<T, typename std::decay<U>::type>::value ||
                                                 std::is_convertible<typename std::decay<U>::type, T>::value,
                                             bool>::type = false>
  LIBCOPP_UTIL_FORCEINLINE static void construct_storage(storage_type &out, U &&in) noexcept(
      std::is_nothrow_assignable<storage_type, U>::value) {
    out = in;
  }

  LIBCOPP_UTIL_FORCEINLINE static void clone_storage(storage_type &out, const storage_type &in) noexcept {
    memcpy(&out, &in, sizeof(out));
  }

  LIBCOPP_UTIL_FORCEINLINE static void move_storage(storage_type &out, storage_type &&in) noexcept {
    memcpy(&out, &in, sizeof(out));
    memset(&in, 0, sizeof(in));
  }

  LIBCOPP_UTIL_FORCEINLINE static void swap(storage_type &l, storage_type &r) noexcept {
    storage_type lv = l;
    l = r;
    r = lv;
  }

  LIBCOPP_UTIL_FORCEINLINE static value_type *unwrap(storage_type &storage) noexcept { return &storage; }
  LIBCOPP_UTIL_FORCEINLINE static const value_type *unwrap(const storage_type &storage) noexcept { return &storage; }
  LIBCOPP_UTIL_FORCEINLINE static ptr_type clone_ptr(storage_type &storage) noexcept { return ptr_type(&storage); }
};

template <class T>
struct LIBCOPP_COPP_API_HEAD_ONLY compact_storage<T, LIBCOPP_COPP_NAMESPACE_ID::memory::strong_rc_ptr<T> >
    : public std::false_type {
  using value_type = T;
  using ptr_type = LIBCOPP_COPP_NAMESPACE_ID::memory::strong_rc_ptr<T>;
  using storage_type = ptr_type;

  LIBCOPP_UTIL_FORCEINLINE static bool is_shared_storage() noexcept { return true; }
  LIBCOPP_UTIL_FORCEINLINE static void destroy_storage(storage_type &out) noexcept { out.reset(); }
  LIBCOPP_UTIL_FORCEINLINE static void construct_default_storage(storage_type &out) noexcept { out.reset(); }

  template <class U, class UDELETOR,
            typename std::enable_if<std::is_base_of<T, typename std::decay<U>::type>::value, bool>::type = false>
  LIBCOPP_UTIL_FORCEINLINE static void construct_storage(storage_type &out, std::unique_ptr<U, UDELETOR> &&in) {
    if (in) {
      out = std::move(in);
    } else {
      out.reset();
    }
  }

  template <class U,
            typename std::enable_if<std::is_base_of<T, typename std::decay<U>::type>::value, bool>::type = false>
  LIBCOPP_UTIL_FORCEINLINE static void construct_storage(
      storage_type &out, LIBCOPP_COPP_NAMESPACE_ID::memory::strong_rc_ptr<U> &&in) noexcept {
    if (in) {
      out = LIBCOPP_COPP_NAMESPACE_ID::memory::static_pointer_cast<T>(in);
    } else {
      out.reset();
    }
  }

  template <class... TARGS>
  LIBCOPP_UTIL_FORCEINLINE static void construct_storage(storage_type &out, TARGS &&...in) noexcept(
      std::is_nothrow_constructible<T, TARGS...>::value) {
    out = LIBCOPP_COPP_NAMESPACE_ID::memory::make_strong_rc<T>(std::forward<TARGS>(in)...);
  }

  LIBCOPP_UTIL_FORCEINLINE static void clone_storage(storage_type &out, const storage_type &in) noexcept { out = in; }
  LIBCOPP_UTIL_FORCEINLINE static void move_storage(storage_type &out, storage_type &&in) noexcept {
    out.swap(in);
    in.reset();
  }

  LIBCOPP_UTIL_FORCEINLINE static void swap(storage_type &l, storage_type &r) noexcept { l.swap(r); }

  LIBCOPP_UTIL_FORCEINLINE static value_type *unwrap(storage_type &storage) noexcept { return storage.get(); }
  LIBCOPP_UTIL_FORCEINLINE static const value_type *unwrap(const storage_type &storage) noexcept {
    return storage.get();
  }
  LIBCOPP_UTIL_FORCEINLINE static ptr_type clone_ptr(storage_type &storage) noexcept { return storage; }
};

template <class T>
struct LIBCOPP_COPP_API_HEAD_ONLY compact_storage<T, ::std::shared_ptr<T> > : public std::false_type {
  using value_type = T;
  using ptr_type = ::std::shared_ptr<T>;
  using storage_type = ptr_type;

  LIBCOPP_UTIL_FORCEINLINE static bool is_shared_storage() noexcept { return true; }
  LIBCOPP_UTIL_FORCEINLINE static void destroy_storage(storage_type &out) noexcept { out.reset(); }
  LIBCOPP_UTIL_FORCEINLINE static void construct_default_storage(storage_type &out) noexcept { out.reset(); }

  template <class U, class UDELETOR,
            typename std::enable_if<std::is_base_of<T, typename std::decay<U>::type>::value, bool>::type = false>
  LIBCOPP_UTIL_FORCEINLINE static void construct_storage(storage_type &out, std::unique_ptr<U, UDELETOR> &&in) {
    if (in) {
      out = std::move(in);
    } else {
      out.reset();
    }
  }

  template <class U,
            typename std::enable_if<std::is_base_of<T, typename std::decay<U>::type>::value, bool>::type = false>
  LIBCOPP_UTIL_FORCEINLINE static void construct_storage(storage_type &out, ::std::shared_ptr<U> &&in) noexcept {
    if (in) {
      out = ::std::static_pointer_cast<T>(in);
    } else {
      out.reset();
    }
  }

  template <class... TARGS>
  LIBCOPP_UTIL_FORCEINLINE static void construct_storage(storage_type &out, TARGS &&...in) noexcept(
      std::is_nothrow_constructible<T, TARGS...>::value) {
    out = ::std::make_shared<T>(std::forward<TARGS>(in)...);
  }

  LIBCOPP_UTIL_FORCEINLINE static void clone_storage(storage_type &out, const storage_type &in) noexcept { out = in; }
  LIBCOPP_UTIL_FORCEINLINE static void move_storage(storage_type &out, storage_type &&in) noexcept {
    out.swap(in);
    in.reset();
  }

  LIBCOPP_UTIL_FORCEINLINE static void swap(storage_type &l, storage_type &r) noexcept { l.swap(r); }

  LIBCOPP_UTIL_FORCEINLINE static value_type *unwrap(storage_type &storage) noexcept { return storage.get(); }
  LIBCOPP_UTIL_FORCEINLINE static const value_type *unwrap(const storage_type &storage) noexcept {
    return storage.get();
  }
  LIBCOPP_UTIL_FORCEINLINE static ptr_type clone_ptr(storage_type &storage) noexcept { return storage; }
};

template <class T>
struct default_compact_storage : public compact_storage<T, typename compact_storage_selector<T>::type> {
  using type = compact_storage<T, typename compact_storage_selector<T>::type>;
};

template <class TOK, class TERR, bool is_all_trivial>
class LIBCOPP_COPP_API_HEAD_ONLY result_base;

template <class TOK, class TERR>
class LIBCOPP_COPP_API_HEAD_ONLY result_base<TOK, TERR, true> {
 public:
  using success_type = TOK;
  using error_type = TERR;
  enum mode_type {
    EN_RESULT_SUCCESS = 0,
    EN_RESULT_ERROR = 1,
  };

  LIBCOPP_UTIL_FORCEINLINE bool is_success() const noexcept { return mode_ == EN_RESULT_SUCCESS; }
  LIBCOPP_UTIL_FORCEINLINE bool is_error() const noexcept { return mode_ == EN_RESULT_ERROR; }

  LIBCOPP_UTIL_FORCEINLINE const success_type *get_success() const noexcept {
    return is_success() ? &success_value_ : nullptr;
  }
  LIBCOPP_UTIL_FORCEINLINE success_type *get_success() noexcept { return is_success() ? &success_value_ : nullptr; }
  LIBCOPP_UTIL_FORCEINLINE const error_type *get_error() const noexcept { return is_error() ? &error_value_ : nullptr; }
  LIBCOPP_UTIL_FORCEINLINE error_type *get_error() noexcept { return is_error() ? &error_value_ : nullptr; }

 private:
  template <class UOK, class UERR>
  friend class result_type;
  template <class TRESULT, bool>
  friend struct _make_result_instance_helper;

  template <class TARGS>
  LIBCOPP_UTIL_FORCEINLINE void construct_success(TARGS &&args) noexcept {
    make_success_base(std::forward<TARGS>(args));
  }

  template <class TARGS>
  LIBCOPP_UTIL_FORCEINLINE void construct_error(TARGS &&args) noexcept {
    make_error_base(std::forward<TARGS>(args));
  }

  template <class TARGS>
  LIBCOPP_UTIL_FORCEINLINE void make_success_base(TARGS &&args) noexcept {
    success_value_ = args;
    mode_ = EN_RESULT_SUCCESS;
  }

  template <class TARGS>
  LIBCOPP_UTIL_FORCEINLINE void make_error_base(TARGS &&args) noexcept {
    error_value_ = args;
    mode_ = EN_RESULT_ERROR;
  }

  inline void swap(result_base &other) noexcept {
    using std::swap;
    if (is_success()) {
      swap(success_value_, other.success_value_);
    } else {
      swap(error_value_, other.error_value_);
    }
    swap(mode_, other.mode_);
  }

  LIBCOPP_UTIL_FORCEINLINE friend void swap(result_base &l, result_base &r) noexcept { l.swap(r); }

 private:
  union {
    success_type success_value_;
    error_type error_value_;
  };
  mode_type mode_;
};

template <class TOK, class TERR>
class LIBCOPP_COPP_API_HEAD_ONLY result_base<TOK, TERR, false> {
 public:
  using success_type = TOK;
  using error_type = TERR;
  enum mode_type {
    EN_RESULT_SUCCESS = 0,
    EN_RESULT_ERROR = 1,
    EN_RESULT_NONE = 2,
  };

  using success_storage_type = typename default_compact_storage<success_type>::type;
  using error_storage_type = typename default_compact_storage<error_type>::type;
<<<<<<< HEAD
  using success_value_type = typename success_storage_type::storage_type;
  using error_value_type = typename error_storage_type::storage_type;
=======
>>>>>>> 6ead59ad8a78a075ca2cdbde38cc7f06fad5b887

  LIBCOPP_UTIL_FORCEINLINE bool is_success() const noexcept { return mode_ == EN_RESULT_SUCCESS; }
  LIBCOPP_UTIL_FORCEINLINE bool is_error() const noexcept { return mode_ == EN_RESULT_ERROR; }

  LIBCOPP_UTIL_FORCEINLINE const success_type *get_success() const noexcept {
    return is_success() ? success_storage_type::unwrap(success_value_) : nullptr;
  }
  LIBCOPP_UTIL_FORCEINLINE success_type *get_success() noexcept {
    return is_success() ? success_storage_type::unwrap(success_value_) : nullptr;
  }
  LIBCOPP_UTIL_FORCEINLINE const error_type *get_error() const noexcept {
    return is_error() ? error_storage_type::unwrap(error_value_) : nullptr;
  }
  LIBCOPP_UTIL_FORCEINLINE error_type *get_error() noexcept {
    return is_error() ? error_storage_type::unwrap(error_value_) : nullptr;
  }

  result_base() noexcept : mode_(EN_RESULT_NONE) {
    success_storage_type::construct_default_storage(success_value_);
    error_storage_type::construct_default_storage(error_value_);
  }
  ~result_base() { reset(); }

  result_base(result_base &&other) noexcept : mode_(EN_RESULT_NONE) {
    success_storage_type::construct_default_storage(success_value_);
    error_storage_type::construct_default_storage(error_value_);

    swap(other);
  }

  result_base &operator=(result_base &&other) noexcept {
    swap(other);
    other.reset();
    return *this;
  }

  LIBCOPP_UTIL_FORCEINLINE void swap(result_base &other) noexcept {
    using std::swap;
    success_storage_type::swap(success_value_, other.success_value_);
    error_storage_type::swap(error_value_, other.error_value_);
    swap(mode_, other.mode_);
  }

  LIBCOPP_UTIL_FORCEINLINE friend void swap(result_base &l, result_base &r) noexcept { l.swap(r); }

 private:
  template <class UOK, class UERR>
  friend class result_type;
  template <class TRESULT, bool>
  friend struct _make_result_instance_helper;

  template <class... TARGS>
  LIBCOPP_UTIL_FORCEINLINE void construct_success(TARGS &&...args) noexcept(noexcept(
      success_storage_type::construct_storage(std::declval<success_value_type &>(), std::forward<TARGS>(args)...))) {
    reset();
    success_storage_type::construct_storage(success_value_, std::forward<TARGS>(args)...);
    mode_ = EN_RESULT_SUCCESS;
  }

  template <class... TARGS>
  LIBCOPP_UTIL_FORCEINLINE void construct_error(TARGS &&...args) noexcept(noexcept(
      error_storage_type::construct_storage(std::declval<error_value_type &>(), std::forward<TARGS>(args)...))) {
    reset();
    error_storage_type::construct_storage(error_value_, std::forward<TARGS>(args)...);
    mode_ = EN_RESULT_ERROR;
  }

  template <class... TARGS>
  LIBCOPP_UTIL_FORCEINLINE void make_success_base(TARGS &&...args) noexcept(
      noexcept(make_object<success_storage_type>(std::declval<success_value_type &>(), std::forward<TARGS>(args)...))) {
    reset();
    make_object<success_storage_type>(success_value_, std::forward<TARGS>(args)...);
    mode_ = EN_RESULT_SUCCESS;
  }

  template <class... TARGS>
  LIBCOPP_UTIL_FORCEINLINE void make_error_base(TARGS &&...args) noexcept(
      noexcept(make_object<error_storage_type>(std::declval<error_value_type &>(), std::forward<TARGS>(args)...))) {
    reset();
    make_object<error_storage_type>(error_value_, std::forward<TARGS>(args)...);
    mode_ = EN_RESULT_ERROR;
  }

  inline void reset() noexcept {
    if (EN_RESULT_SUCCESS == mode_) {
      success_storage_type::destroy_storage(success_value_);
    } else if (EN_RESULT_ERROR == mode_) {
      error_storage_type::destroy_storage(error_value_);
    }

    mode_ = EN_RESULT_NONE;
  }

 private:
  template <class TSTORAGE, class... TARGS>
  LIBCOPP_UTIL_FORCEINLINE static void make_object(typename TSTORAGE::storage_type &out, TARGS &&...args) noexcept(
      noexcept(TSTORAGE::construct_storage(out, std::forward<TARGS>(args)...))) {
    TSTORAGE::construct_storage(out, std::forward<TARGS>(args)...);
  }

  template <class TSTORAGE, class... TARGS>
  LIBCOPP_UTIL_FORCEINLINE static void make_object(
      LIBCOPP_COPP_NAMESPACE_ID::memory::default_strong_rc_ptr<typename TSTORAGE::storage_type> &out,
      TARGS &&...args) noexcept(noexcept(TSTORAGE::construct_storage(out, LIBCOPP_COPP_NAMESPACE_ID::memory::
                                                                              default_make_strong<
                                                                                  typename TSTORAGE::storage_type>(
                                                                                  std::forward<TARGS>(args)...)))) {
    TSTORAGE::construct_storage(out,
                                LIBCOPP_COPP_NAMESPACE_ID::memory::default_make_strong<typename TSTORAGE::storage_type>(
                                    std::forward<TARGS>(args)...));
  }

<<<<<<< HEAD
  success_value_type success_value_;
  error_value_type error_value_;
=======
  typename success_storage_type::storage_type success_data_;
  typename error_storage_type::storage_type error_data_;
>>>>>>> 6ead59ad8a78a075ca2cdbde38cc7f06fad5b887
  mode_type mode_;
};

template <class TRESULT, bool>
struct LIBCOPP_COPP_API_HEAD_ONLY _make_result_instance_helper;

template <class TRESULT>
struct LIBCOPP_COPP_API_HEAD_ONLY _make_result_instance_helper<TRESULT, false> {
  using type = std::unique_ptr<TRESULT>;

  template <class... TARGS>
  inline static type make_success(TARGS &&...args) noexcept(
      noexcept(std::declval<TRESULT>().make_success_base(std::forward<TARGS>(args)...))) {
    type ret = LIBCOPP_COPP_NAMESPACE_ID::future::make_unique<TRESULT>();
    if (ret) {
      ret->make_success_base(std::forward<TARGS>(args)...);
    }

    return ret;
  }

  template <class... TARGS>
  inline static type make_error(TARGS &&...args) noexcept(
      noexcept(std::declval<TRESULT>().make_error_base(std::forward<TARGS>(args)...))) {
    type ret = LIBCOPP_COPP_NAMESPACE_ID::future::make_unique<TRESULT>();
    if (ret) {
      ret->make_error_base(std::forward<TARGS>(args)...);
    }

    return ret;
  }
};

template <class TRESULT>
struct LIBCOPP_COPP_API_HEAD_ONLY _make_result_instance_helper<TRESULT, true> {
  using type = TRESULT;

  template <class... TARGS>
  LIBCOPP_UTIL_FORCEINLINE static type make_success(TARGS &&...args) noexcept(
      noexcept(std::declval<TRESULT>().make_success_base(std::forward<TARGS>(args)...))) {
    TRESULT ret;
    ret.make_success_base(std::forward<TARGS>(args)...);
    return ret;
  }

  template <class... TARGS>
  LIBCOPP_UTIL_FORCEINLINE static type make_error(TARGS &&...args) noexcept(
      noexcept(std::declval<TRESULT>().make_error_base(std::forward<TARGS>(args)...))) {
    TRESULT ret;
    ret.make_error_base(std::forward<TARGS>(args)...);
    return ret;
  }
};

template <class TOK, class TERR>
class LIBCOPP_COPP_API_HEAD_ONLY result_type
    : public result_base<TOK, TERR, default_compact_storage<TOK>::value && default_compact_storage<TERR>::value> {
 public:
  using base_type = result_base<TOK, TERR, default_compact_storage<TOK>::value && default_compact_storage<TERR>::value>;
  using self_type = result_type<TOK, TERR>;

 private:
  using _make_instance_type = _make_result_instance_helper<
      self_type, poll_storage_base<base_type, typename poll_storage_ptr_selector<base_type>::type>::value>;

 public:
  using storage_type = typename _make_instance_type::type;

  template <class... TARGS>
  LIBCOPP_UTIL_FORCEINLINE static self_type create_success(TARGS &&...args) noexcept(
      noexcept(std::declval<self_type>().construct_success(std::forward<TARGS>(args)...))) {
    self_type ret;
    ret.construct_success(std::forward<TARGS>(args)...);
    return ret;
  }

  template <class... TARGS>
  LIBCOPP_UTIL_FORCEINLINE static self_type create_error(TARGS &&...args) noexcept(
      noexcept(std::declval<self_type>().construct_error(std::forward<TARGS>(args)...))) {
    self_type ret;
    ret.construct_error(std::forward<TARGS>(args)...);
    return ret;
  }

 public:
  template <class... TARGS>
  LIBCOPP_UTIL_FORCEINLINE static storage_type make_success(TARGS &&...args) noexcept(
      noexcept(_make_instance_type::make_success(std::forward<TARGS>(args)...))) {
    return _make_instance_type::make_success(std::forward<TARGS>(args)...);
  }

  template <class... TARGS>
  LIBCOPP_UTIL_FORCEINLINE static storage_type make_error(TARGS &&...args) noexcept(
      noexcept(_make_instance_type::make_error(std::forward<TARGS>(args)...))) {
    return _make_instance_type::make_error(std::forward<TARGS>(args)...);
  }
};
}  // namespace future
LIBCOPP_COPP_NAMESPACE_END

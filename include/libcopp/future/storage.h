// Copyright 2023 owent

#pragma once

#include <libcopp/utils/config/libcopp_build_features.h>

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
EXPLICIT_NODISCARD_ATTR std::unique_ptr<T> make_unique(TARGS &&...args) {  // make a unique_ptr
  return std::unique_ptr<T>(new T(std::forward<TARGS>(args)...));
}

template <class T, typename std::enable_if<std::is_array<T>::value && std::extent<T>::value == 0, int>::type = 0>
EXPLICIT_NODISCARD_ATTR std::unique_ptr<T> make_unique(size_t sz) {  // make a unique_ptr
  using TELEM = typename std::remove_extent<T>::type;
  return std::unique_ptr<T>(new TELEM[sz]());
}

template <class T, class... TARGS, typename std::enable_if<std::extent<T>::value != 0, int>::type = 0>
void make_unique(TARGS &&...) = delete;

template <class T>
struct LIBCOPP_COPP_API_HEAD_ONLY small_object_optimize_storage_deleter {
  UTIL_FORCEINLINE void operator()(T *) const LIBCOPP_MACRO_NOEXCEPT {
    // Do nothing
  }
  template <class U>
  UTIL_FORCEINLINE void operator()(U *) const LIBCOPP_MACRO_NOEXCEPT {
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
  using type = typename std::conditional<COPP_IS_TIRVIALLY_COPYABLE_V(T) && sizeof(T) < (sizeof(size_t) << 2),
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
  using type = typename std::conditional<COPP_IS_TIRVIALLY_COPYABLE_V(T) && sizeof(T) <= (sizeof(size_t) << 2),
                                         std::unique_ptr<T, small_object_optimize_storage_deleter<T> >,
                                         std::shared_ptr<T> >::type;
};

template <class T, class TPTR>
struct LIBCOPP_COPP_API_HEAD_ONLY poll_storage_base;

template <>
struct LIBCOPP_COPP_API_HEAD_ONLY
    poll_storage_base<void, std::unique_ptr<void, small_object_optimize_storage_deleter<void> > >
    : public std::true_type {
  using value_type = void;
  using ptr_type = std::unique_ptr<void, small_object_optimize_storage_deleter<void> >;
  using storage_type = ptr_type;

  UTIL_FORCEINLINE static void construct_default_storage(storage_type &out) LIBCOPP_MACRO_NOEXCEPT { out.reset(); }

  template <class U, class UDELETOR,
            typename std::enable_if<std::is_convertible<typename std::decay<U>::type, bool>::value, bool>::type = false>
  UTIL_FORCEINLINE static void construct_storage(storage_type &out,
                                                 std::unique_ptr<U, UDELETOR> &&in) LIBCOPP_MACRO_NOEXCEPT {
    if (in) {
      out.reset(reinterpret_cast<void *>(&out));
    } else {
      out.reset();
    }
  }

  template <class U,
            typename std::enable_if<std::is_convertible<typename std::decay<U>::type, bool>::value, bool>::type = false>
  UTIL_FORCEINLINE static void construct_storage(storage_type &out, U &&in) LIBCOPP_MACRO_NOEXCEPT {
    if (in) {
      out.reset(reinterpret_cast<void *>(&out));
    } else {
      out.reset();
    }
  }

  UTIL_FORCEINLINE static void move_storage(storage_type &out, storage_type &&in) LIBCOPP_MACRO_NOEXCEPT {
    if (in) {
      out.reset(reinterpret_cast<void *>(&out));
    } else {
      out.reset();
    }

    in.reset();
  }

  UTIL_FORCEINLINE static void reset(storage_type &storage) LIBCOPP_MACRO_NOEXCEPT { storage.reset(); }
  UTIL_FORCEINLINE static void swap(storage_type &l, storage_type &r) LIBCOPP_MACRO_NOEXCEPT {
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

  UTIL_FORCEINLINE static const ptr_type &unwrap(const storage_type &storage) LIBCOPP_MACRO_NOEXCEPT { return storage; }
  UTIL_FORCEINLINE static ptr_type &unwrap(storage_type &storage) LIBCOPP_MACRO_NOEXCEPT { return storage; }
};

template <class T>
struct LIBCOPP_COPP_API_HEAD_ONLY poll_storage_base<T, std::unique_ptr<T, small_object_optimize_storage_deleter<T> > >
    : public std::true_type {
  using value_type = T;
  using ptr_type = std::unique_ptr<T, small_object_optimize_storage_deleter<T> >;
  using storage_type = std::pair<T, ptr_type>;

  UTIL_FORCEINLINE static void construct_default_storage(storage_type &out) LIBCOPP_MACRO_NOEXCEPT {
    memset(&out.first, 0, sizeof(out.first));
    out.second.reset();
  }

  template <class U, class UDELETOR,
            typename std::enable_if<std::is_base_of<T, typename std::decay<U>::type>::value ||
                                        std::is_convertible<typename std::decay<U>::type, T>::value,
                                    bool>::type = false>
  UTIL_FORCEINLINE static void construct_storage(storage_type &out,
                                                 std::unique_ptr<U, UDELETOR> &&in) LIBCOPP_MACRO_NOEXCEPT {
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
  UTIL_FORCEINLINE static void construct_storage(storage_type &out, U &&in) LIBCOPP_MACRO_NOEXCEPT {
    out.first = in;
    out.second.reset(&out.first);
  }

  template <class... U>
  UTIL_FORCEINLINE static void construct_storage(storage_type &out, U &&...in) LIBCOPP_MACRO_NOEXCEPT {
    out.first = value_type(std::forward<U>(in)...);
    out.second.reset(&out.first);
  }

  UTIL_FORCEINLINE static void move_storage(storage_type &out, storage_type &&in) LIBCOPP_MACRO_NOEXCEPT {
    if (in.second) {
      out.first = in.first;
      out.second.reset(&out.first);
      in.second.reset();
    } else {
      memset(&out.first, 0, sizeof(out.first));
      out.second.reset();
    }
  }

  UTIL_FORCEINLINE static void reset(storage_type &storage) LIBCOPP_MACRO_NOEXCEPT { storage.second.reset(); }
  UTIL_FORCEINLINE static void swap(storage_type &l, storage_type &r) LIBCOPP_MACRO_NOEXCEPT {
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

  UTIL_FORCEINLINE static const ptr_type &unwrap(const storage_type &storage) LIBCOPP_MACRO_NOEXCEPT {
    return storage.second;
  }
  UTIL_FORCEINLINE static ptr_type &unwrap(storage_type &storage) LIBCOPP_MACRO_NOEXCEPT { return storage.second; }
};

template <class T, class TPTR>
struct LIBCOPP_COPP_API_HEAD_ONLY poll_storage_base : public std::false_type {
  using value_type = T;
  using ptr_type = TPTR;
  using storage_type = ptr_type;

  UTIL_FORCEINLINE static void construct_default_storage(storage_type &out) LIBCOPP_MACRO_NOEXCEPT { out.reset(); }

  template <class U, class UDELETOR,
            typename std::enable_if<std::is_base_of<T, typename std::decay<U>::type>::value, bool>::type = false>
  UTIL_FORCEINLINE static void construct_storage(storage_type &out,
                                                 std::unique_ptr<U, UDELETOR> &&in) LIBCOPP_MACRO_NOEXCEPT {
    out = std::move(in);
  }

  template <class U, typename std::enable_if<std::is_base_of<T, typename std::decay<U>::type>::value &&
                                                 type_traits::is_shared_ptr<ptr_type>::value,
                                             bool>::type = false>
  UTIL_FORCEINLINE static void construct_storage(storage_type &out, std::shared_ptr<U> &&in) LIBCOPP_MACRO_NOEXCEPT {
    out = std::move(std::static_pointer_cast<typename ptr_type::element_type>(in));
  }

  template <class... U>
  UTIL_FORCEINLINE static void construct_storage(storage_type &out, U &&...in) LIBCOPP_MACRO_NOEXCEPT {
    out.reset(new value_type(std::forward<U>(in)...));
  }

  UTIL_FORCEINLINE static void move_storage(storage_type &out, storage_type &&in) LIBCOPP_MACRO_NOEXCEPT {
    out = std::move(in);
  }

  UTIL_FORCEINLINE static void reset(storage_type &storage) LIBCOPP_MACRO_NOEXCEPT { storage.reset(); }
  UTIL_FORCEINLINE static void swap(storage_type &l, storage_type &r) LIBCOPP_MACRO_NOEXCEPT { std::swap(l, r); }

  UTIL_FORCEINLINE static const ptr_type &unwrap(const storage_type &storage) LIBCOPP_MACRO_NOEXCEPT { return storage; }
  UTIL_FORCEINLINE static ptr_type &unwrap(storage_type &storage) LIBCOPP_MACRO_NOEXCEPT { return storage; }
};

template <class T, class TPTR>
struct LIBCOPP_COPP_API_HEAD_ONLY compact_storage;

template <class T>
struct LIBCOPP_COPP_API_HEAD_ONLY compact_storage<T, std::unique_ptr<T, small_object_optimize_storage_deleter<T> > >
    : public std::true_type {
  using value_type = T;
  using ptr_type = std::unique_ptr<T, small_object_optimize_storage_deleter<T> >;
  using storage_type = T;

  UTIL_FORCEINLINE static bool is_shared_storage() LIBCOPP_MACRO_NOEXCEPT { return false; }
  UTIL_FORCEINLINE static void destroy_storage(storage_type &) {
    // do nothing for trival copyable object
  }
  UTIL_FORCEINLINE static void construct_default_storage(storage_type &out) LIBCOPP_MACRO_NOEXCEPT {
    memset(&out, 0, sizeof(out));
  }

  UTIL_FORCEINLINE static void construct_storage(storage_type &out) LIBCOPP_MACRO_NOEXCEPT {
    construct_default_storage(out);
  }

  template <class U, class UDELETOR,
            typename std::enable_if<std::is_base_of<T, typename std::decay<U>::type>::value ||
                                        std::is_convertible<typename std::decay<U>::type, T>::value,
                                    bool>::type = false>
  UTIL_FORCEINLINE static void construct_storage(storage_type &out,
                                                 std::unique_ptr<U, UDELETOR> &&in) LIBCOPP_MACRO_NOEXCEPT {
    if (in) {
      out = *in;
    } else {
      memset(&out, 0, sizeof(out));
    }
  }

  template <class U, typename std::enable_if<std::is_base_of<T, typename std::decay<U>::type>::value ||
                                                 std::is_convertible<typename std::decay<U>::type, T>::value,
                                             bool>::type = false>
  UTIL_FORCEINLINE static void construct_storage(storage_type &out, U &&in) LIBCOPP_MACRO_NOEXCEPT {
    out = in;
  }

  UTIL_FORCEINLINE static void clone_storage(storage_type &out, const storage_type &in) LIBCOPP_MACRO_NOEXCEPT {
    memcpy(&out, &in, sizeof(out));
  }

  UTIL_FORCEINLINE static void move_storage(storage_type &out, storage_type &&in) LIBCOPP_MACRO_NOEXCEPT {
    memcpy(&out, &in, sizeof(out));
    memset(&in, 0, sizeof(in));
  }

  UTIL_FORCEINLINE static void swap(storage_type &l, storage_type &r) LIBCOPP_MACRO_NOEXCEPT {
    storage_type lv = l;
    l = r;
    r = lv;
  }

  UTIL_FORCEINLINE static value_type *unwrap(storage_type &storage) LIBCOPP_MACRO_NOEXCEPT { return &storage; }
  UTIL_FORCEINLINE static const value_type *unwrap(const storage_type &storage) LIBCOPP_MACRO_NOEXCEPT {
    return &storage;
  }
  UTIL_FORCEINLINE static ptr_type clone_ptr(storage_type &storage) LIBCOPP_MACRO_NOEXCEPT {
    return ptr_type(&storage);
  }
};

template <class T>
struct LIBCOPP_COPP_API_HEAD_ONLY compact_storage<T, std::shared_ptr<T> > : public std::false_type {
  using value_type = T;
  using ptr_type = std::shared_ptr<T>;
  using storage_type = ptr_type;

  UTIL_FORCEINLINE static bool is_shared_storage() LIBCOPP_MACRO_NOEXCEPT { return true; }
  UTIL_FORCEINLINE static void destroy_storage(storage_type &out) { out.reset(); }
  UTIL_FORCEINLINE static void construct_default_storage(storage_type &out) { out.reset(); }

  template <class U, class UDELETOR,
            typename std::enable_if<std::is_base_of<T, typename std::decay<U>::type>::value, bool>::type = false>
  UTIL_FORCEINLINE static void construct_storage(storage_type &out, std::unique_ptr<U, UDELETOR> &&in) {
    if (in) {
      out = std::move(in);
    } else {
      out.reset();
    }
  }

  template <class U,
            typename std::enable_if<std::is_base_of<T, typename std::decay<U>::type>::value, bool>::type = false>
  UTIL_FORCEINLINE static void construct_storage(storage_type &out, std::shared_ptr<U> &&in) {
    if (in) {
      out = std::static_pointer_cast<T>(in);
    } else {
      out.reset();
    }
  }

  template <class... TARGS>
  UTIL_FORCEINLINE static void construct_storage(storage_type &out, TARGS &&...in) {
    out = std::make_shared<T>(std::forward<TARGS>(in)...);
  }

  UTIL_FORCEINLINE static void clone_storage(storage_type &out, const storage_type &in) { out = in; }
  UTIL_FORCEINLINE static void move_storage(storage_type &out, storage_type &&in) LIBCOPP_MACRO_NOEXCEPT {
    out.swap(in);
    in.reset();
  }

  UTIL_FORCEINLINE static void swap(storage_type &l, storage_type &r) LIBCOPP_MACRO_NOEXCEPT { l.swap(r); }

  UTIL_FORCEINLINE static value_type *unwrap(storage_type &storage) LIBCOPP_MACRO_NOEXCEPT { return storage.get(); }
  UTIL_FORCEINLINE static const value_type *unwrap(const storage_type &storage) LIBCOPP_MACRO_NOEXCEPT {
    return storage.get();
  }
  UTIL_FORCEINLINE static ptr_type clone_ptr(storage_type &storage) LIBCOPP_MACRO_NOEXCEPT { return storage; }
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

  UTIL_FORCEINLINE bool is_success() const LIBCOPP_MACRO_NOEXCEPT { return mode_ == EN_RESULT_SUCCESS; }
  UTIL_FORCEINLINE bool is_error() const LIBCOPP_MACRO_NOEXCEPT { return mode_ == EN_RESULT_ERROR; }

  UTIL_FORCEINLINE const success_type *get_success() const LIBCOPP_MACRO_NOEXCEPT {
    return is_success() ? &success_data_ : nullptr;
  }
  UTIL_FORCEINLINE success_type *get_success() LIBCOPP_MACRO_NOEXCEPT {
    return is_success() ? &success_data_ : nullptr;
  }
  UTIL_FORCEINLINE const error_type *get_error() const LIBCOPP_MACRO_NOEXCEPT {
    return is_error() ? &error_data_ : nullptr;
  }
  UTIL_FORCEINLINE error_type *get_error() LIBCOPP_MACRO_NOEXCEPT { return is_error() ? &error_data_ : nullptr; }

 private:
  template <class UOK, class UERR>
  friend class result_type;
  template <class TRESULT, bool>
  friend struct _make_result_instance_helper;

  template <class TARGS>
  UTIL_FORCEINLINE void construct_success(TARGS &&args) LIBCOPP_MACRO_NOEXCEPT {
    make_success_base(std::forward<TARGS>(args));
  }

  template <class TARGS>
  UTIL_FORCEINLINE void construct_error(TARGS &&args) LIBCOPP_MACRO_NOEXCEPT {
    make_error_base(std::forward<TARGS>(args));
  }

  template <class TARGS>
  UTIL_FORCEINLINE void make_success_base(TARGS &&args) LIBCOPP_MACRO_NOEXCEPT {
    success_data_ = args;
    mode_ = EN_RESULT_SUCCESS;
  }

  template <class TARGS>
  UTIL_FORCEINLINE void make_error_base(TARGS &&args) LIBCOPP_MACRO_NOEXCEPT {
    error_data_ = args;
    mode_ = EN_RESULT_ERROR;
  }

  inline void swap(result_base &other) LIBCOPP_MACRO_NOEXCEPT {
    using std::swap;
    if (is_success()) {
      swap(success_data_, other.success_data_);
    } else {
      swap(error_data_, other.error_data_);
    }
    swap(mode_, other.mode_);
  }

  UTIL_FORCEINLINE friend void swap(result_base &l, result_base &r) LIBCOPP_MACRO_NOEXCEPT { l.swap(r); }

 private:
  union {
    success_type success_data_;
    error_type error_data_;
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

  UTIL_FORCEINLINE bool is_success() const LIBCOPP_MACRO_NOEXCEPT { return mode_ == EN_RESULT_SUCCESS; }
  UTIL_FORCEINLINE bool is_error() const LIBCOPP_MACRO_NOEXCEPT { return mode_ == EN_RESULT_ERROR; }

  UTIL_FORCEINLINE const success_type *get_success() const LIBCOPP_MACRO_NOEXCEPT {
    return is_success() ? success_storage_type::unwrap(success_data_) : nullptr;
  }
  UTIL_FORCEINLINE success_type *get_success() LIBCOPP_MACRO_NOEXCEPT {
    return is_success() ? success_storage_type::unwrap(success_data_) : nullptr;
  }
  UTIL_FORCEINLINE const error_type *get_error() const LIBCOPP_MACRO_NOEXCEPT {
    return is_error() ? error_storage_type::unwrap(error_data_) : nullptr;
  }
  UTIL_FORCEINLINE error_type *get_error() LIBCOPP_MACRO_NOEXCEPT {
    return is_error() ? error_storage_type::unwrap(error_data_) : nullptr;
  }

  result_base() : mode_(EN_RESULT_NONE) {
    success_storage_type::construct_default_storage(success_data_);
    error_storage_type::construct_default_storage(error_data_);
  }
  ~result_base() { reset(); }

  result_base(result_base &&other) : mode_(EN_RESULT_NONE) {
    success_storage_type::construct_default_storage(success_data_);
    error_storage_type::construct_default_storage(error_data_);

    swap(other);
  }

  result_base &operator=(result_base &&other) {
    swap(other);
    other.reset();
    return *this;
  }

  UTIL_FORCEINLINE void swap(result_base &other) LIBCOPP_MACRO_NOEXCEPT {
    using std::swap;
    success_storage_type::swap(success_data_, other.success_data_);
    error_storage_type::swap(error_data_, other.error_data_);
    swap(mode_, other.mode_);
  }

  UTIL_FORCEINLINE friend void swap(result_base &l, result_base &r) LIBCOPP_MACRO_NOEXCEPT { l.swap(r); }

 private:
  template <class UOK, class UERR>
  friend class result_type;
  template <class TRESULT, bool>
  friend struct _make_result_instance_helper;

  template <class... TARGS>
  UTIL_FORCEINLINE void construct_success(TARGS &&...args) {
    reset();
    success_storage_type::construct_storage(success_data_, std::forward<TARGS>(args)...);
    mode_ = EN_RESULT_SUCCESS;
  }

  template <class... TARGS>
  UTIL_FORCEINLINE void construct_error(TARGS &&...args) {
    reset();
    error_storage_type::construct_storage(error_data_, std::forward<TARGS>(args)...);
    mode_ = EN_RESULT_ERROR;
  }

  template <class... TARGS>
  UTIL_FORCEINLINE void make_success_base(TARGS &&...args) {
    reset();
    make_object<success_storage_type>(success_data_, std::forward<TARGS>(args)...);
    mode_ = EN_RESULT_SUCCESS;
  }

  template <class... TARGS>
  UTIL_FORCEINLINE void make_error_base(TARGS &&...args) {
    reset();
    make_object<error_storage_type>(error_data_, std::forward<TARGS>(args)...);
    mode_ = EN_RESULT_ERROR;
  }

  inline void reset() {
    if (EN_RESULT_SUCCESS == mode_) {
      success_storage_type::destroy_storage(success_data_);
    } else if (EN_RESULT_ERROR == mode_) {
      error_storage_type::destroy_storage(error_data_);
    }

    mode_ = EN_RESULT_NONE;
  }

 private:
  template <class TSTORAGE, class... TARGS>
  UTIL_FORCEINLINE static void make_object(typename TSTORAGE::storage_type &out, TARGS &&...args) {
    TSTORAGE::construct_storage(out, std::forward<TARGS>(args)...);
  }

  template <class TSTORAGE, class... TARGS>
  UTIL_FORCEINLINE static void make_object(std::shared_ptr<typename TSTORAGE::storage_type> &out, TARGS &&...args) {
    TSTORAGE::construct_storage(out, std::make_shared<typename TSTORAGE::storage_type>(std::forward<TARGS>(args)...));
  }

  using success_storage_type = typename default_compact_storage<success_type>::type;
  using error_storage_type = typename default_compact_storage<error_type>::type;

  typename success_storage_type::storage_type success_data_;
  typename error_storage_type::storage_type error_data_;
  mode_type mode_;
};

template <class TRESULT, bool>
struct LIBCOPP_COPP_API_HEAD_ONLY _make_result_instance_helper;

template <class TRESULT>
struct LIBCOPP_COPP_API_HEAD_ONLY _make_result_instance_helper<TRESULT, false> {
  using type = std::unique_ptr<TRESULT>;

  template <class... TARGS>
  inline static type make_success(TARGS &&...args) {
    type ret = LIBCOPP_COPP_NAMESPACE_ID::future::make_unique<TRESULT>();
    if (ret) {
      ret->make_success_base(std::forward<TARGS>(args)...);
    }

    return ret;
  }

  template <class... TARGS>
  inline static type make_error(TARGS &&...args) {
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
  UTIL_FORCEINLINE static type make_success(TARGS &&...args) {
    TRESULT ret;
    ret.make_success_base(std::forward<TARGS>(args)...);
    return ret;
  }

  template <class... TARGS>
  UTIL_FORCEINLINE static type make_error(TARGS &&...args) {
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
  UTIL_FORCEINLINE static self_type create_success(TARGS &&...args) {
    self_type ret;
    ret.construct_success(std::forward<TARGS>(args)...);
    return ret;
  }

  template <class... TARGS>
  UTIL_FORCEINLINE static self_type create_error(TARGS &&...args) {
    self_type ret;
    ret.construct_error(std::forward<TARGS>(args)...);
    return ret;
  }

 public:
  template <class... TARGS>
  UTIL_FORCEINLINE static storage_type make_success(TARGS &&...args) {
    return _make_instance_type::make_success(std::forward<TARGS>(args)...);
  }

  template <class... TARGS>
  UTIL_FORCEINLINE static storage_type make_error(TARGS &&...args) {
    return _make_instance_type::make_error(std::forward<TARGS>(args)...);
  }
};
}  // namespace future
LIBCOPP_COPP_NAMESPACE_END

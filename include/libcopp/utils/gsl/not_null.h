// Copyright 2023 owent

#pragma once

#include <libcopp/utils/config/libcopp_build_features.h>

// clang-format off
#include <libcopp/utils/config/stl_include_prefix.h>  // NOLINT(build/include_order)
// clang-format on
#include <assert.h>      // for forward
#include <algorithm>     // for forward
#include <cstddef>       // for ptrdiff_t, nullptr_t, size_t
#include <iosfwd>        // for ostream
#include <system_error>  // for hash
#include <type_traits>   // for enable_if_t, is_convertible, is_assignable

// clang-format off
#include <libcopp/utils/config/stl_include_suffix.h>  // NOLINT(build/include_order)
// clang-format on

LIBCOPP_COPP_NAMESPACE_BEGIN

namespace gsl {

namespace details {
template <typename T, typename = void>
struct is_comparable_to_nullptr : std::false_type {};

template <typename T>
struct is_comparable_to_nullptr<
    T, typename std::enable_if<std::is_convertible<decltype(std::declval<T>() != nullptr), bool>::value>::type>
    : std::true_type {};
}  // namespace details

//
// GSL.owner: ownership pointers
//

//
// owner
//
// owner<T> is designed as a bridge for code that must deal directly with owning pointers for some
// reason
//
// T must be a pointer type
// - disallow construction from any type other than pointer type
//
template <class T, class = typename std::enable_if<std::is_pointer<T>::value>::type>
using owner = T;

//
// not_null
//
// Restricts a pointer or smart pointer to only hold non-null values.
//
// Has zero size overhead over T.
//
// If T is a pointer (i.e. T == U*) then
// - allow construction from U*
// - disallow construction from nullptr_t
// - disallow default construction
// - ensure construction from null U* fails
// - allow implicit conversion to U*
//
template <class T>
class not_null {
 public:
  using value_type = typename std::conditional<std::is_copy_constructible<T>::value, T, const T&>::type;

 public:
  static_assert(details::is_comparable_to_nullptr<T>::value, "T cannot be compared to nullptr.");

  template <class U, class = typename std::enable_if<std::is_convertible<U, T>::value>::type>
  constexpr not_null(U&& u) : ptr_(std::forward<U>(u)) {}

  template <class = typename std::enable_if<!std::is_same<std::nullptr_t, T>::value>::type>
  constexpr not_null(T u) : ptr_(std::move(u)) {}

  template <class U, class = typename std::enable_if<std::is_convertible<U, T>::value>::type>
  constexpr not_null(const not_null<U>& other) : not_null(other.get()) {}

  not_null(const not_null& other) = default;
  not_null& operator=(const not_null& other) = default;
  constexpr value_type get() const {
    assert(ptr_ != nullptr);
    return ptr_;
  }

  constexpr operator T() const { return get(); }
  constexpr value_type operator->() const { return get(); }
  constexpr typename std::remove_pointer<value_type>::type operator*() const { return *get(); }

  // prevents compilation when someone attempts to assign a null pointer constant
  not_null(std::nullptr_t) = delete;
  not_null& operator=(std::nullptr_t) = delete;

  // unwanted operators...pointers only point to single objects!
  not_null& operator++() = delete;
  not_null& operator--() = delete;
  not_null operator++(int) = delete;
  not_null operator--(int) = delete;
  not_null& operator+=(std::ptrdiff_t) = delete;
  not_null& operator-=(std::ptrdiff_t) = delete;
  void operator[](std::ptrdiff_t) const = delete;

 private:
  T ptr_;
};

template <class T>
auto make_not_null(T&& t) noexcept {
  return not_null<typename std::decay<T>::type>{std::forward<T>(t)};
}

template <class T, class U>
auto operator==(const not_null<T>& lhs, const not_null<U>& rhs) noexcept(noexcept(lhs.get() == rhs.get()))
    -> decltype(lhs.get() == rhs.get()) {
  return lhs.get() == rhs.get();
}

template <class T, class U>
auto operator!=(const not_null<T>& lhs, const not_null<U>& rhs) noexcept(noexcept(lhs.get() != rhs.get()))
    -> decltype(lhs.get() != rhs.get()) {
  return lhs.get() != rhs.get();
}

template <class T, class U>
auto operator<(const not_null<T>& lhs, const not_null<U>& rhs) noexcept(noexcept(lhs.get() < rhs.get()))
    -> decltype(lhs.get() < rhs.get()) {
  return lhs.get() < rhs.get();
}

template <class T, class U>
auto operator<=(const not_null<T>& lhs, const not_null<U>& rhs) noexcept(noexcept(lhs.get() <= rhs.get()))
    -> decltype(lhs.get() <= rhs.get()) {
  return lhs.get() <= rhs.get();
}

template <class T, class U>
auto operator>(const not_null<T>& lhs, const not_null<U>& rhs) noexcept(noexcept(lhs.get() > rhs.get()))
    -> decltype(lhs.get() > rhs.get()) {
  return lhs.get() > rhs.get();
}

template <class T, class U>
auto operator>=(const not_null<T>& lhs, const not_null<U>& rhs) noexcept(noexcept(lhs.get() >= rhs.get()))
    -> decltype(lhs.get() >= rhs.get()) {
  return lhs.get() >= rhs.get();
}

// more unwanted operators
template <class T, class U>
std::ptrdiff_t operator-(const not_null<T>&, const not_null<U>&) = delete;
template <class T>
not_null<T> operator-(const not_null<T>&, std::ptrdiff_t) = delete;
template <class T>
not_null<T> operator+(const not_null<T>&, std::ptrdiff_t) = delete;
template <class T>
not_null<T> operator+(std::ptrdiff_t, const not_null<T>&) = delete;

}  // namespace gsl

namespace gsl {

//
// strict_not_null
//
// Restricts a pointer or smart pointer to only hold non-null values,
//
// - provides a strict (i.e. explicit constructor from T) wrapper of not_null
// - to be used for new code that wishes the design to be cleaner and make not_null
//   checks intentional, or in old code that would like to make the transition.
//
//   To make the transition from not_null, incrementally replace not_null
//   by strict_not_null and fix compilation errors
//
//   Expect to
//   - remove all unneeded conversions from raw pointer to not_null and back
//   - make API clear by specifying not_null in parameters where needed
//   - remove unnecessary asserts
//
template <class T>
class strict_not_null : public not_null<T> {
 public:
  template <class U, class = typename std::enable_if<std::is_convertible<U, T>::value>::type>
  constexpr explicit strict_not_null(U&& u) : not_null<T>(std::forward<U>(u)) {}

  template <class = typename std::enable_if<!std::is_same<std::nullptr_t, T>::value>::type>
  constexpr explicit strict_not_null(T u) : not_null<T>(u) {}

  template <class U, class = typename std::enable_if<std::is_convertible<U, T>::value>::type>
  constexpr strict_not_null(const not_null<U>& other) : not_null<T>(other) {}

  template <class U, class = typename std::enable_if<std::is_convertible<U, T>::value>::type>
  constexpr strict_not_null(const strict_not_null<U>& other) : not_null<T>(other) {}

  strict_not_null(strict_not_null&& other) = default;
  strict_not_null(const strict_not_null& other) = default;
  strict_not_null& operator=(const strict_not_null& other) = default;
  strict_not_null& operator=(const not_null<T>& other) {
    not_null<T>::operator=(other);
    return *this;
  }

  // prevents compilation when someone attempts to assign a null pointer constant
  strict_not_null(std::nullptr_t) = delete;
  strict_not_null& operator=(std::nullptr_t) = delete;

  // unwanted operators...pointers only point to single objects!
  strict_not_null& operator++() = delete;
  strict_not_null& operator--() = delete;
  strict_not_null operator++(int) = delete;
  strict_not_null operator--(int) = delete;
  strict_not_null& operator+=(std::ptrdiff_t) = delete;
  strict_not_null& operator-=(std::ptrdiff_t) = delete;
  void operator[](std::ptrdiff_t) const = delete;
};

// more unwanted operators
template <class T, class U>
std::ptrdiff_t operator-(const strict_not_null<T>&, const strict_not_null<U>&) = delete;
template <class T>
strict_not_null<T> operator-(const strict_not_null<T>&, std::ptrdiff_t) = delete;
template <class T>
strict_not_null<T> operator+(const strict_not_null<T>&, std::ptrdiff_t) = delete;
template <class T>
strict_not_null<T> operator+(std::ptrdiff_t, const strict_not_null<T>&) = delete;

template <class T>
auto make_strict_not_null(T&& t) noexcept {
  return strict_not_null<typename std::remove_cv<typename std::remove_reference<T>::type>::type>{std::forward<T>(t)};
}

#if (defined(__cpp_deduction_guides) && (__cpp_deduction_guides >= 201611L))

// deduction guides to prevent the ctad-maybe-unsupported warning
template <class T>
not_null(T) -> not_null<T>;
template <class T>
strict_not_null(T) -> strict_not_null<T>;

#endif  // ( defined(__cpp_deduction_guides) && (__cpp_deduction_guides >= 201611L) )

}  // namespace gsl

LIBCOPP_COPP_NAMESPACE_END

namespace std {

template <class T>
struct hash<LIBCOPP_COPP_NAMESPACE_ID::gsl::not_null<T>> {
  std::size_t operator()(const LIBCOPP_COPP_NAMESPACE_ID::gsl::not_null<T>& value) const {
    return hash<T>{}(value.get());
  }
};

template <class T>
ostream& operator<<(ostream& os, const LIBCOPP_COPP_NAMESPACE_ID::gsl::not_null<T>& val) {
  os << val.get();
  return os;
}

template <class T>
struct hash<LIBCOPP_COPP_NAMESPACE_ID::gsl::strict_not_null<T>> {
  std::size_t operator()(const LIBCOPP_COPP_NAMESPACE_ID::gsl::strict_not_null<T>& value) const {
    return hash<T>{}(value.get());
  }
};

}  // namespace std

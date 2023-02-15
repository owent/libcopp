// Copyright 2023 owent

#include <libcopp/utils/config/libcopp_build_features.h>

#ifndef WIN32_LEAN_AND_MEAN
#  define WIN32_LEAN_AND_MEAN
#endif

// clang-format off
#include <libcopp/utils/config/stl_include_prefix.h>  // NOLINT(build/include_order)
// clang-format on

extern "C" {
#include <Windows.h>
#include <assert.h>
}

#if defined(COPP_MACRO_COMPILER_MSVC)
#  pragma warning(push)
#  pragma warning(disable : 4244 4267)
#endif

#include <algorithm>
#include <cmath>
#include <cstddef>
#include <cstring>
#include <stdexcept>

#if __cplusplus >= 201103L
#  include <mutex>
#  include <thread>
#endif
// clang-format off
#include <libcopp/utils/config/stl_include_suffix.h>  // NOLINT(build/include_order)
// clang-format on

#include "libcopp/stack/stack_context.h"
#include "libcopp/stack/stack_traits.h"

// x86_64
// test x86_64 before i386 because icc might
// define __i686__ for x86_64 too
#if defined(__x86_64__) || defined(__x86_64) || defined(__amd64__) || defined(__amd64) || defined(_M_X64) || \
    defined(_M_AMD64)

// Windows seams not to provide a constant or function
// telling the minimal stacksize
#  define MIN_STACKSIZE 8 * 1024
#else
#  define MIN_STACKSIZE 4 * 1024
#endif

#ifdef COPP_HAS_ABI_HEADERS
#  include COPP_ABI_PREFIX
#endif

LIBCOPP_COPP_NAMESPACE_BEGIN

namespace detail {
#if __cplusplus < 201103L
static void system_info_(SYSTEM_INFO *si) { ::GetSystemInfo(si); }

static SYSTEM_INFO system_info() {
  static SYSTEM_INFO si;
  static bool inited = false;
  if (inited) {
    return si;
  }

  system_info_(&si);
  inited = true;
  return si;
}
#else

static SYSTEM_INFO system_info() {
  static SYSTEM_INFO si;
  static std::once_flag flag;
  std::call_once(flag, []() { ::GetSystemInfo(&si); });
  return si;
}

#endif

static std::size_t pagesize() { return static_cast<std::size_t>(system_info().dwPageSize); }

/**
static std::size_t page_count(std::size_t stacksize) {
    return static_cast< std::size_t >(
        std::floor(
            static_cast< float >( stacksize) / pagesize()));
}
**/
}  // namespace detail

// Windows seams not to provide a limit for the stacksize
// libcoco uses 32k+4k bytes as minimum
LIBCOPP_COPP_API bool stack_traits::is_unbounded() LIBCOPP_MACRO_NOEXCEPT { return true; }

LIBCOPP_COPP_API std::size_t stack_traits::page_size() LIBCOPP_MACRO_NOEXCEPT { return detail::pagesize(); }

LIBCOPP_COPP_API std::size_t stack_traits::default_size() LIBCOPP_MACRO_NOEXCEPT {
  std::size_t size = 64 * 1024;  // 64 KB
  if (is_unbounded()) return (std::max)(size, minimum_size());

  assert(is_unbounded() || maximum_size() >= minimum_size());
  return is_unbounded() ? size : (std::min)(size, maximum_size());
}

// because Windows seams not to provide a limit for minimum stacksize
LIBCOPP_COPP_API std::size_t stack_traits::minimum_size() LIBCOPP_MACRO_NOEXCEPT { return MIN_STACKSIZE; }

// because Windows seams not to provide a limit for maximum stacksize
// maximum_size() can never be called (pre-condition ! is_unbounded() )
LIBCOPP_COPP_API std::size_t stack_traits::maximum_size() LIBCOPP_MACRO_NOEXCEPT {
  assert(is_unbounded());
  return SIZE_MAX;
}

LIBCOPP_COPP_API std::size_t stack_traits::round_to_page_size(std::size_t stacksize) LIBCOPP_MACRO_NOEXCEPT {
  // page size must be 2^N
  return static_cast<std::size_t>((stacksize + stack_traits::page_size() - 1) & (~(stack_traits::page_size() - 1)));
}
LIBCOPP_COPP_NAMESPACE_END

#ifdef COPP_HAS_ABI_HEADERS
#  include COPP_ABI_SUFFIX
#endif
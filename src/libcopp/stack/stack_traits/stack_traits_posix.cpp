// Copyright 2023 owent

#include <libcopp/utils/config/libcopp_build_features.h>

#if !defined(_MSC_VER)

#  include <libcopp/stack/stack_traits.h>

// clang-format off
#include <libcopp/utils/config/stl_include_prefix.h>  // NOLINT(build/include_order)
// clang-format on
extern "C" {
#  include <assert.h>
#  include <signal.h>
#  include <sys/resource.h>
#  include <sys/time.h>
#  include <unistd.h>
}

// #if _POSIX_C_SOURCE >= 200112L

#  include <algorithm>
#  include <cmath>
#  include <limits>

#  if !defined(MINSIGSTKSZ)
#    define MINSIGSTKSZ (128 * 1024)
#    define UDEF_MINSIGSTKSZ
#  endif

#  ifdef COPP_HAS_ABI_HEADERS
#    include COPP_ABI_PREFIX
#  endif
// clang-format off
#include <libcopp/utils/config/stl_include_suffix.h>  // NOLINT(build/include_order)
// clang-format on

LIBCOPP_COPP_NAMESPACE_BEGIN

namespace detail {
static std::size_t pagesize() {
  std::size_t size = ::sysconf(_SC_PAGESIZE);
  return size;
}

static rlimit stacksize_limit_() {
  rlimit limit;
  // conforming to POSIX.1-2001
  ::getrlimit(RLIMIT_STACK, &limit);
  return limit;
}

static rlimit stacksize_limit() {
  static rlimit limit = stacksize_limit_();
  return limit;
}
}  // namespace detail

LIBCOPP_COPP_API bool stack_traits::is_unbounded() LIBCOPP_MACRO_NOEXCEPT {
  return RLIM_INFINITY == detail::stacksize_limit().rlim_max;
}

LIBCOPP_COPP_API std::size_t stack_traits::page_size() LIBCOPP_MACRO_NOEXCEPT { return detail::pagesize(); }

LIBCOPP_COPP_API std::size_t stack_traits::default_size() LIBCOPP_MACRO_NOEXCEPT {
  std::size_t size = 8 * minimum_size();  // 64 KB
  if (is_unbounded()) return size;

  assert(maximum_size() >= minimum_size());
  return maximum_size() == size ? size : (std::min)(size, maximum_size());
}

LIBCOPP_COPP_API std::size_t stack_traits::minimum_size() LIBCOPP_MACRO_NOEXCEPT { return MINSIGSTKSZ; }

LIBCOPP_COPP_API std::size_t stack_traits::maximum_size() LIBCOPP_MACRO_NOEXCEPT {
  if (is_unbounded()) return std::numeric_limits<std::size_t>::max();
  return detail::stacksize_limit().rlim_max;
}

LIBCOPP_COPP_API std::size_t stack_traits::round_to_page_size(std::size_t stacksize) LIBCOPP_MACRO_NOEXCEPT {
  // page size must be 2^N
  return static_cast<std::size_t>((stacksize + detail::pagesize() - 1) & (~(detail::pagesize() - 1)));
}
LIBCOPP_COPP_NAMESPACE_END

#  ifdef COPP_HAS_ABI_HEADERS
#    include COPP_ABI_SUFFIX
#  endif

#  ifdef UDEF_MINSIGSTKSZ
#    undef MINSIGSTKSZ
#  endif

#endif

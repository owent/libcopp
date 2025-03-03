// Copyright 2025 owent
// Licenses under the MIT License

#include "libcopp/utils/memory/rc_ptr.h"

#include <cstdlib>

LIBCOPP_COPP_NAMESPACE_BEGIN
namespace memory {

LIBCOPP_COPP_API __rc_ptr_counted_data_base::~__rc_ptr_counted_data_base() noexcept {}

LIBCOPP_COPP_API void __rc_ptr_counted_data_base::throw_bad_weak_ptr() {
#if defined(LIBCOPP_MACRO_ENABLE_EXCEPTION) && LIBCOPP_MACRO_ENABLE_EXCEPTION
  throw std::bad_weak_ptr();
#else
  abort();
#endif
}

}  // namespace memory
LIBCOPP_COPP_NAMESPACE_END

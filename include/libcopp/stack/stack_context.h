// Copyright 2023 owent

#pragma once

#include <libcopp/utils/config/libcopp_build_features.h>

#include <libcopp/utils/features.h>

// clang-format off
#include <libcopp/utils/config/stl_include_prefix.h>  // NOLINT(build/include_order)
// clang-format on
#include <cstddef>
// clang-format off
#include <libcopp/utils/config/stl_include_suffix.h>  // NOLINT(build/include_order)
// clang-format on

LIBCOPP_COPP_NAMESPACE_BEGIN
struct LIBCOPP_COPP_API stack_context {
  size_t size; /** @brief stack size **/
  void *sp;    /** @brief stack end pointer **/

#ifdef LIBCOPP_MACRO_USE_SEGMENTED_STACKS
  using segments_context_t = void *[COPP_MACRO_SEGMENTED_STACK_NUMBER];
  segments_context_t segments_ctx; /** @brief gcc split segment stack data **/
#endif

#ifdef LIBCOPP_MACRO_USE_VALGRIND
  unsigned valgrind_stack_id;
#endif

  stack_context() LIBCOPP_MACRO_NOEXCEPT;
  ~stack_context() LIBCOPP_MACRO_NOEXCEPT;

  stack_context(const stack_context &other) LIBCOPP_MACRO_NOEXCEPT;
  stack_context &operator=(const stack_context &other) LIBCOPP_MACRO_NOEXCEPT;
  stack_context(stack_context &&other) LIBCOPP_MACRO_NOEXCEPT;
  stack_context &operator=(stack_context &&other) LIBCOPP_MACRO_NOEXCEPT;

  void reset() LIBCOPP_MACRO_NOEXCEPT;

  void copy_from(const stack_context &other) LIBCOPP_MACRO_NOEXCEPT;
};
LIBCOPP_COPP_NAMESPACE_END

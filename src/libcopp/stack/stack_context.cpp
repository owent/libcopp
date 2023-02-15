// Copyright 2023 owent

#include <libcopp/utils/config/libcopp_build_features.h>

#include <libcopp/stack/stack_context.h>

// clang-format off
#include <libcopp/utils/config/stl_include_prefix.h>  // NOLINT(build/include_order)
// clang-format on
#include <cstring>
// clang-format off
#include <libcopp/utils/config/stl_include_suffix.h>  // NOLINT(build/include_order)
// clang-format on

LIBCOPP_COPP_NAMESPACE_BEGIN

LIBCOPP_COPP_API stack_context::stack_context() LIBCOPP_MACRO_NOEXCEPT : size(0),
                                                                         sp(nullptr)
#ifdef LIBCOPP_MACRO_USE_SEGMENTED_STACKS
    ,
                                                                         segments_ctx()
#endif
#ifdef LIBCOPP_MACRO_USE_VALGRIND
    ,
                                                                         valgrind_stack_id(0)
#endif
{
}

LIBCOPP_COPP_API stack_context::~stack_context() LIBCOPP_MACRO_NOEXCEPT {}

LIBCOPP_COPP_API stack_context::stack_context(const stack_context &other) LIBCOPP_MACRO_NOEXCEPT { copy_from(other); }

LIBCOPP_COPP_API stack_context &stack_context::operator=(const stack_context &other) LIBCOPP_MACRO_NOEXCEPT {
  copy_from(other);
  return *this;
}

LIBCOPP_COPP_API stack_context::stack_context(stack_context &&other) LIBCOPP_MACRO_NOEXCEPT {
  copy_from(other);
  other.reset();
}

LIBCOPP_COPP_API stack_context &stack_context::operator=(stack_context &&other) LIBCOPP_MACRO_NOEXCEPT {
  copy_from(other);
  other.reset();
  return *this;
}

LIBCOPP_COPP_API void stack_context::reset() LIBCOPP_MACRO_NOEXCEPT {
  size = 0;
  sp = nullptr;
#ifdef LIBCOPP_MACRO_USE_SEGMENTED_STACKS
  memset(segments_ctx, 0, sizeof(segments_ctx));
#endif
#ifdef LIBCOPP_MACRO_USE_VALGRIND
  valgrind_stack_id = 0;
#endif
}

LIBCOPP_COPP_API void stack_context::copy_from(const stack_context &other) LIBCOPP_MACRO_NOEXCEPT {
  size = other.size;
  sp = other.sp;
#ifdef LIBCOPP_MACRO_USE_SEGMENTED_STACKS
  memcpy(segments_ctx, other.segments_ctx, sizeof(segments_ctx));
#endif
#ifdef LIBCOPP_MACRO_USE_VALGRIND
  valgrind_stack_id = other.valgrind_stack_id;
#endif
}
LIBCOPP_COPP_NAMESPACE_END

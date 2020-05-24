#include <cstring>

#include <libcopp/stack/stack_context.h>
#include <libcopp/utils/config/compiler_features.h>

namespace copp {

    LIBCOPP_COPP_API stack_context::stack_context() LIBCOPP_MACRO_NOEXCEPT : size(0),
                                                                      sp(NULL)
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

#if defined(UTIL_CONFIG_COMPILER_CXX_RVALUE_REFERENCES) && UTIL_CONFIG_COMPILER_CXX_RVALUE_REFERENCES
    LIBCOPP_COPP_API stack_context::stack_context(stack_context &&other) LIBCOPP_MACRO_NOEXCEPT {
        copy_from(other);
        other.reset();
    }

    LIBCOPP_COPP_API stack_context &stack_context::operator=(stack_context &&other) LIBCOPP_MACRO_NOEXCEPT {
        copy_from(other);
        other.reset();
        return *this;
    }
#endif


    LIBCOPP_COPP_API void stack_context::reset() LIBCOPP_MACRO_NOEXCEPT {
        size = 0;
        sp   = UTIL_CONFIG_NULLPTR;
#ifdef LIBCOPP_MACRO_USE_SEGMENTED_STACKS
        memset(segments_ctx, 0, sizeof(segments_ctx));
#endif
#ifdef LIBCOPP_MACRO_USE_VALGRIND
        valgrind_stack_id = 0;
#endif
    }

    LIBCOPP_COPP_API void stack_context::copy_from(const stack_context &other) LIBCOPP_MACRO_NOEXCEPT {
        size = other.size;
        sp   = other.sp;
#ifdef LIBCOPP_MACRO_USE_SEGMENTED_STACKS
        memcpy(segments_ctx, other.segments_ctx, sizeof(segments_ctx));
#endif
#ifdef LIBCOPP_MACRO_USE_VALGRIND
        valgrind_stack_id = other.valgrind_stack_id;
#endif
    }
} // namespace copp

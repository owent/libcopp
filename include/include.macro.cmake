# =========== include - macro =========== 
set (PROJECT_ROOT_INC_DIR ${CMAKE_CURRENT_LIST_DIR})

include_directories(${PROJECT_ROOT_INC_DIR})

# 功能检测选项
include(WriteCompilerDetectionHeader)

if (NOT EXISTS "${PROJECT_ROOT_INC_DIR}/libcopp/utils/config")
    file(MAKE_DIRECTORY "${PROJECT_ROOT_INC_DIR}/libcopp/utils/config")
endif()

# generate check header
write_compiler_detection_header(
    FILE "${PROJECT_ROOT_INC_DIR}/libcopp/utils/config/compiler_features.h"
    PREFIX UTIL_CONFIG
    COMPILERS GNU Clang AppleClang MSVC
    FEATURES cxx_auto_type cxx_constexpr cxx_decltype cxx_decltype_auto cxx_defaulted_functions cxx_deleted_functions cxx_final cxx_override cxx_range_for cxx_noexcept cxx_nullptr cxx_static_assert cxx_thread_local cxx_variadic_templates cxx_lambdas
)
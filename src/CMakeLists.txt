

EchoWithColor(COLOR GREEN "-- Configure libraries")

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
    FEATURES cxx_auto_type cxx_constexpr cxx_decltype cxx_decltype_auto cxx_defaulted_functions cxx_deleted_functions cxx_final cxx_override cxx_range_for cxx_noexcept cxx_nullptr cxx_rvalue_references cxx_static_assert cxx_thread_local cxx_variadic_templates cxx_lambdas
)

if (LIBCOPP_ENABLE_SEGMENTED_STACKS)
    if(NOT ${CMAKE_CXX_COMPILER_ID} STREQUAL "GNU")
        EchoWithColor(COLOR YELLOW "-- set LIBCOPP_ENABLE_SEGMENTED_STACKS but only gcc support segmented stacks")
        unset(LIBCOPP_ENABLE_SEGMENTED_STACKS)
    elseif(CMAKE_CXX_COMPILER_VERSION VERSION_LESS "4.7.0")
        EchoWithColor(COLOR YELLOW "-- set LIBCOPP_ENABLE_SEGMENTED_STACKS but gcc 4.7.0 and upper support segmented stacks")
        unset(LIBCOPP_ENABLE_SEGMENTED_STACKS)
    else()
        EchoWithColor(COLOR GREEN "-- Enable segmented stacks")
        add_definitions(-fsplit-stack)
        set(COPP_MACRO_USE_SEGMENTED_STACKS 1)
    endif()
endif()


configure_file(
    "${PROJECT_ROOT_INC_DIR}/libcopp/utils/config/build_feature.h.in"
    "${PROJECT_ROOT_INC_DIR}/libcopp/utils/config/build_feature.h"
    @ONLY
)


add_subdirectory("${PROJECT_ROOT_SRC_DIR}/libcopp")

# ========= libcotask =========  
if (LIBCOTASK_ENABLE)
    add_subdirectory("${PROJECT_ROOT_SRC_DIR}/libcotask")
endif()

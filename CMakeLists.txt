cmake_minimum_required(VERSION 3.1.0)
project(libcopp)

########################################################################
# CMake 模块 (递归包含模块, 带颜色输出模块, 平台检测模块)
set (PROJECT_CMAKE_MODULE_DIR "${CMAKE_CURRENT_LIST_DIR}/project/cmake")
include("${PROJECT_CMAKE_MODULE_DIR}/ProjectBuildOption.cmake")
include("${PROJECT_CMAKE_MODULE_DIR}/IncludeDirectoryRecurse.cmake")
include("${PROJECT_CMAKE_MODULE_DIR}/EchoWithColor.cmake")
include("${PROJECT_CMAKE_MODULE_DIR}/FindPlatform.cmake")
include("${PROJECT_CMAKE_MODULE_DIR}/ProjectTools.cmake")


#####################################################################
# 导入编译器和编译选项配置
include("${PROJECT_CMAKE_MODULE_DIR}/CompilerOption.cmake")
include("${PROJECT_CMAKE_MODULE_DIR}/TargetOption.cmake")
EchoWithColor(COLOR GREEN "-- Build Type: ${CMAKE_BUILD_TYPE}")

########################################################################
# 导入项目配置
## 导入所有 macro 定义
include_macro_recurse(${CMAKE_CURRENT_LIST_DIR})

# feature detect
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

## 导入所有工程项目
add_subdirectory("${CMAKE_CURRENT_LIST_DIR}/src")

if(PROJECT_ENABLE_SAMPLE)
    add_subdirectory("${CMAKE_CURRENT_LIST_DIR}/sample")
endif()

if(PROJECT_ENABLE_UNITTEST)
    add_subdirectory("${CMAKE_CURRENT_LIST_DIR}/test")
endif()
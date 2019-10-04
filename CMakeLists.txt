cmake_minimum_required(VERSION 3.7.0)
if (CMAKE_VERSION VERSION_GREATER_EQUAL "3.13")
    cmake_policy(SET CMP0074 NEW)
endif()

enable_testing()

project(libcopp C CXX ASM)

set(LIBCOPP_VERSION_MAJOR "1")
set(LIBCOPP_VERSION_MINOR "2")
set(LIBCOPP_VERSION_PATCH "1")
set(LIBCOPP_VERSION "${LIBCOPP_VERSION_MAJOR}.${LIBCOPP_VERSION_MINOR}.${LIBCOPP_VERSION_PATCH}")

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
unset (PROJECT_LIBCOPP_SYSLIB_LINK_NAMES)
## 导入所有 macro 定义
include("${CMAKE_CURRENT_LIST_DIR}/include/include.macro.cmake")
include("${CMAKE_CURRENT_LIST_DIR}/src/libcopp.macro.cmake")

# feature detect
include(WriteCompilerDetectionHeader)

if (NOT EXISTS "${PROJECT_LIBCOPP_ROOT_INC_DIR}/libcopp/utils/config")
    file(MAKE_DIRECTORY "${PROJECT_LIBCOPP_ROOT_INC_DIR}/libcopp/utils/config")
endif()

# generate check header
write_compiler_detection_header(
    FILE "${PROJECT_LIBCOPP_ROOT_INC_DIR}/libcopp/utils/config/compiler_features.h"
    PREFIX UTIL_CONFIG
    COMPILERS GNU Clang AppleClang MSVC
    FEATURES cxx_auto_type cxx_constexpr cxx_decltype cxx_decltype_auto cxx_defaulted_functions cxx_deleted_functions cxx_final cxx_override cxx_range_for cxx_noexcept cxx_nullptr cxx_rvalue_references cxx_static_assert cxx_thread_local cxx_variadic_templates cxx_lambdas
)

if (LIBCOTASK_ENABLE)
    set(LIBCOTASK_MACRO_ENABLED 1)
endif ()

if (LIBCOTASK_AUTO_CLEANUP_MANAGER)
    set(LIBCOTASK_MACRO_AUTO_CLEANUP_MANAGER 1)
endif ()

find_package(Threads)
if (CMAKE_USE_PTHREADS_INIT)
    list(APPEND PROJECT_LIBCOPP_SYSLIB_LINK_NAMES pthread)
    set(THREAD_TLS_USE_PTHREAD 1)
    if (THREADS_PREFER_PTHREAD_FLAG)
        add_definitions(${THREADS_PREFER_PTHREAD_FLAG})
    endif ()
endif ()

configure_file(
    "${PROJECT_LIBCOPP_ROOT_INC_DIR}/libcopp/utils/config/build_feature.h.in"
    "${PROJECT_LIBCOPP_ROOT_INC_DIR}/libcopp/utils/config/build_feature.h"
    @ONLY
)

if (${CMAKE_CXX_COMPILER_ID} STREQUAL "Clang")
    get_filename_component(CMAKE_CXX_COMPILER_WE ${CMAKE_CXX_COMPILER} NAME_WE CACHE)
    if (NOT ${CMAKE_CXX_COMPILER_WE} STREQUAL "clang-cl")
        if (COMPILER_STRICT_EXTRA_CFLAGS)
            add_definitions(${COMPILER_STRICT_EXTRA_CFLAGS})
        endif ()

        if (COMPILER_STRICT_CFLAGS)
            add_definitions(${COMPILER_STRICT_CFLAGS})
        endif ()
    endif ()
else ()
    if (COMPILER_STRICT_EXTRA_CFLAGS)
        add_definitions(${COMPILER_STRICT_EXTRA_CFLAGS})
    endif ()

    if (COMPILER_STRICT_CFLAGS)
        add_definitions(${COMPILER_STRICT_CFLAGS})
    endif ()
endif ()

## 导入所有工程项目
add_subdirectory("${CMAKE_CURRENT_LIST_DIR}/src")

if (_VALGRIND_EXECUTABLE)
    EchoWithColor(COLOR GREEN "-- Memcheck: valgrind found at ${_VALGRIND_EXECUTABLE}, enable target 'memcheck'")
    add_custom_target(memcheck)
    add_custom_target(callgrind)
endif()

if(PROJECT_ENABLE_SAMPLE)
    add_custom_target(benchmark)
    add_custom_target(run_sample)
    add_subdirectory("${CMAKE_CURRENT_LIST_DIR}/sample")
endif()

if(PROJECT_ENABLE_UNITTEST)
    add_custom_target(run_test)
    add_subdirectory("${CMAKE_CURRENT_LIST_DIR}/test")
endif()


## 生成文档和导入配置

# Install configuration
set(CMAKE_INSTALL_CMAKEDIR "${CMAKE_INSTALL_LIBDIR}/cmake" CACHE STRING "Directory relative to CMAKE_INSTALL to install the cmake configuration files")

configure_file(
    "${CMAKE_CURRENT_LIST_DIR}/docs/libcopp.doxyfile.in" "${CMAKE_CURRENT_LIST_DIR}/docs/libcopp.doxyfile"
    @ONLY
    NEWLINE_STYLE CRLF
)

include(CMakePackageConfigHelpers)
set(INCLUDE_INSTALL_DIR include)

file(MAKE_DIRECTORY "${CMAKE_LIBRARY_OUTPUT_DIRECTORY}/cmake")

configure_package_config_file(
    "${CMAKE_CURRENT_LIST_DIR}/libcopp-config.cmake.in"
    "${CMAKE_LIBRARY_OUTPUT_DIRECTORY}/cmake/libcopp-config.cmake"
    INSTALL_DESTINATION ${CMAKE_INSTALL_CMAKEDIR}
    PATH_VARS LIBCOPP_VERSION INCLUDE_INSTALL_DIR CMAKE_INSTALL_LIBDIR PROJECT_SOURCE_DIR
    NO_CHECK_REQUIRED_COMPONENTS_MACRO
)

write_basic_package_version_file(
    "${CMAKE_LIBRARY_OUTPUT_DIRECTORY}/cmake/libcopp-config-version.cmake"
    VERSION ${LIBCOPP_VERSION}
    COMPATIBILITY SameMajorVersion
)

install(
    FILES "${CMAKE_LIBRARY_OUTPUT_DIRECTORY}/cmake/libcopp-config.cmake" "${CMAKE_LIBRARY_OUTPUT_DIRECTORY}/cmake/libcopp-config-version.cmake"
    DESTINATION ${CMAKE_INSTALL_CMAKEDIR}
)
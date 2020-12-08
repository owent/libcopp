cmake_minimum_required(VERSION 3.12.0)
if (CMAKE_VERSION VERSION_GREATER_EQUAL "3.13")
    cmake_policy(SET CMP0074 NEW)
endif()

enable_testing()

project(libcopp C CXX ASM)

set(LIBCOPP_VERSION_MAJOR "1")
set(LIBCOPP_VERSION_MINOR "3")
set(LIBCOPP_VERSION_PATCH "4")
set(LIBCOPP_VERSION "${LIBCOPP_VERSION_MAJOR}.${LIBCOPP_VERSION_MINOR}.${LIBCOPP_VERSION_PATCH}")

########################################################################
# CMake 模块 (递归包含模块, 带颜色输出模块, 平台检测模块)
set (PROJECT_CMAKE_MODULE_DIR "${CMAKE_CURRENT_LIST_DIR}/project/cmake")
include("${PROJECT_CMAKE_MODULE_DIR}/ProjectBuildOption.cmake")
include("${PROJECT_CMAKE_MODULE_DIR}/IncludeDirectoryRecurse.cmake")
include("${PROJECT_CMAKE_MODULE_DIR}/EchoWithColor.cmake")
include("${PROJECT_CMAKE_MODULE_DIR}/FindPlatform.cmake")


#####################################################################
# 导入编译器和编译选项配置
include("${PROJECT_CMAKE_MODULE_DIR}/CompilerOption.cmake")
include("${PROJECT_CMAKE_MODULE_DIR}/TargetOption.cmake")
EchoWithColor(COLOR GREEN "-- Build Type: ${CMAKE_BUILD_TYPE}")

########################################################################
# Check Fiber API for Win32
if (WIN32)
    check_cxx_source_compiles("
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif

#include <Windows.h>
#include <cstring>
#include <cstdlib>

static VOID __stdcall FiberMainFunc(LPVOID main_fiber) {
    // codes...
    SwitchToFiber(main_fiber);
}

int main() {
    LPVOID main_fiber = ConvertThreadToFiber(NULL);
    LPVOID sub_fiber = CreateFiber(0, FiberMainFunc, (LPVOID)&main_fiber);
    SwitchToFiber(sub_fiber);
    return 0;
}
    " LIBCOPP_MACRO_ENABLE_WIN_FIBER)
endif ()
# Import options
if (COMPILER_OPTIONS_TEST_STD_COROUTINE)
    set(LIBCOPP_MACRO_ENABLE_STD_COROUTINE 1)
    unset(LIBCOPP_MACRO_USE_STD_EXPERIMENTAL_COROUTINE)
elseif(COMPILER_OPTIONS_TEST_STD_COROUTINE_TS)
    set(LIBCOPP_MACRO_ENABLE_STD_COROUTINE 1)
    set(LIBCOPP_MACRO_USE_STD_EXPERIMENTAL_COROUTINE 1)
else()
    unset(LIBCOPP_MACRO_ENABLE_STD_COROUTINE)
    unset(LIBCOPP_MACRO_USE_STD_EXPERIMENTAL_COROUTINE)
endif()

if (COMPILER_OPTIONS_TEST_EXCEPTION)
    set(LIBCOPP_MACRO_ENABLE_EXCEPTION 1)
else()
    unset(LIBCOPP_MACRO_ENABLE_EXCEPTION)
endif ()
if (COMPILER_OPTIONS_TEST_STD_EXCEPTION_PTR)
    set(LIBCOPP_MACRO_ENABLE_STD_EXCEPTION_PTR 1)
else()
    unset(LIBCOPP_MACRO_ENABLE_STD_EXCEPTION_PTR)
endif ()
if (COMPILER_OPTIONS_TEST_RTTI)
    set(LIBCOPP_MACRO_ENABLE_RTTI 1)
else()
    unset(LIBCOPP_MACRO_ENABLE_RTTI)
endif ()

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
    FEATURES
        c_std_90 c_std_99 c_std_11 c_restrict c_static_assert c_variadic_macros
        cxx_std_98 cxx_std_11 cxx_std_14 cxx_std_17 cxx_std_20 cxx_alias_templates cxx_attributes cxx_attribute_deprecated cxx_auto_type 
        cxx_constexpr cxx_decltype cxx_decltype_auto cxx_default_function_template_args cxx_defaulted_functions cxx_delegating_constructors 
        cxx_deleted_functions cxx_final cxx_generic_lambdas cxx_inheriting_constructors cxx_lambdas cxx_long_long_type cxx_noexcept
        cxx_nonstatic_member_init cxx_nullptr cxx_override cxx_range_for cxx_raw_string_literals cxx_relaxed_constexpr cxx_return_type_deduction
        cxx_rvalue_references cxx_sizeof_member cxx_static_assert cxx_thread_local cxx_variadic_templates 
)

if (LIBCOTASK_ENABLE)
    set(LIBCOTASK_MACRO_ENABLED 1)
endif ()

if (LIBCOTASK_AUTO_CLEANUP_MANAGER)
    set(LIBCOTASK_MACRO_AUTO_CLEANUP_MANAGER 1)
endif ()

unset(LIBCOPP_SPECIFY_CXX_FLAGS)

find_package(Threads)
if (CMAKE_USE_PTHREADS_INIT)
    list(APPEND PROJECT_LIBCOPP_SYSLIB_LINK_NAMES pthread)
    set(THREAD_TLS_USE_PTHREAD 1)
    if (THREADS_PREFER_PTHREAD_FLAG)
        list(APPEND LIBCOPP_SPECIFY_CXX_FLAGS ${THREADS_PREFER_PTHREAD_FLAG})
        add_compile_options(${THREADS_PREFER_PTHREAD_FLAG})
    endif ()
endif ()

configure_file(
    "${PROJECT_LIBCOPP_ROOT_INC_DIR}/libcopp/utils/config/libcopp_build_features.h.in"
    "${PROJECT_LIBCOPP_ROOT_INC_DIR}/libcopp/utils/config/libcopp_build_features.h"
    @ONLY
)

if (${CMAKE_CXX_COMPILER_ID} STREQUAL "Clang")
    get_filename_component(CMAKE_CXX_COMPILER_WE ${CMAKE_CXX_COMPILER} NAME_WE CACHE)
    if (NOT ${CMAKE_CXX_COMPILER_WE} STREQUAL "clang-cl")
        if (COMPILER_STRICT_EXTRA_CFLAGS)
            list(APPEND LIBCOPP_SPECIFY_CXX_FLAGS ${COMPILER_STRICT_EXTRA_CFLAGS})
        endif ()

        if (COMPILER_STRICT_CFLAGS)
            list(APPEND LIBCOPP_SPECIFY_CXX_FLAGS ${COMPILER_STRICT_CFLAGS})
        endif ()
    endif ()
else ()
    if (COMPILER_STRICT_EXTRA_CFLAGS)
        list(APPEND LIBCOPP_SPECIFY_CXX_FLAGS ${COMPILER_STRICT_EXTRA_CFLAGS})
    endif ()

    if (COMPILER_STRICT_CFLAGS)
        list(APPEND LIBCOPP_SPECIFY_CXX_FLAGS ${COMPILER_STRICT_CFLAGS})
    endif ()
endif ()

string(REPLACE ";" " " LIBCOPP_SPECIFY_CXX_FLAGS "${LIBCOPP_SPECIFY_CXX_FLAGS}")
if (LIBCOPP_SPECIFY_CXX_FLAGS)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${LIBCOPP_SPECIFY_CXX_FLAGS}")
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${LIBCOPP_SPECIFY_CXX_FLAGS}")
endif()
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

set(CPACK_PACKAGE_VENDOR "libcopp")
set(CPACK_PACKAGE_VERSION_MAJOR "${LIBCOPP_VERSION_MAJOR}")
set(CPACK_PACKAGE_VERSION_MINOR "${LIBCOPP_VERSION_MINOR}")
set(CPACK_PACKAGE_VERSION_PATCH "${LIBCOPP_VERSION_PATCH}")
set(CPACK_PACKAGE_DESCRIPTION "libcopp ${LIBCOPP_VERSION_MAJOR}.${LIBCOPP_VERSION_MINOR}.${LIBCOPP_VERSION_PATCH}")
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "${CPACK_PACKAGE_DESCRIPTION}")
set(CPACK_PACKAGE_CONTACT "admin@owent.net")
set(CPACK_PACKAGE_HOMEPAGE_URL "https://libcopp.atframe.work/")
include (CPack)

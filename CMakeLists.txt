cmake_minimum_required(VERSION 3.16.0)
cmake_policy(SET CMP0054 NEW)
cmake_policy(SET CMP0022 NEW)
cmake_policy(SET CMP0067 NEW)
cmake_policy(SET CMP0074 NEW)
cmake_policy(SET CMP0091 NEW)

enable_testing()

project(
  libcopp
  VERSION "1.5.0"
  DESCRIPTION "Cross-platform coroutine library in C++ ."
  HOMEPAGE_URL "https://libcopp.atframe.work"
  LANGUAGES C CXX ASM)

# ######################################################################################################################
include("${PROJECT_SOURCE_DIR}/project/cmake/ProjectBuildOption.cmake")

# # #############################################################################
echowithcolor(COLOR GREEN "-- Build Type: ${CMAKE_BUILD_TYPE}")

# # #############################################################################

# Check Fiber API for Win32
if(WIN32)
  check_cxx_source_compiles(
    "
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
    LPVOID main_fiber = ConvertThreadToFiber(nullptr);
    LPVOID sub_fiber = CreateFiber(0, FiberMainFunc, (LPVOID)&main_fiber);
    SwitchToFiber(sub_fiber);
    return 0;
}
    "
    LIBCOPP_MACRO_ENABLE_WIN_FIBER)
endif()
# Import options
if(COMPILER_OPTIONS_TEST_STD_COROUTINE)
  set(LIBCOPP_MACRO_ENABLE_STD_COROUTINE
      ON
      CACHE BOOL "Enable C++20 coroutine support for libcopp")
  set(LIBCOPP_MACRO_USE_STD_EXPERIMENTAL_COROUTINE
      OFF
      CACHE BOOL "Enable C++20 coroutine with experimental mode")
elseif(COMPILER_OPTIONS_TEST_STD_COROUTINE_TS)
  set(LIBCOPP_MACRO_ENABLE_STD_COROUTINE
      ON
      CACHE BOOL "Enable C++20 coroutine support for libcopp")
  set(LIBCOPP_MACRO_USE_STD_EXPERIMENTAL_COROUTINE
      ON
      CACHE BOOL "Enable C++20 coroutine with experimental mode")
else()
  set(LIBCOPP_MACRO_ENABLE_STD_COROUTINE
      OFF
      CACHE BOOL "Enable C++20 coroutine support for libcopp")
  set(LIBCOPP_MACRO_USE_STD_EXPERIMENTAL_COROUTINE
      OFF
      CACHE BOOL "Enable C++20 coroutine with experimental mode")
endif()

if(COMPILER_OPTIONS_TEST_EXCEPTION)
  set(LIBCOPP_MACRO_ENABLE_EXCEPTION
      ON
      CACHE BOOL "Enable exception support for libcopp")
else()
  set(LIBCOPP_MACRO_ENABLE_EXCEPTION
      OFF
      CACHE BOOL "Enable exception support for libcopp")
endif()
if(COMPILER_OPTIONS_TEST_STD_EXCEPTION_PTR)
  set(LIBCOPP_MACRO_ENABLE_STD_EXCEPTION_PTR
      ON
      CACHE BOOL "Enable C++11 std::exception_ptr support for libcopp")
else()
  set(LIBCOPP_MACRO_ENABLE_STD_EXCEPTION_PTR
      OFF
      CACHE BOOL "Enable C++11 std::exception_ptr support for libcopp")
endif()
if(COMPILER_OPTIONS_TEST_RTTI)
  set(LIBCOPP_MACRO_ENABLE_RTTI
      ON
      CACHE BOOL "Enable RTTI support for libcopp")
else()
  set(LIBCOPP_MACRO_ENABLE_RTTI
      OFF
      CACHE BOOL "Enable RTTI support for libcopp")
endif()

unset(PROJECT_LIBCOPP_SYSLIB_LINK_NAMES)
# 导入所有 macro 定义
include("${CMAKE_CURRENT_LIST_DIR}/include/include.macro.cmake")
include("${CMAKE_CURRENT_LIST_DIR}/src/libcopp.macro.cmake")

if(NOT EXISTS "${PROJECT_LIBCOPP_ROOT_INC_DIR}/libcopp/utils/config")
  file(MAKE_DIRECTORY "${PROJECT_LIBCOPP_ROOT_INC_DIR}/libcopp/utils/config")
endif()

if(LIBCOTASK_ENABLE)
  set(LIBCOTASK_MACRO_ENABLED 1)
endif()

if(LIBCOTASK_AUTO_CLEANUP_MANAGER)
  set(LIBCOTASK_MACRO_AUTO_CLEANUP_MANAGER 1)
endif()

unset(LIBCOPP_SPECIFY_CXX_FLAGS)

find_package(Threads)
if(CMAKE_USE_PTHREADS_INIT)
  list(APPEND PROJECT_LIBCOPP_SYSLIB_LINK_NAMES pthread)
  set(THREAD_TLS_USE_PTHREAD 1)
  if(THREADS_PREFER_PTHREAD_FLAG)
    list(APPEND LIBCOPP_SPECIFY_CXX_FLAGS ${THREADS_PREFER_PTHREAD_FLAG})
    add_compile_options(${THREADS_PREFER_PTHREAD_FLAG})
  endif()
endif()

set(LIBCOPP_VERSION_MAJOR ${PROJECT_VERSION_MAJOR})
set(LIBCOPP_VERSION_MINOR ${PROJECT_VERSION_MINOR})
set(LIBCOPP_VERSION_PATCH ${PROJECT_VERSION_PATCH})
set(LIBCOPP_VERSION "${PROJECT_VERSION}")

configure_file("${PROJECT_LIBCOPP_ROOT_INC_DIR}/libcopp/utils/config/libcopp_build_features.h.in"
               "${PROJECT_LIBCOPP_ROOT_INC_DIR}/libcopp/utils/config/libcopp_build_features.h" @ONLY)

if(${CMAKE_CXX_COMPILER_ID} STREQUAL "Clang")
  get_filename_component(CMAKE_CXX_COMPILER_WE ${CMAKE_CXX_COMPILER} NAME_WE CACHE)
  if(NOT ${CMAKE_CXX_COMPILER_WE} STREQUAL "clang-cl")
    if(COMPILER_STRICT_EXTRA_CFLAGS)
      list(APPEND LIBCOPP_SPECIFY_CXX_FLAGS ${COMPILER_STRICT_EXTRA_CFLAGS})
    endif()

    if(COMPILER_STRICT_CFLAGS)
      list(APPEND LIBCOPP_SPECIFY_CXX_FLAGS ${COMPILER_STRICT_CFLAGS})
    endif()
  endif()
else()
  if(COMPILER_STRICT_EXTRA_CFLAGS)
    list(APPEND LIBCOPP_SPECIFY_CXX_FLAGS ${COMPILER_STRICT_EXTRA_CFLAGS})
  endif()

  if(COMPILER_STRICT_CFLAGS)
    list(APPEND LIBCOPP_SPECIFY_CXX_FLAGS ${COMPILER_STRICT_CFLAGS})
  endif()
endif()

string(REPLACE ";" " " LIBCOPP_SPECIFY_CXX_FLAGS "${LIBCOPP_SPECIFY_CXX_FLAGS}")
if(LIBCOPP_SPECIFY_CXX_FLAGS)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${LIBCOPP_SPECIFY_CXX_FLAGS}")
  set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${LIBCOPP_SPECIFY_CXX_FLAGS}")
endif()
# 导入所有工程项目
add_subdirectory("${CMAKE_CURRENT_LIST_DIR}/src")

if(_VALGRIND_EXECUTABLE)
  echowithcolor(COLOR GREEN "-- Memcheck: valgrind found at ${_VALGRIND_EXECUTABLE}, enable target 'memcheck'")
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

# 生成文档和导入配置

# Install configuration
set(CMAKE_INSTALL_CMAKEDIR
    "${CMAKE_INSTALL_LIBDIR}/cmake/libcopp"
    CACHE STRING "Directory relative to CMAKE_INSTALL to install the cmake configuration files")

configure_file("${CMAKE_CURRENT_LIST_DIR}/docs/libcopp.doxyfile.in" "${CMAKE_CURRENT_LIST_DIR}/docs/libcopp.doxyfile"
               @ONLY NEWLINE_STYLE CRLF)

include(CMakePackageConfigHelpers)
set(INCLUDE_INSTALL_DIR include)

file(MAKE_DIRECTORY "${CMAKE_LIBRARY_OUTPUT_DIRECTORY}/cmake/libcopp")

configure_package_config_file(
  "${CMAKE_CURRENT_LIST_DIR}/libcopp-config.cmake.in"
  "${CMAKE_LIBRARY_OUTPUT_DIRECTORY}/cmake/libcopp/libcopp-config.cmake"
  INSTALL_DESTINATION "${CMAKE_INSTALL_CMAKEDIR}"
  PATH_VARS PROJECT_VERSION INCLUDE_INSTALL_DIR CMAKE_INSTALL_LIBDIR PROJECT_SOURCE_DIR
  NO_CHECK_REQUIRED_COMPONENTS_MACRO)

write_basic_package_version_file(
  "${CMAKE_LIBRARY_OUTPUT_DIRECTORY}/cmake/libcopp/libcopp-config-version.cmake"
  VERSION ${PROJECT_VERSION}
  COMPATIBILITY SameMajorVersion)

install(FILES "${CMAKE_LIBRARY_OUTPUT_DIRECTORY}/cmake/libcopp/libcopp-config.cmake"
              "${CMAKE_LIBRARY_OUTPUT_DIRECTORY}/cmake/libcopp/libcopp-config-version.cmake"
        DESTINATION ${CMAKE_INSTALL_CMAKEDIR})

set(CPACK_PACKAGE_VENDOR "libcopp")
set(CPACK_PACKAGE_VERSION_MAJOR "${PROJECT_VERSION_MAJOR}")
set(CPACK_PACKAGE_VERSION_MINOR "${PROJECT_VERSION_MINOR}")
set(CPACK_PACKAGE_VERSION_PATCH "${PROJECT_VERSION_PATCH}")
set(CPACK_PACKAGE_DESCRIPTION "libcopp ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}.${PROJECT_VERSION_PATCH}")
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "${CPACK_PACKAGE_DESCRIPTION}")
set(CPACK_PACKAGE_CONTACT "admin@owent.net")
set(CPACK_PACKAGE_HOMEPAGE_URL "https://libcopp.atframe.work/")
include(CPack)

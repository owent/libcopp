echowithcolor(COLOR GREEN "-- Configure Test Projects")

find_package(GTest)
set(PROJECT_LIBCOPP_TEST_LIB_LINK "")

# =========== find gtest ===========
if(GTEST_FOUND)
  echowithcolor(COLOR GREEN "-- GTest Found: ${GTEST_LIBRARIES}")
  add_compiler_define(COPP_MACRO_TEST_ENABLE_GTEST=1)
  set(PROJECT_LIBCOPP_TEST_LIB_LINK ${GTEST_LIBRARIES})
  include_directories(${GTEST_INCLUDE_DIRS})

  # =========== enable find boost.test ===========
elseif(LIBCOPP_TEST_ENABLE_BOOST_UNIT_TEST)

  find_package(Boost COMPONENTS unit_test_framework)
  set(Boost_AUTO_LIBS "${Boost_LIBRARIES}")

  if(Boost_FOUND)
    set(Boost_USE_STATIC_LIBS ON)
    find_package(Boost COMPONENTS unit_test_framework)
    if(NOT Boost_FOUND)
      set(Boost_USE_STATIC_LIBS OFF)
      find_package(Boost COMPONENTS unit_test_framework)
    elseif(NOT "${Boost_LIBRARIES}" EQUAL "${Boost_AUTO_LIBS}")
      set(Boost_USE_STATIC_LIBS OFF)
      find_package(Boost COMPONENTS unit_test_framework)
    endif()

    add_compiler_define(COPP_MACRO_TEST_ENABLE_BOOST_TEST=1)
    set(PROJECT_LIBCOPP_TEST_LIB_LINK ${Boost_LIBRARIES})
    include_directories(${Boost_INCLUDE_DIRS})
    echowithcolor(COLOR GREEN "-- Boost.test Found: ${Boost_UNIT_TEST_FRAMEWORK_LIBRARY}")

    if(NOT Boost_USE_STATIC_LIBS)
      echowithcolor(COLOR GREEN "-- Boost.test using dynamic library define BOOST_TEST_DYN_LINK")
      add_compiler_define(BOOST_TEST_DYN_LINK)
    endif()
  else()
    echowithcolor(COLOR RED "-- Enable boost unit test but boost.test not found.")
  endif()
endif()

include("${CMAKE_CURRENT_LIST_DIR}/test.custom-macro.cmake")
include_directories(${PROJECT_TEST_INC_DIR})

file(
  GLOB_RECURSE
  COPP_TEST_SRC_LIST
  ${PROJECT_TEST_SRC_DIR}/*.h
  ${PROJECT_TEST_SRC_DIR}/*.hpp
  ${PROJECT_TEST_SRC_DIR}/*.c
  ${PROJECT_TEST_SRC_DIR}/*.cpp
  ${PROJECT_TEST_SRC_DIR}/*.cc
  ${PROJECT_TEST_SRC_DIR}/*.cxx)
source_group_by_dir(COPP_TEST_SRC_LIST)

# ================ multi thread ================
if(NOT MSVC)
  add_definitions(-D_POSIX_MT_)
  set(PROJECT_LIBCOPP_TEST_LIB_LINK ${PROJECT_LIBCOPP_TEST_LIB_LINK} pthread)
endif()

# ============ test - coroutine test frame ============
set(CMAKE_BUILD_RPATH "$ORIGIN" "$ORIGIN/../lib" "$ORIGIN/../lib64")
if(NOT (WIN32 AND (BUILD_SHARED_LIBS OR LIBCOPP_USE_DYNAMIC_LIBRARY)))
  set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}/test")
endif()

add_executable(libcopp_unit_test ${COPP_TEST_SRC_LIST})
set_target_properties(
  libcopp_unit_test
  PROPERTIES INSTALL_RPATH_USE_LINK_PATH YES
             BUILD_WITH_INSTALL_RPATH NO
             BUILD_RPATH_USE_ORIGIN YES)

target_link_libraries(
  libcopp_unit_test ${PROJECT_LIBCOTASK_LIB_LINK} ${PROJECT_LIBCOPP_LIB_LINK} ${PROJECT_LIBCOPP_TEST_LIB_LINK}
  ${PROJECT_LIBCOPP_SYSLIB_LINK_NAMES} ${COMPILER_OPTION_EXTERN_CXX_LIBS})

if(MSVC AND MSVC_VERSION LESS 1910)
  target_compile_options(libcopp_unit_test PRIVATE /wd4503)
endif()

if(CMAKE_CXX_COMPILER_ID MATCHES "GNU")
  # GCC 12&13 BUG: https://gcc.gnu.org/bugzilla/show_bug.cgi?id=106297
  if(CMAKE_CXX_COMPILER_VERSION VERSION_GREATER_EQUAL "12.0.0" AND CMAKE_CXX_COMPILER_VERSION VERSION_LESS "14.0.0")
    target_compile_options(libcopp_unit_test PRIVATE -Wno-error=stringop-overflow)
  endif()
endif()

add_test(
  NAME "libcopp.unit_test"
  COMMAND "$<TARGET_FILE:libcopp_unit_test>"
  WORKING_DIRECTORY "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}")
set_tests_properties("libcopp.unit_test" PROPERTIES LABELS "libcopp;test;libcopp.unit_test")

if(_VALGRIND_EXECUTABLE)
  add_test(
    NAME "libcopp.unit_test-memcheck"
    COMMAND
      "${_VALGRIND_EXECUTABLE}" "--max-threads=4096" "--tool=memcheck" "--log-file=libcopp_unit_test.memcheck.log"
      "--leak-check=full" "--show-leak-kinds=all" "--malloc-fill=ef" "--free-fill=e5" "$<TARGET_FILE:libcopp_unit_test>"
    WORKING_DIRECTORY "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}")
  set_tests_properties("libcopp.unit_test-memcheck" PROPERTIES LABELS "libcopp;memcheck;libcopp.memcheck")

endif()

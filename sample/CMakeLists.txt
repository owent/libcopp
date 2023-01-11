echowithcolor(COLOR GREEN "-- Configure Sample Projects")

include("${CMAKE_CURRENT_LIST_DIR}/sample.custom-macro.cmake")
# ============ sample - [...] ============

file(
  GLOB SAMPLE_SRC_LIST
  RELATIVE "${PROJECT_SAMPLE_SRC_DIR}"
  ${PROJECT_SAMPLE_SRC_DIR}/*.cpp ${PROJECT_SAMPLE_SRC_DIR}/*.cc ${PROJECT_SAMPLE_SRC_DIR}/*.c
  ${PROJECT_SAMPLE_SRC_DIR}/*.cxx)

# find if chrono exists
include(CheckIncludeFileCXX)
check_include_file_cxx("chrono" PROJECT_LIBCOPP_SAMPLE_HAS_CHRONO)
if(PROJECT_LIBCOPP_SAMPLE_HAS_CHRONO)
  add_compiler_define(PROJECT_LIBCOPP_SAMPLE_HAS_CHRONO=1)
endif()

if(NOT (WIN32 AND (BUILD_SHARED_LIBS OR LIBCOPP_USE_DYNAMIC_LIBRARY)))
  set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}/sample")
endif()

foreach(SAMPLE_SRC_FILE IN LISTS SAMPLE_SRC_LIST)
  get_filename_component(SAMPLE_SRC_BIN_NAME ${SAMPLE_SRC_FILE} NAME_WE)
  if(SAMPLE_SRC_BIN_NAME MATCHES "sample_benchmark_std_couroutine_callable_recursive")
    set(SAMPLE_BENCHMARK_LIMIT_STACK ON)
  else()
    set(SAMPLE_BENCHMARK_LIMIT_STACK OFF)
  endif()
  if(WIN32)
    # Patch for windows path limit
    string(REGEX REPLACE "([^_])[A-Za-z]*_?" "\\1" SAMPLE_SRC_BIN_NAME "${SAMPLE_SRC_BIN_NAME}")
    set(SAMPLE_BIN_NAME_RUN_MEMCHECK_PREFIX "rm_")
  else()
    set(SAMPLE_BIN_NAME_RUN_MEMCHECK_PREFIX "run_memcheck_")
  endif()
  set(SAMPLE_SRC_BIN_NAME "libcopp_${SAMPLE_SRC_BIN_NAME}")

  add_executable(${SAMPLE_SRC_BIN_NAME} ${SAMPLE_SRC_FILE})
  set_target_properties(
    ${SAMPLE_SRC_BIN_NAME}
    PROPERTIES INSTALL_RPATH_USE_LINK_PATH YES
               BUILD_WITH_INSTALL_RPATH NO
               BUILD_RPATH_USE_ORIGIN YES)

  target_link_libraries(${SAMPLE_SRC_BIN_NAME} ${PROJECT_LIBCOTASK_LIB_LINK} ${PROJECT_LIBCOPP_LIB_LINK}
                        ${PROJECT_LIBCOPP_SYSLIB_LINK_NAMES} ${COMPILER_OPTION_EXTERN_CXX_LIBS})

  if(MSVC AND MSVC_VERSION LESS 1910)
    target_compile_options(${SAMPLE_SRC_BIN_NAME} PRIVATE /wd4503)
  endif()

  if(CMAKE_CXX_COMPILER_ID MATCHES "GNU")
    # GCC 12&13 BUG: https://gcc.gnu.org/bugzilla/show_bug.cgi?id=106297
    if(CMAKE_CXX_COMPILER_VERSION VERSION_GREATER_EQUAL "12.0.0" AND CMAKE_CXX_COMPILER_VERSION VERSION_LESS "14.0.0")
      target_compile_options(libcopp_unit_test PRIVATE -Wno-error=stringop-overflow)
    endif()
  endif()

  # add benchmark script
  string(FIND ${SAMPLE_SRC_BIN_NAME} "_benchmark_" SAMPLE_SRC_IS_BENCHMARK)

  if(${SAMPLE_SRC_IS_BENCHMARK} LESS 0)
    add_test(
      NAME "libcopp.sample-${SAMPLE_SRC_BIN_NAME}"
      COMMAND "$<TARGET_FILE:${SAMPLE_SRC_BIN_NAME}>"
      WORKING_DIRECTORY "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}")
    set_tests_properties("libcopp.sample-${SAMPLE_SRC_BIN_NAME}" PROPERTIES LABELS "libcopp;sample;libcopp.sample")

    if(_VALGRIND_EXECUTABLE)
      add_test(
        NAME "libcopp.memcheck-${SAMPLE_SRC_BIN_NAME}"
        COMMAND
          "${_VALGRIND_EXECUTABLE}" "--tool=memcheck" "--log-file=${SAMPLE_SRC_BIN_NAME}.memcheck.log"
          "--leak-check=full" "--show-leak-kinds=all" "--malloc-fill=ef" "--free-fill=e5"
          "$<TARGET_FILE:${SAMPLE_SRC_BIN_NAME}>"
        WORKING_DIRECTORY "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}")
      set_tests_properties("libcopp.memcheck-${SAMPLE_SRC_BIN_NAME}" PROPERTIES LABELS
                                                                                "libcopp;memcheck;libcopp.memcheck")

    endif()
  else()
    add_test(
      NAME "libcopp.benchmark-${SAMPLE_SRC_BIN_NAME}-30000-100-64"
      COMMAND "$<TARGET_FILE:${SAMPLE_SRC_BIN_NAME}>" 30000 100 64
      WORKING_DIRECTORY "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}")
    set_tests_properties("libcopp.benchmark-${SAMPLE_SRC_BIN_NAME}-30000-100-64"
                         PROPERTIES LABELS "libcopp;benchmark;libcopp.benchmark")

    add_test(
      NAME "libcopp.benchmark-${SAMPLE_SRC_BIN_NAME}-1000-1000-2048"
      COMMAND "$<TARGET_FILE:${SAMPLE_SRC_BIN_NAME}>" 1000 1000 2048
      WORKING_DIRECTORY "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}")
    set_tests_properties("libcopp.benchmark-${SAMPLE_SRC_BIN_NAME}-1000-1000-2048"
                         PROPERTIES LABELS "libcopp;benchmark;libcopp.benchmark")

    if(SAMPLE_BENCHMARK_LIMIT_STACK)
      add_test(
        NAME "libcopp.benchmark-${SAMPLE_SRC_BIN_NAME}-1-10000-16"
        COMMAND "$<TARGET_FILE:${SAMPLE_SRC_BIN_NAME}>" 1 10000 16
        WORKING_DIRECTORY "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}")
      set_tests_properties("libcopp.benchmark-${SAMPLE_SRC_BIN_NAME}-1-10000-16"
                           PROPERTIES LABELS "libcopp;benchmark;libcopp.benchmark")
    else()
      add_test(
        NAME "libcopp.benchmark-${SAMPLE_SRC_BIN_NAME}-1-1000000-16"
        COMMAND "$<TARGET_FILE:${SAMPLE_SRC_BIN_NAME}>" 1 1000000 16
        WORKING_DIRECTORY "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}")
      set_tests_properties("libcopp.benchmark-${SAMPLE_SRC_BIN_NAME}-1-1000000-16"
                           PROPERTIES LABELS "libcopp;benchmark;libcopp.benchmark")
    endif()

    # for valgrind callgrind
    if(_VALGRIND_EXECUTABLE)
      add_test(
        NAME "libcopp.callgrind-${SAMPLE_SRC_BIN_NAME}-10000-1000-64"
        COMMAND
          "${_VALGRIND_EXECUTABLE}" "--tool=callgrind" "--log-file=${SAMPLE_SRC_BIN_NAME}.callgrind.log"
          "--callgrind-out-file=callgrind.out.${SAMPLE_SRC_BIN_NAME}" "--cache-sim=yes" "--branch-sim=yes"
          "--dump-instr=yes" "--collect-jumps=yes" "$<TARGET_FILE:${SAMPLE_SRC_BIN_NAME}>" 10000 1000 64
        WORKING_DIRECTORY "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}")
      set_tests_properties("libcopp.callgrind-${SAMPLE_SRC_BIN_NAME}-10000-1000-64"
                           PROPERTIES LABELS "libcopp;callgrind;libcopp.callgrind")
    endif()
  endif()
endforeach()

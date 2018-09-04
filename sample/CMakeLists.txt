

EchoWithColor(COLOR GREEN "-- Configure Sample Projects")

include ("${CMAKE_CURRENT_LIST_DIR}/sample.custom-macro.cmake")
# ============ sample - [...] ============

file(GLOB SAMPLE_SRC_LIST RELATIVE "${PROJECT_SAMPLE_SRC_DIR}"
    ${PROJECT_SAMPLE_SRC_DIR}/*.cpp
    ${PROJECT_SAMPLE_SRC_DIR}/*.cc
    ${PROJECT_SAMPLE_SRC_DIR}/*.c
    ${PROJECT_SAMPLE_SRC_DIR}/*.cxx
)

# find if chrono exists
include(CheckIncludeFileCXX)
CHECK_INCLUDE_FILE_CXX("chrono" PROJECT_LIBCOPP_SAMPLE_HAS_CHRONO)
if (PROJECT_LIBCOPP_SAMPLE_HAS_CHRONO)
    add_compiler_define(PROJECT_LIBCOPP_SAMPLE_HAS_CHRONO=1)
endif()

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}/sample")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELWITHDEBINFO ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_MINSIZEREL ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})

foreach(SAMPLE_SRC_FILE IN LISTS SAMPLE_SRC_LIST)
    get_filename_component(SAMPLE_SRC_BIN_NAME ${SAMPLE_SRC_FILE} NAME_WE)
    set(SAMPLE_SRC_BIN_NAME "libcopp_${SAMPLE_SRC_BIN_NAME}")
    
    add_executable(${SAMPLE_SRC_BIN_NAME} ${SAMPLE_SRC_FILE})
    target_link_libraries(${SAMPLE_SRC_BIN_NAME}
	    ${PROJECT_LIBCOTASK_LIB_LINK} 
		${PROJECT_LIBCOPP_LIB_LINK}
        ${COMPILER_OPTION_EXTERN_CXX_LIBS}
    )

    # add benchmark script
    string(FIND ${SAMPLE_SRC_BIN_NAME} "_benchmark_" SAMPLE_SRC_IS_BENCHMARK)

    if (${SAMPLE_SRC_IS_BENCHMARK} LESS 0)
        add_custom_target("run_sample_${SAMPLE_SRC_BIN_NAME}"
            "./${SAMPLE_SRC_BIN_NAME}${CMAKE_EXECUTABLE_SUFFIX}"
            WORKING_DIRECTORY ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}
        )
        add_dependencies(run_sample "run_sample_${SAMPLE_SRC_BIN_NAME}")
        add_dependencies("run_sample_${SAMPLE_SRC_BIN_NAME}" ${SAMPLE_SRC_BIN_NAME})

        if (_VALGRIND_EXECUTABLE)
            add_custom_target("run_memcheck_${SAMPLE_SRC_BIN_NAME}"
                ${_VALGRIND_EXECUTABLE} "--tool=memcheck" "--log-file=${SAMPLE_SRC_BIN_NAME}.memcheck.log" --leak-check=full --show-leak-kinds=all --malloc-fill=ef --free-fill=e5 "./${SAMPLE_SRC_BIN_NAME}${CMAKE_EXECUTABLE_SUFFIX}"
                WORKING_DIRECTORY ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}
            )
            add_dependencies(memcheck "run_memcheck_${SAMPLE_SRC_BIN_NAME}")
            add_dependencies("run_memcheck_${SAMPLE_SRC_BIN_NAME}" ${SAMPLE_SRC_BIN_NAME})
        endif()
    else()
        add_custom_target("run_test_${SAMPLE_SRC_BIN_NAME}_30000"
            "./${SAMPLE_SRC_BIN_NAME}${CMAKE_EXECUTABLE_SUFFIX}" 30000 100 64
            WORKING_DIRECTORY ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}
        )
        add_dependencies(benchmark "run_test_${SAMPLE_SRC_BIN_NAME}_30000")
        add_dependencies("run_test_${SAMPLE_SRC_BIN_NAME}_30000" ${SAMPLE_SRC_BIN_NAME})

        add_custom_target("run_test_${SAMPLE_SRC_BIN_NAME}_1000"
            "./${SAMPLE_SRC_BIN_NAME}${CMAKE_EXECUTABLE_SUFFIX}" 1000 1000 2048
            WORKING_DIRECTORY ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}
        )
        add_dependencies(benchmark "run_test_${SAMPLE_SRC_BIN_NAME}_1000")
        add_dependencies("run_test_${SAMPLE_SRC_BIN_NAME}_1000" ${SAMPLE_SRC_BIN_NAME})

        add_custom_target("run_test_${SAMPLE_SRC_BIN_NAME}_1"
            "./${SAMPLE_SRC_BIN_NAME}${CMAKE_EXECUTABLE_SUFFIX}" 1 3000000 16
            WORKING_DIRECTORY ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}
        )
        add_dependencies(benchmark "run_test_${SAMPLE_SRC_BIN_NAME}_1")
        add_dependencies("run_test_${SAMPLE_SRC_BIN_NAME}_1" ${SAMPLE_SRC_BIN_NAME})

        # for valgrind callgrind
        if (_VALGRIND_EXECUTABLE)
            add_custom_target("run_callgrind_${SAMPLE_SRC_BIN_NAME}_10000"
                ${_VALGRIND_EXECUTABLE} "--tool=callgrind" "--log-file=${SAMPLE_SRC_BIN_NAME}.callgrind.log" "--callgrind-out-file=callgrind.out.${SAMPLE_SRC_BIN_NAME}" --cache-sim=yes --branch-sim=yes --dump-instr=yes --collect-jumps=yes "./${SAMPLE_SRC_BIN_NAME}${CMAKE_EXECUTABLE_SUFFIX}" 10000 1000 64
                WORKING_DIRECTORY ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}
            )
            add_dependencies(callgrind "run_callgrind_${SAMPLE_SRC_BIN_NAME}_10000")
            add_dependencies("run_callgrind_${SAMPLE_SRC_BIN_NAME}_10000" ${SAMPLE_SRC_BIN_NAME})
        endif()
    endif()
endforeach()

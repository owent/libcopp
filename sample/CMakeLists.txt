

EchoWithColor(COLOR GREEN "-- Configure Sample Projects")

# ============ sample - [...] ============

file(GLOB SAMPLE_SRC_LIST RELATIVE "${PROJECT_SAMPLE_SRC_DIR}"
    ${PROJECT_SAMPLE_SRC_DIR}/*.cpp
    ${PROJECT_SAMPLE_SRC_DIR}/*.cc
    ${PROJECT_SAMPLE_SRC_DIR}/*.c
    ${PROJECT_SAMPLE_SRC_DIR}/*.cxx
)

set(EXECUTABLE_OUTPUT_PATH "${PROJECT_BINARY_DIR}/sample")

foreach(SAMPLE_SRC_FILE IN LISTS SAMPLE_SRC_LIST)
    get_filename_component(SAMPLE_SRC_BIN_NAME "${SAMPLE_SRC_FILE}" NAME_WE)
    
    add_executable(${SAMPLE_SRC_BIN_NAME} ${SAMPLE_SRC_FILE})
    target_link_libraries(${SAMPLE_SRC_BIN_NAME}
	    ${PROJECT_LIBCOTASK_LIB_LINK} 
		${PROJECT_LIBCOPP_LIB_LINK}
        ${COMPILER_OPTION_EXTERN_CXX_LIBS}
    )

    # add benchmark script
    add_custom_target("run_test_${SAMPLE_SRC_BIN_NAME}_10000"
        "./${SAMPLE_SRC_BIN_NAME}" 10000 1000 200
        WORKING_DIRECTORY ${EXECUTABLE_OUTPUT_PATH}
    )
    add_dependencies(benchmark "run_test_${SAMPLE_SRC_BIN_NAME}_10000")
    add_dependencies("run_test_${SAMPLE_SRC_BIN_NAME}_10000" ${SAMPLE_SRC_BIN_NAME})

    add_custom_target("run_test_${SAMPLE_SRC_BIN_NAME}_1000"
        "./${SAMPLE_SRC_BIN_NAME}" 1000 1000 2048
        WORKING_DIRECTORY ${EXECUTABLE_OUTPUT_PATH}
    )
    add_dependencies(benchmark "run_test_${SAMPLE_SRC_BIN_NAME}_1000")
    add_dependencies("run_test_${SAMPLE_SRC_BIN_NAME}_1000" ${SAMPLE_SRC_BIN_NAME})
endforeach()

# =========== libcopp/src - fcontext : ${LIBCOPP_FCONTEXT_OS_PLATFORM} - sysv - ${LIBCOPP_FCONTEXT_BIN_FORMAT} - ${LIBCOPP_FCONTEXT_AS_T
file(MAKE_DIRECTORY "${PROJECT_LIBCOPP_FCONTEXT_BIN_DIR}")

if(MSVC)
    list(APPEND COPP_SRC_LIST
            "${PROJECT_LIBCOPP_FCONTEXT_ASM_DIR}/${PROJECT_LIBCOPP_FCONTEXT_SRC_NAME_MAKE}.asm"
            "${PROJECT_LIBCOPP_FCONTEXT_ASM_DIR}/${PROJECT_LIBCOPP_FCONTEXT_SRC_NAME_JUMP}.asm"
            "${PROJECT_LIBCOPP_FCONTEXT_ASM_DIR}/${PROJECT_LIBCOPP_FCONTEXT_SRC_NAME_ONTOP}.asm"
    )
    set(MASMFound FALSE)
    enable_language(ASM_MASM)
    if(CMAKE_ASM_MASM_COMPILER_WORKS)
        SET(MASMFound TRUE)
    else()
        EchoWithColor(COLOR RED "-- enable masm failed")
        message(FATAL_ERROR "enable ASM_MASM failed")
    endif(CMAKE_ASM_MASM_COMPILER_WORKS)
else()
    list(APPEND COPP_SRC_LIST 
        "${PROJECT_LIBCOPP_FCONTEXT_ASM_DIR}/${PROJECT_LIBCOPP_FCONTEXT_SRC_NAME_MAKE}.asm"
        "${PROJECT_LIBCOPP_FCONTEXT_ASM_DIR}/${PROJECT_LIBCOPP_FCONTEXT_SRC_NAME_JUMP}.asm"
        "${PROJECT_LIBCOPP_FCONTEXT_ASM_DIR}/${PROJECT_LIBCOPP_FCONTEXT_SRC_NAME_ONTOP}.asm"
    )
    # unset (LIBCOPP_FCONTEXT_AS_TOOL_OPTION )
    # 
    # if (LIBCOPP_FCONTEXT_USE_TSX)
    #     list (APPEND LIBCOPP_FCONTEXT_AS_TOOL_OPTION "/DLIBCOPP_FCONTEXT_AS_TOOL_OPTION=1")
    # endif()
    # 
    # if (CMAKE_ASM_MASM_COMPILER)
    #     set(LIBCOPP_FCONTEXT_AS_TOOL_BIN ${CMAKE_ASM_MASM_COMPILER})
    # else ()
    #     find_program(LIBCOPP_FCONTEXT_AS_TOOL_BIN armasm)
    #     if(NOT LIBCOPP_FCONTEXT_AS_TOOL_BIN)
    #         EchoWithColor(COLOR RED "-- armasm not found in path, please add as's directory into path")
    #         message(FATAL_ERROR "armasm not found")
    #     endif()
    # endif ()
    # 
	# add_custom_command(
    #     OUTPUT
    #         "${PROJECT_LIBCOPP_FCONTEXT_BIN_DIR}/${PROJECT_LIBCOPP_FCONTEXT_BIN_NAME_JUMP}"
    #         "${PROJECT_LIBCOPP_FCONTEXT_BIN_DIR}/${PROJECT_LIBCOPP_FCONTEXT_BIN_NAME_MAKE}"
    #         "${PROJECT_LIBCOPP_FCONTEXT_BIN_DIR}/${PROJECT_LIBCOPP_FCONTEXT_BIN_NAME_ONTOP}"
    #     COMMAND ${LIBCOPP_FCONTEXT_AS_TOOL_BIN} ${LIBCOPP_FCONTEXT_AS_TOOL_OPTION} "${PROJECT_LIBCOPP_FCONTEXT_ASM_DIR}/${PROJECT_LIBCOPP_FCONTEXT_SRC_NAME_JUMP}.asm" "${PROJECT_LIBCOPP_FCONTEXT_BIN_DIR}/${PROJECT_LIBCOPP_FCONTEXT_BIN_NAME_JUMP}"
    #     COMMAND ${LIBCOPP_FCONTEXT_AS_TOOL_BIN} ${LIBCOPP_FCONTEXT_AS_TOOL_OPTION} "${PROJECT_LIBCOPP_FCONTEXT_ASM_DIR}/${PROJECT_LIBCOPP_FCONTEXT_SRC_NAME_MAKE}.asm" "${PROJECT_LIBCOPP_FCONTEXT_BIN_DIR}/${PROJECT_LIBCOPP_FCONTEXT_BIN_NAME_MAKE}"
    #     COMMAND ${LIBCOPP_FCONTEXT_AS_TOOL_BIN} ${LIBCOPP_FCONTEXT_AS_TOOL_OPTION} "${PROJECT_LIBCOPP_FCONTEXT_ASM_DIR}/${PROJECT_LIBCOPP_FCONTEXT_SRC_NAME_ONTOP}.asm" "${PROJECT_LIBCOPP_FCONTEXT_BIN_DIR}/${PROJECT_LIBCOPP_FCONTEXT_BIN_NAME_ONTOP}"
    #     DEPENDS
    #         "${PROJECT_LIBCOPP_FCONTEXT_ASM_DIR}/${PROJECT_LIBCOPP_FCONTEXT_SRC_NAME_JUMP}.asm"
    #         "${PROJECT_LIBCOPP_FCONTEXT_ASM_DIR}/${PROJECT_LIBCOPP_FCONTEXT_SRC_NAME_MAKE}.asm"
    #         "${PROJECT_LIBCOPP_FCONTEXT_ASM_DIR}/${PROJECT_LIBCOPP_FCONTEXT_SRC_NAME_ONTOP}.asm"
    #     WORKING_DIRECTORY ${PROJECT_LIBCOPP_FCONTEXT_BIN_DIR}
    #     COMMENT "Generate ${PROJECT_LIBCOPP_FCONTEXT_BIN_NAME_JUMP},${PROJECT_LIBCOPP_FCONTEXT_BIN_NAME_MAKE},${PROJECT_LIBCOPP_FCONTEXT_BIN_NAME_ONTOP}"
	# )
	# 
    # # execute_process (
    # #     COMMAND ${LIBCOPP_FCONTEXT_AS_TOOL_BIN} ${LIBCOPP_FCONTEXT_AS_TOOL_OPTION} "${PROJECT_LIBCOPP_FCONTEXT_ASM_DIR}/${PROJECT_LIBCOPP_FCONTEXT_SRC_NAME_MAKE}.asm"
    # #     "${PROJECT_LIBCOPP_FCONTEXT_BIN_DIR}/${PROJECT_LIBCOPP_FCONTEXT_BIN_NAME_MAKE}"
    # #     WORKING_DIRECTORY "${PROJECT_LIBCOPP_FCONTEXT_BIN_DIR}"
    # # )
	# 
    # # execute_process (
    # #     COMMAND ${LIBCOPP_FCONTEXT_AS_TOOL_BIN} ${LIBCOPP_FCONTEXT_AS_TOOL_OPTION} "${PROJECT_LIBCOPP_FCONTEXT_ASM_DIR}/${PROJECT_LIBCOPP_FCONTEXT_SRC_NAME_JUMP}.asm"
    # #     "${PROJECT_LIBCOPP_FCONTEXT_BIN_DIR}/${PROJECT_LIBCOPP_FCONTEXT_BIN_NAME_JUMP}"
    # #     WORKING_DIRECTORY "${PROJECT_LIBCOPP_FCONTEXT_BIN_DIR}"
    # # )
	# 
    # # execute_process (
    # #     COMMAND ${LIBCOPP_FCONTEXT_AS_TOOL_BIN} ${LIBCOPP_FCONTEXT_AS_TOOL_OPTION} "${PROJECT_LIBCOPP_FCONTEXT_ASM_DIR}/${PROJECT_LIBCOPP_FCONTEXT_SRC_NAME_ONTOP}.asm"
    # #     "${PROJECT_LIBCOPP_FCONTEXT_BIN_DIR}/${PROJECT_LIBCOPP_FCONTEXT_BIN_NAME_ONTOP}"
    # #     WORKING_DIRECTORY "${PROJECT_LIBCOPP_FCONTEXT_BIN_DIR}"
    # # )
    # 
    # list(APPEND COPP_OBJ_LIST 
    #     "${PROJECT_LIBCOPP_FCONTEXT_BIN_DIR}/${PROJECT_LIBCOPP_FCONTEXT_BIN_NAME_JUMP}" 
    #     "${PROJECT_LIBCOPP_FCONTEXT_BIN_DIR}/${PROJECT_LIBCOPP_FCONTEXT_BIN_NAME_MAKE}"
    #     "${PROJECT_LIBCOPP_FCONTEXT_BIN_DIR}/${PROJECT_LIBCOPP_FCONTEXT_BIN_NAME_ONTOP}"
    # )
endif()

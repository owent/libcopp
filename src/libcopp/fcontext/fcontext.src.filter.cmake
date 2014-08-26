# =========== libcopp/src - fcontext =========== 
set (PROJECT_LIBCOPP_FCONTEXT_SRC_DIR "${CMAKE_CURRENT_LIST_DIR}")
set (PROJECT_LIBCOPP_FCONTEXT_ASM_DIR "${PROJECT_LIBCOPP_FCONTEXT_SRC_DIR}/asm")

set (PROJECT_LIBCOPP_FCONTEXT_BIN_DIR "${PROJECT_BINARY_DIR}/src/libcopp/fcontext")
set (PROJECT_LIBCOPP_FCONTEXT_BIN_NAME_MAKE "fcontext_make.o")
set (PROJECT_LIBCOPP_FCONTEXT_BIN_NAME_JUMP "fcontext_jump.o")

# ========== set platform, abi, binary format and as tool ==========
include("${PROJECT_LIBCOPP_FCONTEXT_SRC_DIR}/detect/fcontext.detect.cmake")

# ========== tools check ==========
find_program(LIBCOPP_FCONTEXT_AS_TOOL_BIN ${LIBCOPP_FCONTEXT_AS_TOOL})
if(NOT LIBCOPP_FCONTEXT_AS_TOOL_BIN)
	EchoWithColor(COLOR RED "-- ${LIBCOPP_FCONTEXT_AS_TOOL} not found in path, please add ${LIBCOPP_FCONTEXT_AS_TOOL}'s directory into path")
	message(FATAL_ERROR "${LIBCOPP_FCONTEXT_AS_TOOL} not found")
endif()
include("${PROJECT_LIBCOPP_FCONTEXT_SRC_DIR}/tools/${LIBCOPP_FCONTEXT_ABI}.cmake")

# ========== show fcontext info ==========
EchoWithColor(COLOR GREEN "-- fcontext.os_platform => ${LIBCOPP_FCONTEXT_OS_PLATFORM}")
EchoWithColor(COLOR GREEN "-- fcontext.abi => ${LIBCOPP_FCONTEXT_ABI}")
EchoWithColor(COLOR GREEN "-- fcontext.bin_formation => ${LIBCOPP_FCONTEXT_BIN_FORMAT}")
EchoWithColor(COLOR GREEN "-- fcontext.as_tool => ${LIBCOPP_FCONTEXT_AS_TOOL}")

# ========== msvc x86 disable safeseh ==========
if (MSVC AND "${LIBCOPP_FCONTEXT_OS_PLATFORM}" STREQUAL "i386")
	set (CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} /SAFESEH:NO")
	set (CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} /SAFESEH:NO")
	set (CMAKE_MODULE_LINKER_FLAGS "${CMAKE_MODULE_LINKER_FLAGS} /SAFESEH:NO")
endif()

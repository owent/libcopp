# =========== libcopp/src - fcontext - detect =========== 
set (PROJECT_LIBCOPP_FCONTEXT_DETECT_DIR "${PROJECT_LIBCOPP_FCONTEXT_SRC_DIR}/detect")

# ========== set os platform ==========
if (NOT LIBCOPP_FCONTEXT_OS_PLATFORM)
	if ("${PLATFORM_SUFFIX}" STREQUAL "64")
		set(LIBCOPP_FCONTEXT_OS_PLATFORM "x86_64")
	else()
		set(LIBCOPP_FCONTEXT_OS_PLATFORM "i386")
	endif()
endif()

# ========== set abi ==========
if(NOT LIBCOPP_FCONTEXT_ABI)
	set(LIBCOPP_FCONTEXT_ABI "sysv")
	
	if (WIN32)
		set(LIBCOPP_FCONTEXT_ABI "ms")
	elseif( "${LIBCOPP_FCONTEXT_OS_PLATFORM}" STREQUAL "arm" )
		set(LIBCOPP_FCONTEXT_ABI "aapcs")
	elseif( "${LIBCOPP_FCONTEXT_OS_PLATFORM}" STREQUAL "mips" )
		set(LIBCOPP_FCONTEXT_ABI "o32")
	endif()
endif()

# ========== set binary format ==========
if(NOT LIBCOPP_FCONTEXT_BIN_FORMAT)
	set(LIBCOPP_FCONTEXT_BIN_FORMAT "elf")
	
	if (WIN32 AND "${LIBCOPP_FCONTEXT_OS_PLATFORM}" STREQUAL "arm" )
		set(LIBCOPP_FCONTEXT_BIN_FORMAT "pe_armasm")
	elseif( WIN32 )
		set(LIBCOPP_FCONTEXT_BIN_FORMAT "pe_masm")
	elseif(APPLE)
		set(LIBCOPP_FCONTEXT_BIN_FORMAT "macho")
	endif()
endif()

# ========== set as tool ==========
if(NOT LIBCOPP_FCONTEXT_AS_TOOL)
	set(LIBCOPP_FCONTEXT_AS_TOOL "as")
	
	if (WIN32 AND "${LIBCOPP_FCONTEXT_OS_PLATFORM}" STREQUAL "arm" )
		set(LIBCOPP_FCONTEXT_AS_TOOL "armasm")
	elseif( WIN32 AND "${PLATFORM_SUFFIX}" STREQUAL "64" )
		set(LIBCOPP_FCONTEXT_AS_TOOL "ml64")
	elseif(WIN32)
		set(LIBCOPP_FCONTEXT_AS_TOOL "ml")
	endif()
endif()